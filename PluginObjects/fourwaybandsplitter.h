#pragma once

#ifndef __FourWayBandSplitter__
#define __FourWayBandSplitter__

#include "fxobjects.h"


// TESTING


/**
\struct FourWayBandSplitterParameters
\ingroup FX-Objects
\brief
Custom parameter structure for the FourWayBandSplitter object.

\author <Christian George> <http://www.yourwebsite.com>
\remark <Put any remarks or notes here>
\version Revision : 1.0
\date Date : 2019 / 01 / 31
*/

enum class splitSelection {kSplitter, kLPF, kLowBand, kHighBand, kHPF};

struct FourWayBandSplitterParameters
{
	FourWayBandSplitterParameters() {}

	/** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
	FourWayBandSplitterParameters& operator=(const FourWayBandSplitterParameters& params)	// need this override for collections to work
	{
		// --- it is possible to try to make the object equal to itself
		//     e.g. thisObject = thisObject; so this code catches that
		//     trivial case and just returns this object
		if (this == &params)
			return *this;

		// --- copy from params (argument) INTO our variables
		lowSplit = params.lowSplit;
		midSplit = params.midSplit;
		highSplit = params.highSplit;

		splitView = params.splitView;

		lpfVolume = params.lpfVolume;
		lowbandVolume = params.lowbandVolume;
		highbandVolume = params.highbandVolume;
		hpfVolume = params.hpfVolume;
		dryVolume = params.dryVolume;

		lpfSaturation = params.lpfSaturation;
		lowbandSaturation = params.lowbandSaturation;
		highbandSaturation = params.highbandSaturation;
		hpfSaturation = params.hpfSaturation;
		enableSplitter = params.enableSplitter;
		enableModFilter = params.enableModFilter;

		splitterBoost = params.splitterBoost;



		// --- MUST be last
		return *this;
	}

	// --- individual parameters
	double lowSplit = 400.0;
	double midSplit = 1000.0;
	double highSplit = 10000.0;

	splitSelection splitView = splitSelection::kSplitter;

	double lpfVolume = 0.0;
	double lowbandVolume = 0.0;
	double highbandVolume = 0.0;
	double hpfVolume = 0.0;
	double dryVolume = 0.0;

	double lpfSaturation = 0.0;
	double lowbandSaturation = 0.0;
	double highbandSaturation = 0.0;
	double hpfSaturation = 0.0;

	bool enableSplitter = false;
	bool enableModFilter = false;

	double splitterBoost = 0.0;
};


/**
\class FourWayBandSplitter
\ingroup FX-Objects
\brief
The FourWayBandSplitter object implements ....

Audio I/O:
- Processes mono input to mono output.
- *** Optionally, process frame *** Modify this according to your object functionality

Control I/F:
- Use FourWayBandSplitterParameters structure to get/set object params.

\author <Your Name> <http://www.yourwebsite.com>
\remark <Put any remarks or notes here>
\version Revision : 1.0
\date Date : 2019 / 01 / 31
*/
class FourWayBandSplitter : public IAudioSignalProcessor
{
public:
	FourWayBandSplitter(void) {}	/* C-TOR */
	~FourWayBandSplitter(void) {}	/* D-TOR */

public:
	/** reset members to initialized state */
	virtual bool reset(double _sampleRate)
	{
		// --- store the sample rate
		sampleRate = _sampleRate;

		// --- do any other per-audio-run inits here
		splitterFilters[0].reset(sampleRate);
		splitterFilters[1].reset(sampleRate);
		splitterFilters[2].reset(sampleRate);
		splitterFilters[3].reset(sampleRate);
		splitterFilters[4].reset(sampleRate);
		splitterFilters[5].reset(sampleRate);

		return true;
	}

	/** process MONO input */
	/**
	\param xn input
	\return the processed sample
	*/
	virtual double processAudioSample(double xn)
	{
		// --- the output variable
		double ynL = 0.0;

		// --- do your DSP magic here to create yn
		FilterBankOutput lowSplit_Left = splitterFilters[0].processFilterBank(xn);
		FilterBankOutput midSplit_Left = splitterFilters[2].processFilterBank(xn);
		FilterBankOutput highSplit_Left = splitterFilters[4].processFilterBank(xn);

		double lpf_Left = lowSplit_Left.LFOut * lpfVolume_cooked;
		double lowBand_Left = midSplit_Left.LFOut * lowbandVolume_cooked;
		double highBand_Left = highSplit_Left.LFOut * highbandVolume_cooked;
		double hpf_Left = highSplit_Left.HFOut * hpfVolume_cooked;

		if (parameters.splitView == splitSelection::kSplitter)
			ynL = lpf_Left + lowBand_Left + highBand_Left + hpf_Left;
		else if (parameters.splitView == splitSelection::kLPF)
			ynL = lpf_Left;
		else if (parameters.splitView == splitSelection::kLowBand)
			ynL = lowBand_Left;
		else if (parameters.splitView == splitSelection::kHighBand)
			ynL = highBand_Left;
		else if (parameters.splitView == splitSelection::kHPF)
			ynL = hpf_Left;

		// --- done
		return ynL;
	}

	/** query to see if this object can process frames */
	virtual bool canProcessAudioFrame() { return true; } // <-- change this!

	/** process audio frame: implement this function if you answer "true" to above query */
	virtual bool processAudioFrame(const float* inputFrame,	/* ptr to one frame of data: pInputFrame[0] = left, pInputFrame[1] = right, etc...*/
		float* outputFrame,
		uint32_t inputChannels,
		uint32_t outputChannels)
	{

		double xnL = inputFrame[0];
		double xnR = inputFrame[1];
		double ynL = xnL;
		double ynR = xnR;

		if (parameters.enableSplitter)
		{
			// --- Filter Bank Outputs
			FilterBankOutput lowSplit_Left = splitterFilters[0].processFilterBank(xnL);
			FilterBankOutput lowSplit_Right = splitterFilters[1].processFilterBank(xnR);

			FilterBankOutput midSplit_Left = splitterFilters[2].processFilterBank(lowSplit_Left.HFOut);
			FilterBankOutput midSplit_Right = splitterFilters[3].processFilterBank(lowSplit_Right.HFOut);

			FilterBankOutput highSplit_Left = splitterFilters[4].processFilterBank(midSplit_Left.HFOut);
			FilterBankOutput highSplit_Right = splitterFilters[5].processFilterBank(midSplit_Right.HFOut);


			// --- Channel Bands
			// Left
			double lpf_Left = lowSplit_Left.LFOut * lpfVolume_cooked;
			double lowBand_Left = midSplit_Left.LFOut * lowbandVolume_cooked;
			double highBand_Left = highSplit_Left.LFOut * highbandVolume_cooked;
			double hpf_Left = highSplit_Left.HFOut * hpfVolume_cooked;

			// Right
			double lpf_Right = lowSplit_Right.LFOut * lpfVolume_cooked;
			double lowBand_Right = midSplit_Right.LFOut * lowbandVolume_cooked;
			double highBand_Right = highSplit_Right.LFOut * highbandVolume_cooked;
			double hpf_Right = highSplit_Right.HFOut * hpfVolume_cooked;


			// --- Saturation
			if (parameters.lpfSaturation > 1)
			{
				lpf_Left = tanh(lpf_Left * lpfK) / tanh(lpfK);
				lpf_Right = tanh(lpf_Right * lpfK) / tanh(lpfK);
			}
			if (parameters.lowbandSaturation > 1)
			{
				lowBand_Left = tanh(lowBand_Left * lowbandK) / tanh(lowbandK);
				lowBand_Right = tanh(lowBand_Right * lowbandK) / tanh(lowbandK);
			}
			if (parameters.highbandSaturation > 1)
			{
				highBand_Left = tanh(highBand_Left * highbandK) / tanh(highbandK);
				highBand_Right = tanh(highBand_Right * highbandK) / tanh(highbandK);
			}
			if (parameters.hpfSaturation > 1)
			{
				hpf_Left = tanh(hpf_Left * hpfK) / tanh(hpfK);
				hpf_Right = tanh(hpf_Right * hpfK) / tanh(hpfK);
			}
				
			

			// --- Dry Signal
			double dryInput_Left = (lowSplit_Left.LFOut + lowSplit_Left.HFOut
									+ midSplit_Left.LFOut + midSplit_Left.HFOut 
									+ highSplit_Left.LFOut + highSplit_Left.HFOut)
									* dryVolume_cooked;

			double dryInput_Right = (lowSplit_Right.LFOut + lowSplit_Right.HFOut
									+ midSplit_Right.LFOut + midSplit_Right.HFOut
									+ highSplit_Right.LFOut + highSplit_Right.HFOut)
									* dryVolume_cooked;


			// --- Channel Split Output
			if (parameters.splitView == splitSelection::kSplitter)
			{
				ynL = (lpf_Left + lowBand_Left + highBand_Left + hpf_Left) * boostCooked + dryInput_Left;
				ynR = (lpf_Right + lowBand_Right + highBand_Right + hpf_Right) * boostCooked + dryInput_Right;
			}
			else if (parameters.splitView == splitSelection::kLPF)
			{
				ynL = lpf_Left;
				ynR = lpf_Right;
			}
			else if (parameters.splitView == splitSelection::kLowBand)
			{
				ynL = lowBand_Left;
				ynR = lowBand_Right;
			}
			else if (parameters.splitView == splitSelection::kHighBand)
			{
				ynL = highBand_Left;
				ynR = highBand_Right;
			}
			else if (parameters.splitView == splitSelection::kHPF)
			{
				ynL = hpf_Left;
				ynR = hpf_Right;
			}
		}

		
	

		// --- Mono
		if (inputChannels == 1 &&
			outputChannels == 1)
		{
			outputFrame[0] = ynL;
			return true; /// processed
		}


		// --- Mono-In/Stereo-Out
		else if (inputChannels == 1 &&
				 outputChannels == 2)
		{
			outputFrame[0] = ynL;
			outputFrame[1] = ynL;

			return true; /// processed
		}

		// --- Stereo-In/Stereo-Out
		else if (inputChannels == 2 &&
				outputChannels == 2)
		{
			outputFrame[0] = ynL;
			outputFrame[1] = ynR;

			return true; /// processed
		}
		// --- do nothing
		return false; // NOT handled
	}


	/** get parameters: note use of custom structure for passing param data */
	/**
	\return FourWayBandSplitterParameters custom data structure
	*/
	FourWayBandSplitterParameters getParameters()
	{
		return parameters;
	}

	/** set parameters: note use of custom structure for passing param data */
	/**
	\param FourWayBandSplitterParameters custom data structure
	*/
	void setParameters(const FourWayBandSplitterParameters& params)
	{
		// --- copy them; note you may choose to ignore certain items
		//     and copy the variables one at a time, or you may test
		//     to see if cook-able variables have changed; if not, then
		//     do not re-cook them as it just wastes CPU
		parameters = params;

		

		// --- cook band volumes
		lpfVolume_cooked = pow(10, parameters.lpfVolume / 20);
		lowbandVolume_cooked = pow(10, parameters.lowbandVolume / 20);
		highbandVolume_cooked = pow(10, parameters.highbandVolume / 20);
		hpfVolume_cooked = pow(10, parameters.hpfVolume / 20);

		boostCooked = pow(10, parameters.splitterBoost / 20);

		// --- set range of dry volume
		if (parameters.dryVolume <= -15.0)
		{
			dryVolume_cooked = 0.0;
		}
		else
		{
			dryVolume_cooked = pow(10, parameters.dryVolume / 20);
		}

		// --- Cook Saturation
		lpfK = parameters.lpfSaturation;
		lowbandK = parameters.lowbandSaturation;
		highbandK = parameters.highbandSaturation;
		hpfK = parameters.hpfSaturation;

		
		LRFilterBankParameters bankParams = splitterFilters[0].getParameters();

		 // --- Low Split
		if (parameters.lowSplit >= parameters.midSplit)
			parameters.lowSplit = parameters.midSplit;

		// Frequency
		bankParams.splitFrequency = parameters.lowSplit;
		splitterFilters[0].setParameters(bankParams);
		splitterFilters[1].setParameters(bankParams);

		// Mid Split
		if (parameters.midSplit <= parameters.lowSplit)
			parameters.midSplit = parameters.lowSplit;
		else if (parameters.midSplit >= parameters.highSplit)
			parameters.midSplit = parameters.highSplit;

		bankParams.splitFrequency = parameters.midSplit;
		splitterFilters[2].setParameters(bankParams);
		splitterFilters[3].setParameters(bankParams);

		// High Split
		if (parameters.highSplit <= parameters.midSplit)
			parameters.highSplit = parameters.midSplit;

		bankParams.splitFrequency = parameters.highSplit;
		splitterFilters[4].setParameters(bankParams);
		splitterFilters[5].setParameters(bankParams);

	}


private:
	FourWayBandSplitterParameters parameters; ///< object parameters

	LRFilterBank splitterFilters[6];

	double lpfVolume_cooked;
	double lowbandVolume_cooked;
	double highbandVolume_cooked;
	double hpfVolume_cooked;
	double dryVolume_cooked;

	double lpfK = 1.0;
	double lowbandK = 1.0;
	double highbandK = 1.0;
	double hpfK = 1.0;

	double boostCooked = 0.0;

	// --- local variables used by this object
	double sampleRate = 0.0;	///< sample rate

};

#endif
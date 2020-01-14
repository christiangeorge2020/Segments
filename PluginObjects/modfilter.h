#pragma once

#ifndef __ModFilter__
#define __ModFilter__

#include "fxobjects.h"
#include "superlfo.h"

/**
\struct ModFilterParameters
\ingroup FX-Objects
\brief
Custom parameter structure for the ModFilter object.

\author <Your Name> <http://www.yourwebsite.com>
\remark <Put any remarks or notes here>
\version Revision : 1.0
\date Date : 2019 / 01 / 31
*/

enum class FilterSelect {kLPF,kBPF,kHPF};

struct ModFilterParameters
{
	ModFilterParameters() {}

	/** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
	ModFilterParameters& operator=(const ModFilterParameters& params)	// need this override for collections to work
	{
		// --- it is possible to try to make the object equal to itself
		//     e.g. thisObject = thisObject; so this code catches that
		//     trivial case and just returns this object
		if (this == &params)
			return *this;

		// --- copy from params (argument) INTO our variables
		fcModFilter = params.fcModFilter;
		qModFilter = params.qModFilter;
		threshold = params.threshold;
		attackTime = params.attackTime;
		releaseTime = params.releaseTime;
		sensitivity = params.sensitivity;
		enableModFilter = params.enableModFilter;
		filterSelection = params.filterSelection;

		dryVolume = params.dryVolume;
		filterBoost = params.filterBoost;

		// LFO Parameters
		enableLFO = params.enableLFO;
		lfoRate = params.lfoRate;
		lfoDepth = params.lfoDepth;

		invertModulation = params.invertModulation;
		coupleQ = params.coupleQ;

		enableDouble = params.enableDouble;
		enableQMod = params.enableQMod;

		enableLFOThresh = params.enableLFOThresh;


		// --- MUST be last
		return *this;
	}

	// --- individual parameters
	double fcModFilter = 0.0;
	double qModFilter = 0.0;
	double threshold = 0.0;
	double attackTime = 10.0;
	double releaseTime = 10.0;
	double sensitivity = 1.0;

	bool enableModFilter;
	FilterSelect filterSelection = FilterSelect::kLPF;

	double dryVolume = 0.0;
	double filterBoost = 0.0;

	bool enableLFO = false;
	double lfoRate = 0.0;
	double lfoDepth = 0.0;

	bool invertModulation = false;
	bool coupleQ = false;
	
	bool enableDouble = false;
	bool enableQMod = false;
	bool enableLFOThresh = false;
};


/**
\class ModFilter
\ingroup FX-Objects
\brief
The ModFilter object implements ....

Audio I/O:
- Processes mono input to mono output.
- *** Optionally, process frame *** Modify this according to your object functionality

Control I/F:
- Use ModFilterParameters structure to get/set object params.

\author <Your Name> <http://www.yourwebsite.com>
\remark <Put any remarks or notes here>
\version Revision : 1.0
\date Date : 2019 / 01 / 31
*/
class ModFilter : public IAudioSignalProcessor
{
public:
	ModFilter(void) {}	/* C-TOR */
	~ModFilter(void) {}	/* D-TOR */

public:
	/** reset members to initialized state */
	virtual bool reset(double _sampleRate)
	{
		// --- store the sample rate
		sampleRate = (_sampleRate);

		modLFO.reset(_sampleRate);

		SuperLFOParameters lfoParams = modLFO.getParameters();
		lfoParams.waveform = LFOWaveform::kRSH;
		modLFO.setParameters(lfoParams);


		modFilters[0].reset(_sampleRate);
		modFilters[1].reset(_sampleRate);
		modFilters[2].reset(_sampleRate);
		modFilters[3].reset(_sampleRate);

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
		double yn = xn;

		// --- done
		return yn;
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


		// --- check for filter enable
		if (parameters.enableModFilter)
		{
			// --- check enable LFO
			if (parameters.enableLFO)
			{
				// --- set lfo parameters
				EnvelopeFollowerParameters filterParams = modFilters[0].getParameters();
				SuperLFOParameters lfoParams = modLFO.getParameters();

				lfoParams.frequency_Hz = parameters.lfoRate;
				lfoParams.outputAmplitude = (parameters.lfoDepth / 100) * 0.5;

				modLFO.setParameters(lfoParams);


				// --- generate output
				SignalModulatorOutput lfoOutput = modLFO.renderModulatorOutput();


				// --- determine modulation destination
				double lfoModifier = doUnipolarModulationFromMin(lfoOutput.unipolarOutputFromMin, 20, 
														10000.0); //- parameters.fcModFilter);

				filterParams.lfoModifier = lfoModifier;


				// --- fc modulation without threshold
				if (!parameters.enableLFOThresh)
				{
					filterParams.threshold_dB = 0.0;
					filterParams.fc = parameters.fcModFilter + lfoModifier;
				}
				

				// --- q modulation
				if (parameters.enableQMod)
					filterParams.Q = parameters.qModFilter + (lfoModifier / 600);

				
				modFilters[0].setParameters(filterParams);
				modFilters[1].setParameters(filterParams);
			}


			// --- generate envelope follower output
			ynL = modFilters[0].processAudioSample(xnL);
			ynR = modFilters[1].processAudioSample(xnR);


			// --- check for doubled filter
			if (parameters.enableDouble)
			{
				filterDouble_L = modFilters[2].processAudioSample(xnL);
				filterDouble_R = modFilters[3].processAudioSample(xnR);
			}


			// --- filter boost for ALL filters, not just BPF
			if (parameters.filterSelection == FilterSelect::kBPF ||
				parameters.filterSelection == FilterSelect::kHPF)
			{
				ynL *= filterBoost_cooked;
				ynR *= filterBoost_cooked;

				/*filterDouble_L *= filterBoost_cooked;
				filterDouble_R *= filterBoost_cooked;*/
			}
			else if (parameters.filterSelection == FilterSelect::kLPF)
			{
				ynL *= (filterBoost_cooked / 2);
				ynR *= (filterBoost_cooked / 2);

				/*filterDouble_L *= (filterBoost_cooked / 2);
				filterDouble_R *= (filterBoost_cooked / 2);*/
			}
			
		}

		// --- check channel selection
		if (inputChannels == 1 && outputChannels == 1)
		{
			outputFrame[0] = ynL + xnL * dryVolume_cooked + filterDouble_L;

			return true;
		}

		else if (inputChannels == 1 && outputChannels == 2)
		{
			outputFrame[0] = ynL + xnL * dryVolume_cooked + filterDouble_L;
			outputFrame[1] = ynL + xnL * dryVolume_cooked + filterDouble_L;

			return true;
		}

		else if (inputChannels == 2 && outputChannels == 2)
		{
			outputFrame[0] = ynL + xnL * dryVolume_cooked + filterDouble_L;
			outputFrame[1] = ynR + xnR * dryVolume_cooked + filterDouble_R;

			return true;
		}

	}


	/** get parameters: note use of custom structure for passing param data */
	/**
	\return ModFilterParameters custom data structure
	*/
	ModFilterParameters getParameters()
	{
		return parameters;
	}

	/** set parameters: note use of custom structure for passing param data */
	/**
	\param ModFilterParameters custom data structure
	*/
	void setParameters(const ModFilterParameters& params)
	{
		// --- copy them; note you may choose to ignore certain items
		//     and copy the variables one at a time, or you may test
		//     to see if cook-able variables have changed; if not, then
		//     do not re-cook them as it just wastes CPU
		parameters = params;

		// --- set dry volume range
		if (parameters.dryVolume <= -15.0)
		{
			dryVolume_cooked = 0.0;
		}
		else
		{
			dryVolume_cooked = pow(10, parameters.dryVolume / 20);
		}
		
		parameters.filterBoost = params.filterBoost;
		filterBoost_cooked = pow(10, parameters.filterBoost / 20);
		
		

		EnvelopeFollowerParameters filterParams = modFilters[0].getParameters();
		// --- envelope follower parameters
		filterParams.Q = parameters.qModFilter;
		filterParams.threshold_dB = parameters.threshold;
		filterParams.attackTime_mSec = parameters.attackTime;
		filterParams.releaseTime_mSec = parameters.releaseTime;
		filterParams.sensitivity = parameters.sensitivity;
		filterParams.fc = parameters.fcModFilter;

		filterParams.enableLFOThreshold = parameters.enableLFOThresh;


		// --- coupled q switch
		if (parameters.coupleQ)
		{
			filterParams.Q = parameters.fcModFilter / 200.0;
		}


		// --- check filtertype
		if (parameters.filterSelection == FilterSelect::kLPF)
		{
			filterParams.filterType = filterAlgorithm::kMMALPF2;
		}
		else if (parameters.filterSelection == FilterSelect::kBPF)
		{
			filterParams.filterType = filterAlgorithm::kBPF2;
		}
		else if (parameters.filterSelection == FilterSelect::kHPF)
		{
			filterParams.filterType = filterAlgorithm::kHPF2;
		}

		filterParams.invertModulation = params.invertModulation;

		modFilters[0].setParameters(filterParams);
		modFilters[1].setParameters(filterParams);


		// --- doubled filter
		if (parameters.enableDouble)
		{
			doubledFc = parameters.fcModFilter * 10;
			if (doubledFc >= kMaxFilterFrequency)
				doubledFc = kMaxFilterFrequency;

			filterParams.fc = doubledFc;
			modFilters[2].setParameters(filterParams);
			modFilters[3].setParameters(filterParams);
		}
	}

private:
	ModFilterParameters parameters; ///< object parameters

	EnvelopeFollower modFilters[4];
	SuperLFO modLFO;

	// --- local variables used by this object
	double sampleRate = 0.0;	///< sample rate
	double dryVolume_cooked;
	double filterBoost_cooked;
	double lfoModifier = 0.0;

	double filterDouble_L = 0.0;
	double filterDouble_R = 0.0;
	double doubledFc = 0.0;


};

#endif
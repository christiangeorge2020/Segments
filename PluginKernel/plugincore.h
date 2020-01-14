// -----------------------------------------------------------------------------
//    ASPiK Plugin Kernel File:  plugincore.h
//
/**
    \file   plugincore.h
    \author Will Pirkle
    \date   17-September-2018
    \brief  base class interface file for ASPiK plugincore object
    		- http://www.aspikplugins.com
    		- http://www.willpirkle.com
*/
// -----------------------------------------------------------------------------
#ifndef __pluginCore_h__
#define __pluginCore_h__

#include "pluginbase.h"
#include "fourwaybandsplitter.h"
#include "modfilter.h"

// **--0x7F1F--**

// --- Plugin Variables controlID Enumeration 

enum controlID {
	splitSelect = 1,
	splitLow = 2,
	splitHigh = 22,
	midSplit = 12,
	lpfVolume = 4,
	lowbandVolume = 14,
	highbandVolume = 24,
	hpfVolume = 34,
	lpfSaturation = 3,
	lowbandSaturation = 13,
	highbandSaturation = 23,
	hpfSaturation = 33,
	dryVolume = 44,
	fcModFilter = 6,
	qModFilter = 16,
	threshold_db = 26,
	attack_ms = 36,
	release_ms = 46,
	sensitivity = 56,
	enableSplit = 11,
	enableModFilter = 66,
	typeModFilter = 7,
	filterBoost = 17,
	dryFilter = 27,
	enableLFO = 37,
	lfoRate = 47,
	lfoDepth = 57,
	invertFilterMod = 9,
	coupleQ = 19,
	enableDouble = 29,
	splitterBoost = 54,
	signalFlow = 51,
	m_uMySwitchVariable = 48,
	enableQMod = 39,
	enableThreshMod = 49
};

	// **--0x0F1F--**

/**
\class PluginCore
\ingroup ASPiK-Core
\brief
The PluginCore object is the default PluginBase derived object for ASPiK projects.
Note that you are fre to change the name of this object (as long as you change it in the compiler settings, etc...)


PluginCore Operations:
- overrides the main processing functions from the base class
- performs reset operation on sub-modules
- processes audio
- processes messages for custom views
- performs pre and post processing functions on parameters and audio (if needed)

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class PluginCore : public PluginBase
{
public:
    PluginCore();

	/** Destructor: empty in default version */
    virtual ~PluginCore(){}

	// --- PluginBase Overrides ---
	//
	/** this is the creation function for all plugin parameters */
	bool initPluginParameters();

	/** called when plugin is loaded, a new audio file is playing or sample rate changes */
	virtual bool reset(ResetInfo& resetInfo);

	/** one-time post creation init function; pluginInfo contains path to this plugin */
	virtual bool initialize(PluginInfo& _pluginInfo);

	// --- preProcess: sync GUI parameters here; override if you don't want to use automatic variable-binding
	virtual bool preProcessAudioBuffers(ProcessBufferInfo& processInfo);

	/** process frames of data */
	virtual bool processAudioFrame(ProcessFrameInfo& processFrameInfo);

	// --- uncomment and override this for buffer processing; see base class implementation for
	//     help on breaking up buffers and getting info from processBufferInfo
	//virtual bool processAudioBuffers(ProcessBufferInfo& processBufferInfo);

	/** preProcess: do any post-buffer processing required; default operation is to send metering data to GUI  */
	virtual bool postProcessAudioBuffers(ProcessBufferInfo& processInfo);

	/** called by host plugin at top of buffer proccess; this alters parameters prior to variable binding operation  */
	virtual bool updatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo);

	/** called by host plugin at top of buffer proccess; this alters parameters prior to variable binding operation  */
	virtual bool updatePluginParameterNormalized(int32_t controlID, double normalizedValue, ParameterUpdateInfo& paramInfo);

	/** this can be called: 1) after bound variable has been updated or 2) after smoothing occurs  */
	virtual bool postUpdatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo);

	/** this is ony called when the user makes a GUI control change */
	virtual bool guiParameterChanged(int32_t controlID, double actualValue);

	/** processMessage: messaging system; currently used for custom/special GUI operations */
	virtual bool processMessage(MessageInfo& messageInfo);

	/** processMIDIEvent: MIDI event processing */
	virtual bool processMIDIEvent(midiEvent& event);

	/** specialized joystick servicing (currently not used) */
	virtual bool setVectorJoystickParameters(const VectorJoystickData& vectorJoysickData);

	/** create the presets */
	bool initPluginPresets();

	// --- BEGIN USER VARIABLES AND FUNCTIONS -------------------------------------- //
	//	   Add your variables and methods here
	FourWayBandSplitter fourwaybandSplitter;
	ModFilter modFilter;
	
	
	void updateParameters();

	// --- END USER VARIABLES AND FUNCTIONS -------------------------------------- //

private:
	//  **--0x07FD--**

	// --- Continuous Plugin Variables 
	double splitLow = 0.0;
	double splitHigh = 0.0;
	double midSplit = 0.0;
	double lpfVolume = 0.0;
	double lowbandVolume = 0.0;
	double highbandVolume = 0.0;
	double hpfVolume = 0.0;
	double lpfSaturation = 0.0;
	double lowbandSaturation = 0.0;
	double highbandSaturation = 0.0;
	double hpfSaturation = 0.0;
	double dryVolume = 0.0;
	double fcModFilter = 0.0;
	double qModFilter = 0.0;
	double threshold_db = 0.0;
	double attack_ms = 0.0;
	double release_ms = 0.0;
	double sensitivity = 0.0;
	double filterBoost = 0.0;
	double dryFilter = 0.0;
	double lfoRate = 0.0;
	double lfoDepth = 0.0;
	double splitterBoost = 0.0;

	// --- Discrete Plugin Variables 
	int splitSelect = 0;
	enum class splitSelectEnum { Splitter,LPF,Low_Band,High_Band,HPF };	// to compare: if(compareEnumToInt(splitSelectEnum::Splitter, splitSelect)) etc... 

	int enableSplit = 0;
	enum class enableSplitEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(enableSplitEnum::SWITCH_OFF, enableSplit)) etc... 

	int enableModFilter = 0;
	enum class enableModFilterEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(enableModFilterEnum::SWITCH_OFF, enableModFilter)) etc... 

	int typeModFilter = 0;
	enum class typeModFilterEnum { LPF,Band,HPF };	// to compare: if(compareEnumToInt(typeModFilterEnum::LPF, typeModFilter)) etc... 

	int enableLFO = 0;
	enum class enableLFOEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(enableLFOEnum::SWITCH_OFF, enableLFO)) etc... 

	int invertFilterMod = 0;
	enum class invertFilterModEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(invertFilterModEnum::SWITCH_OFF, invertFilterMod)) etc... 

	int coupleQ = 0;
	enum class coupleQEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(coupleQEnum::SWITCH_OFF, coupleQ)) etc... 

	int enableDouble = 0;
	enum class enableDoubleEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(enableDoubleEnum::SWITCH_OFF, enableDouble)) etc... 

	int signalFlow = 0;
	enum class signalFlowEnum { Splitter_Filter,Filter_Splitter,Parallel };	// to compare: if(compareEnumToInt(signalFlowEnum::Splitter_Filter, signalFlow)) etc... 

	int m_uMySwitchVariable = 0;
	enum class m_uMySwitchVariableEnum { Fc,Q,Threshold };	// to compare: if(compareEnumToInt(m_uMySwitchVariableEnum::Fc, m_uMySwitchVariable)) etc... 

	int enableQMod = 0;
	enum class enableQModEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(enableQModEnum::SWITCH_OFF, enableQMod)) etc... 

	int enableThreshMod = 0;
	enum class enableThreshModEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(enableThreshModEnum::SWITCH_OFF, enableThreshMod)) etc... 

	// **--0x1A7F--**
    // --- end member variables

public:
    /** static description: bundle folder name

	\return bundle folder name as a const char*
	*/
    static const char* getPluginBundleName();

    /** static description: name

	\return name as a const char*
	*/
    static const char* getPluginName();

	/** static description: short name

	\return short name as a const char*
	*/
	static const char* getShortPluginName();

	/** static description: vendor name

	\return vendor name as a const char*
	*/
	static const char* getVendorName();

	/** static description: URL

	\return URL as a const char*
	*/
	static const char* getVendorURL();

	/** static description: email

	\return email address as a const char*
	*/
	static const char* getVendorEmail();

	/** static description: Cocoa View Factory Name

	\return Cocoa View Factory Name as a const char*
	*/
	static const char* getAUCocoaViewFactoryName();

	/** static description: plugin type

	\return type (FX or Synth)
	*/
	static pluginType getPluginType();

	/** static description: VST3 GUID

	\return VST3 GUID as a const char*
	*/
	static const char* getVSTFUID();

	/** static description: 4-char code

	\return 4-char code as int
	*/
	static int32_t getFourCharCode();

	/** initalizer */
	bool initPluginDescriptors();

};




#endif /* defined(__pluginCore_h__) */

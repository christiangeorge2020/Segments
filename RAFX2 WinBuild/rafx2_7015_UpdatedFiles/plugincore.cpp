// -----------------------------------------------------------------------------
//    ASPiK Plugin Kernel File:  plugincore.cpp
//
/**
    \file   plugincore.cpp
    \author Will Pirkle
    \date   17-September-2018
    \brief  Implementation file for PluginCore object
    		- http://www.aspikplugins.com
    		- http://www.willpirkle.com
*/
// -----------------------------------------------------------------------------
#include "plugincore.h"
#include "plugindescription.h"

/**
\brief PluginCore constructor is launching pad for object initialization

Operations:
- initialize the plugin description (strings, codes, numbers, see initPluginDescriptors())
- setup the plugin's audio I/O channel support
- create the PluginParameter objects that represent the plugin parameters (see FX book if needed)
- create the presets
*/
PluginCore::PluginCore()
{
    // --- describe the plugin; call the helper to init the static parts you setup in plugindescription.h
    initPluginDescriptors();

    // --- default I/O combinations
	// --- for FX plugins
	if (getPluginType() == kFXPlugin)
	{
		addSupportedIOCombination({ kCFMono, kCFMono });
		addSupportedIOCombination({ kCFMono, kCFStereo });
		addSupportedIOCombination({ kCFStereo, kCFStereo });
	}
	else // --- synth plugins have no input, only output
	{
		addSupportedIOCombination({ kCFNone, kCFMono });
		addSupportedIOCombination({ kCFNone, kCFStereo });
	}

	// --- for sidechaining, we support mono and stereo inputs; auxOutputs reserved for future use
	addSupportedAuxIOCombination({ kCFMono, kCFNone });
	addSupportedAuxIOCombination({ kCFStereo, kCFNone });

	// --- create the parameters
    initPluginParameters();

    // --- create the presets
    initPluginPresets();
}

/**
\brief create all of your plugin parameters here

\return true if parameters were created, false if they already existed
*/
bool PluginCore::initPluginParameters()
{
	if (pluginParameterMap.size() > 0)
		return false;

    // --- Add your plugin parameter instantiation code bewtween these hex codes
	// **--0xDEA7--**


	// --- Declaration of Plugin Parameter Objects 
	PluginParameter* piParam = nullptr;

	// --- discrete control: Split Select
	piParam = new PluginParameter(controlID::splitSelect, "Split Select", "Splitter,LPF,Low Band,High Band,HPF", "Splitter");
	piParam->setBoundVariable(&splitSelect, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: Low_Split
	piParam = new PluginParameter(controlID::splitLow, "Low_Split", "Hertz", controlVariableType::kDouble, 50.000000, 2000.000000, 400.000000, taper::kVoltOctaveTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&splitLow, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: High Split
	piParam = new PluginParameter(controlID::splitHigh, "High Split", "Hertz", controlVariableType::kDouble, 5000.000000, 20000.000000, 10000.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&splitHigh, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Mid Split
	piParam = new PluginParameter(controlID::midSplit, "Mid Split", "Hertz", controlVariableType::kDouble, 300.000000, 12000.000000, 1000.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&midSplit, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: LPF Volume
	piParam = new PluginParameter(controlID::lpfVolume, "LPF Volume", "dB", controlVariableType::kDouble, -60.000000, 6.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lpfVolume, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Low Band Volume
	piParam = new PluginParameter(controlID::lowbandVolume, "Low Band Volume", "dB", controlVariableType::kDouble, -60.000000, 6.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lowbandVolume, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: High Band Volume
	piParam = new PluginParameter(controlID::highbandVolume, "High Band Volume", "dB", controlVariableType::kDouble, -60.000000, 6.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&highbandVolume, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: HPF Volume
	piParam = new PluginParameter(controlID::hpfVolume, "HPF Volume", "dB", controlVariableType::kDouble, -60.000000, 6.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&hpfVolume, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: LPF Saturation
	piParam = new PluginParameter(controlID::lpfSaturation, "LPF Saturation", "Units", controlVariableType::kDouble, 1.000000, 10.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lpfSaturation, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Low Band Saturation
	piParam = new PluginParameter(controlID::lowbandSaturation, "Low Band Saturation", "Units", controlVariableType::kDouble, 1.000000, 10.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lowbandSaturation, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: High Band Saturation
	piParam = new PluginParameter(controlID::highbandSaturation, "High Band Saturation", "Units", controlVariableType::kDouble, 1.000000, 10.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&highbandSaturation, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: HPF Saturation
	piParam = new PluginParameter(controlID::hpfSaturation, "HPF Saturation", "Units", controlVariableType::kDouble, 1.000000, 10.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&hpfSaturation, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Dry Volume
	piParam = new PluginParameter(controlID::dryVolume, "Dry Volume", "dB", controlVariableType::kDouble, -15.000000, 0.000000, -15.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&dryVolume, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: ModFilter Cutoff
	piParam = new PluginParameter(controlID::fcModFilter, "ModFilter Cutoff", "Hertz", controlVariableType::kDouble, 80.000000, 5000.000000, 800.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&fcModFilter, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: ModFilter Q
	piParam = new PluginParameter(controlID::qModFilter, "ModFilter Q", "Units", controlVariableType::kDouble, 0.500000, 10.000000, 2.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&qModFilter, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Threshold
	piParam = new PluginParameter(controlID::threshold_db, "Threshold", "dB", controlVariableType::kDouble, -30.000000, 0.000000, -15.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&threshold_db, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Attack
	piParam = new PluginParameter(controlID::attack_ms, "Attack", "ms", controlVariableType::kDouble, 1.000000, 10.000000, 8.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&attack_ms, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Release
	piParam = new PluginParameter(controlID::release_ms, "Release", "ms", controlVariableType::kDouble, 20.000000, 400.000000, 80.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&release_ms, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Sensitivity
	piParam = new PluginParameter(controlID::sensitivity, "Sensitivity", "Units", controlVariableType::kDouble, 0.250000, 5.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&sensitivity, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: Enable Splitter
	piParam = new PluginParameter(controlID::enableSplit, "Enable Splitter", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&enableSplit, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Enable ModFilter
	piParam = new PluginParameter(controlID::enableModFilter, "Enable ModFilter", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&enableModFilter, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: ModFilter Type
	piParam = new PluginParameter(controlID::typeModFilter, "ModFilter Type", "LPF,Band,HPF", "LPF");
	piParam->setBoundVariable(&typeModFilter, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: Boost
	piParam = new PluginParameter(controlID::filterBoost, "Boost", "dB", controlVariableType::kDouble, -5.000000, 10.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&filterBoost, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Dry Filter
	piParam = new PluginParameter(controlID::dryFilter, "Dry Filter", "dB", controlVariableType::kDouble, -15.000000, 0.000000, -15.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&dryFilter, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: Enable LFO
	piParam = new PluginParameter(controlID::enableLFO, "Enable LFO", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&enableLFO, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: LFO Rate
	piParam = new PluginParameter(controlID::lfoRate, "LFO Rate", "Hz", controlVariableType::kDouble, 2.000000, 15.000000, 8.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfoRate, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: LFO Depth
	piParam = new PluginParameter(controlID::lfoDepth, "LFO Depth", "Units", controlVariableType::kDouble, 0.000000, 100.000000, 50.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfoDepth, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: Up/Down
	piParam = new PluginParameter(controlID::invertFilterMod, "Up/Down", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&invertFilterMod, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Coupled Q
	piParam = new PluginParameter(controlID::coupleQ, "Coupled Q", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&coupleQ, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Enable Double
	piParam = new PluginParameter(controlID::enableDouble, "Enable Double", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&enableDouble, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: Splitter Boost
	piParam = new PluginParameter(controlID::splitterBoost, "Splitter Boost", "dB", controlVariableType::kDouble, -5.000000, 10.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&splitterBoost, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: Signal Flow
	piParam = new PluginParameter(controlID::signalFlow, "Signal Flow", "Splitter Filter,Filter Splitter,Parallel", "Splitter Filter");
	piParam->setBoundVariable(&signalFlow, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: lfoDestination
	piParam = new PluginParameter(controlID::m_uMySwitchVariable, "lfoDestination", "Fc,Q,Threshold", "Fc");
	piParam->setBoundVariable(&m_uMySwitchVariable, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: SampleQ
	piParam = new PluginParameter(controlID::enableQMod, "SampleQ", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&enableQMod, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: SampleThreshold
	piParam = new PluginParameter(controlID::enableThreshMod, "SampleThreshold", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&enableThreshMod, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- Aux Attributes
	AuxParameterAttribute auxAttribute;

	// --- RAFX GUI attributes
	// --- controlID::splitSelect
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::splitSelect, auxAttribute);

	// --- controlID::splitLow
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::splitLow, auxAttribute);

	// --- controlID::splitHigh
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::splitHigh, auxAttribute);

	// --- controlID::midSplit
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::midSplit, auxAttribute);

	// --- controlID::lpfVolume
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lpfVolume, auxAttribute);

	// --- controlID::lowbandVolume
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lowbandVolume, auxAttribute);

	// --- controlID::highbandVolume
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::highbandVolume, auxAttribute);

	// --- controlID::hpfVolume
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::hpfVolume, auxAttribute);

	// --- controlID::lpfSaturation
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lpfSaturation, auxAttribute);

	// --- controlID::lowbandSaturation
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lowbandSaturation, auxAttribute);

	// --- controlID::highbandSaturation
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::highbandSaturation, auxAttribute);

	// --- controlID::hpfSaturation
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::hpfSaturation, auxAttribute);

	// --- controlID::dryVolume
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::dryVolume, auxAttribute);

	// --- controlID::fcModFilter
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::fcModFilter, auxAttribute);

	// --- controlID::qModFilter
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::qModFilter, auxAttribute);

	// --- controlID::threshold_db
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::threshold_db, auxAttribute);

	// --- controlID::attack_ms
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::attack_ms, auxAttribute);

	// --- controlID::release_ms
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::release_ms, auxAttribute);

	// --- controlID::sensitivity
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::sensitivity, auxAttribute);

	// --- controlID::enableSplit
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::enableSplit, auxAttribute);

	// --- controlID::enableModFilter
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::enableModFilter, auxAttribute);

	// --- controlID::typeModFilter
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::typeModFilter, auxAttribute);

	// --- controlID::filterBoost
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::filterBoost, auxAttribute);

	// --- controlID::dryFilter
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::dryFilter, auxAttribute);

	// --- controlID::enableLFO
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::enableLFO, auxAttribute);

	// --- controlID::lfoRate
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfoRate, auxAttribute);

	// --- controlID::lfoDepth
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfoDepth, auxAttribute);

	// --- controlID::invertFilterMod
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::invertFilterMod, auxAttribute);

	// --- controlID::coupleQ
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::coupleQ, auxAttribute);

	// --- controlID::enableDouble
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::enableDouble, auxAttribute);

	// --- controlID::splitterBoost
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::splitterBoost, auxAttribute);

	// --- controlID::signalFlow
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(268435456);
	setParamAuxAttribute(controlID::signalFlow, auxAttribute);

	// --- controlID::m_uMySwitchVariable
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::m_uMySwitchVariable, auxAttribute);

	// --- controlID::enableQMod
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::enableQMod, auxAttribute);

	// --- controlID::enableThreshMod
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::enableThreshMod, auxAttribute);


	// **--0xEDA5--**
   
    // --- BONUS Parameter
    // --- SCALE_GUI_SIZE
    PluginParameter* piParamBonus = new PluginParameter(SCALE_GUI_SIZE, "Scale GUI", "tiny,small,medium,normal,large,giant", "normal");
    addPluginParameter(piParamBonus);

	// --- create the super fast access array
	initPluginParameterArray();

    return true;
}

/**
\brief initialize object for a new run of audio; called just before audio streams

Operation:
- store sample rate and bit depth on audioProcDescriptor - this information is globally available to all core functions
- reset your member objects here

\param resetInfo structure of information about current audio format

\return true if operation succeeds, false otherwise
*/
bool PluginCore::reset(ResetInfo& resetInfo)
{
    // --- save for audio processing
    audioProcDescriptor.sampleRate = resetInfo.sampleRate;
    audioProcDescriptor.bitDepth = resetInfo.bitDepth;

	fourwaybandSplitter.reset(resetInfo.sampleRate);
	modFilter.reset(resetInfo.sampleRate);

    // --- other reset inits
    return PluginBase::reset(resetInfo);
}

/**
\brief one-time initialize function called after object creation and before the first reset( ) call

Operation:
- saves structure for the plugin to use; you can also load WAV files or state information here
*/
bool PluginCore::initialize(PluginInfo& pluginInfo)
{
	// --- add one-time init stuff here

	return true;
}

/**
\brief do anything needed prior to arrival of audio buffers

Operation:
- syncInBoundVariables when preProcessAudioBuffers is called, it is *guaranteed* that all GUI control change information
  has been applied to plugin parameters; this binds parameter changes to your underlying variables
- NOTE: postUpdatePluginParameter( ) will be called for all bound variables that are acutally updated; if you need to process
  them individually, do so in that function
- use this function to bulk-transfer the bound variable data into your plugin's member object variables

\param processInfo structure of information about *buffer* processing

\return true if operation succeeds, false otherwise
*/
bool PluginCore::preProcessAudioBuffers(ProcessBufferInfo& processInfo)
{
    // --- sync internal variables to GUI parameters; you can also do this manually if you don't
    //     want to use the auto-variable-binding
    syncInBoundVariables();

    return true;
}

/**
\brief frame-processing method

Operation:
- decode the plugin type - for synth plugins, fill in the rendering code; for FX plugins, delete the if(synth) portion and add your processing code
- note that MIDI events are fired for each sample interval so that MIDI is tightly sunk with audio
- doSampleAccurateParameterUpdates will perform per-sample interval smoothing

\param processFrameInfo structure of information about *frame* processing

\return true if operation succeeds, false otherwise
*/
bool PluginCore::processAudioFrame(ProcessFrameInfo& processFrameInfo)
{
    // --- fire any MIDI events for this sample interval
    processFrameInfo.midiEventQueue->fireMidiEvents(processFrameInfo.currentFrame);

	// --- do per-frame updates; VST automation and parameter smoothing
	doSampleAccurateParameterUpdates();

    // --- decode the channelIOConfiguration and process accordingly
	updateParameters();

	bool splitterProcessed = false;
	bool filterProcessed = false;
	bool processed = false;

	// --- check signal flow list

	// --- splitter to filter
	if (compareIntToEnum(signalFlow, signalFlowEnum::Splitter_Filter))
	{
		bool splitterProcessed = fourwaybandSplitter.processAudioFrame(
			processFrameInfo.audioInputFrame,
			processFrameInfo.audioOutputFrame,
			processFrameInfo.numAudioInChannels,
			processFrameInfo.numAudioOutChannels);


		float* splitterOutput = processFrameInfo.audioOutputFrame;

		bool filterProcessed = modFilter.processAudioFrame(
			splitterOutput,
			processFrameInfo.audioOutputFrame,
			processFrameInfo.numAudioInChannels,
			processFrameInfo.numAudioOutChannels);
	}

	// --- parallel
	else if (compareIntToEnum(signalFlow, signalFlowEnum::Parallel))
	{
		float filterOutput[2]; 
		float splitterOutput[2];

		bool filterProcessed = modFilter.processAudioFrame(
			processFrameInfo.audioInputFrame,
			processFrameInfo.audioOutputFrame,
			processFrameInfo.numAudioInChannels,
			processFrameInfo.numAudioOutChannels);

		filterOutput[0] = processFrameInfo.audioOutputFrame[0];
		filterOutput[1] = processFrameInfo.audioOutputFrame[1];

		bool splitterProcessed = fourwaybandSplitter.processAudioFrame(
			processFrameInfo.audioInputFrame,
			processFrameInfo.audioOutputFrame,
			processFrameInfo.numAudioInChannels,
			processFrameInfo.numAudioOutChannels);

		splitterOutput[0] = processFrameInfo.audioOutputFrame[0];
		splitterOutput[1] = processFrameInfo.audioOutputFrame[1];

		processFrameInfo.audioOutputFrame[0] = 0.5 * (filterOutput[0] + splitterOutput[0]);
		processFrameInfo.audioOutputFrame[1] = 0.5 * (filterOutput[1] + splitterOutput[1]);


	}

	// --- filter to splitter
	else if (compareIntToEnum(signalFlow, signalFlowEnum::Filter_Splitter))
	{
		bool filterProcessed = modFilter.processAudioFrame(
			processFrameInfo.audioInputFrame,
			processFrameInfo.audioOutputFrame,
			processFrameInfo.numAudioInChannels,
			processFrameInfo.numAudioOutChannels);

		float* filterOutput = processFrameInfo.audioOutputFrame;


		bool splitterProcessed = fourwaybandSplitter.processAudioFrame(
			filterOutput,
			processFrameInfo.audioOutputFrame,
			processFrameInfo.numAudioInChannels,
			processFrameInfo.numAudioOutChannels);

		float* splitterOutput = processFrameInfo.audioOutputFrame;
		
	}



	// --- check if everything processed
	if (splitterProcessed && filterProcessed)
	{
		FourWayBandSplitterParameters splitterParams = fourwaybandSplitter.getParameters();
		ModFilterParameters filterParams = modFilter.getParameters();
		processed = true;
	}

	return processed;
}

void PluginCore::updateParameters() 
{
	// --- Band Splitter
	FourWayBandSplitterParameters splitterParams = fourwaybandSplitter.getParameters();

	// --- split frequency
	splitterParams.highSplit = splitHigh;
	splitterParams.lowSplit = splitLow;
	splitterParams.midSplit = midSplit;

	// --- view control - only for view, not on gui
	splitterParams.splitView = convertIntToEnum(splitSelect, splitSelection);
	
	// --- volume
	splitterParams.lpfVolume = lpfVolume;
	splitterParams.lowbandVolume = lowbandVolume;
	splitterParams.highbandVolume = highbandVolume;
	splitterParams.hpfVolume = hpfVolume;
	splitterParams.dryVolume = dryVolume;

	// --- saturation
	splitterParams.lpfSaturation = lpfSaturation;
	splitterParams.lowbandSaturation = lowbandSaturation;
	splitterParams.highbandSaturation = highbandSaturation;
	splitterParams.hpfSaturation = hpfSaturation;

	splitterParams.enableSplitter = enableSplit;

	splitterParams.splitterBoost = splitterBoost;

	fourwaybandSplitter.setParameters(splitterParams);

	// --- ModFilter
	ModFilterParameters filterParams = modFilter.getParameters();

	filterParams.fcModFilter = fcModFilter;
	filterParams.qModFilter = qModFilter;
	filterParams.threshold = threshold_db;
	filterParams.attackTime = attack_ms;
	filterParams.releaseTime = release_ms;
	filterParams.sensitivity = sensitivity;

	filterParams.enableModFilter = enableModFilter;

	filterParams.dryVolume = dryVolume;
	filterParams.filterBoost = filterBoost;

	filterParams.filterSelection = convertIntToEnum(typeModFilter, FilterSelect); 

	// --- sample and hold lfo
	filterParams.enableLFO = enableLFO;
	filterParams.lfoRate = lfoRate;
	filterParams.lfoDepth = lfoDepth;
	filterParams.invertModulation = invertFilterMod;
	filterParams.coupleQ = coupleQ;
	filterParams.enableDouble = enableDouble;
	filterParams.enableQMod = enableQMod;
	filterParams.enableLFOThresh = enableThreshMod;

	modFilter.setParameters(filterParams);

}


/**
\brief do anything needed prior to arrival of audio buffers

Operation:
- updateOutBoundVariables sends metering data to the GUI meters

\param processInfo structure of information about *buffer* processing

\return true if operation succeeds, false otherwise
*/
bool PluginCore::postProcessAudioBuffers(ProcessBufferInfo& processInfo)
{
	// --- update outbound variables; currently this is meter data only, but could be extended
	//     in the future
	updateOutBoundVariables();

    return true;
}

/**
\brief update the PluginParameter's value based on GUI control, preset, or data smoothing (thread-safe)

Operation:
- update the parameter's value (with smoothing this initiates another smoothing process)
- call postUpdatePluginParameter to do any further processing

\param controlID the control ID value of the parameter being updated
\param controlValue the new control value
\param paramInfo structure of information about why this value is being udpated (e.g as a result of a preset being loaded vs. the top of a buffer process cycle)

\return true if operation succeeds, false otherwise
*/
bool PluginCore::updatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo)
{
    // --- use base class helper
    setPIParamValue(controlID, controlValue);

    // --- do any post-processing
    postUpdatePluginParameter(controlID, controlValue, paramInfo);

    return true; /// handled
}

/**
\brief update the PluginParameter's value based on *normlaized* GUI control, preset, or data smoothing (thread-safe)

Operation:
- update the parameter's value (with smoothing this initiates another smoothing process)
- call postUpdatePluginParameter to do any further processing

\param controlID the control ID value of the parameter being updated
\param normalizedValue the new control value in normalized form
\param paramInfo structure of information about why this value is being udpated (e.g as a result of a preset being loaded vs. the top of a buffer process cycle)

\return true if operation succeeds, false otherwise
*/
bool PluginCore::updatePluginParameterNormalized(int32_t controlID, double normalizedValue, ParameterUpdateInfo& paramInfo)
{
	// --- use base class helper, returns actual value
	double controlValue = setPIParamValueNormalized(controlID, normalizedValue, paramInfo.applyTaper);

	// --- do any post-processing
	postUpdatePluginParameter(controlID, controlValue, paramInfo);

	return true; /// handled
}

/**
\brief perform any operations after the plugin parameter has been updated; this is one paradigm for
	   transferring control information into vital plugin variables or member objects. If you use this
	   method you can decode the control ID and then do any cooking that is needed. NOTE: do not
	   overwrite bound variables here - this is ONLY for any extra cooking that is required to convert
	   the GUI data to meaninful coefficients or other specific modifiers.

\param controlID the control ID value of the parameter being updated
\param controlValue the new control value
\param paramInfo structure of information about why this value is being udpated (e.g as a result of a preset being loaded vs. the top of a buffer process cycle)

\return true if operation succeeds, false otherwise
*/
bool PluginCore::postUpdatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo)
{
    // --- now do any post update cooking; be careful with VST Sample Accurate automation
    //     If enabled, then make sure the cooking functions are short and efficient otherwise disable it
    //     for the Parameter involved
    /*switch(controlID)
    {
        case 0:
        {
            return true;    /// handled
        }

        default:
            return false;   /// not handled
    }*/

    return false;
}

/**
\brief has nothing to do with actual variable or updated variable (binding)

CAUTION:
- DO NOT update underlying variables here - this is only for sending GUI updates or letting you
  know that a parameter was changed; it should not change the state of your plugin.

WARNING:
- THIS IS NOT THE PREFERRED WAY TO LINK OR COMBINE CONTROLS TOGETHER. THE PROPER METHOD IS
  TO USE A CUSTOM SUB-CONTROLLER THAT IS PART OF THE GUI OBJECT AND CODE.
  SEE http://www.willpirkle.com for more information

\param controlID the control ID value of the parameter being updated
\param actualValue the new control value

\return true if operation succeeds, false otherwise
*/
bool PluginCore::guiParameterChanged(int32_t controlID, double actualValue)
{
	/*
	switch (controlID)
	{
		case controlID::<your control here>
		{

			return true; // handled
		}

		default:
			break;
	}*/

	return false; /// not handled
}

/**
\brief For Custom View and Custom Sub-Controller Operations

NOTES:
- this is for advanced users only to implement custom view and custom sub-controllers
- see the SDK for examples of use

\param messageInfo a structure containing information about the incoming message

\return true if operation succeeds, false otherwise
*/
bool PluginCore::processMessage(MessageInfo& messageInfo)
{
	// --- decode message
	switch (messageInfo.message)
	{
		// --- add customization appearance here
	case PLUGINGUI_DIDOPEN:
	{
		return false;
	}

	// --- NULL pointers so that we don't accidentally use them
	case PLUGINGUI_WILLCLOSE:
	{
		return false;
	}

	// --- update view; this will only be called if the GUI is actually open
	case PLUGINGUI_TIMERPING:
	{
		return false;
	}

	// --- register the custom view, grab the ICustomView interface
	case PLUGINGUI_REGISTER_CUSTOMVIEW:
	{

		return false;
	}

	case PLUGINGUI_REGISTER_SUBCONTROLLER:
	case PLUGINGUI_QUERY_HASUSERCUSTOM:
	case PLUGINGUI_USER_CUSTOMOPEN:
	case PLUGINGUI_USER_CUSTOMCLOSE:
	case PLUGINGUI_EXTERNAL_SET_NORMVALUE:
	case PLUGINGUI_EXTERNAL_SET_ACTUALVALUE:
	{

		return false;
	}

	default:
		break;
	}

	return false; /// not handled
}


/**
\brief process a MIDI event

NOTES:
- MIDI events are 100% sample accurate; this function will be called repeatedly for every MIDI message
- see the SDK for examples of use

\param event a structure containing the MIDI event data

\return true if operation succeeds, false otherwise
*/
bool PluginCore::processMIDIEvent(midiEvent& event)
{
	return true;
}

/**
\brief (for future use)

NOTES:
- MIDI events are 100% sample accurate; this function will be called repeatedly for every MIDI message
- see the SDK for examples of use

\param vectorJoysickData a structure containing joystick data

\return true if operation succeeds, false otherwise
*/
bool PluginCore::setVectorJoystickParameters(const VectorJoystickData& vectorJoysickData)
{
	return true;
}

/**
\brief use this method to add new presets to the list

NOTES:
- see the SDK for examples of use
- for non RackAFX users that have large paramter counts, there is a secret GUI control you
  can enable to write C++ code into text files, one per preset. See the SDK or http://www.willpirkle.com for details

\return true if operation succeeds, false otherwise
*/
bool PluginCore::initPluginPresets()
{
	// **--0xFF7A--**

	// --- Plugin Presets 
	int index = 0;
	PresetInfo* preset = nullptr;

	// --- Preset: Factory Preset
	preset = new PresetInfo(index++, "Factory Preset");
	initPresetParameters(preset->presetParameters);
	setPresetParameter(preset->presetParameters, controlID::splitSelect, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::splitLow, 400.000000);
	setPresetParameter(preset->presetParameters, controlID::splitHigh, 10000.000000);
	setPresetParameter(preset->presetParameters, controlID::midSplit, 1000.000000);
	setPresetParameter(preset->presetParameters, controlID::lpfVolume, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::lowbandVolume, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::highbandVolume, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::hpfVolume, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::lpfSaturation, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::lowbandSaturation, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::highbandSaturation, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::hpfSaturation, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::dryVolume, -15.000000);
	setPresetParameter(preset->presetParameters, controlID::fcModFilter, 800.000000);
	setPresetParameter(preset->presetParameters, controlID::qModFilter, 2.000000);
	setPresetParameter(preset->presetParameters, controlID::threshold_db, -15.000000);
	setPresetParameter(preset->presetParameters, controlID::attack_ms, 8.000000);
	setPresetParameter(preset->presetParameters, controlID::release_ms, 80.000000);
	setPresetParameter(preset->presetParameters, controlID::sensitivity, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::enableSplit, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableModFilter, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::typeModFilter, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::filterBoost, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::dryFilter, -15.000000);
	setPresetParameter(preset->presetParameters, controlID::enableLFO, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfoRate, 8.000000);
	setPresetParameter(preset->presetParameters, controlID::lfoDepth, 50.000000);
	setPresetParameter(preset->presetParameters, controlID::invertFilterMod, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::coupleQ, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableDouble, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::splitterBoost, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::signalFlow, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::m_uMySwitchVariable, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableQMod, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableThreshMod, -0.000000);
	addPreset(preset);


	// **--0xA7FF--**

    return true;
}

/**
\brief setup the plugin description strings, flags and codes; this is ordinarily done through the ASPiKreator or CMake

\return true if operation succeeds, false otherwise
*/
bool PluginCore::initPluginDescriptors()
{
    pluginDescriptor.pluginName = PluginCore::getPluginName();
    pluginDescriptor.shortPluginName = PluginCore::getShortPluginName();
    pluginDescriptor.vendorName = PluginCore::getVendorName();
    pluginDescriptor.pluginTypeCode = PluginCore::getPluginType();

	// --- describe the plugin attributes; set according to your needs
	pluginDescriptor.hasSidechain = kWantSidechain;
	pluginDescriptor.latencyInSamples = kLatencyInSamples;
	pluginDescriptor.tailTimeInMSec = kTailTimeMsec;
	pluginDescriptor.infiniteTailVST3 = kVSTInfiniteTail;

    // --- AAX
    apiSpecificInfo.aaxManufacturerID = kManufacturerID;
    apiSpecificInfo.aaxProductID = kAAXProductID;
    apiSpecificInfo.aaxBundleID = kAAXBundleID;  /* MacOS only: this MUST match the bundle identifier in your info.plist file */
    apiSpecificInfo.aaxEffectID = "aaxDeveloper.";
    apiSpecificInfo.aaxEffectID.append(PluginCore::getPluginName());
    apiSpecificInfo.aaxPluginCategoryCode = kAAXCategory;

    // --- AU
    apiSpecificInfo.auBundleID = kAUBundleID;   /* MacOS only: this MUST match the bundle identifier in your info.plist file */

    // --- VST3
    apiSpecificInfo.vst3FUID = PluginCore::getVSTFUID(); // OLE string format
    apiSpecificInfo.vst3BundleID = kVST3BundleID;/* MacOS only: this MUST match the bundle identifier in your info.plist file */
	apiSpecificInfo.enableVST3SampleAccurateAutomation = kVSTSAA;
	apiSpecificInfo.vst3SampleAccurateGranularity = kVST3SAAGranularity;

    // --- AU and AAX
    apiSpecificInfo.fourCharCode = PluginCore::getFourCharCode();

    return true;
}

// --- static functions required for VST3/AU only --------------------------------------------- //
const char* PluginCore::getPluginBundleName() { return kAUBundleName; }
const char* PluginCore::getPluginName(){ return kPluginName; }
const char* PluginCore::getShortPluginName(){ return kShortPluginName; }
const char* PluginCore::getVendorName(){ return kVendorName; }
const char* PluginCore::getVendorURL(){ return kVendorURL; }
const char* PluginCore::getVendorEmail(){ return kVendorEmail; }
const char* PluginCore::getAUCocoaViewFactoryName(){ return AU_COCOA_VIEWFACTORY_STRING; }
pluginType PluginCore::getPluginType(){ return kPluginType; }
const char* PluginCore::getVSTFUID(){ return kVSTFUID; }
int32_t PluginCore::getFourCharCode(){ return kFourCharCode; }

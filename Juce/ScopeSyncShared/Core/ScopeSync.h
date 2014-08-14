/**
 * The Core of BCModular's ScopeSync system. Handles configuration,
 * parameter management and pulling together the various aspects
 * of communication between different parts of the system.
 *
 * Operates in two modes (known as AppContexts): plugin and scopefx
 * The former is when ScopeSync is hosted within a DAW or plugin
 * host and the latter is when it is hosted within a Scope app,
 * either Scope or Scope SDK.
 * 
 * The ScopeSync object is instantiated differently in each of
 * these two different contexts and there are different DLL
 * builds for each, relying on a __DLL_EFFECT__ pre-processor
 * directive to identify the scopefx context.
 *
 * The wrapper objects (PluginProcessor and ScopeFX respectively)
 * control ScopeSync's timer callbacks, which is why it does not
 * inherit from Timer itself.
 *
 *  (C) Copyright 2014 bcmodular (http://www.bcmodular.co.uk/)
 *
 * This file is part of ScopeSync.
 *
 * ScopeSync is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * ScopeSync is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ScopeSync.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contributors:
 *  Simon Russell
 *  Will Ellis
 *  Jessica Brandt
 */

#ifndef SCOPESYNC_H_INCLUDED
#define SCOPESYNC_H_INCLUDED

class PluginProcessor;
class ScopeFX;
#include "../Comms/ScopeSyncAudio.h"

#ifdef __DLL_EFFECT__
    #include "../Comms/ScopeSyncAsync.h"
#endif // __DLL_EFFECT__

#include <JuceHeader.h>
#include "../Components/BCMLookAndFeel.h"

class ScopeSync : public Value::Listener
{
public:
    /* ========================== Initialisation ============================= */
    ScopeSync();
#ifndef __DLL_EFFECT__
    ScopeSync(PluginProcessor* owner);
#else
    ScopeSync(ScopeFX* owner);
#endif // __DLL_EFFECT__
    ~ScopeSync();

    /* ========================== Public Actions ============================= */
    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);
    void snapshot();
    void beginParameterChangeGesture(int paramIdx);
    void endParameterChangeGesture(int paramIdx);
    bool guiNeedsReloading();
    void setGUIReload(bool reloadGUIFlag);
    void timerCallback();
    bool loadConfiguration(bool loadLoader, bool retainState, bool clearSystemErrorMessage);
    void addBCMLookAndFeel(BCMLookAndFeel* bcmLookAndFeel);
    BCMLookAndFeel* ScopeSync::getBCMLookAndFeelById(String id);
    void clearBCMLookAndFeels();
    int  getNumBCMLookAndFeels();
    
    /* ====================== Public Parameter Methods ======================= */
    // Returns the number of parameters to inform the host about. Actually returns
    // the "minHostParameters" value if the real numHostParameters is smaller.
    // This is to prevent issues with switching between configurations that
    // have different parameter counts.
    int   getNumParametersForHost();

    int    getNumParameters();
    float  getParameterHostValue(int index);
    String getParameterText(int index);
    int    getParameterScopeIntValue(int index);
    int    getParameterIdxByName(String& name);
    int    getParameterIdxFromScopeCode(int scopeCode);
    void   getParameterNameForHost(int index, String& parameterName);
    void   setParameterFromHost(int index, float newValue);
    void   setParameterFromGUI(int index, float newValue);
    void   handleScopeSyncAsyncUpdate(Array<int>& asyncValues);
    void   createSnapshot();
    void   getSnapshot(Array<std::pair<int,int>>& snapshotSubset, int numElements);
          
    void   setAffectedByUI(int paramIdx, bool isAffected);
    bool   isAffectedByUI(int paramIdx);
    int    getScopeSyncId(int paramIdx);
    int    getScopeLocalId(int paramIdx);
    void   getParameterSettings(int paramIdx, ValueTree& settings);
    void   mapToParameterUIValue(int paramIdx, Value& valueToMapTo);
    void   getParameterDescriptions(int paramIdx, String& shortDesc, String& fullDesc);
    void   getParameterUIRanges(int paramIdx, double& rangeMin, double& rangeMax, double& rangeInt, String& uiSuffix);
    bool   getParameterUIResetValue(int paramIdx, double& uiResetValue);
    bool   getParameterUISkewFactor(int paramIdx, String& uiSkewFactorType, double& uiSkewFactor);

    /* =================== Public Configuration Methods ====================== */
    Value&          getConfigurationName();
    Value&          getConfigurationFilePath();
    void            setConfigurationFilePath(const String& newFilePath, bool retainState);
    XmlElement&     getConfiguration() { return configurationXml; };
    XmlElement*     getSystemLookAndFeels();
    bool            configurationIsLoading();
    void            setConfigurationLoading(bool configurationLoadingFlag);
    ValueTree&      getDeviceParameters(){ return deviceParameters; };
    PropertiesFile& getAppProperties();
    void            storeParameterValues();
    void            storeParameterValues(XmlElement& parameterValues);
    void            restoreParameterValues();
    XmlElement&     getParameterValueStore() { return parameterValueStore; };
    Value&          getSystemError();
    void            setSystemError(const String& errorText) { systemError.setValue(errorText); };
     
    /* =========================== Identifiers =============================== */
    static const Identifier paramTypesId;
    static const Identifier paramTypeId;
    static const Identifier paramTypeNameId;
    static const Identifier paramTypeValueTypeId;
    static const Identifier paramTypeHostRangeMinId;
    static const Identifier paramTypeHostRangeMaxId;
    static const Identifier paramTypeHostRangeIntervalId;
    static const Identifier paramTypeUISuffixId;
    static const Identifier paramTypeUIRangeMinId;
    static const Identifier paramTypeUIRangeMaxId;
    static const Identifier paramTypeUIRangeIntervalId;
    static const Identifier paramTypeUIResetValueId;
    static const Identifier paramTypeUISkewFactorId;
    static const Identifier paramTypeUISkewFactorTypeId;
//    static const Identifier paramTypeUISkewFactorInvertedId;
    static const Identifier paramTypeHostSkewFactorId;
    static const Identifier paramTypeHostSkewMidPointId;
    static const Identifier paramTypeHostSkewFactorTypeId;
    static const Identifier paramTypeHostSkewFactorInvertedId;
    static const Identifier paramTypeScopeRangeMinId;
    static const Identifier paramTypeScopeRangeMaxId;
    static const Identifier paramTypeScopeRangeMinFltId;
    static const Identifier paramTypeScopeRangeMaxFltId;
//    static const Identifier paramTypeScopeSkewFactorId;
//    static const Identifier paramTypeScopeSkewFactorTypeId;
//    static const Identifier paramTypeScopeSkewFactorInvertedId;
    static const Identifier paramTypeSettingsId;
    static const Identifier paramTypeSettingId;
    static const Identifier paramTypeSettingNameId;
    static const Identifier paramTypeSettingValueId;

    static const Identifier deviceId;
    static const Identifier paramId;
    static const Identifier paramNameId;
    static const Identifier paramShortDescId;
    static const Identifier paramFullDescId;
    static const Identifier paramScopeSyncId;
    static const Identifier paramScopeLocalId;
    static const Identifier paramHostValueId;
    static const Identifier paramUIValueId;
    static const Identifier affectedByUIId;
    
    /* ============================ Enumerations ============================== */
    enum ParameterValueType {continuous, discrete}; // Possible types of Parameter Value
    enum AppContext {plugin, scopefx}; // Contexts under which the app may be running
    
    static const bool inPluginContext()  { return (appContext == plugin)  ? true : false; };
    static const bool inScopeFXContext() { return (appContext == scopefx) ? true : false; };

private:
    /* ========================== Initialisation ============================== */
    void initialise();
    
    /* ========================== Private Actions ============================= */
    void receiveUpdatesFromScopeAudio();
    void receiveUpdatesFromScopeAsync();
    bool scopeAsyncHasUpdates();
    void sendToScopeSyncAudio(int paramIdx, float newValue);
    void sendToScopeSyncAsync(int paramIdx, float newValue);
    void valueChanged(Value& valueThatChanged);

    /* ====================== Private Parameter Methods ======================= */
    void   setParameterValues(int index, float newHostValue, float newUIValue);
    float  convertUIToHostValue(int paramIdx, float value);
    float  convertHostToUIValue(int paramIdx, float value);
    float  convertScopeFltToHostValue(int paramIdx, float value);
    float  convertScopeIntToHostValue(int paramIdx, int value);
    float  convertHostToScopeFltValue(int paramIdx, float value);
    int    convertHostToScopeIntValue(int paramIdx, float value);
    int    findNearestParameterSetting(const ValueTree& settings, float value);
    double skewHostValue(int paramIdx, float hostValue, bool invert);
    
    /* =================== Private Configuration Methods =======================*/
    bool loadSystemParameterTypes();
    bool overrideParameterTypes(XmlElement& parameterTypesXml);
    void getParameterTypeFromXML(XmlElement& xml, ValueTree& parameterType);
    void readSkewFactorXml(const Identifier& xmlId, const XmlElement& child, ValueTree& parameterType);
    bool loadDeviceParameters(XmlElement& deviceXml);
    bool loadMappingFile(XmlElement& mappingXml);
    bool loadLayoutFile(XmlElement& layoutXml);
    void clearSystemError() { systemError.setValue(String::empty); };
    
    /* ===================== Private member variables ========================= */
#ifndef __DLL_EFFECT__
    PluginProcessor* pluginProcessor;
#else
    ScopeFX*       scopeFX;
    ScopeSyncAsync scopeSyncAsync;
#endif // __DLL_EFFECT__

    ScopeSyncAudio             scopeSyncAudio;
    XmlElement                 parameterValueStore;
    OwnedArray<BCMLookAndFeel> bcmLookAndFeels;

    CriticalSection flagLock;
   
    bool configurationLoading;      // Configuration load is disabled while in the process of loading
    bool reloadGUI;                 // Flag to indicate whether the GUI needs to be reloaded
    bool retainParameterState;      // Flag to indicate whether parameter values should be restored after loading configuration
    bool initialiseScopeParameters; // All Scope Parameters are set from Async the first time we receive an update
    
    Array<std::pair<int,float>, CriticalSection> audioControlUpdates;   // Updates received from the ScopeSync audio input
    Array<std::pair<int,int>, CriticalSection>   asyncControlUpdates;   // Updates received from the ScopeFX async input to be passed on to the ScopeSync system
    ValueTree                                    deviceParameters;      // ValueTree containing the definition of all Device Parameters
    ValueTree                                    parameterTypes;        // ValueTree containing the definition of all Parameter Types
    XmlElement                                   configurationXml;      // Configuration XML loaded from definition file(s)

    int numParameters;
    
    static const int minHostParameters;   // Minimum parameter count to return to host
    static const int MIN_SCOPE_INTEGER;   // Minimum (signed) integer understood by Scope
    static const int MAX_SCOPE_INTEGER;   // Maximum (signed) integer understood by Scope
    static const int numAsyncParameters;  // Number of Parameters being processed by the Scope Async system
    static const int numAsyncLocalValues; // Number of Local Values being processed by the Scope Async system

    static const String systemParameterTypes; // XML configuration for the default parameter types
    static const String systemLookAndFeels;   // XML configuration for the built-in LookAndFeels
    static const String loaderConfiguration;  // XML configuration for the loader
    
    static const AppContext  appContext;      // Context under which the app is currently running
    static const StringArray scopeSyncCodes;  // Array of ScopeSync codes for looking up during configuration
    static const StringArray scopeLocalCodes; // Array of ScopeLocal codes for looking up during configuration
    
    Value configurationFilePath; // Full file path for currently loaded configuration
    Value configurationName;     // Name of currently loaded configuration
    Value systemError;           // Latest system error text
    
    ApplicationProperties appProperties; // Main application properties
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopeSync)
};

#endif  // SCOPESYNC_H_INCLUDED

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
#include "BCMParameter.h"

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
    void beginParameterChangeGesture(BCMParameter& parameter);
    void endParameterChangeGesture(BCMParameter& parameter);
    bool guiNeedsReloading();
    void setGUIReload(bool reloadGUIFlag);
    void timerCallback();
    bool loadConfiguration(bool loadLoader, bool retainState, bool clearSystemErrorMessage);
    void addBCMLookAndFeel(BCMLookAndFeel* bcmLookAndFeel);
    BCMLookAndFeel* getBCMLookAndFeelById(String id);
    void clearBCMLookAndFeels();
    int  getNumBCMLookAndFeels();
    
    /* ====================== Public Parameter Methods ======================= */
    // Returns the number of parameters to inform the host about. Actually returns
    // the "minHostParameters" value if the real numHostParameters is smaller.
    // This is to prevent issues with switching between configurations that
    // have different parameter counts.
    int    getNumParametersForHost();

    BCMParameter* getParameterByName(const String& name);
    float  getParameterHostValue(int hostIdx);
    void   setParameterFromHost(int hostIdx, float newValue);
    void   setParameterFromGUI(BCMParameter& parameter, float newValue);
    void   getParameterNameForHost(int hostIdx, String& parameterName);
    void   getParameterText(int hostIdx, String& parameterText);
    void   handleScopeSyncAsyncUpdate(Array<int>& asyncValues);
    void   createSnapshot();
    void   getSnapshot(Array<std::pair<int,int>>& snapshotSubset, int numElements);
          
    /* =================== Public Configuration Methods ====================== */
    String          getConfigurationName();
    String          getConfigurationFilePath();
    void            setConfigurationFilePath(const String& newFilePath, bool retainState);
    XmlElement&     getConfiguration() { return configurationXml; };
    XmlElement*     getSystemLookAndFeels();
    bool            configurationIsLoading();
    void            setConfigurationLoading(bool configurationLoadingFlag);
    PropertiesFile& getAppProperties();
    void            storeParameterValues();
    void            storeParameterValues(XmlElement& parameterValues);
    void            restoreParameterValues();
    XmlElement&     getParameterValueStore() { return parameterValueStore; };
    Value&          getSystemError();
    void            setSystemError(const String& errorText) { systemError.setValue(errorText); };
     
    /* ============================ Enumerations ============================== */
    enum AppContext {plugin, scopefx}; // Contexts under which the app may be running
    
    static const bool inPluginContext()  { return (appContext == plugin)  ? true : false; };
    static const bool inScopeFXContext() { return (appContext == scopefx) ? true : false; };

private:
    /* ========================== Initialisation ============================== */
    void initialise();
    
    /* ========================== Private Actions ============================= */
    void receiveUpdatesFromScopeAudio();
    void receiveUpdatesFromScopeAsync();
    void sendToScopeSyncAudio(BCMParameter& parameter);
    void sendToScopeSyncAsync(BCMParameter& parameter);
    void valueChanged(Value& valueThatChanged);

    /* =================== Private Configuration Methods =======================*/
    bool loadSystemParameterTypes();
    bool overrideParameterTypes(XmlElement& parameterTypesXml);
    void getParameterTypeFromXML(XmlElement& xml, ValueTree& parameterType);
    void readUISkewFactorXml   (const XmlElement& xml, ValueTree& parameterType, double uiMinValue, double uiMaxValue);
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
    OwnedArray<BCMParameter>   hostParameters;         // Parameters that the host is interested in
    OwnedArray<BCMParameter>   scopeLocalParameters;   // Parameters that are only relevant in the Scope DLL
    Array<int>                 paramIdxByScopeSyncId;  // Index of parameters by their ScopeSyncId
    Array<int>                 paramIdxByScopeLocalId; // Index of parameters by their ScopeLocalId

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
    static const int numScopeSyncParameters;  // Number of ScopeSync Parameters being processed by the Scope Async system
    static const int numScopeLocalParameters; // Number of ScopeLocal Parameters being processed by the Scope Async system

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

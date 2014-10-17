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
class Configuration;
class ConfigurationManagerWindow;

#include "../Comms/ScopeSyncAudio.h"
#include "BCMParameter.h"

#ifdef __DLL_EFFECT__
    #include "../Comms/ScopeSyncAsync.h"
#endif // __DLL_EFFECT__

#include <JuceHeader.h>
#include "../Components/BCMLookAndFeel.h"
#include "../Configuration/Configuration.h"

class ScopeSync
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
    void unload();

    /* ========================== Public Actions ============================= */
    static int  getNumScopeSyncInstances();
    static void reloadAllGUIs();
    static const String& getScopeSyncCode(int scopeSync);
    static const String& getScopeLocalCode(int scopeLocal);
    static const StringArray& getScopeSyncCodes()  { return scopeSyncCodes; };
    static const StringArray& getScopeLocalCodes() { return scopeLocalCodes; };
    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);
    void snapshot();
    void beginParameterChangeGesture(BCMParameter* parameter);
    void endParameterChangeGesture(BCMParameter* parameter);
    void setGUIEnabled(bool shouldBeEnabled);
    bool guiNeedsReloading();
    void setGUIReload(bool reloadGUIFlag);
    void receiveUpdates();
    void reloadLayout();
    void unloadConfiguration();
    void addBCMLookAndFeel(BCMLookAndFeel* bcmLookAndFeel);
    BCMLookAndFeel* getBCMLookAndFeelById(String id);
    void clearBCMLookAndFeels();
    int  getNumBCMLookAndFeels();
    StringArray getBCMLookAndFeelIds(const Identifier& componentType);

    ConfigurationManagerWindow* getConfigurationManagerWindow() { return configurationManagerWindow; }
    void showConfigurationManager(int posX, int posY);
    void hideConfigurationManager();
    ApplicationCommandManager* getCommandManager() { return commandManager; }
    UndoManager& getUndoManager() { return undoManager; }

    void toggleEditToolbar()     { showEditToolbar = !showEditToolbar; }
    bool shouldShowEditToolbar() { return showEditToolbar; }

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
    void            applyConfiguration();
    void            saveConfiguration();
    void            saveConfigurationAs(const String& fileName);
    void            reloadSavedConfiguration();
    bool            configurationHasUnsavedChanges();
    Configuration&  getConfiguration() { return *configuration; };
    ValueTree       getConfigurationRoot() { return configuration->getConfigurationRoot(); };
    String          getConfigurationName(bool showUnsavedIndicator);
    const File&     getConfigurationFile() { return configuration->getFile(); };
    const File&     getLastFailedConfigurationFile() { return configuration->getLastFailedFile(); };
    String          getConfigurationDirectory() { return configuration->getConfigurationDirectory(); }
    String          getLayoutDirectory() { return configuration->getLayoutDirectory(); };
    bool            hasConfigurationUpdate(String& fileName);
    void            changeConfiguration(const String& fileName, bool asyncLoad);
    bool            processConfigurationChange();
    ValueTree       getMapping() { return configuration->getMapping(); };
    XmlElement&     getLayout(String& errorText, String& errorDetails, bool forceReload) { return configuration->getLayout(errorText, errorDetails, forceReload); };
    XmlElement*     getSystemLookAndFeels();
    void            storeParameterValues();
    void            storeParameterValues(XmlElement& parameterValues);
    void            restoreParameterValues();
    XmlElement&     getParameterValueStore() { return parameterValueStore; };
    Value&          getSystemError();
    Value&          getSystemErrorDetails();
    void            setSystemError(const String& errorText, const String& errorDetailsText);
     
private:

    /* ========================== Initialisation ============================== */
    void initialise();
    void resetScopeCodeIndexes();
    void initCommandManager();
    
    /* ========================== Private Actions ============================= */
    void receiveUpdatesFromScopeAudio();
    void receiveUpdatesFromScopeAsync();
    void sendToScopeSyncAudio(BCMParameter& parameter);
    void sendToScopeSyncAsync(BCMParameter& parameter);
    void endAllParameterChangeGestures();
    
    /* =================== Private Configuration Methods =======================*/
    bool loadSystemParameterTypes();
    bool overrideParameterTypes(XmlElement& parameterTypesXml, bool loadLoader);
    void getParameterTypeFromXML(XmlElement& xml, ValueTree& parameterType);
    void readUISkewFactorXml   (const XmlElement& xml, ValueTree& parameterType, double uiMinValue, double uiMaxValue);
    bool loadDeviceParameters(XmlElement& deviceXml, bool loadLoader);
    bool loadMappingFile(XmlElement& mappingXml);
    bool loadLayoutFile(XmlElement& layoutXml);
    
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
    ScopedPointer<ApplicationCommandManager> commandManager;
    static Array<ScopeSync*>   scopeSyncInstances;     // Tracks instances of this object, so Juce can be shutdown when no more remain
    ScopedPointer<ConfigurationManagerWindow> configurationManagerWindow;
    UndoManager                undoManager;

    BigInteger changingParams;
    
    CriticalSection flagLock;
   
    bool reloadGUI;                 // Flag to indicate whether the GUI needs to be reloaded
    bool retainParameterState;      // Flag to indicate whether parameter values should be restored after loading configuration
    bool initialiseScopeParameters; // All Scope Parameters are set from Async the first time we receive an update
    bool configurationLoading;
    
    bool showEditToolbar; // Indicates whether the EditToolbar should be shown in the GUI's Main Component

    Array<std::pair<int,float>, CriticalSection> audioControlUpdates;  // Updates received from the ScopeSync audio input
    Array<std::pair<int,int>, CriticalSection>   asyncControlUpdates;  // Updates received from the ScopeFX async input to be passed on to the ScopeSync system
    Array<String, CriticalSection>               configurationChanges;
    ScopedPointer<Configuration>                 configuration;
    
    static const int minHostParameters;       // Minimum parameter count to return to host
    
    static const String systemParameterTypes; // XML configuration for the default parameter types
    static const String systemLookAndFeels;   // XML configuration for the built-in LookAndFeels
    static const String loaderConfiguration;  // XML configuration for the loader
    
    static const StringArray scopeSyncCodes;  // Array of ScopeSync codes for looking up during configuration
    static const StringArray scopeLocalCodes; // Array of ScopeLocal codes for looking up during configuration
    
    Value systemError;        // Latest system error text
    Value systemErrorDetails; // Latest system error details text
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopeSync)
};

#endif  // SCOPESYNC_H_INCLUDED

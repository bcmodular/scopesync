/**
 * The Core of BCModular's ScopeSync system. Handles configuration,
 * parameter management and pulling together the various aspects
 * of communication between different parts of the system.
 *
 *  (C) Copyright 2014 bcmodular (http://www.bcmodular.co.uk/)
 *
 * This file is part of ScopeSync.
 *
 * ScopeSync is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include "ScopeSync.h"

#include "ScopeSyncApplication.h"
#include "Global.h"
#include "../Configuration/ConfigurationManager.h"
#include "../Core/ScopeSyncGUI.h"
#include "BCMParameterController.h"
#include "BCMParameter.h"
#include "../Utils/BCMMisc.h"

#ifndef __DLL_EFFECT__
    #include "../Plugin/PluginProcessor.h"
#else
    #include "../ScopeFX/ScopeFX.h"
#endif // __DLL_EFFECT__

const String ScopeSync::scopeSyncVersionString = "0.6.0-Prerelease";
const StringArray ScopeSync::fixedParameterNames = StringArray::fromTokens("DUMMY,posX,posY,show,configID,presetlist,patchwindow,monoeffect,bypasseffect,shellpresetwindow,voicecount,midichannel,Device Type,midiactivity", ",", "");

Array<ScopeSync*> ScopeSync::scopeSyncInstances;

#ifndef __DLL_EFFECT__
ScopeSync::ScopeSync(PluginProcessor* owner)
{
    scopeSyncInstances.add(this);
    pluginProcessor = owner;
    initialise();
}
#else
ScopeSync::ScopeSync(ScopeFX* owner)
{
	initialised = false;
    scopeSyncInstances.add(this);
    scopeFX = owner;
    initialise();
}
#endif // __DLL_EFFECT__

ScopeSync::~ScopeSync()
{
	configurationID.removeListener(this);
	userSettings->removeActionListener(this);        
    hideConfigurationManager();
    scopeSyncInstances.removeAllInstancesOf(this);
}

void ScopeSync::initialise()
{
	parameterController = new BCMParameterController(this);

    parameterController->getParameterByName("Config ID")->mapToUIValue(configurationID);
    configurationID.addListener(this);

    showEditToolbar = false;
    commandManager = new ApplicationCommandManager();

    configuration = new Configuration();
    applyConfiguration();

	initialised = true;
}

void ScopeSync::valueChanged(Value& valueThatChanged)
{
    if (valueThatChanged.refersToSameSourceAs(configurationID))
        changeConfiguration(int(valueThatChanged.getValue()));
}

bool ScopeSync::oscUIDInUse(int uid, ScopeSync* currentInstance)
{
	for (int i = 0; i < getNumScopeSyncInstances(); i++)
		if (scopeSyncInstances[i] != currentInstance && scopeSyncInstances[i]->getOSCUID() == uid)
			return true;
	
	return false;
}

void ScopeSync::initOSCUID()
{
	int initialOSCUID = 0;

	while (initialOSCUID < INT_MAX && oscUIDInUse(initialOSCUID, this))
		initialOSCUID++;
	
   oscUID.setValue(initialOSCUID);
}

void ScopeSync::referToOSCUID(Value & valueToLink) const
{
    valueToLink.referTo(oscUID);
}

void ScopeSync::showConfigurationManager(int posX, int posY)
{
    if (configurationManagerWindow == nullptr)
    {
        configurationManagerWindow = new ConfigurationManagerWindow(*this, posX, posY);
        
        if (ScopeSyncApplication::inScopeFXContext())
            configurationManagerWindow->setAlwaysOnTop(true);
    }

    configurationManagerWindow->toFront(true);
}

void ScopeSync::hideConfigurationManager()
{
	configurationManagerWindow = nullptr;
}

ApplicationCommandManager* ScopeSync::getCommandManager() const
{
    return commandManager;
}

BCMParameterController* ScopeSync::getParameterController() const
{
    return parameterController;
}

#ifndef __DLL_EFFECT__
PluginProcessor* ScopeSync::getPluginProcessor()
{
    return pluginProcessor;
}
#endif // __DLL_EFFECT__

void ScopeSync::unload()
{
    if (configurationManagerWindow != nullptr)
    {
        configurationManagerWindow->unload();
        configurationManagerWindow = nullptr;
    }

    configuration->saveIfNeededAndUserAgrees(false);
}

int ScopeSync::getNumScopeSyncInstances() 
{ 
    return scopeSyncInstances.size(); 
};

void ScopeSync::reloadAllGUIs()
{
    for (int i = 0; i < scopeSyncInstances.size(); i++)
        scopeSyncInstances[i]->setGUIReload(true);
}

void ScopeSync::snapshotAll()
{
	for (int i = 0; i < getNumScopeSyncInstances(); i++)
		scopeSyncInstances[i]->getParameterController()->snapshot();
}
	
bool ScopeSync::guiNeedsReloading() const
{
    const ScopedLock lock(flagLock);
    return reloadGUI;
};

void ScopeSync::setGUIReload(bool reloadGUIFlag)
{
    const ScopedLock lock(flagLock);
    reloadGUI = reloadGUIFlag;
};
    
Value& ScopeSync::getSystemError()
{
    return systemError;
};

Value& ScopeSync::getSystemErrorDetails()
{
    return systemErrorDetails;
};

void ScopeSync::setSystemError(const String& errorText, const String& errorDetailsText)
{
    systemError        = errorText;
    systemErrorDetails = errorDetailsText;
}
    
XmlElement* ScopeSync::getSystemLookAndFeels()
{
	String systemLookAndFeels(BinaryData::system_lookandfeels);
    XmlDocument lookAndFeelsDocument(systemLookAndFeels);
    XmlElement* lookAndFeelsElement = lookAndFeelsDocument.getDocumentElement();
    return lookAndFeelsElement;
}

XmlElement* ScopeSync::getStandardContent(const String& contentToShow)
{
    String xmlContent;

    if (contentToShow.equalsIgnoreCase("header"))
		xmlContent = String(BinaryData::standardHeaderContent_layout);
	else if (contentToShow.equalsIgnoreCase("footer"))
		xmlContent = String(BinaryData::standardFooterContent_layout);
	else if (contentToShow.equalsIgnoreCase("scopesyncsliderlnfs"))
        xmlContent = String(BinaryData::standardSliderLnFs_layout);

    XmlDocument standardContentDocument(xmlContent);
    XmlElement* standardContentElement = standardContentDocument.getDocumentElement();
    return standardContentElement;
}

bool ScopeSync::hasConfigurationUpdate(String& fileName)
{
    const ScopedLock lock(configurationChanges.getLock());

    if (configurationChanges.size() > 0)
    {
        fileName = configurationChanges.getLast();
        configurationChanges.clear();
        return true;
    }
    else
    {
        return false;
    }
};

bool ScopeSync::processConfigurationChange()
{
    if (ScopeSyncApplication::inPluginContext())
        parameterController->storeParameterValues(); 

    String newFileName;

    if (hasConfigurationUpdate(newFileName))
    {
        if (configuration->replaceConfiguration(newFileName))
        {
            applyConfiguration();
            return true;
        }
        else
        {
            setSystemError(configuration->getLastError(), configuration->getLastErrorDetails());
            return false;
        }
    }
    else
    {
        return false;
    }
}

ValueTree ScopeSync::getMapping() const
{
    return configuration->getMapping();
}

XmlElement& ScopeSync::getLayout(String& errorText, String& errorDetails, bool forceReload) const
{
    return configuration->getLayout(errorText, errorDetails, forceReload);
}

void ScopeSync::changeConfiguration(const String& fileName)
{
    configurationChanges.add(fileName);
    processConfigurationChange();
}

void ScopeSync::changeConfiguration(int uid)
{
    if (uid != 0)
    {
        String fileName = userSettings->getConfigurationFilePathFromUID(uid);

        if (fileName.isNotEmpty())
            configurationChanges.add(fileName);
    }
}

void ScopeSync::unloadConfiguration()
{
    configuration->replaceConfiguration(String::empty);
    applyConfiguration();
}

void ScopeSync::reloadLayout()
{
    setGUIReload(true);
}

bool ScopeSync::shouldShowEditToolbar() const
{
    return showEditToolbar;
}

void ScopeSync::applyConfiguration()
{
	DBG("ScopeSync::applyConfiguration");
	// TODO: Disable OSC here?
	
	setGUIEnabled(false);
    parameterController->endAllParameterChangeGestures();

    systemError        = String::empty;
    systemErrorDetails = String::empty;
    
    if (ScopeSyncApplication::inPluginContext())
        parameterController->storeParameterValues();

    parameterController->reset();
    
    // Firstly create the BCMParameter entries for each of the Host Parameters
    ValueTree parameterTree = configuration->getParameters();

    for (int i = 0; i < parameterTree.getNumChildren(); i++)
        parameterController->addParameter(parameterTree.getChild(i));

	parameterController->setupHostParameters();

#ifndef __DLL_EFFECT__
    pluginProcessor->updateHostDisplay();
#endif // __DLL_EFFECT__

#ifndef __DLL_EFFECT__
    parameterController->restoreParameterValues();
#else
	scopeFX->snapshot();
#endif // __DLL_EFFECT__

    userSettings->updateConfigurationLibraryEntry(getConfigurationFile().getFullPathName(),
                                                  getConfigurationFile().getFileName(),
                                                  getConfigurationRoot());
    setGUIReload(true);

    if (configurationManagerWindow != nullptr)
    {
        configurationManagerWindow->refreshContent();
        configurationManagerWindow->restoreWindowPosition();
    }

	configurationName = configuration->getDocumentTitle();
}

bool ScopeSync::isInitialised() const
{
	return initialised;
}

void ScopeSync::setGUIEnabled(bool shouldBeEnabled) const
{
#ifndef __DLL_EFFECT__
    pluginProcessor->setGUIEnabled(shouldBeEnabled);
#else
    scopeFX->setGUIEnabled(shouldBeEnabled);
#endif // __DLL_EFFECT__
}

void ScopeSync::saveConfiguration() const
{
    configuration->save(true, true);
}

bool ScopeSync::saveConfigurationAs()
{
    File configurationFileDirectory = getConfigurationDirectory();
    
    FileChooser fileChooser("Save Configuration File As...",
                            configurationFileDirectory,
                            "*.configuration");
    
    if (fileChooser.browseForFileToSave(true))
    {
        String fileName = fileChooser.getResult().getFullPathName();

        if (File::isAbsolutePath(fileName))
        {
            configuration->setLastFailedFile(configuration->getFile());
            configuration->setFile(File(fileName));
            getConfigurationRoot().setProperty(Ids::ID, createAlphaNumericUID(), nullptr);
            saveConfiguration();
        }

        applyConfiguration();

        userSettings->addActionListener(this);
        userSettings->rebuildFileLibrary(true, false, false);

        return true;
    }
    else
        return false;
}

void ScopeSync::addConfiguration(Rectangle<int> windowPosition)
{
    newConfigWindowPosition = windowPosition;

    FileChooser fileChooser("New Configuration File...",
                            File::nonexistent,
                            "*.configuration");
    
    File newFile;

    if (fileChooser.browseForFileToSave(true))
    {
        newFile = fileChooser.getResult();
    }
    else
    {
        return;
    }

    addConfigurationWindow = new NewConfigurationWindow
                                 (
                                 newConfigWindowPosition.getCentreX(), 
                                 newConfigWindowPosition.getCentreY(), 
                                 *this,
                                 newFile,
                                 commandManager
                                 );
    
    addConfigurationWindow->setVisible(true);
    
    if (ScopeSyncApplication::inScopeFXContext())
        addConfigurationWindow->setAlwaysOnTop(true);

    addConfigurationWindow->toFront(true);
}

void ScopeSync::hideAddConfigurationWindow()
{
    addConfigurationWindow = nullptr;
}

void ScopeSync::addConfiguration(File newFile, ValueTree newSettings)
{ 
    configuration->createConfiguration(newFile, newSettings);
    configuration->save(true, true);
    applyConfiguration();

    // Rebuild the library, so we can check whether the new configuration
    // was put into a File Location. We will get an action callback
    // once the rebuild is complete
    userSettings->addActionListener(this);
    userSettings->rebuildFileLibrary(true, false, false);
}

void ScopeSync::actionListenerCallback(const String& message)
{
    if (message == "configurationlibraryupdated")
    {
        if (newConfigIsInLocation())
        {
            userSettings->removeActionListener(this);
            
            if (configurationManagerWindow != nullptr)
                configurationManagerWindow->refreshContent();
        }
    }
}

bool ScopeSync::newConfigIsInLocation()
{
    int uid = configuration->getConfigurationUID();

    if (userSettings->getConfigurationFilePathFromUID(uid).isEmpty())
    {
		String errorMessage = "Your new Configuration was not automatically added to the library. You probably need to add a new location.";
        errorMessage << newLine;
		errorMessage << "Press OK to launch the File Location Editor or Cancel if you intend to do it later.";

        AlertWindow::showOkCancelBox(AlertWindow::InfoIcon,
                                    "Check locations",
                                    errorMessage,
                                    String::empty,
                                    String::empty,
                                    nullptr,
                                    ModalCallbackFunction::withParam(alertBoxLaunchLocationEditor, newConfigWindowPosition, this));
        return false;
    }

    return true;
}

UserSettings* ScopeSync::getUserSettings() const
{
	return userSettings;
}

void ScopeSync::alertBoxLaunchLocationEditor(int result, Rectangle<int> newConfigWindowPosition, ScopeSync* scopeSync)
{
    if (result)
    {
        // User clicked OK, so launch the location editor
        scopeSync->getUserSettings()->editFileLocations(newConfigWindowPosition.getCentreX(),
                                                        newConfigWindowPosition.getCentreY());    
    }
    else
    {
        // User clicked cancel, so we just give up for now
        scopeSync->getUserSettings()->removeActionListener(scopeSync);
    }
}

void ScopeSync::reloadSavedConfiguration()
{
    configuration->loadFrom(configuration->getFile(), true);
    configuration->setMissingDefaultValues();
    applyConfiguration();
    undoManager.clearUndoHistory();
}

String ScopeSync::getConfigurationName(bool showUnsavedIndicator) const
{
    String name = configurationName.toString();

    if (showUnsavedIndicator && configuration->hasChangedSinceSaved())
        name += " *";

    return name;
}

void ScopeSync::listenForConfigurationNameChanges(Value& listener) const
{
	listener.referTo(configurationName);
}

bool ScopeSync::configurationHasUnsavedChanges() const
{
    return configuration->hasChangedSinceSaved();
}

Configuration& ScopeSync::getConfiguration() const
{
    return *configuration;
}

ValueTree ScopeSync::getConfigurationRoot() const
{
    return configuration->getConfigurationRoot();
}

bool ScopeSync::configurationIsReadOnly() const
{
    return getConfigurationRoot().getProperty(Ids::readOnly, false);
}

const File& ScopeSync::getConfigurationFile() const
{
    return configuration->getFile();
}

const File& ScopeSync::getLastFailedConfigurationFile() const
{
    return configuration->getLastFailedFile();
}

String ScopeSync::getConfigurationDirectory() const
{
    return configuration->getConfigurationDirectory();
}

int ScopeSync::getConfigurationUID() const
{
    return configuration->getConfigurationUID();
}

String ScopeSync::getLayoutDirectory() const
{
    return configuration->getLayoutDirectory();
}

void ScopeSync::addBCMLookAndFeel(BCMLookAndFeel* bcmLookAndFeel)
{
    bcmLookAndFeels.add(bcmLookAndFeel);
}

BCMLookAndFeel* ScopeSync::getBCMLookAndFeelById(String id) const
{
    // Try to find the LookAndFeel with a matching id
    for (int i = 0; i < bcmLookAndFeels.size(); i++)
    {
        String foundId = bcmLookAndFeels[i]->getId();
        
        if (foundId.equalsIgnoreCase(id))
            return bcmLookAndFeels[i];
    }

    return nullptr;
}

void ScopeSync::clearBCMLookAndFeels() { bcmLookAndFeels.clear(); }

int ScopeSync::getNumBCMLookAndFeels() const
{
    return bcmLookAndFeels.size();
}

StringArray ScopeSync::getBCMLookAndFeelIds(const Identifier& componentType) const
{
    StringArray specificList;
    StringArray genericList;
    
    for (int i = 0; i < getNumBCMLookAndFeels(); i++)
    {
        int res = bcmLookAndFeels[i]->appliesToComponentType(componentType);

        if (res == 1)
            specificList.add(bcmLookAndFeels[i]->getId());
        else if (res == 2)
            genericList.add(bcmLookAndFeels[i]->getId());
    }

    specificList.add(String::empty);
    specificList.addArray(genericList);

    return specificList;
}

ConfigurationManagerWindow* ScopeSync::getConfigurationManagerWindow() const
{
    return configurationManagerWindow;
}

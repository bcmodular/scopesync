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

#include "ScopeSync.h"

#include "../Utils/BCMMath.h"
#include "../Utils/BCMXml.h"
#include "../Resources/ImageLoader.h"
#include "ScopeSyncApplication.h"
#include "Global.h"
#include <utility>
#include "../Configuration/ConfigurationManager.h"
#include "../Windows/UserSettings.h"
#include "../Resources/Icons.h"
#include "../Core/ScopeSyncGUI.h"
#include "BCMParameterController.h"
#include "BCMParameter.h"
#include "../Comms/ScopeSyncOSC.h"

#ifndef __DLL_EFFECT__
    #include "../Plugin/PluginProcessor.h"
#else
    #include "../ScopeFX/ScopeFX.h"
#endif // __DLL_EFFECT__

int          ScopeSync::performanceModeGlobalDisable = 0;
const String ScopeSync::scopeSyncVersionString = "0.5.0-Prerelease";
const int ScopeSync::numScopeCodes = 398;

const StringArray ScopeSync::scopeCodes = StringArray::fromTokens(
"A1,A2,A3,A4,A5,A6,A7,A8,\
B1,B2,B3,B4,B5,B6,B7,B8,\
C1,C2,C3,C4,C5,C6,C7,C8,\
D1,D2,D3,D4,D5,D6,D7,D8,\
E1,E2,E3,E4,E5,E6,E7,E8,\
F1,F2,F3,F4,F5,F6,F7,F8,\
G1,G2,G3,G4,G5,G6,G7,G8,\
H1,H2,H3,H4,H5,H6,H7,H8,\
I1,I2,I3,I4,I5,I6,I7,I8,\
J1,J2,J3,J4,J5,J6,J7,J8,\
K1,K2,K3,K4,K5,K6,K7,K8,\
L1,L2,L3,L4,L5,L6,L7,L8,\
M1,M2,M3,M4,M5,M6,M7,M8,\
N1,N2,N3,N4,N5,N6,N7,N8,\
O1,O2,O3,O4,O5,O6,O7,O8,\
P1,P2,P3,P4,P5,P6,P7,P8,\
LA1,LA2,LA3,LA4,LA5,LA6,LA7,LA8,\
LB1,LB2,LB3,LB4,LB5,LB6,LB7,LB8,\
LC1,LC2,LC3,LC4,LC5,LC6,LC7,LC8,\
LD1,LD2,LD3,LD4,LD5,LD6,LD7,LD8,\
LE1,LE2,LE3,LE4,LE5,LE6,LE7,LE8,\
LF1,LF2,LF3,LF4,LF5,LF6,LF7,LF8,\
LG1,LG2,LG3,LG4,LG5,LG6,LG7,LG8,\
LH1,LH2,LH3,LH4,LH5,LH6,LH7,LH8,\
LI1,LI2,LI3,LI4,LI5,LI6,LI7,LI8,\
LJ1,LJ2,LJ3,LJ4,LJ5,LJ6,LJ7,LJ8,\
LK1,LK2,LK3,LK4,LK5,LK6,LK7,LK8,\
LL1,LL2,LL3,LL4,LL5,LL6,LL7,LL8,\
LM1,LM2,LM3,LM4,LM5,LM6,LM7,LM8,\
LN1,LN2,LN3,LN4,LN5,LN6,LN7,LN8,\
LO1,LO2,LO3,LO4,LO5,LO6,LO7,LO8,\
LP1,LP2,LP3,LP4,LP5,LP6,LP7,LP8,\
FA1,FA2,FA3,FA4,FA5,FA6,FA7,FA8,\
FB1,FB2,FB3,FB4,FB5,FB6,FB7,FB8,\
FC1,FC2,FC3,FC4,FC5,FC6,FC7,FC8,\
FD1,FD2,FD3,FD4,FD5,FD6,FD7,FD8,\
FE1,FE2,FE3,FE4,FE5,FE6,FE7,FE8,\
FF1,FF2,FF3,FF4,FF5,FF6,FF7,FF8,\
FG1,FG2,FG3,FG4,FG5,FG6,FG7,FG8,\
FH1,FH2,FH3,FH4,FH5,FH6,FH7,FH8,\
FI1,FI2,FI3,FI4,FI5,FI6,FI7,FI8,\
FJ1,FJ2,FJ3,FJ4,FJ5,FJ6,FJ7,FJ8,\
FK1,FK2,FK3,FK4,FK5,FK6,FK7,FK8,\
FL1,FL2,FL3,FL4,FL5,FL6,FL7,FL8,\
FM1,FM2,FM3,FM4,FM5,FM6,FM7,FM8,\
FN1,FN2,FN3,FN4,FN5,FN6,FN7,FN8,\
FO1,FO2,FO3,FO4,FO5,FO6,FO7,FO8,\
FP1,FP2,FP3,FP4,FP5,FP6,FP7,FP8,\
X,Y,show,cfg,osc,spr,\
spa,mono,byp,sspr,vc,midc,\
type,mida",
",",""
);

const BCMParameter::ParameterType ScopeSync::scopeCodeTypes[numScopeCodes] = 
{
BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,
BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,
BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,
BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,
BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,
BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,
BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,
BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,
BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,
BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,
BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,
BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,
BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,
BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,
BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,
BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,BCMParameter::scope,
BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,
BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,
BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,
BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,
BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,
BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,
BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,
BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,
BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,
BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,
BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,
BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,
BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,
BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,
BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,
BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,BCMParameter::local,
BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,
BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,
BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,
BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,
BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,
BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,
BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,
BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,
BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,
BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,
BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,
BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,
BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,
BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,
BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,
BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,BCMParameter::feedback,
BCMParameter::fixedBiDir,BCMParameter::fixedBiDir,BCMParameter::fixedBiDir,BCMParameter::fixedBiDir,BCMParameter::fixedBiDir,BCMParameter::fixedBiDir,
BCMParameter::fixedBiDir,BCMParameter::fixedBiDir,BCMParameter::fixedBiDir,BCMParameter::fixedBiDir,BCMParameter::fixedBiDir,BCMParameter::fixedBiDir,
BCMParameter::fixedInputOnly,BCMParameter::fixedInputOnly
};

const String& ScopeSync::getScopeCode(int scopeCodeId) { return scopeCodes[scopeCodeId]; }

const int ScopeSync::getScopeCodeId(const String& scopeCode) { return scopeCodes.indexOf(scopeCode); }

const BCMParameter::ParameterType ScopeSync::getScopeCodeType(const String& scopeCode)
{
	int index = scopeCodes.indexOf(scopeCode);

	if (index >= 0)
		return scopeCodeTypes[index];
	else
		return BCMParameter::none;
};

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
	UserSettings::getInstance()->removeActionListener(this);        
    hideConfigurationManager();
    scopeSyncInstances.removeAllInstancesOf(this);
}

void ScopeSync::initialise()
{
	parameterController = new BCMParameterController(this);

    parameterController->getParameterByScopeCode("cfg")->mapToUIValue(configurationID);
    configurationID.addListener(this);

    showEditToolbar = false;
    commandManager = new ApplicationCommandManager();

    configuration = new Configuration();
    applyConfiguration();

	ScopeSyncOSCServer::getInstance()->setup();

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
		if (scopeSyncInstances[i] != currentInstance && scopeSyncInstances[i]->getParameterController()->getOSCUID() == uid)
			return true;
	
	return false;
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

void ScopeSync::shutDownIfLastInstance()
{
    if (getNumScopeSyncInstances() == 0)
    {
		ScopeSyncOSCServer::deleteInstance();
        StyleOverrideClipboard::deleteInstance();
        ParameterClipboard::deleteInstance();
        Icons::deleteInstance();
        ImageLoader::deleteInstance();
        UserSettings::deleteInstance();
        AboutBoxWindow::deleteInstance();

        ScopeSyncGUI::deleteTooltipWindow();

        if (ScopeSyncApplication::inScopeFXContext())
            shutdownJuce_GUI();
    }
}

void ScopeSync::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    (void)midiMessages;
    (void)buffer;
}

void ScopeSync::snapshotAll()
{
	for (int i = 0; i < getNumScopeSyncInstances(); i++)
		scopeSyncInstances[i]->getParameterController()->snapshot();
}
	
bool ScopeSync::guiNeedsReloading()
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
    XmlDocument lookAndFeelsDocument(systemLookAndFeels);
    XmlElement* lookAndFeelsElement = lookAndFeelsDocument.getDocumentElement();
    return lookAndFeelsElement;
}

XmlElement* ScopeSync::getStandardContent(const String& contentToShow)
{
    String xmlContent;

    if (contentToShow.equalsIgnoreCase("header"))
        xmlContent = standardHeaderContent;
    else if (contentToShow.equalsIgnoreCase("scopesyncsliderlnfs"))
        xmlContent = standardSliderLnFs;

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

void ScopeSync::changeConfiguration(const String& fileName)
{
    configurationChanges.add(fileName);
    processConfigurationChange();
}

void ScopeSync::changeConfiguration(int uid)
{
    if (uid != 0)
    {
        String fileName = UserSettings::getInstance()->getConfigurationFilePathFromUID(uid);

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

void ScopeSync::applyConfiguration()
{
	DBG("ScopeSync::applyConfiguration");
#ifdef __DLL_EFFECT__
    scopeSyncAsync.toggleUpdateProcessing(false);
#endif
	parameterController->toggleAsyncUpdates(false);
    
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
	scopeSyncAsync.snapshot();
    scopeSyncAsync.toggleUpdateProcessing(true);
#endif // __DLL_EFFECT__

    UserSettings::getInstance()->updateConfigurationLibraryEntry(getConfigurationFile().getFullPathName(),
                                                                 getConfigurationFile().getFileName(),
                                                                 getConfigurationRoot());
    setGUIReload(true);

    if (configurationManagerWindow != nullptr)
    {
        configurationManagerWindow->refreshContent();
        configurationManagerWindow->restoreWindowPosition();
    }

    parameterController->toggleAsyncUpdates(true);
}

bool ScopeSync::isInitialised()
{
	return initialised;
}

void ScopeSync::setGUIEnabled(bool shouldBeEnabled)
{
#ifndef __DLL_EFFECT__
    pluginProcessor->setGUIEnabled(shouldBeEnabled);
#else
    scopeFX->setGUIEnabled(shouldBeEnabled);
#endif // __DLL_EFFECT__
}

void ScopeSync::saveConfiguration()
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

        UserSettings::getInstance()->addActionListener(this);
        UserSettings::getInstance()->rebuildFileLibrary(true, false, false);

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
    UserSettings::getInstance()->addActionListener(this);
    UserSettings::getInstance()->rebuildFileLibrary(true, false, false);
}

void ScopeSync::actionListenerCallback(const String& message)
{
    if (message == "configurationlibraryupdated")
    {
        if (newConfigIsInLocation())
        {
            UserSettings::getInstance()->removeActionListener(this);
            
            if (configurationManagerWindow != nullptr)
                configurationManagerWindow->refreshContent();
        }
    }
}

bool ScopeSync::newConfigIsInLocation()
{
    int uid = configuration->getConfigurationUID();

    if (UserSettings::getInstance()->getConfigurationFilePathFromUID(uid).isEmpty())
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

void ScopeSync::alertBoxLaunchLocationEditor(int result, Rectangle<int> newConfigWindowPosition, ScopeSync* scopeSync)
{
    if (result)
    {
        // User clicked OK, so launch the location editor
        UserSettings::getInstance()->editFileLocations(newConfigWindowPosition.getCentreX(),
                                                       newConfigWindowPosition.getCentreY());    
    }
    else
    {
        // User clicked cancel, so we just give up for now
        UserSettings::getInstance()->removeActionListener(scopeSync);
    }
}

void ScopeSync::reloadSavedConfiguration()
{
    configuration->loadFrom(configuration->getFile(), true);
    configuration->setMissingDefaultValues();
    applyConfiguration();
    undoManager.clearUndoHistory();
}

String ScopeSync::getConfigurationName(bool showUnsavedIndicator)
{
    String name = configuration->getDocumentTitle();

    if (showUnsavedIndicator && configuration->hasChangedSinceSaved())
        name += " *";

    return name;
}

bool ScopeSync::configurationHasUnsavedChanges()
{
    return configuration->hasChangedSinceSaved();
}

bool ScopeSync::configurationIsReadOnly()
{
    return getConfigurationRoot().getProperty(Ids::readOnly, false);
}

void ScopeSync::addBCMLookAndFeel(BCMLookAndFeel* bcmLookAndFeel) { bcmLookAndFeels.add(bcmLookAndFeel); }

BCMLookAndFeel* ScopeSync::getBCMLookAndFeelById(String id)
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

int ScopeSync::getNumBCMLookAndFeels()
{
    return bcmLookAndFeels.size();
}

StringArray ScopeSync::getBCMLookAndFeelIds(const Identifier& componentType)
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

const String ScopeSync::systemLookAndFeels =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<lookandfeels>\n"
"  <lookandfeel id=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <colours>\n"
"      <slider textboxbackgroundcolourid=\"ff000000\" textboxoutlinecolourid=\"ff121517\" textboxtextcolo"
"urid=\"ffb3bbbd\" textboxhighlightcolourid=\"ffa7aaae\" thumbcolourid=\"00ffffff\" backgroundcolourid=\"ff0"
"00000\"></slider>\n"
"      <label textcolourid=\"ff323739\"></label>\n"
"      <texteditor focusedoutlinecolourid=\"002f353a\"></texteditor>\n"
"      <tabbedcomponent outlinecolourid=\"00000000\"></tabbedcomponent>\n"
"      <tabbar tabtextcolourid=\"ffc3cace\" fronttextcolourid=\"ffff7f00\" taboutlinecolourid=\"00000000\">"
"</tabbar>\n"
"      <textbutton textcolouroffid=\"00ffffff\" textcolouronid=\"00ff0000\" buttoncolourid=\"ffff0000\"></t"
"extbutton>\n"
"    </colours>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:load_config_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"loadConfigButtonOff\" downfilename=\"loadConfigButtonOn\" mouseoverupfile"
"name=\"loadConfigButtonOver\" mouseoverdownfilename=\"loadConfigButtonOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:new_config_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"newConfigOff\" downfilename=\"newConfigOn\" mouseoverupfilename=\"newConfi"
"gOver\" mouseoverdownfilename=\"newConfigOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:reload_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"reloadButtonOff\" downfilename=\"reloadButtonOn\" mouseoverupfilename=\"re"
"loadButtonOver\" mouseoverdownfilename=\"reloadButtonOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:remove_config_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"removeConfigButtonOff\" downfilename=\"removeConfigButtonOn\" mouseoverup"
"filename=\"removeConfigButtonOver\" mouseoverdownfilename=\"removeConfigButtonOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:patch_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"patchWindowButtonOff\" downfilename=\"patchWindowButtonOn\" mouseoverupfi"
"lename=\"patchWindowButtonOver\" mouseoverdownfilename=\"patchWindowButtonOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:presets_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"presetsButtonOff\" downfilename=\"presetsButtonOn\" mouseoverupfilename=\""
"presetsButtonOver\" mouseoverdownfilename=\"presetsButtonOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:settings_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"settingsButtonOff\" downfilename=\"settingsButtonOn\" mouseoverupfilename"
"=\"settingsButtonOver\" mouseoverdownfilename=\"settingsButtonOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:snapshot_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"snapshotOff\" downfilename=\"snapshotOn\" mouseoverupfilename=\"snapshotOv"
"er\" mouseoverdownfilename=\"snapshotOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:showconfigurationmanager_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"showconfigurationmanagerOff\" downfilename=\"showconfigurationmanagerOn\""
" mouseoverupfilename=\"showconfigurationmanagerOver\" mouseoverdownfilename=\"showconfigurationmanagerO"
"n\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:scopesynclogo_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"scopeSyncLogoOff\" downfilename=\"scopeSyncLogoOn\" mouseoverupfilename=\""
"scopeSyncLogoOn\" mouseoverdownfilename=\"scopeSyncLogoOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:configname\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <colours>\n"
"      <label textcolourid=\"90ffffff\" outlinecolourid=\"90ffffff\"></label>\n"
"    </colours>\n"
"  </lookandfeel>\n"
"</lookandfeels>\n";

const String ScopeSync::standardHeaderContent =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"\n"
"  <component id=\"header\">\n"
"    <bounds relativerectangle=\"right - parent.width, 1, parent.width, top + 40\"></bounds>\n"
"    <label lfid=\"system:configname\" name=\"configurationname\" text=\"Current Configuration\">\n"
"      <bounds x=\"172\" y=\"11\" width=\"131\" height=\"17\"></bounds>\n"
"      <font bold=\"true\" height=\"12\"></font>\n"
"    </label>\n"
"    <textbutton lfid=\"system:scopesynclogo_button\" name=\"showaboutbox\" id=\"showaboutbox\" text=\"\" too"
"ltip=\"Show About Box\">\n"
"      <bounds relativerectangle=\"13, 11, left + 151, top + 21\"></bounds>\n"
"    </textbutton>\n"
"    <textbutton lfid=\"system:new_config_button\" name=\"newconfiguration\" tooltip=\"New Configuration\">"
"\n"
"      <bounds x=\"311\" y=\"9\" width=\"20\" height=\"21\"></bounds>\n"
"    </textbutton>\n"
"    <textbutton lfid=\"system:load_config_button\" name=\"chooseconfiguration\" tooltip=\"Load Configurat"
"ion\">\n"
"      <bounds x=\"331\" y=\"9\" width=\"21\" height=\"21\"></bounds>\n"
"    </textbutton>\n"
"    <textbutton lfid=\"system:reload_button\" name=\"reloadconfiguration\" tooltip=\"Reload Current Confi"
"guration\">\n"
"      <bounds x=\"356\" y=\"9\" width=\"21\" height=\"21\"></bounds>\n"
"    </textbutton>\n"
"    <textbutton lfid=\"system:showconfigurationmanager_button\" name=\"showconfigurationmanager\" id=\"sh"
"owconfigurationmanager\" text=\"\" tooltip=\"Open Configuration Manager panel\">\n"
"      <bounds x=\"379\" y=\"9\" width=\"21\" height=\"21\"></bounds>\n"
"    </textbutton>\n"
"    <textbutton lfid=\"system:settings_button\" name=\"showusersettings\" id=\"showusersettings\" text=\"\" "
"tooltip=\"Open User Settings panel\">\n"
"      <bounds relativerectangle=\"right - 28, showconfigurationmanager.top, parent.width - 4, top + 2"
"1\"></bounds>\n"
"    </textbutton>\n"
"    <textbutton lfid=\"system:snapshot_button\" name=\"snapshot\" id=\"snapshot\" text=\"\" tooltip=\"Send a "
"snapshot of all current parameter values. Ctrl-click to send snapshot for all ScopeSync instances\">\n"
"      <bounds relativerectangle=\"right - 32, showusersettings.top, showusersettings.left - 3, top + "
"21\"></bounds>\n"
"    </textbutton>\n"
"    <!--Shows additional buttons in Scope DLL header based on device type setting-->\n"
"    <tabbedcomponent displaycontext=\"scope\" name=\"Device Type\" showdropshadow=\"false\">\n"
"      <bounds relativerectangle=\"right - 100, 6, snapshot.left - 4, top + 32\"></bounds>\n"
"      <tabbar orientation=\"right\" depth=\"0\"></tabbar>\n"
"      <tab idx=\"1\" name=\"NOPRESET\">\n"
"        <component>\n"
"          <!--No Controls-->\n"
"        </component>\n"
"      </tab>\n"
"      <tab idx=\"2\" name=\"PRESET\">\n"
"        <component>\n"
"          <textbutton lfid=\"system:presets_button\" name=\"PresetList\" tooltip=\"Open Preset Browser\">\n"
"            <bounds x=\"81\" y=\"2\" width=\"19\" height=\"21\"></bounds>\n"
"          </textbutton>\n"
"        </component>\n"
"      </tab>\n"
"      <tab idx=\"3\" name=\"PRESETFX\">\n"
"        <component>\n"
"          <textbutton lfid=\"FX_Mono_button\" name=\"MonoEffect\" tooltip=\"Mono (use left input)\">\n"
"            <bounds x=\"30\" y=\"3\" width=\"27\" height=\"19\"></bounds>\n"
"          </textbutton>\n"
"          <textbutton lfid=\"FX_Bypass_button\" name=\"BypassEffect\" tooltip=\"Bypass\">\n"
"            <bounds x=\"58\" y=\"2\" width=\"20\" height=\"19\"></bounds>\n"
"          </textbutton>\n"
"          <textbutton lfid=\"system:presets_button\" name=\"PresetList\" tooltip=\"Open Preset Browser\">\n"
"            <bounds x=\"81\" y=\"2\" width=\"19\" height=\"21\"></bounds>\n"
"          </textbutton>\n"
"        </component>\n"
"      </tab>\n"
"      <tab idx=\"4\" name=\"BCPRESET\">\n"
"        <component>\n"
"          <textbutton lfid=\"system:patch_button\" name=\"PatchWindow\" tooltip=\"Open Modular Patch Wind"
"ow\">\n"
"            <bounds x=\"50\" y=\"2\" width=\"27\" height=\"21\"></bounds>\n"
"          </textbutton>\n"
"          <textbutton lfid=\"system:presets_button\" name=\"PresetList\" tooltip=\"Open Preset Browser\">\n"
"            <bounds x=\"81\" y=\"2\" width=\"19\" height=\"21\"></bounds>\n"
"          </textbutton>\n"
"        </component>\n"
"      </tab>\n"
"      <tab idx=\"5\" name=\"BCPRESETFX\">\n"
"        <component>\n"
"          <textbutton lfid=\"system:patch_button\" name=\"PatchWindow\" tooltip=\"Open Modular Patch Wind"
"ow\">\n"
"            <bounds x=\"0\" y=\"2\" width=\"27\" height=\"21\"></bounds>\n"
"          </textbutton>\n"
"          <textbutton lfid=\"FX_Mono_button\" name=\"MonoEffect\" tooltip=\"Mono (use left input)\">\n"
"            <bounds x=\"30\" y=\"3\" width=\"27\" height=\"19\"></bounds>\n"
"          </textbutton>\n"
"          <textbutton lfid=\"FX_Bypass_button\" name=\"BypassEffect\" tooltip=\"Bypass\">\n"
"            <bounds x=\"58\" y=\"2\" width=\"20\" height=\"19\"></bounds>\n"
"          </textbutton>\n"
"          <textbutton lfid=\"system:presets_button\" name=\"PresetList\" tooltip=\"Open Preset Browser\">\n"
"            <bounds x=\"81\" y=\"2\" width=\"19\" height=\"21\"></bounds>\n"
"          </textbutton>\n"
"        </component>\n"
"      </tab>\n"
"    </tabbedcomponent>\n"
"  </component>";

const String ScopeSync::standardSliderLnFs =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"\n"
"<lookandfeels>\n"
"  <lookandfeel id=\"JUCE knob\">\n"
"    <colours>\n"
"      <slider backgroundcolourid=\"ffb8bbbf\" thumbcolourid=\"ffffffff\" trackcolourid=\"ff0000ff\" rotary"
"sliderfillcolourid=\"ff0084ff\" rotaryslideroutlinecolourid=\"ff212121\" textboxbackgroundcolourid=\"00b8"
"bbbf\" textboxoutlinecolourid=\"00ffffff\" textboxtextcolourid=\"ff000000\" textboxhighlightcolourid=\"ff2"
"d3035\"></slider>\n"
"      <texteditor backgroundcolourid=\"ffffffff\" textcolourid=\"ffffffff\" highlightcolourid=\"ff2d3035\""
" highlightedtextcolourid=\"ffffffff\" outlinecolourid=\"7f000000\" focusedoutlinecolourid=\"7f2d3035\" sha"
"dowcolourid=\"ff000000\"></texteditor>\n"
"    </colours>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1A - White with LED surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Surround.png\" backgrounduselnfcolours=\"true\" outli"
"nebackgroundfilename=\"gfx/surround_knob_line.png\">\n"
"          <image filename=\"gfx/Knob1a_white.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1A - Black with LED surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Surround.png\" backgrounduselnfcolours=\"true\" outli"
"nebackgroundfilename=\"gfx/surround_knob_line.png\">\n"
"          <image filename=\"gfx/Knob1a_black.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1B - White with bipolar LED surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Surround.png\" backgrounduselnfcolours=\"true\" outli"
"nebackgroundfilename=\"gfx/surround_knob_line.png\">\n"
"          <image filename=\"gfx/Knob1b_white.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1B - Black with bipolar LED surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Surround.png\" backgrounduselnfcolours=\"true\" outli"
"nebackgroundfilename=\"gfx/surround_knob_line.png\">\n"
"          <image filename=\"gfx/Knob1b_black.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1C - White with dotted surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/dotted_surround.png\">\n"
"          <image filename=\"gfx/Knob1c_white.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1C - White with lined surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/lined_surround1.png\">\n"
"          <image filename=\"gfx/Knob1c_white.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1C - White with numbered surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/numbered_surround1.png\">\n"
"          <image filename=\"gfx/Knob1c_white.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1C - White with alt numbered surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/numbered_surround3.png\">\n"
"          <image filename=\"gfx/Knob1c_white.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1C - White with Bipolar surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/Bipolar-surround1.png\">\n"
"          <image filename=\"gfx/Knob1c_white.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1C - Black with dotted surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/dotted_surround.png\">\n"
"          <image filename=\"gfx/Knob1c_black.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1C - Black with lined surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/lined_surround1.png\">\n"
"          <image filename=\"gfx/Knob1c_black.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1C - Black with numbered surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/numbered_surround1.png\">\n"
"          <image filename=\"gfx/Knob1c_black.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1C - Black with alt numbered surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/numbered_surround3.png\">\n"
"          <image filename=\"gfx/Knob1c_black.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1C - Black with Bipolar surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/Bipolar-surround1.png\">\n"
"          <image filename=\"gfx/Knob1c_black.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1D - White with dotted surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/dotted_surround.png\">\n"
"          <image filename=\"gfx/Knob1d_white.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1D - White with lined surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/lined_surround1.png\">\n"
"          <image filename=\"gfx/Knob1d_white.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1D - White with numbered surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/numbered_surround1.png\">\n"
"          <image filename=\"gfx/Knob1d_white.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1D - White with alt numbered surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/numbered_surround3.png\">\n"
"          <image filename=\"gfx/Knob1d_white.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1D - White with Bipolar surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/Bipolar-surround1.png\">\n"
"          <image filename=\"gfx/Knob1d_white.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1D - Black with dotted surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/dotted_surround.png\">\n"
"          <image filename=\"gfx/Knob1d_black.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1D - Black with lined surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/lined_surround1.png\">\n"
"          <image filename=\"gfx/Knob1d_black.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1D - Black with numbered surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/numbered_surround1.png\">\n"
"          <image filename=\"gfx/Knob1d_black.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1D - Black with alt numbered surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/numbered_surround3.png\">\n"
"          <image filename=\"gfx/Knob1d_black.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1D - Black with Bipolar surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/Bipolar-surround1.png\">\n"
"          <image filename=\"gfx/Knob1d_black.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1E - Black with dotted surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/dotted_surround.png\">\n"
"          <image filename=\"gfx/Knob1e_black.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1E - Black with lined surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/lined_surround1.png\">\n"
"          <image filename=\"gfx/Knob1e_black.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1E - Black with numbered surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/numbered_surround1.png\">\n"
"          <image filename=\"gfx/Knob1e_black.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1E - Black with alt numbered surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/numbered_surround3.png\">\n"
"          <image filename=\"gfx/Knob1e_black.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 1E - Black with Bipolar surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/Bipolar-surround1.png\">\n"
"          <image filename=\"gfx/Knob1e_black.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 2A - Large Black knob with Coloured Cap\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"\">\n"
"          <image filename=\"gfx/Knob2a_matte.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <colours>\n"
"      <label textcolourid=\"fffa56c9\"></label>\n"
"    </colours>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 2B - Large Black knob with Metallic Cap\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"\">\n"
"          <image filename=\"gfx/Knob2b_metallic.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <colours>\n"
"      <label textcolourid=\"fffa56c9\"></label>\n"
"    </colours>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 3A - Small Black knob with dotted surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/dotted_surround.png\">\n"
"          <image filename=\"gfx/Knob3a_matte.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 3A - Small Black knob with lined surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/lined_surround1.png\">\n"
"          <image filename=\"gfx/Knob3a_matte.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 3A - Small Black knob with numbered surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/numbered_surround2.png\">\n"
"          <image filename=\"gfx/Knob3a_matte.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 3A - Small Black knob with alt numbered surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/numbered_surround4.png\">\n"
"          <image filename=\"gfx/Knob3a_matte.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 3A - Small Black knob with Bipolar surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/Bipolar-surround1.png\">\n"
"          <image filename=\"gfx/Knob3a_matte.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 3B - Small Black knob with dotted surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/dotted_surround.png\">\n"
"          <image filename=\"gfx/Knob3b_metallic.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 3B - Small Black knob with lined surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/lined_surround1.png\">\n"
"          <image filename=\"gfx/Knob3b_metallic.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 3B - Small Black knob with numbered surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/numbered_surround2.png\">\n"
"          <image filename=\"gfx/Knob3b_metallic.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 3B - Small Black knob with alt numbered surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/numbered_surround4.png\">\n"
"          <image filename=\"gfx/Knob3b_metallic.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 3B - Small Black knob with Bipolar surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/Bipolar-surround1.png\">\n"
"          <image filename=\"gfx/Knob3b_metallic.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 4 - Black knob with dotted surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/dotted_surround.png\">\n"
"          <image filename=\"gfx/Knob4.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 4 - Black knob with lined surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/lined_surround1.png\">\n"
"          <image filename=\"gfx/Knob4.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 4 - Black knob with numbered surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/numbered_surround1.png\">\n"
"          <image filename=\"gfx/Knob4.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 4 - Black knob with alt numbered surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/numbered_surround3.png\">\n"
"          <image filename=\"gfx/Knob4.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"Knob 4 - Black knob with Bipolar surround\" parentid=\"JUCE knob\">\n"
"    <images>\n"
"      <slider>\n"
"        <rotary fillbackgroundfilename=\"gfx/Knob1_Cap.png\" backgrounduselnfcolours=\"true\" outlinebac"
"kgroundfilename=\"gfx/Bipolar-surround1.png\">\n"
"          <image filename=\"gfx/Knob4.png\" numframes=\"63\" ishorizontal=\"false\" />\n"
"        </rotary>\n"
"      </slider>\n"
"    </images>\n"
"    <appliesto componenttype=\"slider\" />\n"
"  </lookandfeel>\n"
"</lookandfeels>";

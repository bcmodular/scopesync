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
    #include "../../ScopeSyncPlugin/Source/PluginProcessor.h"
#else
    #include "../../ScopeSyncFX/Source/ScopeFX.h"
#endif // __DLL_EFFECT__

int          ScopeSync::performanceModeGlobalDisable = 0;
const String ScopeSync::scopeSyncVersionString = "0.5.0-Prerelease";

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
Y1,Y2,Y3,Y4,Y5,Y6,Y7,Y8,\
Z1,Z2,Z3,Z4,Z5,Z6,Z7,Z8,\
X,Y",
",",""
);

const String& ScopeSync::getScopeCode(int scopeCodeId) { return scopeCodes[scopeCodeId]; }

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
	UserSettings::getInstance()->removeActionListener(this);        
    hideConfigurationManager();
    scopeSyncInstances.removeAllInstancesOf(this);
}

void ScopeSync::initialise()
{
	parameterController = new BCMParameterController(this);

    setPerformanceMode(0);

	setDeviceType(0);
	setShowPatchWindow(true);
	setShowPresetWindow(false);

	showEditToolbar = false;
    commandManager = new ApplicationCommandManager();

    
    configuration = new Configuration();
    applyConfiguration();

	ScopeSyncOSCServer::getInstance()->setup();

	initialised = true;
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

void ScopeSync::setPerformanceModeAll(int newSetting)
{
	for (int i = 0; i < getNumScopeSyncInstances(); i++)
		scopeSyncInstances[i]->setPerformanceMode(newSetting);
}

int  ScopeSync::getOSCUID() { return parameterController->getOSCUID(); }
void ScopeSync::setOSCUID(int uid) { parameterController->setOSCUID(uid); }
void ScopeSync::referToOSCUID(Value& valueToLink) { parameterController->referToOSCUID(valueToLink); }
	
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
        parameterController->addParameter(i, parameterTree.getChild(i), BCMParameter::regular);

#ifndef __DLL_EFFECT__
    pluginProcessor->updateHostDisplay();
#endif // __DLL_EFFECT__

#ifndef __DLL_EFFECT__
    parameterController->restoreParameterValues();
#else
	scopeSyncAsync.snapshot();
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
"<component id=\"header\">\n"
"  <bounds relativerectangle=\"right - parent.width, 1, parent.width, top + 40\"></bounds>\n"
"  <label lfid=\"system:configname\" name=\"configurationname\" text=\"Current Configuration\">\n"
"    <bounds x=\"177\" y=\"11\" width=\"160\" height=\"17\"></bounds>\n"
"    <font bold=\"true\" height=\"12\"></font>\n"
"  </label>\n"
"  <textbutton lfid=\"system:scopesynclogo_button\" name=\"showaboutbox\" id=\"showaboutbox\" text=\"\" toolt"
"ip=\"Show About Box\">\n"
"    <bounds relativerectangle=\"13, 11, left + 151, top + 21\"></bounds>\n"
"  </textbutton>\n"
"  <textbutton lfid=\"system:new_config_button\" name=\"newconfiguration\" tooltip=\"New Configuration\">\n"
"    <bounds x=\"347\" y=\"9\" width=\"21\" height=\"21\"></bounds>\n"
"  </textbutton>\n"
"  <textbutton lfid=\"system:load_config_button\" name=\"chooseconfiguration\" tooltip=\"Load Configuratio"
"n\">\n"
"    <bounds x=\"373\" y=\"9\" width=\"21\" height=\"21\"></bounds>\n"
"  </textbutton>\n"
"  <textbutton lfid=\"system:reload_button\" name=\"reloadconfiguration\" tooltip=\"Reload Current Configu"
"ration\">\n"
"    <bounds x=\"399\" y=\"9\" width=\"21\" height=\"21\"></bounds>\n"
"  </textbutton>\n"
"  <textbutton lfid=\"system:showconfigurationmanager_button\" name=\"showconfigurationmanager\" id=\"show"
"configurationmanager\" text=\"\" tooltip=\"Open Configuration Manager panel\">\n"
"    <bounds x=\"425\" y=\"9\" width=\"21\" height=\"21\"></bounds>\n"
"  </textbutton>\n"
"  <textbutton lfid=\"system:settings_button\" name=\"showusersettings\" id=\"showusersettings\" text=\"\" to"
"oltip=\"Open User Settings panel\">\n"
"    <bounds relativerectangle=\"right - 32, showconfigurationmanager.top, parent.width - 1, top + 21\""
"></bounds>\n"
"  </textbutton>\n"
"  <textbutton lfid=\"system:snapshot_button\" name=\"snapshot\" text=\"\" tooltip=\"Send a snapshot of all "
"current parameter values. Ctrl-click to send snapshot for all ScopeSync instances\">\n"
"    <bounds relativerectangle=\"right - 32, showusersettings.top, showusersettings.left - 5, top + 21"
"\"></bounds>\n"
"  </textbutton>\n"
"  <!--Shows Patch and Preset Buttons for BC Modular shell based on CP<->Host connection state-->\n"
"  <tabbedcomponent displaycontext=\"scope\" name=\"Device Type\" showdropshadow=\"false\">\n"
"    <bounds relativerectangle=\"right - 58, 6, parent.width - 70, top + 32\"></bounds>\n"
"    <tabbar orientation=\"right\" depth=\"0\"></tabbar>\n"
"    <tab idx=\"1\" name=\"DISCONNECTED\">\n"
"      <component>\n"
"        <!--No Controls-->\n"
"      </component>\n"
"    </tab>\n"
"    <tab idx=\"2\" name=\"CONNECTED\">\n"
"      <component>\n"
"        <textbutton lfid=\"system:patch_button\" name=\"PatchWindow\">\n"
"          <bounds x=\"0\" y=\"2\" width=\"27\" height=\"21\"></bounds>\n"
"        </textbutton>\n"
"        <textbutton lfid=\"system:presets_button\" name=\"PresetList\">\n"
"          <bounds x=\"33\" y=\"2\" width=\"19\" height=\"21\"></bounds>\n"
"        </textbutton>\n"
"      </component>\n"
"    </tab>\n"
"  </tabbedcomponent>\n"
"</component>";

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

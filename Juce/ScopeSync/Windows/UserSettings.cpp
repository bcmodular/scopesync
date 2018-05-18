/*
 * User Settings popup handler.
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

#include "UserSettings.h"
#include "../Core/Global.h"
#include "../Core/ScopeSyncApplication.h"
#include "../Core/ScopeSync.h"
#include "../Utils/BCMMisc.h"
#include "FileLocationEditor.h"
#include "../Presets/PresetManager.h"

/* =========================================================================
 * EncoderSnapProperty
 */
class EncoderSnapProperty : public ChoicePropertyComponent
{
public:
    EncoderSnapProperty(UserSettings& userSettings) : ChoicePropertyComponent("Slider Snap"), owner(userSettings)
    {
        choices.add ("No Override");
        choices.add ("Don\'t Snap");
        choices.add ("Snap");
    }

    void setIndex (int newIndex) override { owner.setPropertyIntValue("encodersnap", newIndex); }
    int  getIndex() const override { return owner.getPropertyIntValue("encodersnap", 0); }

private:
    UserSettings& owner;
};

/* =========================================================================
 * RotaryMovementProperty
 */
class RotaryMovementProperty : public ChoicePropertyComponent
{
public:
    RotaryMovementProperty(UserSettings& userSettings) : ChoicePropertyComponent("Rotary Slider Movement"), owner(userSettings)
    {
        choices.add ("No Override");
        choices.add ("Rotary");
        choices.add ("Vertical");
        choices.add ("Horizontal");
        choices.add ("Horizontal & Vertical");
    }

    void setIndex (int newIndex) override { owner.setPropertyIntValue("rotarymovement", newIndex); }
    int  getIndex() const override { return owner.getPropertyIntValue("rotarymovement", 0); }

private:
    UserSettings& owner;
};

/* =========================================================================
 * IncDecButtonModeProperty
 */
class IncDecButtonModeProperty : public ChoicePropertyComponent
{
public:
    IncDecButtonModeProperty(UserSettings& userSettings) : ChoicePropertyComponent("Inc/Dec Button Mode"), owner(userSettings)
    {
        choices.add ("No Override");
        choices.add ("Not Draggable");
        choices.add ("Auto Direction");
        choices.add ("Horizontal");
        choices.add ("Vertical");
    }

    void setIndex (int newIndex) override { owner.setPropertyIntValue("incdecbuttonmode", newIndex); }
    int  getIndex() const override { return owner.getPropertyIntValue("incdecbuttonmode", 0); }

private:
    UserSettings& owner;
};

/* =========================================================================
 * PopupEnabledProperty
 */
class PopupEnabledProperty : public ChoicePropertyComponent
{
public:
    PopupEnabledProperty(UserSettings& userSettings) : ChoicePropertyComponent("Slider Popup Enabled"), owner(userSettings)
    {
        choices.add ("No Override");
        choices.add ("Enabled");
        choices.add ("Disabled");
    }

    void setIndex (int newIndex) override { owner.setPropertyIntValue("popupenabled", newIndex); }
    int  getIndex() const override { return owner.getPropertyIntValue("popupenabled", 0); }

private:
    UserSettings& owner;
};

/* =========================================================================
 * EncoderVelocityModeProperty
 */
class EncoderVelocityModeProperty : public ChoicePropertyComponent
{
public:
    EncoderVelocityModeProperty(UserSettings& userSettings) : ChoicePropertyComponent("Slider Velocity Mode"), owner(userSettings)
    {
        choices.add ("No Override");
        choices.add ("Enabled");
        choices.add ("Disabled");
    }

    void setIndex (int newIndex) override { owner.setPropertyIntValue("velocitybasedmode", newIndex); }
    int  getIndex() const override { return owner.getPropertyIntValue("velocitybasedmode", 0); }

private:
    UserSettings& owner;
};

/* =========================================================================
 * EnableTooltipsProperty
 */
class EnableTooltipsProperty : public ChoicePropertyComponent
{
public:
    EnableTooltipsProperty(UserSettings& userSettings) : ChoicePropertyComponent("Enable Tooltips"), owner(userSettings)
    {
        choices.add ("No Override");
        choices.add ("Enabled");
        choices.add ("Disabled");
    }

    void setIndex (int newIndex) override { owner.setPropertyIntValue("enabletooltips", newIndex); }
    int  getIndex() const override { return owner.getPropertyIntValue("enabletooltips", 0); }

private:
    UserSettings& owner;
};

/* =========================================================================
 * UserSettings
 */
UserSettings::UserSettings()
    : fileLocationsButton("File Locations..."),
      presetManagerButton("Preset Manager...")
      
{
    commandManager = new ApplicationCommandManager();

    commandManager->registerAllCommandsForTarget(this);
    addKeyListener(commandManager->getKeyMappings());

    PropertiesFile::Options options;
    options.applicationName     = ProjectInfo::projectName;
    options.folderName          = "ScopeSync";
    options.filenameSuffix      = "settings";
    options.osxLibrarySubFolder = "Application Support";
    appProperties.setStorageParameters(options);

    options.applicationName     = "ScopeSync";
    options.folderName          = "ScopeSync";
    globalProperties.setStorageParameters(options);

    loadSwatchColours();
   
    addAndMakeVisible(propertyPanel);
    setWantsKeyboardFocus(true);
    propertyPanel.setWantsKeyboardFocus(true);

    Component::setName("User Settings");

	pluginHost.addListener(this);
	pluginHost.setValue(getPropertyValue("pluginhost", "127.0.0.1"));
    
	pluginListenerPort.addListener(this);
	pluginListenerPort.setValue(getPropertyIntValue("pluginlistenerport", 8002));
    
	scopeHost.addListener(this);
	scopeHost.setValue(getPropertyValue("scopehost", "127.0.0.1"));
    
	scopeSyncModuleListenerPort.addListener(this);
	scopeSyncModuleListenerPort.setValue(getPropertyIntValue("scopesyncmodulelistenerport", 8001));
    
	scopeOSCReceiverListenerPort.addListener(this);
	scopeOSCReceiverListenerPort.setValue(getPropertyIntValue("scopeoscreceiverlistenerport",  8000));
	    
    tooltipDelayTime.addListener(this);
	tooltipDelayTime.setValue(getPropertyIntValue("tooltipdelaytime", -1));
    
    useImageCache.addListener(this);
	useImageCache.setValue(getPropertyBoolValue("enableimagecache", true));
    
	autoRebuildLibrary.addListener(this);
	autoRebuildLibrary.setValue(getPropertyBoolValue("autorebuildlibrary", false));
	
    setupPanel();

    addAndMakeVisible(fileLocationsButton);
    fileLocationsButton.setCommandToTrigger(commandManager, CommandIDs::editFileLocations, true);
    
    addAndMakeVisible(presetManagerButton);
    presetManagerButton.setCommandToTrigger(commandManager, CommandIDs::showPresetManager, true);
    
    setSize(getLocalBounds().getWidth(), getLocalBounds().getHeight());

    startTimer(500);
}

UserSettings::~UserSettings()
{
    removeKeyListener(commandManager->getKeyMappings());
    stopTimer();

    saveSwatchColours();

    presetManagerWindow.reset();
    fileLocationEditorWindow.reset();
    tooltipDelayTime.removeListener(this);
}

void UserSettings::setupPanel()
{
    PropertyListBuilder props;
    
    props.clear();
    props.add(new EnableTooltipsProperty(*this),                                "Choose whether tooltips are displayed when hovering over GUI elements");
    props.add(new IntRangeProperty(tooltipDelayTime, "Tooltip Delay Time", -1), "Enter the number of milliseconds to wait before a tooltip is displayed over GUI elements (-1 to use defaults)");

    propertyPanel.addSection("Tooltip Settings", props.components, true);
    
    props.clear();
    props.add(new EncoderSnapProperty(*this),         "Choose whether sliders snap to valid parameter values");
    props.add(new RotaryMovementProperty(*this),      "Choose which mouse movement type is to be used by rotary sliders");
    props.add(new IncDecButtonModeProperty(*this),    "Choose the mode for Inc/Dec button style sliders");
    props.add(new PopupEnabledProperty(*this),        "Choose whether sliders show a popup with current value when dragging");
    props.add(new EncoderVelocityModeProperty(*this), "Choose whether Velocity Based Mode is enabled for sliders");
    
    propertyPanel.addSection("Slider Settings", props.components, true);
	
	props.clear();
	props.add(new TextPropertyComponent(pluginHost, "Plugin/DAW Host", 256, false), "Enter the host name or IP address for the computer that the ScopeSync Plugin is running on (leave as 127.0.0.1 if Scope and DAW on same machine)");
	props.add(new IntRangeProperty(pluginListenerPort, "Plugin/DAW Listener Port", 1, 65535),  "Enter the port number that the ScopeSync Plugin should listen on (defaults to 8002)");
	props.add(new TextPropertyComponent(scopeHost, "Scope Host", 256, false), "Enter the host name or IP address for the computer that Scope is running on (leave as 127.0.0.1 if Scope and DAW on same machine)");
	props.add(new IntRangeProperty(scopeSyncModuleListenerPort, "ScopeSync Module Listener Port", 1, 65535), "Enter the port number that the ScopeSync module in Scope should listen on (defaults to 8001)");
	props.add(new IntRangeProperty(scopeOSCReceiverListenerPort, "Scope OSC Receiver Listener Port", 1, 65535), "Enter the port number that the OSC Receiver module in Scope should listen on (defaults to 8000)");

	propertyPanel.addSection("Network Settings", props.components, true);

	props.clear();
    props.add(new BooleanPropertyComponent(useImageCache,      "Image Cache",                   "Enabled"), "Disabling the Image Cache will mean that images will be refreshed immediately, but will slow down the GUI rendering");
	props.add(new BooleanPropertyComponent(autoRebuildLibrary, "Automatically Rebuild Library", "Enabled"), "Automatically rebuild the relevant library on opening a Chooser window. Depending on size of library, enabling this may cause a performance problem.");
	
    propertyPanel.addSection("Expert Settings", props.components, false);
}

void UserSettings::paint (Graphics& g)
{
    g.fillAll (Colour (0xff434343));

    g.setColour(Colours::lightgrey);

    Rectangle<int> localBounds(getLocalBounds().reduced(4));
    g.fillRect(localBounds);
}

void UserSettings::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    
    localBounds.removeFromLeft(4);
    localBounds.removeFromRight(8);
    localBounds.removeFromTop(8);

    Rectangle<int> toolbarButtons = localBounds.removeFromTop(40);
    toolbarButtons.removeFromLeft(4);

    fileLocationsButton.setBounds(toolbarButtons.removeFromLeft(140).reduced(4, 4));
    presetManagerButton.setBounds(toolbarButtons.removeFromLeft(140).reduced(4, 4));

    localBounds.removeFromTop(5);
    propertyPanel.setBounds(localBounds.reduced(4, 4));
}

String UserSettings::getPropertyValue(const String& propertyName, const String& defaultValue)
{
	return getAppProperties()->getValue(propertyName, defaultValue);
}

void UserSettings::setPropertyValue(const String& propertyName, const String& newValue)
{
	getAppProperties()->setValue(propertyName, newValue);
}

int UserSettings::getPropertyIntValue(const String& propertyName, int defaultValue)
{
	return getAppProperties()->getIntValue(propertyName, defaultValue);
}

void UserSettings::setPropertyIntValue(const String& propertyName, int newValue)
{
    getAppProperties()->setValue(propertyName, newValue);
}

bool UserSettings::getPropertyBoolValue(const String& propertyName, bool defaultValue)
{
    return getAppProperties()->getBoolValue(propertyName, defaultValue);
}

void UserSettings::setPropertyBoolValue(const String& propertyName, bool newValue)
{
    getAppProperties()->setValue(propertyName, newValue);
}

void UserSettings::referToPluginOSCSettings(Value& localPort, Value& remoteHost, Value& remotePort) const
{
	localPort.referTo(pluginListenerPort);
	remoteHost.referTo(scopeHost);
	remotePort.referTo(scopeOSCReceiverListenerPort);
}

#ifdef __DLL_EFFECT__
void UserSettings::referToScopeSyncOSCSettings(Value& localPort, Value& remotePort) const
{
	localPort.referTo(scopeSyncModuleListenerPort);
	remotePort.referTo(scopeOSCReceiverListenerPort);
}

void UserSettings::referToScopeFXOSCSettings(Value& plugHost, Value& plugPort, Value& scopeSyncPort) const
{
	plugHost.referTo(pluginHost);
	plugPort.referTo(pluginListenerPort);
	scopeSyncPort.referTo(scopeSyncModuleListenerPort);
}
#endif //__DLL_EFFECT__

void UserSettings::valueChanged(Value& valueThatChanged)
{
    if (valueThatChanged.refersToSameSourceAs(tooltipDelayTime))
	{
        setPropertyIntValue("tooltipdelaytime", valueThatChanged.getValue());
		return;
	}

    if (valueThatChanged.refersToSameSourceAs(useImageCache))
	{
        setPropertyBoolValue("useimagecache", valueThatChanged.getValue());
		return;
	}

	if (valueThatChanged.refersToSameSourceAs(autoRebuildLibrary))
	{
		setPropertyBoolValue("autorebuildlibrary", valueThatChanged.getValue());
		return;
	}
	
	if (valueThatChanged.refersToSameSourceAs(pluginHost))
	{
		setPropertyValue("pluginhost", valueThatChanged.getValue());
		return;
	}

	if (valueThatChanged.refersToSameSourceAs(pluginListenerPort))
	{
		setPropertyIntValue("pluginlistenerport", valueThatChanged.getValue());
		return;
	}
	
	if (valueThatChanged.refersToSameSourceAs(scopeHost))
	{
		setPropertyValue("scopehost", valueThatChanged.getValue());
		return;
	}

	if (valueThatChanged.refersToSameSourceAs(scopeSyncModuleListenerPort))
	{
		setPropertyIntValue("scopesyncmodulelistenerport", valueThatChanged.getValue());
		return;
	}

	if (valueThatChanged.refersToSameSourceAs(scopeOSCReceiverListenerPort))
	{
		setPropertyIntValue("scopeoscreceiverlistenerport", valueThatChanged.getValue());
		return;
	}
}

void UserSettings::userTriedToCloseWindow()
{
    hideFileLocationsWindow();
    ScopeSync::reloadAllGUIs(); 
    removeFromDesktop();
}

PropertiesFile* UserSettings::getAppProperties()
{
    return appProperties.getUserSettings();
}

PropertiesFile* UserSettings::getGlobalProperties()
{
    return globalProperties.getUserSettings();
}

void UserSettings::show(int posX, int posY)
{
    setOpaque(true);
    setVisible(true);
    
    setBounds(posX, posY, 600, 350);
    
    addToDesktop(ComponentPeer::windowHasTitleBar | ComponentPeer::windowHasCloseButton | ComponentPeer::windowHasDropShadow, nullptr);
    
    if (ScopeSyncApplication::inScopeFXContext())
        setAlwaysOnTop(true);

    toFront(true);
}
    
void UserSettings::hide()
{
    hideFileLocationsWindow();
    removeFromDesktop();
}

ValueTree UserSettings::getFileLocations()
{
    return getValueTreeFromGlobalProperties("fileLocations");
}

ValueTree UserSettings::getLayoutLibrary()
{
    return getValueTreeFromGlobalProperties("layoutLibrary");
}

ValueTree UserSettings::getConfigurationLibrary()
{
    return getValueTreeFromGlobalProperties("configurationLibrary");
}

ValueTree UserSettings::getPresetLibrary()
{
    return getValueTreeFromGlobalProperties("presetLibrary");
}

ValueTree UserSettings::getLayoutFromFilePath(const String& filePath, const ValueTree& layoutLibrary)
{
    for (int i = 0; i < layoutLibrary.getNumChildren(); i++)
    {
        ValueTree layout = layoutLibrary.getChild(i);

        if (layout.getProperty(Ids::filePath).toString().equalsIgnoreCase(filePath))
            return layout;
    }
    
    return ValueTree();
}

String UserSettings::getLayoutFilename(const String& name, const String& librarySet)
{
    ValueTree layoutLibrary = getLayoutLibrary();

    for (int i = 0; i < layoutLibrary.getNumChildren(); i++)
    {
        ValueTree layout = layoutLibrary.getChild(i);

        if (   layout.getProperty(Ids::name).toString().equalsIgnoreCase(name)
            && layout.getProperty(Ids::librarySet).toString().equalsIgnoreCase(librarySet))
        {
            return layout.getProperty(Ids::filePath);
        }
    }

    return String();
}

ValueTree UserSettings::getConfigurationFromFilePath(const String& filePath, const ValueTree& configurationLibrary)
{
    for (int i = 0; i < configurationLibrary.getNumChildren(); i++)
    {
        ValueTree configuration = configurationLibrary.getChild(i);

        if (configuration.getProperty(Ids::filePath).toString().equalsIgnoreCase(filePath))
            return configuration;
    }
    
    return ValueTree();
}

String UserSettings::getConfigurationFilePathFromUID(int uid)
{
    for (int i = 0; i < getConfigurationLibrary().getNumChildren(); i++)
    {
        ValueTree configuration = getConfigurationLibrary().getChild(i);

        if (int(configuration.getProperty(Ids::UID)) == uid)
            return configuration.getProperty(Ids::filePath);
    }

    return String();
}

ValueTree UserSettings::getPresetFileFromFilePath(const String& filePath, const ValueTree& presetLibrary)
{
    for (int i = 0; i < presetLibrary.getNumChildren(); i++)
    {
        ValueTree presetFile = presetLibrary.getChild(i);

        if (   presetFile.hasType(Ids::presetFile) 
            && presetFile.getProperty(Ids::filePath).toString().equalsIgnoreCase(filePath))
            return presetFile;
    }
    
    return ValueTree();
}

ValueTree UserSettings::getPresetFromNameAndFilePath(const String& name, const String& filePath, const ValueTree& presetLibrary)
{
    for (int i = 0; i < presetLibrary.getNumChildren(); i++)
    {
        ValueTree preset = presetLibrary.getChild(i);

        if (   preset.hasType(Ids::preset)
            && preset.getProperty(Ids::name).toString().equalsIgnoreCase(name)
            && preset.getProperty(Ids::filePath).toString().equalsIgnoreCase(filePath))
            return preset;
    }
    
    return ValueTree();
}

void UserSettings::setLastTimeLayoutLoaded(const String& filePath)
{
    ValueTree layoutLibrary = getLayoutLibrary();
    ValueTree layout = getLayoutFromFilePath(filePath, layoutLibrary);

    if (layout.isValid())
    {
        layout.setProperty(Ids::mruTime, Time::getCurrentTime().formatted("%Y-%m-%d %H:%M:%S"), nullptr);
        
        ScopedPointer<XmlElement> xml = layoutLibrary.createXml();
        getGlobalProperties()->setValue(Ids::layoutLibrary.toString(), xml);
    }
}

void UserSettings::setLastTimeConfigurationLoaded(const String& filePath)
{
    ValueTree configurationLibrary = getConfigurationLibrary();
    ValueTree configuration = getConfigurationFromFilePath(filePath, configurationLibrary);

    if (configuration.isValid())
    {
        configuration.setProperty(Ids::mruTime, Time::getCurrentTime().formatted("%Y-%m-%d %H:%M:%S"), nullptr);
        
        ScopedPointer<XmlElement> xml = configurationLibrary.createXml();
        getGlobalProperties()->setValue(Ids::configurationLibrary.toString(), xml);
    }
}

void UserSettings::updateLayoutLibraryEntry(const String&     filePath,
                                            const XmlElement& sourceXmlElement,
                                            ValueTree&        layoutLibrary)
{
    ValueTree layout = getLayoutFromFilePath(filePath, layoutLibrary);

    if (!layout.isValid())
    {
        layout = ValueTree(Ids::layout);
        layoutLibrary.addChild(layout, -1, nullptr);
    }

    layout.setProperty(Ids::name,               sourceXmlElement.getStringAttribute(Ids::name), nullptr);
    layout.setProperty(Ids::librarySet,         sourceXmlElement.getStringAttribute(Ids::libraryset), nullptr);
    layout.setProperty(Ids::author,             sourceXmlElement.getStringAttribute(Ids::author), nullptr);
    layout.setProperty(Ids::numButtons,         sourceXmlElement.getIntAttribute(Ids::numbuttons), nullptr);
    layout.setProperty(Ids::numEncoders,        sourceXmlElement.getIntAttribute(Ids::numencoders), nullptr);
    layout.setProperty(Ids::numFaders,          sourceXmlElement.getIntAttribute(Ids::numfaders), nullptr);
    layout.setProperty(Ids::panelWidth,         sourceXmlElement.getIntAttribute(Ids::panelwidth), nullptr);
    layout.setProperty(Ids::panelHeight,        sourceXmlElement.getIntAttribute(Ids::panelheight), nullptr);
    layout.setProperty(Ids::numParameters,      sourceXmlElement.getIntAttribute(Ids::numparameters), nullptr);
    layout.setProperty(Ids::thumbnail,          sourceXmlElement.getStringAttribute(Ids::thumbnail), nullptr);
    layout.setProperty(Ids::excludeFromChooser, sourceXmlElement.getBoolAttribute(Ids::excludefromchooser, false), nullptr);
    layout.setProperty(Ids::blurb,              sourceXmlElement.getStringAttribute(Ids::blurb), nullptr);
    layout.setProperty(Ids::filePath,           filePath, nullptr);
}

void UserSettings::updateConfigurationLibraryEntry(const String&    filePath,
                                                   const String&    fileName,
                                                   const ValueTree& sourceValueTree)
{
    ValueTree configurationLibrary = getConfigurationLibrary();
    updateConfigurationLibraryEntry(filePath, fileName, sourceValueTree, configurationLibrary);

    ScopedPointer<XmlElement> xml = configurationLibrary.createXml();
    getGlobalProperties()->setValue(Ids::configurationLibrary.toString(), xml);
}

void UserSettings::updateConfigurationLibraryEntry(const String&    filePath,
                                                   const String&    fileName,
                                                   const ValueTree& sourceValueTree,
                                                   ValueTree&       configurationLibrary)
{
    ValueTree configuration = getConfigurationFromFilePath(filePath, configurationLibrary);

    if (!configuration.isValid())
    {
        configuration = ValueTree(Ids::configuration);
        configurationLibrary.addChild(configuration, -1, nullptr);
    }

    configuration.setProperty(Ids::name,               sourceValueTree.getProperty(Ids::name), nullptr);
    configuration.setProperty(Ids::librarySet,         sourceValueTree.getProperty(Ids::librarySet), nullptr);
    configuration.setProperty(Ids::author,             sourceValueTree.getProperty(Ids::author), nullptr);
    configuration.setProperty(Ids::layoutName,         sourceValueTree.getProperty(Ids::layoutName), nullptr);
    configuration.setProperty(Ids::layoutLibrarySet,   sourceValueTree.getProperty(Ids::layoutLibrarySet), nullptr);
    configuration.setProperty(Ids::excludeFromChooser, sourceValueTree.getProperty(Ids::excludeFromChooser), nullptr);
    configuration.setProperty(Ids::blurb,              sourceValueTree.getProperty(Ids::blurb), nullptr);
    configuration.setProperty(Ids::ID,                 sourceValueTree.getProperty(Ids::ID), nullptr);
    configuration.setProperty(Ids::UID,                sourceValueTree.getProperty(Ids::UID), nullptr);
    configuration.setProperty(Ids::filePath,           filePath, nullptr);
    configuration.setProperty(Ids::fileName,           fileName, nullptr);
}

void UserSettings::updatePresetLibraryEntry(const String&    filePath,
                                            const String&    fileName,
                                            const ValueTree& sourceValueTree)
{
    ValueTree presetLibrary = getPresetLibrary();
    updatePresetLibraryEntry(filePath, fileName, sourceValueTree, presetLibrary);

    ScopedPointer<XmlElement> xml = presetLibrary.createXml();
    getGlobalProperties()->setValue(Ids::presetLibrary.toString(), xml);

}

void UserSettings::updatePresetLibraryEntry(const String&    filePath,
                                            const String&    fileName,
                                            const ValueTree& sourceValueTree,
                                                  ValueTree& presetLibrary)
{
    ValueTree presetFile = getPresetFileFromFilePath(filePath, presetLibrary);

    if (!presetFile.isValid())
    {
        presetFile = ValueTree(Ids::presetFile);
        presetLibrary.addChild(presetFile, -1, nullptr);
    }

    presetFile.setProperty(Ids::name,       sourceValueTree.getProperty(Ids::name), nullptr);
    presetFile.setProperty(Ids::librarySet, sourceValueTree.getProperty(Ids::librarySet), nullptr);
    presetFile.setProperty(Ids::author,     sourceValueTree.getProperty(Ids::author), nullptr);
    presetFile.setProperty(Ids::blurb,      sourceValueTree.getProperty(Ids::blurb), nullptr);
    presetFile.setProperty(Ids::filePath,   filePath, nullptr);
    presetFile.setProperty(Ids::fileName,   fileName, nullptr);

    updatePresets(filePath, fileName, sourceValueTree, presetLibrary);
}

void UserSettings::updatePresets(const String&    filePath,
                                 const String&    fileName,
                                 const ValueTree& sourceValueTree,
                                       ValueTree& presetLibrary)
{
    // Firstly trim Presets that no longer exist in this file
    for (int i = presetLibrary.getNumChildren() - 1; i >= 0 ; i--)
    {
        ValueTree presetLibraryItem = presetLibrary.getChild(i);

        if (   presetLibraryItem.hasType(Ids::preset) 
            && presetLibraryItem.getProperty(Ids::filePath).toString().equalsIgnoreCase(filePath))
        {
            String presetName = presetLibraryItem.getProperty(Ids::name);

            if (sourceValueTree.getChildWithProperty(Ids::name, presetName).isValid())
                continue;
            else
                presetLibrary.removeChild(i, nullptr);
        }
        else
            continue;
    }

    // Now update/add the set of Presets from this file
    for (int i = 0; i < sourceValueTree.getNumChildren(); i++)
    {
        ValueTree sourceItem = sourceValueTree.getChild(i);
        String presetName    = sourceItem.getProperty(Ids::name);
        
        ValueTree preset = getPresetFromNameAndFilePath(presetName, filePath, presetLibrary);
        
        if (!preset.isValid())
        {
            preset = ValueTree(Ids::preset);
            presetLibrary.addChild(preset, -1, nullptr);
        }

        preset.copyPropertiesFrom(sourceItem, nullptr);
        
        ValueTree settings = sourceItem.getChildWithName(Ids::settings);
        preset.removeAllChildren(nullptr);

        if (settings.isValid())
            preset.addChild(settings.createCopy(), -1, nullptr);

        preset.setProperty(Ids::presetFileName, sourceValueTree.getProperty(Ids::name), nullptr);
        preset.setProperty(Ids::presetFileLibrarySet, sourceValueTree.getProperty(Ids::librarySet), nullptr);
        preset.setProperty(Ids::presetFileAuthor, sourceValueTree.getProperty(Ids::author), nullptr);
        preset.setProperty(Ids::presetFileBlurb, sourceValueTree.getProperty(Ids::blurb), nullptr);
        preset.setProperty(Ids::filePath, filePath, nullptr);
        preset.setProperty(Ids::fileName, fileName, nullptr);
    }
}

ValueTree UserSettings::getValueTreeFromGlobalProperties(const String& valueTreeToGet)
{
    ScopedPointer<XmlElement> xml = getGlobalProperties()->getXmlValue(valueTreeToGet);
    
    if (xml != nullptr)
        return ValueTree::fromXml(*xml);
    else
    {
        ValueTree newTree(valueTreeToGet);
        
        xml = newTree.createXml();
        getGlobalProperties()->setValue(valueTreeToGet, xml);

        return newTree;
    }  
}   

void UserSettings::editFileLocations(int posX, int posY)
{
    if (fileLocationEditorWindow == nullptr)
        fileLocationEditorWindow = new FileLocationEditorWindow(posX, posY, commandManager, undoManager);

    fileLocationEditorWindow->setVisible(true);
    
    if (ScopeSyncApplication::inScopeFXContext())
        fileLocationEditorWindow->setAlwaysOnTop(true);
    
    fileLocationEditorWindow->toFront(true);
}

void UserSettings::hideFileLocationsWindow()
{
    if (fileLocationEditorWindow != nullptr && fileLocationEditorWindow->locationsHaveChanged())
    {
        ValueTree fileLocations = fileLocationEditorWindow->getFileLocations();
        updateFileLocations(fileLocations);
    }

    fileLocationEditorWindow.reset();
}

void UserSettings::showPresetManagerWindow(const String& filePath, int posX, int posY)
{
    if (presetManagerWindow == nullptr)
        presetManagerWindow = new PresetManagerWindow(filePath, commandManager, undoManager, posX, posY);

    presetManagerWindow->setVisible(true);
    
    if (ScopeSyncApplication::inScopeFXContext())
        presetManagerWindow->setAlwaysOnTop(true);
    
    presetManagerWindow->toFront(true);
}

void UserSettings::hidePresetManagerWindow()
{
    if (presetManagerWindow != nullptr && presetManagerWindow->presetsHaveChanged())
    {
        rebuildFileLibrary(false, false, true);
    }

    presetManagerWindow.reset();
}

void UserSettings::updateFileLocations(const ValueTree& fileLocations)
{
    if (fileLocations.isValid())
    {
        ScopedPointer<XmlElement> xml = fileLocations.createXml();
        getGlobalProperties()->setValue(Ids::fileLocations.toString(), xml);

        RebuildFileLibrary rebuild(fileLocations);
    
        rebuild.runThread();
    }
}

void UserSettings::rebuildFileLibrary(bool scanConfigurations, bool scanLayouts, bool scanPresets)
{
    getGlobalProperties()->reload();

    ValueTree fileLocations = getFileLocations();
    RebuildFileLibrary rebuild(fileLocations, scanConfigurations, scanLayouts, scanPresets);
    
    rebuild.runThread();
}
    
void UserSettings::RebuildFileLibrary::run()
{
    DBG("UserSettings::RebuildFileLibrary::run");

    setStatusMessage("Searching for files in library locations...");

    if (!fileLocations.isValid())
        return;

    Array<File> layoutFiles;
    Array<File> configurationFiles;
    Array<File> presetFiles;
        
    int numLocations = fileLocations.getNumChildren();

    for (int i = 0; i < numLocations; i++)
    {
        if (threadShouldExit())
            break;
        
        setProgress((i + 1) / static_cast<double>(numLocations - 1));
        
        String locationFolder = fileLocations.getChild(i).getProperty(Ids::folder);

        if (locationFolder.isEmpty() || !(File::isAbsolutePath(locationFolder)))
            continue;

        File location = File(locationFolder);
        
        if (!(location.isDirectory()))
            continue;
        
		String statusMessage = "Searching for files in library locations...";
		statusMessage << newLine << newLine;
		statusMessage << location.getFullPathName();
        
		setStatusMessage(statusMessage);

        int total = 0;

        for (DirectoryIterator di(location, true); di.next();)
        {
            if (threadShouldExit())
                break;
        
            File newFile = di.getFile();
            
            if (rebuildLayouts && newFile.hasFileExtension("layout"))
                layoutFiles.add(newFile);
            else if (rebuildConfigurations && newFile.hasFileExtension("configuration"))
                configurationFiles.add(newFile);
            else if (rebuildPresets && newFile.hasFileExtension("presets"))
                presetFiles.add(newFile);

            if (total < 100000)
                ++total;
            else
                break;
        }
    }
    
    if (rebuildLayouts)
    {
        updateLayoutLibrary(layoutFiles);
        userSettings->sendActionMessage("layoutlibraryupdated");
    }

    if (rebuildConfigurations)
    {
        updateConfigurationLibrary(configurationFiles);
        userSettings->sendActionMessage("configurationlibraryupdated");
    }

    if (rebuildPresets)
    {
        updatePresetLibrary(presetFiles);
        userSettings->sendActionMessage("presetlibraryupdated");
    }

    userSettings->getGlobalProperties()->saveIfNeeded();
}

void UserSettings::RebuildFileLibrary::trimMissingFiles(const Array<File>& activeFiles, ValueTree& libraryToTrim)
{
    for (int i = libraryToTrim.getNumChildren() - 1; i >= 0 ; i--)
    {
        if (threadShouldExit())
            break;
        
        setProgress(i / static_cast<double>(libraryToTrim.getNumChildren() - 1));
        
        ValueTree libraryItem = libraryToTrim.getChild(i);
        String    filePath    = libraryItem.getProperty(Ids::filePath);

        bool fileFound = false;
       
        for (int j = 0; j < activeFiles.size(); j++)
        {
            if (activeFiles[j].getFullPathName().equalsIgnoreCase(filePath))
            {
                fileFound = true;
                break;
            }
        }
        
        if (!fileFound)
            libraryToTrim.removeChild(i, nullptr);
    }
}

void UserSettings::RebuildFileLibrary::updateLayoutLibrary(const Array<File>& layoutFiles)
{
    setStatusMessage("Removing Layouts that no longer exist...");
    setProgress(0.0f);
        
    ValueTree layoutLibrary = userSettings->getLayoutLibrary();

    trimMissingFiles(layoutFiles, layoutLibrary);

    setStatusMessage("Updating Layouts...");
    setProgress(0.0f);

    // Then update information for existing entries, or add new ones
    for (int i = 0; i < layoutFiles.size(); i++)
    {
        if (threadShouldExit())
            break;
        
        setProgress(i / static_cast<double>(layoutFiles.size() - 1));
        
        ValueTree layout(Ids::layout);
            
        XmlDocument               layoutDocument(layoutFiles[i]);
        ScopedPointer<XmlElement> loadedLayoutXml = layoutDocument.getDocumentElement();

        String layoutName;
        XmlElement layoutXml("dummy");

        if (loadedLayoutXml != nullptr)
        {
            if (loadedLayoutXml->hasTagName(Ids::layout))
            {
                // No XSD validation header
                layoutXml = *loadedLayoutXml;
                layoutName = layoutXml.getStringAttribute(Ids::name);
            }
            else
            {
                // Look for a layout element at the 2nd level down instead
                XmlElement* subXml = loadedLayoutXml->getChildByName(Ids::layout);
            
                if (subXml != nullptr)
                {
                    layoutXml = *subXml;
                    layoutName = layoutXml.getStringAttribute(Ids::name);
                }
                else
                    continue;
            }
        }

        if (layoutName.isNotEmpty())
            userSettings->updateLayoutLibraryEntry(layoutFiles[i].getFullPathName(), layoutXml, layoutLibrary);
    }

    ScopedPointer<XmlElement> xml = layoutLibrary.createXml();
    userSettings->getGlobalProperties()->setValue(Ids::layoutLibrary.toString(), xml);
}

void UserSettings::RebuildFileLibrary::updateConfigurationLibrary(const Array<File>& configurationFiles)
{
    setStatusMessage("Removing Configurations that no longer exist...");
    setProgress(0.0f);

    ValueTree configurationLibrary = userSettings->getConfigurationLibrary();

    trimMissingFiles(configurationFiles, configurationLibrary);

    setStatusMessage("Updating Configurations...");
    setProgress(0.0f);

    // Then update information for existing entries, or add new ones
    for (int i = 0; i < configurationFiles.size(); i++)
    {
        if (threadShouldExit())
            break;
        
        setProgress(i / static_cast<double>(configurationFiles.size() - 1));
        
        ScopedPointer<XmlElement> loadedConfigurationXml(XmlDocument::parse(configurationFiles[i]));

        if (loadedConfigurationXml == nullptr || !(loadedConfigurationXml->hasTagName(Ids::configuration)))
        {
            DBG("UserSettings::updateConfigurationLibrary - Not a valid ScopeSync Configuration: " + configurationFiles[i].getFullPathName());
            continue;
        }
        
        ValueTree configuration(ValueTree::fromXml(*loadedConfigurationXml));

        if (!configuration.hasType(Ids::configuration))
        {
            DBG("UserSettings::updateConfigurationLibrary - The document contains errors and couldn't be parsed: " + configurationFiles[i].getFullPathName());
            continue;
        }
        
        String configurationName = configuration.getProperty(Ids::name);

        if (configurationName.isNotEmpty())
            userSettings->updateConfigurationLibraryEntry(configurationFiles[i].getFullPathName(),
                                                                         configurationFiles[i].getFileName(),
                                                                         configuration, configurationLibrary);
    }

    ScopedPointer<XmlElement> xml = configurationLibrary.createXml();
    userSettings->getGlobalProperties()->setValue(Ids::configurationLibrary.toString(), xml);
}

void UserSettings::RebuildFileLibrary::updatePresetLibrary(const Array<File>& presetFiles)
{
    setStatusMessage("Removing Preset Files that no longer exist...");
    setProgress(0.0f);

    ValueTree presetLibrary = userSettings->getPresetLibrary();

    trimMissingFiles(presetFiles, presetLibrary);

    setStatusMessage("Updating Presets...");
    setProgress(0.0f);

    // Then update information for existing entries, or add new ones
    for (int i = 0; i < presetFiles.size(); i++)
    {
        if (threadShouldExit())
            break;
        
        setProgress(i / static_cast<double>(presetFiles.size() - 1));
        
        ScopedPointer<XmlElement> loadedPresetFileXml(XmlDocument::parse(presetFiles[i]));

        if (loadedPresetFileXml == nullptr || !(loadedPresetFileXml->hasTagName(Ids::presets)))
        {
            DBG("UserSettings::updatePresetLibrary - Not a valid ScopeSync Preset File: " + presetFiles[i].getFullPathName());
            continue;
        }
        
        ValueTree presetFile(ValueTree::fromXml(*loadedPresetFileXml));

        if (!presetFile.hasType(Ids::presets))
        {
            DBG("UserSettings::updatePresetLibrary - The document contains errors and couldn't be parsed: " + presetFiles[i].getFullPathName());
            continue;
        }
        
        //String presetFileName = presetFile.getProperty(Ids::name);
        //
        //if (presetFileName.isNotEmpty())
            userSettings->updatePresetLibraryEntry(presetFiles[i].getFullPathName(),
                                                                  presetFiles[i].getFileName(),
                                                                  presetFile, presetLibrary);
    }

    ScopedPointer<XmlElement> xml = presetLibrary.createXml();
    userSettings->getGlobalProperties()->setValue(Ids::presetLibrary.toString(), xml);
}

void UserSettings::timerCallback()
{
    undoManager.beginNewTransaction();
}

void UserSettings::getAllCommands(Array <CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::editFileLocations,
                              CommandIDs::showPresetManager};

    commands.addArray(ids, numElementsInArray (ids));
}

void UserSettings::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::editFileLocations:
        result.setInfo("File Locations...", "Open File Locations edit window", CommandCategories::usersettings, 0);
        result.defaultKeypresses.add(KeyPress('f', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::showPresetManager:
        result.setInfo("Preset Manager...", "Open Preset Manager window", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress('p', ModifierKeys::commandModifier, 0));
        break;
    }
}

bool UserSettings::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::editFileLocations:    editFileLocations(getParentMonitorArea().getCentreX(), getParentMonitorArea().getCentreY()); break;
        case CommandIDs::showPresetManager:    showPresetManagerWindow(String(), getParentMonitorArea().getCentreX(), getParentMonitorArea().getCentreY()); break;
        default:                               return false;
    }

    return true;
}

ApplicationCommandTarget* UserSettings::getNextCommandTarget()
{
    return nullptr;
}

void UserSettings::loadSwatchColours()
{
    swatchColours.clear();

    const StringArray colours = StringArray::fromTokens(
        "ff434343,\
         ffff9933,\
         ff708090,\
         ffd3d3d3,\
         ff0080ff,\
         ff82e580,\
         ffffef2d,\
         ff1f262b,\
         ff555555,\
         fff0f8ff,\
         ffb2c8ce,\
         ff28a547,\
         ff425142,\
         ff9b73ff,\
         ffd9e0e2,\
         fff4fead,\
         ff5f9ea0,\
         ffa19566,\
         ffd2691e,\
         ffff7f50,\
         ff6495ed,\
         ff0e1e60,\
         ffdc143c,\
         ff628099", ",","");

    const int numSwatchColours = 24;
    PropertiesFile* props = getGlobalProperties();

    for (int i = 0; i < numSwatchColours; ++i)
        swatchColours.add(Colour::fromString(props->getValue("swatchColour" + String(i),
                                                               colours[i])));
}

void UserSettings::saveSwatchColours()
{
    PropertiesFile* props = getGlobalProperties();

    for (int i = 0; i < swatchColours.size(); ++i)
        props->setValue ("swatchColour" + String(i), swatchColours.getReference(i).toString());
}

int UserSettings::ColourSelectorWithSwatches::getNumSwatches() const
{
    return userSettings->swatchColours.size();
}

Colour UserSettings::ColourSelectorWithSwatches::getSwatchColour (int index) const
{
    return userSettings->swatchColours[index];
}

void UserSettings::ColourSelectorWithSwatches::setSwatchColour (int index, const Colour& newColour)
{
    userSettings->swatchColours.set(index, newColour);
}

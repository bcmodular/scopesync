/*
 * User Settings popup handler.
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

#include "UserSettings.h"
#include "../Core/Global.h"
#include "../Core/ScopeSyncApplication.h"
#include "../Core/ScopeSync.h"
#include "../Utils/BCMMisc.h"
#include "FileLocationEditor.h"

/* =========================================================================
 * EncoderSnapProperty
 */
class EncoderSnapProperty : public ChoicePropertyComponent
{
public:
    EncoderSnapProperty(UserSettings& userSettings) : ChoicePropertyComponent("Encoder Snap"), owner(userSettings)
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
    RotaryMovementProperty(UserSettings& userSettings) : ChoicePropertyComponent("Rotary Encoder Movement"), owner(userSettings)
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
    PopupEnabledProperty(UserSettings& userSettings) : ChoicePropertyComponent("Encoder Popup Enabled"), owner(userSettings)
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
    EncoderVelocityModeProperty(UserSettings& userSettings) : ChoicePropertyComponent("Encoder Velocity Mode"), owner(userSettings)
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

juce_ImplementSingleton(UserSettings)

/* =========================================================================
 * UserSettings
 */
UserSettings::UserSettings()
: fileLocationsButton("File Locations...")
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

    // Build Layout Locations and Library trees
    ScopedPointer<XmlElement> xml = getGlobalProperties()->getXmlValue(Ids::fileLocations.toString());
    
    if (xml != nullptr)
    {
        fileLocations = ValueTree::fromXml(*xml);

        xml = getGlobalProperties()->getXmlValue(Ids::layoutLibrary.toString());

        if (xml != nullptr)
            layoutLibrary = ValueTree::fromXml(*xml);
        
        xml = getGlobalProperties()->getXmlValue(Ids::configurationLibrary.toString());
        
        if (xml != nullptr)
            configurationLibrary = ValueTree::fromXml(*xml);
        
        if (!layoutLibrary.isValid() || !configurationLibrary.isValid())
            rebuildFileLibrary();
    }
    else
        fileLocations = ValueTree(Ids::fileLocations);

    loadSwatchColours();
   
    addAndMakeVisible(propertyPanel);
    setWantsKeyboardFocus(true);
    propertyPanel.setWantsKeyboardFocus(true);
    
    setName("User Settings");
    
    tooltipDelayTime.setValue(getPropertyIntValue("tooltipdelaytime", -1));
    tooltipDelayTime.addListener(this);

    useImageCache.setValue(getPropertyBoolValue("enableimagecache", true));
    useImageCache.addListener(this);

    fileLocations.addListener(this);

    setupPanel();

    addAndMakeVisible(fileLocationsButton);
    fileLocationsButton.setCommandToTrigger(commandManager, CommandIDs::editFileLocations, true);
    
    setSize (getLocalBounds().getWidth(), getLocalBounds().getHeight());

    startTimer(500);
}

UserSettings::~UserSettings()
{
    removeKeyListener(commandManager->getKeyMappings());
    stopTimer();

    saveSwatchColours();

    fileLocationEditorWindow = nullptr;
    tooltipDelayTime.removeListener(this);
    clearSingletonInstance();
}

void UserSettings::setupPanel()
{
    PropertyListBuilder props;
    
    props.clear();
    props.add(new EnableTooltipsProperty(*this),                                "Choose whether tooltips are displayed when hovering over GUI elements");
    props.add(new IntRangeProperty(tooltipDelayTime, "Tooltip Delay Time", -1), "Enter the number of milliseconds to wait before a tooltip is displayed over GUI elements (-1 to use defaults)");

    propertyPanel.addSection("Tooltip Settings", props.components, true);
    
    props.clear();
    props.add(new EncoderSnapProperty(*this),                                   "Choose whether encoders snap to valid parameter values");
    props.add(new RotaryMovementProperty(*this),                                "Choose which mouse movement type is to be used by rotary encoders");
    props.add(new IncDecButtonModeProperty(*this),                              "Choose the mode for Inc/Dec button style Sliders");
    props.add(new PopupEnabledProperty(*this),                                  "Choose whether encoders show a popup with current value when dragging");
    props.add(new EncoderVelocityModeProperty(*this),                           "Choose whether Velocity Based Mode is enabled for Encoders");
    
    propertyPanel.addSection("Encoder Settings", props.components, true);

    props.clear();
    props.add(new BooleanPropertyComponent(useImageCache, "Image Cache", "Enabled"), "Disabling the Image Cache will mean that images will be refreshed immediately, but will slow down the GUI rendering");

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

    Rectangle<int> toolbarButtons = localBounds.removeFromTop(30);
    toolbarButtons.removeFromLeft(4);

    fileLocationsButton.setBounds(toolbarButtons.removeFromLeft(120));

    localBounds.removeFromTop(5);
    propertyPanel.setBounds(localBounds);
}

int UserSettings::getPropertyIntValue(const String& propertyName, int defaultValue)
{
    return getAppProperties()->getIntValue(propertyName, defaultValue);
}

void UserSettings::setPropertyIntValue(const String& propertyName, int newValue)
{
    return getAppProperties()->setValue(propertyName, newValue);
}

bool UserSettings::getPropertyBoolValue(const String& propertyName, bool defaultValue)
{
    return getAppProperties()->getBoolValue(propertyName, defaultValue);
}

void UserSettings::setPropertyBoolValue(const String& propertyName, bool newValue)
{
    return getAppProperties()->setValue(propertyName, newValue);
}

void UserSettings::valueChanged(Value& valueThatChanged)
{
    if (valueThatChanged.refersToSameSourceAs(tooltipDelayTime))
        setPropertyIntValue("tooltipdelaytime", valueThatChanged.getValue());
    else if (valueThatChanged.refersToSameSourceAs(useImageCache))
        setPropertyBoolValue("useimagecache", valueThatChanged.getValue());
}

void UserSettings::userTriedToCloseWindow()
{
    fileLocationEditorWindow = nullptr;
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
    setAlwaysOnTop(true);
    toFront(true);
}
    
void UserSettings::hide()
{
    fileLocationEditorWindow = nullptr;
    removeFromDesktop();
}

void UserSettings::changeListenerCallback(ChangeBroadcaster* /* source */)
{
    fileLocationEditorWindow = nullptr;
}

void UserSettings::editFileLocations()
{
    if (fileLocationEditorWindow == nullptr)
        fileLocationEditorWindow = new FileLocationEditorWindow
                                       (
                                       getScreenPosition().getX(), 
                                       getScreenPosition().getY(), 
                                       fileLocations, commandManager, undoManager
                                       );
    
    fileLocationEditorWindow->addChangeListener(this);
    fileLocationEditorWindow->setVisible(true);
    fileLocationEditorWindow->setAlwaysOnTop(true);
    fileLocationEditorWindow->toFront(true);
}

void UserSettings::updateFileLocations()
{
    ScopedPointer<XmlElement> xml = fileLocations.createXml();
    getGlobalProperties()->setValue(Ids::fileLocations.toString(), xml);

    rebuildFileLibrary();
}

void UserSettings::rebuildFileLibrary()
{
    if (!(fileLocations.isValid()) || fileLocations.getNumChildren() == 0)
        return;

    layoutLibrary = ValueTree(Ids::layoutLibrary);
    configurationLibrary = ValueTree(Ids::configurationLibrary);

    for (int i = 0; i < fileLocations.getNumChildren(); i++)
    {
        String locationFolder = fileLocations.getChild(i).getProperty(Ids::folder);

        if (locationFolder.isEmpty() || !(File::isAbsolutePath(locationFolder)))
            continue;

        File location = File(locationFolder);
        
        if (!(location.isDirectory()))
            continue;
        
        addLayoutsToLibrary(location);
        addConfigurationsToLibrary(location);
    }

    flushLibraryToFile();
}

void UserSettings::flushLibraryToFile()
{
    ScopedPointer<XmlElement> xml = layoutLibrary.createXml();
    getGlobalProperties()->setValue(Ids::layoutLibrary.toString(), xml);

    xml = configurationLibrary.createXml();
    getGlobalProperties()->setValue(Ids::configurationLibrary.toString(), xml);
}

void UserSettings::addLayoutsToLibrary(const File& location)
{
    Array<File> layoutFiles;
    location.findChildFiles(layoutFiles, File::findFiles, true, "*.layout");

    for (int i = 0; i < layoutFiles.size(); i++)
    {
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
        {
            layout.setProperty(Ids::name, layoutName, nullptr);
            layout.setProperty(Ids::librarySet, layoutXml.getStringAttribute(Ids::libraryset), nullptr);
            layout.setProperty(Ids::author, layoutXml.getStringAttribute(Ids::author), nullptr);
            layout.setProperty(Ids::numButtons, layoutXml.getIntAttribute(Ids::numbuttons), nullptr);
            layout.setProperty(Ids::numEncoders, layoutXml.getIntAttribute(Ids::numencoders), nullptr);
            layout.setProperty(Ids::numFaders, layoutXml.getIntAttribute(Ids::numfaders), nullptr);
            layout.setProperty(Ids::panelWidth, layoutXml.getIntAttribute(Ids::panelwidth), nullptr);
            layout.setProperty(Ids::panelHeight, layoutXml.getIntAttribute(Ids::panelheight), nullptr);
            layout.setProperty(Ids::numParameters, layoutXml.getIntAttribute(Ids::numparameters), nullptr);
            layout.setProperty(Ids::thumbnail, layoutXml.getStringAttribute(Ids::thumbnail), nullptr);
            layout.setProperty(Ids::excludeFromChooser, layoutXml.getBoolAttribute(Ids::excludefromchooser, false), nullptr);
            layout.setProperty(Ids::blurb, layoutXml.getStringAttribute(Ids::blurb), nullptr);
            layout.setProperty(Ids::filePath, layoutFiles[i].getFullPathName(), nullptr);
            layoutLibrary.addChild(layout, -1, nullptr);
        }
    }
}

void UserSettings::addConfigurationsToLibrary(const File& location)
{
    Array<File> configurationFiles;
    location.findChildFiles(configurationFiles, File::findFiles, true, "*.configuration");

    for (int i = 0; i < configurationFiles.size(); i++)
    {
        ValueTree configuration(Ids::configuration);
            
        XmlDocument               configurationDocument(configurationFiles[i]);
        ScopedPointer<XmlElement> loadedConfigurationXml = configurationDocument.getDocumentElement();

        String     configurationName;
        XmlElement configurationXml("dummy");

        if (loadedConfigurationXml != nullptr)
        {
            if (loadedConfigurationXml->hasTagName(Ids::configuration))
            {
                // No XSD validation header
                configurationXml  = *loadedConfigurationXml;
                configurationName = configurationXml.getStringAttribute(Ids::name);
            }
            else
                continue;
        }

        if (configurationName.isNotEmpty())
        {
            configuration.setProperty(Ids::name, configurationName, nullptr);
            configuration.setProperty(Ids::librarySet, configurationXml.getStringAttribute(Ids::librarySet), nullptr);
            configuration.setProperty(Ids::author, configurationXml.getStringAttribute(Ids::author), nullptr);
            configuration.setProperty(Ids::layoutName, configurationXml.getStringAttribute(Ids::layoutName), nullptr);
            configuration.setProperty(Ids::layoutLibrarySet, configurationXml.getStringAttribute(Ids::layoutLibrarySet), nullptr);
            configuration.setProperty(Ids::excludeFromChooser, configurationXml.getBoolAttribute(Ids::excludeFromChooser, false), nullptr);
            configuration.setProperty(Ids::blurb, configurationXml.getStringAttribute(Ids::blurb), nullptr);
            configuration.setProperty(Ids::ID, configurationXml.getStringAttribute(Ids::ID), nullptr);
            configuration.setProperty(Ids::UID, configurationXml.getStringAttribute(Ids::UID), nullptr);
            configuration.setProperty(Ids::filePath, configurationFiles[i].getFullPathName(), nullptr);
            configuration.setProperty(Ids::fileName, configurationFiles[i].getFileName(), nullptr);
            configurationLibrary.addChild(configuration, -1, nullptr);
        }
    }
}

String UserSettings::getLayoutFilename(const String& name, const String& librarySet)
{
    for (int i = 0; i < layoutLibrary.getNumChildren(); i++)
    {
        ValueTree layout = layoutLibrary.getChild(i);

        if (   layout.getProperty(Ids::name).toString().equalsIgnoreCase(name)
            && layout.getProperty(Ids::librarySet).toString().equalsIgnoreCase(librarySet))
        {
            return layout.getProperty(Ids::filePath);
        }
    }

    return String::empty;
}

void UserSettings::setLastTimeLayoutLoaded(const String& filePath)
{
    for (int i = 0; i < layoutLibrary.getNumChildren(); i++)
    {
        ValueTree layout = layoutLibrary.getChild(i);

        if (layout.getProperty(Ids::filePath).toString().equalsIgnoreCase(filePath))
        {
            layout.setProperty(Ids::mruTime, Time::getCurrentTime().formatted("%Y-%m-%d %H:%M:%S"), nullptr);
            break;
        }
    }

    flushLibraryToFile();
}

void UserSettings::getConfigurationFromFilePath(const String& filePath, ValueTree& configuration)
{
    for (int i = 0; i < configurationLibrary.getNumChildren(); i++)
    {
        configuration = configurationLibrary.getChild(i);

        if (configuration.getProperty(Ids::filePath).toString().equalsIgnoreCase(filePath))
            return;
    }
    
    configuration = ValueTree();
}

String UserSettings::getConfigurationFilePathFromUID(int uid)
{
    for (int i = 0; i < configurationLibrary.getNumChildren(); i++)
    {
        ValueTree configuration = configurationLibrary.getChild(i);

        if (int(configuration.getProperty(Ids::UID)) == uid)
        {
            return configuration.getProperty(Ids::filePath);
        }
    }

    return String::empty;
}

void UserSettings::setLastTimeConfigurationLoaded(const String& filePath)
{
    ValueTree configuration;
    getConfigurationFromFilePath(filePath, configuration);

    if (configuration.isValid())
    {
        configuration.setProperty(Ids::mruTime, Time::getCurrentTime().formatted("%Y-%m-%d %H:%M:%S"), nullptr);
        flushLibraryToFile();
    }
}

void UserSettings::updateConfigurationLibraryEntry(const String& filePath, const ValueTree& sourceValueTree)
{
    ValueTree configuration;
    getConfigurationFromFilePath(filePath, configuration);

    if (configuration.isValid())
    {
        configuration.setProperty(Ids::name,               sourceValueTree.getProperty(Ids::name), nullptr);
        configuration.setProperty(Ids::librarySet,         sourceValueTree.getProperty(Ids::librarySet), nullptr);
        configuration.setProperty(Ids::author,             sourceValueTree.getProperty(Ids::author), nullptr);
        configuration.setProperty(Ids::layoutName,         sourceValueTree.getProperty(Ids::layoutName), nullptr);
        configuration.setProperty(Ids::layoutLibrarySet,   sourceValueTree.getProperty(Ids::layoutLibrarySet), nullptr);
        configuration.setProperty(Ids::excludeFromChooser, sourceValueTree.getProperty(Ids::excludeFromChooser), nullptr);
        configuration.setProperty(Ids::blurb,              sourceValueTree.getProperty(Ids::blurb), nullptr);
        configuration.setProperty(Ids::ID,                 sourceValueTree.getProperty(Ids::ID), nullptr);
        configuration.setProperty(Ids::UID,                sourceValueTree.getProperty(Ids::UID), nullptr);
        
        flushLibraryToFile();
    }
}

void UserSettings::timerCallback()
{
    undoManager.beginNewTransaction();
}

void UserSettings::getAllCommands(Array <CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::editFileLocations };

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
    }
}

bool UserSettings::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::editFileLocations:    editFileLocations(); break;
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

    const Colour colours[] =
    {
        Colours::blue,
        Colours::grey,
        Colours::green,
        Colours::red,
        Colours::yellow,
        Colours::aliceblue,
        Colours::antiquewhite,
        Colours::aqua,
        Colours::aquamarine,
        Colours::azure,
        Colours::beige,
        Colours::bisque,
        Colours::blanchedalmond,
        Colours::blueviolet,
        Colours::brown,
        Colours::burlywood,
        Colours::cadetblue,
        Colours::chartreuse,
        Colours::chocolate,
        Colours::coral,
        Colours::cornflowerblue,
        Colours::cornsilk,
        Colours::crimson,
        Colours::cyan
    };

    const int numSwatchColours = 24;
    PropertiesFile* props = getGlobalProperties();

    for (int i = 0; i < numSwatchColours; ++i)
        swatchColours.add(Colour::fromString(props->getValue("swatchColour" + String(i),
                                                               colours[i].toString())));
}

void UserSettings::saveSwatchColours()
{
    PropertiesFile* props = getGlobalProperties();

    for (int i = 0; i < swatchColours.size(); ++i)
        props->setValue ("swatchColour" + String(i), swatchColours.getReference(i).toString());
}

int UserSettings::ColourSelectorWithSwatches::getNumSwatches() const
{
    return UserSettings::getInstance()->swatchColours.size();
}

Colour UserSettings::ColourSelectorWithSwatches::getSwatchColour (int index) const
{
    return UserSettings::getInstance()->swatchColours[index];
}

void UserSettings::ColourSelectorWithSwatches::setSwatchColour (int index, const Colour& newColour) const
{
    UserSettings::getInstance()->swatchColours.set(index, newColour);
}

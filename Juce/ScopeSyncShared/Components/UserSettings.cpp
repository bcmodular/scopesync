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

    loadSwatchColours();
   
    addAndMakeVisible(propertyPanel);
    setWantsKeyboardFocus(true);
    propertyPanel.setWantsKeyboardFocus(true);
    
    setName("User Settings");
    
    tooltipDelayTime.setValue(getPropertyIntValue("tooltipdelaytime", -1));
    tooltipDelayTime.addListener(this);

    useImageCache.setValue(getPropertyBoolValue("enableimagecache", true));
    useImageCache.addListener(this);

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
    setAlwaysOnTop(true);
    toFront(true);
}
    
void UserSettings::hide()
{
    hideFileLocationsWindow();
    removeFromDesktop();
}

void UserSettings::changeListenerCallback(ChangeBroadcaster* /* source */)
{
    hideFileLocationsWindow();
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

    return String::empty;
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

    return String::empty;
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

ValueTree UserSettings::getValueTreeFromGlobalProperties(const String& valueTreeToGet)
{
    ScopedPointer<XmlElement> xml = getGlobalProperties()->getXmlValue(valueTreeToGet);
    
    if (xml != nullptr)
        return ValueTree::fromXml(*xml);
    else
    {
        ValueTree newTree(valueTreeToGet);
        
        ScopedPointer<XmlElement> xml = newTree.createXml();
        getGlobalProperties()->setValue(valueTreeToGet, xml);

        return newTree;
    }
    
}   

void UserSettings::editFileLocations(int posX, int posY, ChangeListener* listener)
{
    if (fileLocationEditorWindow == nullptr)
        fileLocationEditorWindow = new FileLocationEditorWindow(posX, posY, commandManager, undoManager);

    if (listener != nullptr)
        fileLocationEditorWindow->addChangeListener(listener);

    fileLocationEditorWindow->setVisible(true);
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

    fileLocationEditorWindow = nullptr;
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

void UserSettings::rebuildFileLibrary()
{
    ValueTree fileLocations = getFileLocations();
    
    RebuildFileLibrary rebuild(fileLocations);
    rebuild.runThread();
}
    
void UserSettings::RebuildFileLibrary::run()
{
    setStatusMessage("Searching for files in library locations...");

    if (!(fileLocations.isValid()) || fileLocations.getNumChildren() == 0)
        return;

    Array<File> layoutFiles;
    Array<File> configurationFiles;
        
    int numLocations = fileLocations.getNumChildren();

    for (int i = 0; i < numLocations; i++)
    {
        if (threadShouldExit())
            break;
        
        setProgress((i + 0.5) / (double)(numLocations - 1));
        
        String locationFolder = fileLocations.getChild(i).getProperty(Ids::folder);

        if (locationFolder.isEmpty() || !(File::isAbsolutePath(locationFolder)))
            continue;

        File location = File(locationFolder);
        
        if (!(location.isDirectory()))
            continue;
        
        setStatusMessage("Searching for layouts in library locations..." + newLine + newLine + location.getFullPathName());

        int total = 0;

        for (DirectoryIterator di(location, true); di.next();)
        {
            if (threadShouldExit())
                break;
        
            File newFile = di.getFile();
            
            if (newFile.hasFileExtension("layout"))
                layoutFiles.add(newFile);

            if (total < 100000)
                ++total;
            else
                break;
        }

        //location.findChildFiles(layoutFiles, File::findFiles, true, "*.layout");
        
        setProgress((i + 1) / (double)(numLocations - 1));
        
        setStatusMessage("Searching for configurations in library locations..." + newLine + newLine + location.getFullPathName());

        total = 0;

        for (DirectoryIterator di(location, true); di.next();)
        {
            if (threadShouldExit())
            break;
        
            File newFile = di.getFile();
            
            if (newFile.hasFileExtension("configuration"))
                configurationFiles.add(newFile);

            if (total < 100000)
                ++total;
            else
                break;
        }
        
        //location.findChildFiles(configurationFiles, File::findFiles, true, "*.configuration");
    }
    
    updateLayoutLibrary(layoutFiles);
    updateConfigurationLibrary(configurationFiles);
}

void UserSettings::RebuildFileLibrary::updateLayoutLibrary(const Array<File>& layoutFiles)
{
    setStatusMessage("Removing Layouts that no longer exist...");
    setProgress(0.0f);
        
    ValueTree layoutLibrary = UserSettings::getInstance()->getLayoutLibrary();

        // Firstly trim entries that no longer have files
    for (int i = layoutLibrary.getNumChildren() - 1; i >= 0 ; i--)
    {
        if (threadShouldExit())
            break;
        
        setProgress(i / (double)(layoutLibrary.getNumChildren() - 1));
        
        ValueTree layout   = layoutLibrary.getChild(i);
        String    filePath = layout.getProperty(Ids::filePath);

        bool fileFound = false;
       
        for (int j = 0; j < layoutFiles.size(); j++)
        {
            if (layoutFiles[j].getFullPathName().equalsIgnoreCase(filePath))
            {
                fileFound = true;
                break;
            }
        }
        
        if (!fileFound)
            layoutLibrary.removeChild(i, nullptr);
    }

    setStatusMessage("Updating Layouts...");
    setProgress(0.0f);

    // Then update information for existing entries, or add new ones
    for (int i = 0; i < layoutFiles.size(); i++)
    {
        if (threadShouldExit())
            break;
        
        setProgress(i / (double)(layoutFiles.size() - 1));
        
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
            UserSettings::getInstance()->updateLayoutLibraryEntry(layoutFiles[i].getFullPathName(), layoutXml, layoutLibrary);
    }

    ScopedPointer<XmlElement> xml = layoutLibrary.createXml();
    UserSettings::getInstance()->getGlobalProperties()->setValue(Ids::layoutLibrary.toString(), xml);
}

void UserSettings::RebuildFileLibrary::updateConfigurationLibrary(const Array<File>& configurationFiles)
{
    setStatusMessage("Removing Configurations that no longer exist...");
    setProgress(0.0f);

    ValueTree configurationLibrary = UserSettings::getInstance()->getConfigurationLibrary();

    // Firstly trim entries that no longer have files
    for (int i = configurationLibrary.getNumChildren() - 1; i >= 0 ; i--)
    {
        if (threadShouldExit())
            break;
        
        setProgress(i / (double)(configurationLibrary.getNumChildren() - 1));
        
        ValueTree configuration = configurationLibrary.getChild(i);
        String    filePath      = configuration.getProperty(Ids::filePath);

        bool fileFound = false;
       
        for (int j = 0; j < configurationFiles.size(); j++)
        {
            if (configurationFiles[j].getFullPathName().equalsIgnoreCase(filePath))
            {
                fileFound = true;
                break;
            }
        }
        
        if (!fileFound)
            configurationLibrary.removeChild(i, nullptr);
    }

    setStatusMessage("Updating Configurations...");
    setProgress(0.0f);

    // Then update information for existing entries, or add new ones
    for (int i = 0; i < configurationFiles.size(); i++)
    {
        if (threadShouldExit())
            break;
        
        setProgress(i / (double)(configurationFiles.size() - 1));
        
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
            UserSettings::getInstance()->updateConfigurationLibraryEntry(configurationFiles[i].getFullPathName(),
                                                                         configurationFiles[i].getFileName(),
                                                                         configuration, configurationLibrary);
    }

    ScopedPointer<XmlElement> xml = configurationLibrary.createXml();
    UserSettings::getInstance()->getGlobalProperties()->setValue(Ids::configurationLibrary.toString(), xml);
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
        case CommandIDs::editFileLocations:    editFileLocations(getParentMonitorArea().getCentreX(), getParentMonitorArea().getCentreY(), this); break;
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

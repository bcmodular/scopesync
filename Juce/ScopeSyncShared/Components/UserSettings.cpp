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
#include "LayoutLocationEditor.h"

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
: layoutLocationsButton("Layout Locations...")
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
    ScopedPointer<XmlElement> xml = getGlobalProperties()->getXmlValue(Ids::layoutLocations.toString());
    
    if (xml != nullptr)
    {
        layoutLocations = ValueTree::fromXml(*xml);

        xml = getGlobalProperties()->getXmlValue(Ids::layoutLibrary.toString());

        if (xml != nullptr)
            layoutLibrary = ValueTree::fromXml(*xml);
        else
            rebuildLayoutLibrary();
    }
    else
        initialiseLayoutLocations();
   
    addAndMakeVisible(propertyPanel);
    setWantsKeyboardFocus(true);
    propertyPanel.setWantsKeyboardFocus(true);
    
    setName("User Settings");
    
    tooltipDelayTime.setValue(getPropertyIntValue("tooltipdelaytime", -1));
    tooltipDelayTime.addListener(this);

    useImageCache.setValue(getPropertyBoolValue("enableimagecache", true));
    useImageCache.addListener(this);

    layoutLocations.addListener(this);

    setupPanel();

    addAndMakeVisible(layoutLocationsButton);
    layoutLocationsButton.setCommandToTrigger(commandManager, CommandIDs::editLayoutLocations, true);
    
    setSize (getLocalBounds().getWidth(), getLocalBounds().getHeight());

    startTimer(500);
}

UserSettings::~UserSettings()
{
    removeKeyListener(commandManager->getKeyMappings());
    stopTimer();

    layoutLocationEditorWindow = nullptr;
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

    layoutLocationsButton.setBounds(toolbarButtons.removeFromLeft(120));

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
    layoutLocationEditorWindow = nullptr;
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
    layoutLocationEditorWindow = nullptr;
    removeFromDesktop();
}

void UserSettings::changeListenerCallback(ChangeBroadcaster* /* source */)
{
    layoutLocationEditorWindow = nullptr;
}

void UserSettings::editLayoutLocations()
{
    if (layoutLocationEditorWindow == nullptr)
        layoutLocationEditorWindow = new LayoutLocationEditorWindow
                                            (
                                            getScreenPosition().getX(), 
                                            getScreenPosition().getY(), 
                                            layoutLocations, commandManager, undoManager
                                            );
    
    layoutLocationEditorWindow->addChangeListener(this);
    layoutLocationEditorWindow->setVisible(true);
    layoutLocationEditorWindow->setAlwaysOnTop(true);
    layoutLocationEditorWindow->toFront(true);
}

void UserSettings::updateLayoutLocations()
{
    ScopedPointer<XmlElement> xml = layoutLocations.createXml();
    getGlobalProperties()->setValue(Ids::layoutLocations.toString(), xml);

    rebuildLayoutLibrary();
}

void UserSettings::initialiseLayoutLocations()
{
    if (!(layoutLocations.isValid()))
        layoutLocations = ValueTree(Ids::layoutLocations);

    ValueTree stockLayouts(Ids::location);
    stockLayouts.setProperty(Ids::folder, "C:\\development\\github\\scopesync\\Layouts", &undoManager);

    layoutLocations.addChild(stockLayouts, -1, &undoManager);

    updateLayoutLocations();
}

void UserSettings::rebuildLayoutLibrary()
{
    if (!(layoutLocations.isValid()) || layoutLocations.getNumChildren() == 0)
        initialiseLayoutLocations();

    layoutLibrary = ValueTree(Ids::layoutLibrary);

    for (int i = 0; i < layoutLocations.getNumChildren(); i++)
    {
        String locationFolder = layoutLocations.getChild(i).getProperty(Ids::folder);

        if (locationFolder.isEmpty() || !(File::isAbsolutePath(locationFolder)))
            continue;

        File location = File(locationFolder);
        
        if (!(location.isDirectory()))
            continue;
        
        Array<File> layoutFiles;
        location.findChildFiles(layoutFiles, File::findFiles, true, "*.layout");

        for (int j = 0; j < layoutFiles.size(); j++)
        {
            ValueTree layout(Ids::layout);
            
            XmlDocument               layoutDocument(layoutFiles[j]);
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
                }
            }

            if (layoutName.isNotEmpty())
            {
                layout.setProperty(Ids::name, layoutName, nullptr);
                layout.setProperty(Ids::libraryset, layoutXml.getStringAttribute(Ids::libraryset), nullptr);
                layout.setProperty(Ids::author, layoutXml.getStringAttribute(Ids::author), nullptr);
                layout.setProperty(Ids::numbuttons, layoutXml.getIntAttribute(Ids::numbuttons), nullptr);
                layout.setProperty(Ids::numencoders, layoutXml.getIntAttribute(Ids::numencoders), nullptr);
                layout.setProperty(Ids::numfaders, layoutXml.getIntAttribute(Ids::numfaders), nullptr);
                layout.setProperty(Ids::panelwidth, layoutXml.getIntAttribute(Ids::panelwidth), nullptr);
                layout.setProperty(Ids::panelheight, layoutXml.getIntAttribute(Ids::panelheight), nullptr);
                layout.setProperty(Ids::numparameters, layoutXml.getIntAttribute(Ids::numparameters), nullptr);
                layout.setProperty(Ids::thumbnail, layoutXml.getStringAttribute(Ids::thumbnail), nullptr);
                layout.setProperty(Ids::excludefromchooser, layoutXml.getBoolAttribute(Ids::excludefromchooser), nullptr);
                layout.setProperty(Ids::blurb, layoutXml.getStringAttribute(Ids::blurb), nullptr);
                layout.setProperty(Ids::filePath, layoutFiles[j].getFullPathName(), nullptr);
                layoutLibrary.addChild(layout, -1, nullptr);
            }
        }
    }

    ScopedPointer<XmlElement> xml = layoutLibrary.createXml();
    getGlobalProperties()->setValue(Ids::layoutLibrary.toString(), xml);
}

String UserSettings::getLayoutFilename(const String& name, const String& librarySet)
{
    for (int i = 0; i < layoutLibrary.getNumChildren(); i++)
    {
        ValueTree layout = layoutLibrary.getChild(i);

        if (   layout.getProperty(Ids::name).toString().equalsIgnoreCase(name)
            && layout.getProperty(Ids::libraryset).toString().equalsIgnoreCase(librarySet))
        {
            return layout.getProperty(Ids::filePath);
        }
    }

    return String::empty;
}

void UserSettings::timerCallback()
{
    undoManager.beginNewTransaction();
}

void UserSettings::getAllCommands(Array <CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::editLayoutLocations };

    commands.addArray(ids, numElementsInArray (ids));
}

void UserSettings::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::editLayoutLocations:
        result.setInfo("Layout Locations...", "Open Layout Locations edit window", CommandCategories::usersettings, 0);
        result.defaultKeypresses.add(KeyPress('n', ModifierKeys::commandModifier, 0));
        break;
    }
}

bool UserSettings::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::editLayoutLocations:    editLayoutLocations(); break;
        default:                               return false;
    }

    return true;
}

ApplicationCommandTarget* UserSettings::getNextCommandTarget()
{
    return nullptr;
}

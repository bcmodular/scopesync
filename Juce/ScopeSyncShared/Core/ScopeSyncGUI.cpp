/**
 * GUI controller for the ScopeSync system. Hosted by different parent
 * objects depending on the AppContext (PluginGUI or ScopeFXGUI
 * respectively).
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

#include "ScopeSyncGUI.h"
#include "ScopeSync.h"
#include "BCMParameter.h"
#include "../Utils/BCMMath.h"
#include "../Components/UserSettings.h"
#include "Global.h"
#include "ScopeSyncApplication.h"
#include "../Components/BCMLookAndFeel.h"
#include "../Components/BCMComponent.h"
#include "../Components/BCMTabbedComponent.h"
#include "../Configuration/ConfigurationManager.h"
#include "../Properties/ComponentProperties.h"
#include "../Properties/SliderProperties.h"
#include "../Properties/LabelProperties.h"
#include "../Properties/TextButtonProperties.h"
#include "../Properties/ComboBoxProperties.h"
#include "../Properties/TabbedComponentProperties.h"
#include "../Properties/TabProperties.h"
#include "../Configuration/ConfigurationChooser.h"

const int ScopeSyncGUI::timerFrequency = 100;
ScopedPointer<TooltipWindow> ScopeSyncGUI::tooltipWindow;

ScopeSyncGUI::ScopeSyncGUI(ScopeSync& owner) : scopeSync(owner)
{
    setWantsKeyboardFocus(true);
    
    scopeSync.getCommandManager()->registerAllCommandsForTarget(this);
    
    addKeyListener(scopeSync.getCommandManager()->getKeyMappings());
    createGUI(false);
    scopeSync.setGUIReload(false);
    startTimer(timerFrequency);
}

ScopeSyncGUI::~ScopeSyncGUI()
{
    removeKeyListener(scopeSync.getCommandManager()->getKeyMappings());
    stopTimer();
}

void ScopeSyncGUI::hideUserSettings()
{
    UserSettings::getInstance()->hide();
}

void ScopeSyncGUI::showUserSettings()
{
    UserSettings::getInstance()->show(mainComponent->getScreenBounds().getX(), mainComponent->getScreenBounds().getY());
}

void ScopeSyncGUI::chooseConfiguration()
{
    if (configurationChooserWindow == nullptr)
        configurationChooserWindow = new ConfigurationChooserWindow
                                      (
                                      getParentMonitorArea().getCentreX(), 
                                      getParentMonitorArea().getCentreY(), 
                                      scopeSync,
                                      scopeSync.getCommandManager()
                                      );
    
    configurationChooserWindow->addChangeListener(this);
    configurationChooserWindow->setVisible(true);
    
    if (ScopeSyncApplication::inScopeFXContext())
        configurationChooserWindow->setAlwaysOnTop(true);
    
    configurationChooserWindow->toFront(true);   
}

void ScopeSyncGUI::changeListenerCallback(ChangeBroadcaster* source)
{ 
    if (source == configurationChooserWindow)
        configurationChooserWindow = nullptr;
    else if (source == &scopeSync)
    {
        ScopeSync::checkNewConfigIsInLocation(scopeSync.getConfiguration(), this, this);
    }
    else
        UserSettings::getInstance()->hideFileLocationsWindow();
}

BCMParameter* ScopeSyncGUI::getUIMapping(Identifier componentTypeId, const String& componentName, ValueTree& mapping)
{
    ValueTree componentMapping = deviceMapping.getChildWithName(componentTypeId);

    if (componentMapping.isValid())
    {
        for (int i = 0; i < componentMapping.getNumChildren(); i++)
        {
            String mappedComponentName = componentMapping.getChild(i).getProperty(Ids::name);

            if (componentName.equalsIgnoreCase(mappedComponentName))
            {
                mapping = componentMapping.getChild(i);
                String mapTo = mapping.getProperty(Ids::mapTo).toString();
                return scopeSync.getParameterByName(mapTo);
            }
        }
    }
    
    return nullptr;
}

void ScopeSyncGUI::addTabbedComponent(BCMTabbedComponent* tabbedComponent)
{
    // DBG("ScopeSyncGUI::addTabbedComponent - " + tabbedComponent->getName());
    tabbedComponents.add(tabbedComponent);
}

void ScopeSyncGUI::getTabbedComponentsByName(const String& name, Array<BCMTabbedComponent*>& tabbedComponentArray)
{
    tabbedComponentArray.clear();

    int numTabbedComponents = tabbedComponents.size();

    for (int i = 0; i < numTabbedComponents; i++)
    {
        String componentName = tabbedComponents[i]->getName();

        if (name.equalsIgnoreCase(componentName))
            tabbedComponentArray.add(tabbedComponents[i]);
    }
}

Slider::SliderStyle ScopeSyncGUI::getDefaultRotarySliderStyle()
{
    if (settings.rotaryMovement == BCMSlider::rotary)
        return Slider::Rotary;
    else if (settings.rotaryMovement == BCMSlider::horizontal)
        return Slider::RotaryHorizontalDrag;
    else if (settings.rotaryMovement == BCMSlider::horizontalVertical)
        return Slider::RotaryHorizontalVerticalDrag;
    else
        return Slider::RotaryVerticalDrag;
}

void ScopeSyncGUI::createGUI(bool forceReload)
{
    tooltipWindow = nullptr;
    mainComponent = nullptr;
    scopeSync.setGUIEnabled(true);

    tabbedComponents.clearQuick();

    deviceMapping = scopeSync.getMapping();
    
    scopeSync.clearBCMLookAndFeels();

    // Firstly add the system LookAndFeels
    bool useImageCache = UserSettings::getInstance()->getPropertyBoolValue("useimagecache", true);
    ScopedPointer<XmlElement> systemLookAndFeels = scopeSync.getSystemLookAndFeels();
    
    setupLookAndFeels(*systemLookAndFeels, useImageCache);

    String errorText;
    String errorDetails;

    XmlElement& layoutXml = scopeSync.getLayout(errorText, errorDetails, forceReload);

    if (errorText.isNotEmpty())
        scopeSync.setSystemError(errorText, errorDetails);
    
    // Then override with any layout-specific ones
    XmlElement* child = layoutXml.getChildByName("lookandfeels");

    if (child)
        setupLookAndFeels(*child, useImageCache);

    BCMLookAndFeel* defaultLookAndFeel = scopeSync.getBCMLookAndFeelById("default");

    if (defaultLookAndFeel == nullptr)
        defaultLookAndFeel = scopeSync.getBCMLookAndFeelById("system:default");

    child = layoutXml.getChildByName("defaults");

    if (child)
        setupDefaults(*child);
    else
    {
        XmlElement defaults("defaults");
        setupDefaults(defaults);
    }

    int enableTooltipsUserSetting = UserSettings::getInstance()->getAppProperties()->getIntValue("enabletooltips", 0);
    
    if ((tooltipWindow == nullptr && enableTooltipsUserSetting == 0 && settings.enableTooltips) || enableTooltipsUserSetting == 1)
    {
        tooltipWindow = new TooltipWindow();
        tooltipWindow->setLookAndFeel(defaultLookAndFeel);

        int tooltipDelayTimeUserSetting = UserSettings::getInstance()->getAppProperties()->getIntValue("tooltipdelaytime", -1);

        if (tooltipDelayTimeUserSetting >= 0)
            settings.tooltipDelayTime = tooltipDelayTimeUserSetting;

        tooltipWindow->setMillisecondsBeforeTipAppears(settings.tooltipDelayTime);
    }

    child = layoutXml.getChildByName("component");

    if (child)
        createComponent(*child);
    else
    {
        XmlElement component("component");
        createComponent(component);
    }

    setSize(mainComponent->getWidth(), mainComponent->getHeight());

    grabKeyboardFocus();
}

void ScopeSyncGUI::setupLookAndFeels(XmlElement& lookAndFeelsXML, bool useImageCache)
{
    forEachXmlChildElement(lookAndFeelsXML, child)
    {
        if (child->hasTagName("lookandfeel"))
        {
            setupLookAndFeel(*child, useImageCache);
        }
    }

    return;
}

void ScopeSyncGUI::setupLookAndFeel(XmlElement& lookAndFeelXML, bool useImageCache)
{
    if (lookAndFeelXML.hasAttribute("id"))
    {
        String layoutDirectory = scopeSync.getLayoutDirectory();
        
        String id = lookAndFeelXML.getStringAttribute("id");
        // DBG("ScopeSyncGUI::setupLookAndFeel: Setting up LookAndFeel: id = " + id);

        BCMLookAndFeel* bcmLookAndFeel;

        if (lookAndFeelXML.hasAttribute("parentid"))
        {
            String parentid = lookAndFeelXML.getStringAttribute("parentid");
            BCMLookAndFeel* parentBCMLookAndFeel = scopeSync.getBCMLookAndFeelById(parentid);

            if (parentBCMLookAndFeel != nullptr)
            {
                // DBG("ScopeSyncGUI::setupLookAndFeel: Found parent: id = " + parentid);
                bcmLookAndFeel = new BCMLookAndFeel(lookAndFeelXML, *parentBCMLookAndFeel, layoutDirectory);
            }
            else
                bcmLookAndFeel = new BCMLookAndFeel(lookAndFeelXML, layoutDirectory, useImageCache);
        }
        else
        {
            bcmLookAndFeel = new BCMLookAndFeel(lookAndFeelXML, layoutDirectory, useImageCache);
        }

        if (bcmLookAndFeel != nullptr)
        {
            scopeSync.addBCMLookAndFeel(bcmLookAndFeel);
        }
        else
        {
            // DBG("ScopeSyncGUI::setupLookAndFeel: Failed to set up LookAndFeel id: " + id);
        }
    }
    else
    {
        // DBG("ScopeSyncGUI::setupLookAndFeel: Must supply an id for a LookAndFeel tag");
    }
    return;
}

void ScopeSyncGUI::setupDefaults(XmlElement& defaultsXML)
{
    XmlElement* settingsXML        = defaultsXML.getChildByName("settings");
    XmlElement* sliderXML          = defaultsXML.getChildByName("slider");
    XmlElement* labelXML           = defaultsXML.getChildByName("label");
    XmlElement* textButtonXML      = defaultsXML.getChildByName("textbutton");
    XmlElement* comboBoxXML        = defaultsXML.getChildByName("combobox");
    XmlElement* componentXML       = defaultsXML.getChildByName("component");
    XmlElement* tabbedComponentXML = defaultsXML.getChildByName("tabbedcomponent");
    XmlElement* tabXML             = defaultsXML.getChildByName("tab");
    
    if (settingsXML != nullptr)
        readSettingsXml(*settingsXML);
    else
    {
        XmlElement emptySettings("settings");
        readSettingsXml(emptySettings);
    }    

    if (sliderXML != nullptr)
        defaultSliderProperties = new SliderProperties(*this, *sliderXML);
    else
        defaultSliderProperties = new SliderProperties(*this);
    
    if (labelXML != nullptr)
        defaultLabelProperties = new LabelProperties(*labelXML);
    else
        defaultLabelProperties = new LabelProperties();
    
    if (textButtonXML != nullptr)
        defaultTextButtonProperties = new TextButtonProperties(*textButtonXML);
    else
        defaultTextButtonProperties = new TextButtonProperties();
    
    if (comboBoxXML != nullptr)
        defaultComboBoxProperties = new ComboBoxProperties(*comboBoxXML);
    else
        defaultComboBoxProperties = new ComboBoxProperties();
    
    if (componentXML != nullptr)
        defaultComponentProperties = new ComponentProperties(*componentXML);
    else
        defaultComponentProperties = new ComponentProperties();
    
    if (tabbedComponentXML != nullptr)
        defaultTabbedComponentProperties = new TabbedComponentProperties(*tabbedComponentXML);
    else
        defaultTabbedComponentProperties = new TabbedComponentProperties();
    
    if (tabXML != nullptr)
        defaultTabProperties = new TabProperties(*tabXML);
    else
        defaultTabProperties = new TabProperties();
}

void ScopeSyncGUI::readSettingsXml(XmlElement& settingsXML)
{
    // Encoder Snap
    if (settingsXML.getBoolAttribute("encodersnap", false))
        settings.encoderSnap = BCMSlider::snap;
    else
        settings.encoderSnap = BCMSlider::dontSnap;

    // Popup Enabled
    if (settingsXML.getBoolAttribute("popupenabled", false))
        settings.popupEnabled = BCMSlider::popupEnabled;
    else
        settings.popupEnabled = BCMSlider::popupDisabled;
    
    // Velocity Based Mode
    if (settingsXML.getBoolAttribute("velocitybasedmode", false))
        settings.velocityBasedMode = BCMSlider::velocityBasedModeOn;
    else
        settings.velocityBasedMode = BCMSlider::velocityBasedModeOff;
    
    // Rotary Movement
    String rotaryMovementString = settingsXML.getStringAttribute("rotarymovement", "vertical");
    
    if (rotaryMovementString.equalsIgnoreCase("rotary"))
        settings.rotaryMovement = BCMSlider::rotary;
    else if (rotaryMovementString.equalsIgnoreCase("horizontal"))
        settings.rotaryMovement = BCMSlider::horizontal;
    else if (rotaryMovementString.equalsIgnoreCase("horizontalvertical"))
        settings.rotaryMovement = BCMSlider::horizontalVertical;
    else
        settings.rotaryMovement = BCMSlider::vertical;
    
    // Inc/Dec Button Mode
    String incDecButtonModeString = settingsXML.getStringAttribute("incdecbuttonmode", "notdraggable");
    
    if (incDecButtonModeString.equalsIgnoreCase("autodirection"))
        settings.incDecButtonMode = Slider::incDecButtonsDraggable_AutoDirection;
    else if (incDecButtonModeString.equalsIgnoreCase("horizontal"))
        settings.incDecButtonMode = Slider::incDecButtonsDraggable_Horizontal;
    else if (incDecButtonModeString.equalsIgnoreCase("vertical"))
        settings.incDecButtonMode = Slider::incDecButtonsDraggable_Vertical;
    else
        settings.incDecButtonMode = Slider::incDecButtonsNotDraggable;
    
    // Tooltip settings
    settings.enableTooltips   = settingsXML.getBoolAttribute("enabletooltips", true);
    settings.tooltipDelayTime = settingsXML.getIntAttribute("tooltipdelaytime", 700);
}

void ScopeSyncGUI::createComponent(XmlElement& componentXML)
{
    const String& layoutDirectory = scopeSync.getLayoutDirectory();
        
    addAndMakeVisible(mainComponent = new BCMComponent(*this, "MainComp", true));
    
    mainComponent->applyProperties(componentXML, layoutDirectory);
    return;
}

void ScopeSyncGUI::timerCallback()
{
    if (scopeSync.guiNeedsReloading())
    {
        createGUI(true);
        scopeSync.setGUIReload(false);
    }
}

void ScopeSyncGUI::getAllCommands(Array<CommandID>& commands)
{
    if (!scopeSync.configurationIsReadOnly())
    {
        const CommandID ids[] = { CommandIDs::addConfig,
                                  CommandIDs::saveConfig,
                                  CommandIDs::saveConfigAs,
                                  CommandIDs::undo,
                                  CommandIDs::redo,
                                  CommandIDs::showHideEditToolbar,
                                  CommandIDs::snapshot,
                                  CommandIDs::showUserSettings,
                                  CommandIDs::showConfigurationManager,
                                  CommandIDs::chooseConfiguration,
                                  CommandIDs::reloadSavedConfiguration
                                };

        commands.addArray (ids, numElementsInArray(ids));
    }
    else
    {
        const CommandID ids[] = { CommandIDs::addConfig,
                                  CommandIDs::snapshot,
                                  CommandIDs::showUserSettings,
                                  CommandIDs::chooseConfiguration
                                };

        commands.addArray (ids, numElementsInArray(ids));
    }

    
}

void ScopeSyncGUI::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::undo:
        result.setInfo("Undo", "Undo latest change", CommandCategories::general, !(scopeSync.getUndoManager().canUndo()));
        result.defaultKeypresses.add(KeyPress ('z', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::redo:
        result.setInfo("Redo", "Redo latest change", CommandCategories::general, !(scopeSync.getUndoManager().canRedo()));
        result.defaultKeypresses.add(KeyPress ('y', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::addConfig:
        result.setInfo("Add Configuration", "Create a new Configuration", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('w', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::saveConfig:
        result.setInfo("Save Configuration", "Save Configuration", CommandCategories::configmgr, !(scopeSync.getConfiguration().hasChangedSinceSaved()));
        result.defaultKeypresses.add(KeyPress ('s', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::saveConfigAs:
        result.setInfo("Save Configuration As...", "Save Configuration as a new file", CommandCategories::configmgr, scopeSync.configurationIsReadOnly());
        result.defaultKeypresses.add(KeyPress ('s', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    case CommandIDs::showHideEditToolbar:
        result.setInfo("Toggle Edit Toolbar display", "Switches the Edit Toolbar between shown and hidden modes", CommandCategories::general, scopeSync.configurationIsReadOnly());
        result.defaultKeypresses.add(KeyPress ('e', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::snapshot:
        result.setInfo("Snapshot", "Sends a snapshot of all current parameter values", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('t', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::showUserSettings:
        result.setInfo("Show User Settings", "Shows the User Settings Window", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('u', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::showConfigurationManager:
        result.setInfo("Show Configuration Manager", "Shows the Configuration Manager Window", CommandCategories::general, scopeSync.configurationIsReadOnly());
        result.defaultKeypresses.add(KeyPress ('m', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::chooseConfiguration:
        result.setInfo("Choose Configuration", "Opens the Configuration Chooser", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('o', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::reloadSavedConfiguration:
        result.setInfo("Reload Saved Configuration", "Reloads the most recently saved version of the current Configuration. This will discard all unsaved changes and wipe the undo history.", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('r', ModifierKeys::commandModifier, 0));
        break;
    }
}

bool ScopeSyncGUI::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::undo:                     undo(); break;
        case CommandIDs::redo:                     redo(); break;
        case CommandIDs::addConfig:                addConfig(); break;
        case CommandIDs::saveConfig:               save(); break;
        case CommandIDs::saveConfigAs:             saveAs(); break;
        case CommandIDs::showHideEditToolbar:      mainComponent->showHideEditToolbar(); break;
        case CommandIDs::snapshot:                 snapshot(); break;
        case CommandIDs::showUserSettings:         showUserSettings(); break;
        case CommandIDs::showConfigurationManager: showConfigurationManager(); break;
        case CommandIDs::chooseConfiguration:      chooseConfiguration(); break;
        case CommandIDs::reloadSavedConfiguration: reloadSavedConfiguration(); break;
        default:                                   return false;
    }

    return true;
}

ApplicationCommandTarget* ScopeSyncGUI::getNextCommandTarget() { return nullptr; }

void ScopeSyncGUI::alertBoxReloadConfirm(int result, ScopeSyncGUI* scopeSyncGUI)
{
    if (result)
    {
        scopeSyncGUI->getScopeSync().reloadSavedConfiguration();
    }
}

void ScopeSyncGUI::addConfig()
{
    scopeSync.removeAllChangeListeners();
    scopeSync.addChangeListener(this);
    scopeSync.addConfiguration(getParentMonitorArea());
}

void ScopeSyncGUI::save()
{
    scopeSync.saveConfiguration();
    scopeSync.applyConfiguration();
}

void ScopeSyncGUI::saveAs()
{
    if (scopeSync.saveConfigurationAs())
        ScopeSync::checkNewConfigIsInLocation(scopeSync.getConfiguration(), this, this);
}

void ScopeSyncGUI::undo()
{
    scopeSync.getUndoManager().undo();
    scopeSync.applyConfiguration();
}

void ScopeSyncGUI::redo()
{
    scopeSync.getUndoManager().redo();
    scopeSync.applyConfiguration();
}

void ScopeSyncGUI::snapshot()
{
    scopeSync.snapshot();
}

void ScopeSyncGUI::showConfigurationManager()
{
    scopeSync.showConfigurationManager(getScreenPosition().getX(), getScreenPosition().getY());
}

void ScopeSyncGUI::reloadSavedConfiguration()
{
    if (scopeSync.configurationHasUnsavedChanges())
        AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
                                     "Are you sure?",
                                     "There are unsaved changes. This will discard them and clear the undo history.",
                                     String::empty,
                                     String::empty,
                                     this,
                                     ModalCallbackFunction::forComponent(alertBoxReloadConfirm, this));
    else
        scopeSync.reloadSavedConfiguration();
}
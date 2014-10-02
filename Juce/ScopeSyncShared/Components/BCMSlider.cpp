/**
 * The BCModular version of Juce's Slider, which adds the ability
 * to be created from an XML definition, as well as being tied into
 * the ScopeSync parameter system
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

#include "BCMSlider.h"
#include "../Utils/BCMMath.h"
#include "../Components/BCMLookAndFeel.h"
#include "../Core/ScopeSync.h"
#include "../Core/ScopeSyncGUI.h"
#include "../Properties/SliderProperties.h"
#include "../Core/Global.h"

BCMSlider::BCMSlider(const String& name, ScopeSyncGUI& owner)
    : Slider(name), gui(owner), commandManager(gui.getScopeSync().getCommandManager())
{
    commandManager->registerAllCommandsForTarget(this);
}

BCMSlider::~BCMSlider() {}

void BCMSlider::applyProperties(SliderProperties& properties)
{
    setComponentID(properties.id);
    
    fontHeight         = properties.fontHeight;
    fontStyleFlags     = properties.fontStyleFlags;
    justificationFlags = properties.justificationFlags;
    
    overrideSliderStyle(properties.style);
    overridePopupEnabled(properties.popupEnabled);
    overrideVelocityBasedMode(properties.velocityBasedMode);

    setTextBoxStyle(properties.textBoxPosition,
                    properties.textBoxReadOnly,
                    properties.textBoxWidth,
                    properties.textBoxHeight);
    
    double rangeMin = properties.rangeMin;
    double rangeMax = properties.rangeMax;
    double rangeInt = properties.rangeInt;
    String tooltip  = properties.name;
    
    // Set up any mapping to TabbedComponent Tabs
    mapsToTabs = false;
    
    for (int i = 0; i < properties.tabbedComponents.size(); i++)
    {
        String tabbedComponentName = properties.tabbedComponents[i];
        String tabName = properties.tabNames[i];

        tabbedComponentNames.add(tabbedComponentName);
        tabNames.add(tabName);

        mapsToTabs = true;
            
        DBG("BCMSlider::applyProperties - mapped Tab: " + tabbedComponentName + ", " + tabName);
    }

    mapsToParameter = false;
    
    parameter = gui.getUIMapping(Ids::sliders, getName(), mapping);

    if (parameter != nullptr)
    {
        mapsToParameter = true;      
        
        String shortDesc;
        parameter->getDescriptions(shortDesc, tooltip);

        String uiSuffix = String::empty;
        parameter->getUIRanges(rangeMin, rangeMax, rangeInt, uiSuffix);

        ValueTree parameterSettings;
        parameter->getSettings(parameterSettings);

        if (parameterSettings.isValid() && parameterSettings.getNumChildren() > 0)
        {
            // We have discrete settings for the mapped parameter, so configure these
            settingsNames.clear();

            for (int i = 0; i < parameterSettings.getNumChildren(); i++)
            {
                ValueTree parameterSetting = parameterSettings.getChild(i);
                String    settingName      = parameterSetting.getProperty(Ids::name, "__NO_NAME__");

                settingsNames.add(settingName);
            }
            
            if (getEncoderSnap(properties.encoderSnap))
                rangeInt = 1;
        }

        setRange(rangeMin, rangeMax, rangeInt);
        setTextValueSuffix(uiSuffix);

        setDoubleClickReturnValue(true, parameter->getUIResetValue());
        setSkewFactor(parameter->getUISkewFactor());
        
        DBG("BCMSlider::applyProperties - " + getName() + " mapping to parameter: " + parameter->getName());
        parameter->mapToUIValue(getValueObject());
    }
    else
    {
        setRange(rangeMin, rangeMax, rangeInt);
    }

    setTooltip(tooltip);
    setPopupMenuEnabled(true);
    
    properties.bounds.copyValues(componentBounds);
    BCM_SET_BOUNDS
    BCM_SET_LOOK_AND_FEEL
}

String BCMSlider::getTextFromValue(double v)
{
    if (settingsNames.size() > 0)
    {
        return settingsNames[roundDoubleToInt(v)];
    }
    else
    {
        return Slider::getTextFromValue(v);
    }
}

double BCMSlider::getValueFromText(const String& text)
{
    if (settingsNames.size() > 0)
    {
        for (int i = 0; i < settingsNames.size(); i++)
        {
            if (settingsNames[i].containsIgnoreCase(text))
            {
                return i;
            }
        }

        return text.getIntValue();
    }
    else
    {
        return Slider::getValueFromText(text);
    }
}

void BCMSlider::mouseDown(const MouseEvent& event)
{
    if (event.mods.isPopupMenu())
    {
        showPopup();
    }
    else
    {
        switchToTabs();
        Slider::mouseDown(event);
    }
}

double BCMSlider::valueToProportionOfLength(double value)
{
    const double n = (value - getMinimum()) / (getMaximum() - getMinimum());
    const double skew = getSkewFactor();

    return (skew == 1.0 || n < FLT_EPSILON) ? n : pow (n, skew);
}

void BCMSlider::switchToTabs()
{
    for (int i = 0; i < tabbedComponentNames.size(); i++)
    {
        String tabbedComponentName = tabbedComponentNames[i];
        String tabName             = tabNames[i];

        Array<BCMTabbedComponent*> tabbedComponents;
            
        gui.getTabbedComponentsByName(tabbedComponentName, tabbedComponents);
        int numTabbedComponents = tabbedComponents.size();

        for (int j = 0; j < numTabbedComponents; j++)
        {
            StringArray tabNames = tabbedComponents[j]->getTabNames();
            int tabIndex = tabNames.indexOf(tabName, true);

            DBG("BCMSlider::switchToTabs - " + tabbedComponentName + ", " + tabName + ", " + String(tabIndex)); 
                tabbedComponents[j]->setCurrentTabIndex(tabIndex, true);
        }
    }
}

void BCMSlider::overrideSliderStyle(Slider::SliderStyle style)
{
    RotaryMovement rotaryMovementUserSetting = RotaryMovement(UserSettings::getInstance()->getAppProperties()->getIntValue("rotarymovement", 1));

    if (rotaryMovementUserSetting != noOverride_RM &&
        (  style == Rotary 
        || style == RotaryHorizontalDrag 
        || style == RotaryVerticalDrag 
        || style == RotaryHorizontalVerticalDrag))
    {
             if (rotaryMovementUserSetting == rotary)             setSliderStyle(Rotary);
        else if (rotaryMovementUserSetting == vertical)           setSliderStyle(RotaryVerticalDrag);
        else if (rotaryMovementUserSetting == horizontal)         setSliderStyle(RotaryHorizontalDrag);
        else if (rotaryMovementUserSetting == horizontalVertical) setSliderStyle(RotaryHorizontalVerticalDrag); 
    }
    else
    {
        setSliderStyle(style);
    }
}

void BCMSlider::overridePopupEnabled(bool popupEnabledFlag)
{
    PopupEnabled popupEnabledUserSetting = PopupEnabled(UserSettings::getInstance()->getAppProperties()->getIntValue("popupenabled", 1));

    if (popupEnabledUserSetting != noOverride_PE)
        popupEnabledFlag = (popupEnabledUserSetting == popupEnabled);
    
    setPopupDisplayEnabled(popupEnabledFlag, 0);
}

void BCMSlider::overrideVelocityBasedMode(bool velocityBasedMode)
{
    VelocityBasedMode velocityBasedModeUserSetting = VelocityBasedMode(UserSettings::getInstance()->getAppProperties()->getIntValue("velocitybasedmode", 1));

    if (velocityBasedModeUserSetting != noOverride_VBM)
        velocityBasedMode = (velocityBasedModeUserSetting == velocityBasedModeOn);
    
    setVelocityBasedMode(velocityBasedMode);
}

bool BCMSlider::getEncoderSnap(bool encoderSnap)
{
    EncoderSnap encoderSnapUserSetting = EncoderSnap(UserSettings::getInstance()->getAppProperties()->getIntValue("encodersnap", 1));
    
    if (encoderSnapUserSetting != noOverride_ES)
        encoderSnap = (encoderSnapUserSetting == snap);
    
    return encoderSnap;
}

void BCMSlider::getAllCommands (Array <CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::deleteItems,
                              CommandIDs::editItem,
                              CommandIDs::editMappedItem
                            };

    commands.addArray (ids, numElementsInArray (ids));
}

void BCMSlider::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::deleteItems:
        result.setInfo("Delete", "Delete selected items", CommandCategories::configmgr, mapsToParameter ? 0 : 1);
        result.defaultKeypresses.add (KeyPress (KeyPress::deleteKey, 0, 0));
        result.defaultKeypresses.add (KeyPress (KeyPress::backspaceKey, 0, 0));
        break;
    case CommandIDs::editItem:
        result.setInfo("Edit Item", "Edit the most recently selected item", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('e', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::editMappedItem:
        result.setInfo("Edit Mapped Item", "Edit item mapped to the most recently selected item", CommandCategories::general, mapsToParameter ? 0 : 1);
        result.defaultKeypresses.add(KeyPress ('e', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    }
}

bool BCMSlider::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::deleteItems:          deleteMapping(); break;
        case CommandIDs::editItem:             editMapping(); break;
        case CommandIDs::editMappedItem:       editMappedParameter(); break;
        default:                               return false;
    }

    return true;
}

ApplicationCommandTarget* BCMSlider::getNextCommandTarget()
{
    return nullptr;
}

void BCMSlider::deleteMapping()
{
    gui.getScopeSync().getConfiguration().deleteMapping(Ids::slider, mapping, nullptr);
    gui.getScopeSync().applyConfiguration();
}

void BCMSlider::editMapping()
{
    ConfigurationManagerCallout* configurationManagerCallout = new ConfigurationManagerCallout(gui.getScopeSync(), 400, 34);
    configurationManagerCallout->setMappingPanel(mapping, "Slider", getName());
    CallOutBox::launchAsynchronously(configurationManagerCallout, getScreenBounds(), nullptr);
}

void BCMSlider::editMappedParameter()
{
    ConfigurationManagerCallout* configurationManagerCallout = new ConfigurationManagerCallout(gui.getScopeSync(), 550, 700);
    configurationManagerCallout->setParameterPanel(parameter->getDefinition(), parameter->getParameterType());
    CallOutBox::launchAsynchronously(configurationManagerCallout, getScreenBounds(), nullptr);
}

void BCMSlider::showPopup()
{
    PopupMenu m;
    m.addCommandItem(commandManager, CommandIDs::editItem, "Edit Parameter Mapping");
    m.addCommandItem(commandManager, CommandIDs::editMappedItem, "Edit Mapped Parameter");
    m.addSeparator();
    m.addCommandItem(commandManager, CommandIDs::deleteItems, "Delete Parameter Mapping");
    
    m.showMenuAsync(PopupMenu::Options(), nullptr);  
}

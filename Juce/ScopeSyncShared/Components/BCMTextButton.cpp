/**
 * The BCModular version of Juce's TextButton, which adds the ability
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

#include "BCMTextButton.h"

#include "../Utils/BCMMath.h"
#include "../Components/BCMLookAndFeel.h"
#include "../Core/ScopeSync.h"
#include "../Core/ScopeSyncGUI.h"
#include "../Properties/TextButtonProperties.h"
#include "../Core/Global.h"
#include "../Configuration/ConfigurationManager.h"

BCMTextButton::BCMTextButton(ScopeSyncGUI& owner, String& name)
    : TextButton(name), BCMParameterWidget(owner)
{
    setParentWidget(this);
}

BCMTextButton::~BCMTextButton() {};

void BCMTextButton::applyProperties(TextButtonProperties& properties)
{
    applyWidgetProperties(properties);
    mapsToTabs = false;
    isCommandButton = false;
        
    CommandID commandToTrigger = 0;

    if (getName().equalsIgnoreCase("snapshot"))
        commandToTrigger = CommandIDs::snapshot;
    else if (getName().equalsIgnoreCase("showusersettings"))
        commandToTrigger = CommandIDs::showUserSettings;
    else if (getName().equalsIgnoreCase("showconfigurationmanager"))
        commandToTrigger = CommandIDs::showConfigurationManager;
    else if (getName().equalsIgnoreCase("chooseconfiguration"))
        commandToTrigger = CommandIDs::chooseConfiguration;
    else if (getName().equalsIgnoreCase("reloadconfiguration"))
        commandToTrigger = CommandIDs::reloadSavedConfiguration;
    
    if (commandToTrigger != 0)
    {
        setCommandToTrigger(commandManager, commandToTrigger, true);
        isCommandButton = true;
        setWantsKeyboardFocus(false);
        return;
    }

    String tooltip(properties.tooltip);
    String buttonText(properties.text);

    // DBG("BCMTextButton::applyProperties - setting up button: " + getName());

    int radioGroupId = properties.radioGroupId;
    
    // Set up any mapping to TabbedComponent Tabs
    String radioGroupTag = String::empty;

    for (int i = 0; i < properties.tabbedComponents.size(); i++)
    {
        String tabbedComponentName = properties.tabbedComponents[i];
        String tabName = properties.tabNames[i];

        tabbedComponentNames.add(tabbedComponentName);
        tabNames.add(tabName);

        radioGroupTag += properties.tabbedComponents[i];
        mapsToTabs = true;
            
        // DBG("BCMTextButton::applyProperties - mapped Tab: " + tabbedComponentName + ", " + tabName + ", RadioGrp: " + radioGroupTag);
    }
        
    if (mapsToTabs)
        radioGroupId = radioGroupTag.hashCode();

    upSettingIdx   = -1;
    downSettingIdx = -1;

    mappingType = noToggle;
    displayType = currentSetting;

    setupMapping(Ids::textButton, getName(), properties.mappingParentType, properties.mappingParent);
    
    if (mapsToParameter)
    {
        // DBG("BCMTextButton::applyProperties - mapping found: " + mapping.toXmlString());
        
        String paramShortDesc;
        parameter->getDescriptions(buttonText, tooltip);
        parameter->getSettings(settings);
        parameter->mapToUIValue(parameterValue);
        
        // Grab the correct mapping type
        mappingType = (MappingType)(int(mapping.getProperty(Ids::type)));
        // DBG("BCMTextButton::applyProperties - mappingType: " + String(mappingType));
        
        // For mapped buttons, we want them to send their
        // "down" value parameter changes on mouse down
        // except for toggle buttons, where it doesn't
        // make as much sense
        if (mappingType != toggle)
            setTriggeredOnMouseDown(true);
                
        if (settings.isValid())
        {
            int currentSettingIdx   = roundDoubleToInt(parameterValue.getValue());
            int maxSettingIdx       = settings.getNumChildren() - 1;

            if (mapping.getProperty(Ids::radioGroup).isInt())
            {
                radioGroupId = mapping.getProperty(Ids::radioGroup);
                // DBG("BCMTextButton::applyProperties - radioGroupId: " + String(radioGroupId));
            }
                
            // Set up the button display type and the initial button text
            displayType = (DisplayType)(int(mapping.getProperty(Ids::displayType)));
            
            if (displayType == downSetting)
                buttonText  = mapping.getProperty(Ids::settingDown);
            else if (displayType == custom)
                buttonText  = mapping.getProperty(Ids::customDisplay);
            else
                buttonText = settings.getChild(currentSettingIdx).getProperty(Ids::name, "__NO_NAME__");
            
            if (mappingType == toggle || mappingType == noToggle)
            {
                String settingDown = mapping.getProperty(Ids::settingDown);
                String settingUp   = mapping.getProperty(Ids::settingUp);

                for (int i = 0; i < settings.getNumChildren(); i++)
                {
                    ValueTree setting = settings.getChild(i);
                    String    settingName = setting.getProperty(Ids::name, "__NO_NAME__");

                    if (settingName == settingDown)
                    {
                        downSettingIdx = i;
                        // DBG("BCMTextButton::applyProperties - Found mapped parameter settings - down: " + settingDown);
                    }
                    else if (settingName == settingUp)
                    {
                        upSettingIdx = i;
                        // DBG("BCMTextButton::applyProperties - Found mapped parameter settings - up: " + settingUp);
                    }
                }

                if (mappingType == toggle)
                {
                    setClickingTogglesState(true);

                    if (currentSettingIdx == downSettingIdx)
                    {
                        setToggleState(true, dontSendNotification);
                        
                        if (mapsToTabs)
                            switchToTabs();
                    }
                }
            }
            else
            {
                if (mappingType == inc)
                {
                    if (currentSettingIdx == maxSettingIdx)
                        downSettingIdx = currentSettingIdx;
                    else
                        downSettingIdx = currentSettingIdx + 1;                    
                }
                else if (mappingType == incWrap)
                {
                    if (currentSettingIdx == maxSettingIdx)
                        downSettingIdx = 0;
                    else
                        downSettingIdx = currentSettingIdx + 1;  
                }
                else if (mappingType == dec)
                {
                    if (currentSettingIdx == 0)
                        downSettingIdx = 0;
                    else
                        downSettingIdx = currentSettingIdx - 1;  
                }
                else if (mappingType == decWrap)
                {
                    if (currentSettingIdx == 0)
                        downSettingIdx = maxSettingIdx;
                    else
                        downSettingIdx = currentSettingIdx - 1;  
                }

                String settingDown = mapping.getProperty(Ids::settingDown);
                
            }

            parameterValue.addListener(this);
        }
        else
        {
            buttonText = "MAP ERROR";
            // DBG("BCMTextButton::BCMTextButton - No values");
            mapsToParameter = false;
        }
    }

    url = properties.url;
    setTooltip (tooltip);
    setButtonText(buttonText);
    setRadioGroupId(radioGroupId);

    if (scopeSync.configurationIsReadOnly() && getName().equalsIgnoreCase("showconfigurationmanager"))
    {
        setVisible(false);
    }
}

const Identifier BCMTextButton::getComponentType() const { return Ids::textButton; };

void BCMTextButton::switchToTabs()
{
    for (int i = 0; i < tabbedComponentNames.size(); i++)
    {
        String tabbedComponentName = tabbedComponentNames[i];
        String tabName             = tabNames[i];

        Array<BCMTabbedComponent*> tabbedComponents;
            
        scopeSyncGUI.getTabbedComponentsByName(tabbedComponentName, tabbedComponents);
        int numTabbedComponents = tabbedComponents.size();

        for (int j = 0; j < numTabbedComponents; j++)
        {
            StringArray tabNames = tabbedComponents[j]->getTabNames();
            int tabIndex = tabNames.indexOf(tabName, true);

            DBG("BCMTextButton::switchToTabs - " + tabbedComponentName + ", " + tabName + ", " + String(tabIndex)); 
                tabbedComponents[j]->setCurrentTabIndex(tabIndex, true);
        }
    }
}

void BCMTextButton::setNextValues()
{
    int currentSettingIdx   = roundDoubleToInt(parameterValue.getValue());
    int maxSettingIdx       = settings.getNumChildren() - 1;
        
    if (mappingType == inc)
    {
        if (currentSettingIdx == maxSettingIdx)
            downSettingIdx = currentSettingIdx;
        else
            downSettingIdx = currentSettingIdx + 1;                    
    }
    else if (mappingType == incWrap)
    {
        if (currentSettingIdx == maxSettingIdx)
            downSettingIdx = 0;
        else
            downSettingIdx = currentSettingIdx + 1;  
    }
    else if (mappingType == dec)
    {
        if (currentSettingIdx == 0)
            downSettingIdx = 0;
        else
            downSettingIdx = currentSettingIdx - 1;  
    }
    else if (mappingType == decWrap)
    {
        if (currentSettingIdx == 0)
            downSettingIdx = maxSettingIdx;
        else
            downSettingIdx = currentSettingIdx - 1;  
    }
}

void BCMTextButton::mouseDown(const MouseEvent& event)
{
    DBG("BCMTextButton::mouseDown - button clicked: " + getName());

    if (!isCommandButton && event.mods.isPopupMenu())
    {
        showPopupMenu();
    }
    else
    {
        if (hasParameter())
            scopeSync.beginParameterChangeGesture(getParameter());

        TextButton::mouseDown(event);
    }
}

void BCMTextButton::mouseUp(const MouseEvent& event)
{
    DBG("BCMTextButton::mouseUp - button clicked: " + getName());

    if (!(event.mods.isPopupMenu()))
    {
        if (!isCommandButton && hasParameter() && mappingType == noToggle)
        {
            float valueToSet = (float)upSettingIdx;
            
            if (valueToSet >= 0)
                scopeSyncGUI.getScopeSync().setParameterFromGUI(*(parameter), valueToSet);
        }

        if (hasParameter())
            scopeSync.endParameterChangeGesture(getParameter());

        TextButton::mouseUp(event);
    }
}

void BCMTextButton::clicked()
{
    DBG("BCMTextButton::clicked - button clicked: " + getName());

    if (isCommandButton)
    {
        TextButton::clicked();
        return;
    }

    if (url.isWellFormed())
        url.launchInDefaultBrowser();

    if (hasParameter())
    {
        float valueToSet;

        if (mappingType == toggle)
        {
            if (getToggleState())
                valueToSet = (float)downSettingIdx;
            else
                valueToSet = (float)upSettingIdx;
        }
        else
            valueToSet = (float)downSettingIdx;
            
        if (valueToSet >= 0)
            scopeSyncGUI.getScopeSync().setParameterFromGUI(*(parameter), valueToSet);
    }
}

void BCMTextButton::valueChanged(Value& value)
{
    if (displayType == currentSetting)
    {
        String buttonText = settings.getChild(value.getValue()).getProperty(Ids::name, "__NO_NAME__");
        setButtonText(buttonText);
    }

    if (roundDoubleToInt(value.getValue()) == downSettingIdx && getClickingTogglesState())
    {
        setToggleState(true, dontSendNotification);

        if (mapsToTabs)
            switchToTabs();
    }
    else
    {
        setToggleState(false, dontSendNotification);
    }

    setNextValues();
}

void BCMTextButton::applyLookAndFeel(bool noStyleOverride)
{
    BCMWidget::applyLookAndFeel(noStyleOverride);

    if (!noStyleOverride)
    {
        String fillColourString  = styleOverride.getProperty(Ids::fillColour);
        String lineColourString  = styleOverride.getProperty(Ids::lineColour);
        String fillColour2String = styleOverride.getProperty(Ids::fillColour2);
        String lineColour2String = styleOverride.getProperty(Ids::lineColour2);

        if (fillColourString.isNotEmpty())
        {
            Colour fillColour (Colour::fromString(fillColourString));
            Colour lineColour (Colour::fromString(lineColourString));
            Colour fillColour2(Colour::fromString(fillColour2String));
            Colour lineColour2(Colour::fromString(lineColour2String));

            setColour(TextButton::buttonColourId,   fillColour);
            setColour(TextButton::textColourOffId,  lineColour);
            setColour(TextButton::buttonOnColourId, fillColour2);
            setColour(TextButton::textColourOnId,   lineColour2);
        }
    }
}

void BCMTextButton::overrideStyle()
{
    String fillColourString  = findColour(TextButton::buttonColourId).toString();
    String lineColourString  = findColour(TextButton::textColourOffId).toString();
    String fillColour2String = findColour(TextButton::buttonOnColourId).toString();
    String lineColour2String = findColour(TextButton::textColourOnId).toString();
    
    ConfigurationManagerCallout* configurationManagerCallout = new ConfigurationManagerCallout(scopeSyncGUI.getScopeSync(), 550, 135);
    configurationManagerCallout->setStyleOverridePanel(styleOverride, Ids::textButton, getName(), fillColourString, lineColourString, fillColour2String, lineColour2String);
    configurationManagerCallout->addChangeListener(this);
    CallOutBox::launchAsynchronously(configurationManagerCallout, getScreenBounds(), nullptr);
}

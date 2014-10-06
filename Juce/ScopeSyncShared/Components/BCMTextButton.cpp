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

const int BCMTextButton::clickBlockDuration = 500;

BCMTextButton::BCMTextButton(ScopeSyncGUI& owner, String& name)
    : TextButton(name), BCMParameterWidget(owner, this)
{}

BCMTextButton::~BCMTextButton()
{
    if (getName().equalsIgnoreCase("systemerrormoreinfo"))
        systemErrorDetails.removeListener(this);

    stopTimer();
};

void BCMTextButton::applyProperties(TextButtonProperties& properties)
{
    setComponentID(properties.id);
    
    clicksBlocked = false;

    String tooltip(properties.tooltip);
    String buttonText(properties.text);

    DBG("BCMTextButton::applyProperties - setting up button: " + getName());

    int radioGroupId = properties.radioGroupId;
    
    // Set up any mapping to TabbedComponent Tabs
    mapsToTabs = false;
    String radioGroupTag = String::empty;

    for (int i = 0; i < properties.tabbedComponents.size(); i++)
    {
        String tabbedComponentName = properties.tabbedComponents[i];
        String tabName = properties.tabNames[i];

        tabbedComponentNames.add(tabbedComponentName);
        tabNames.add(tabName);

        radioGroupTag += properties.tabbedComponents[i];
        mapsToTabs = true;
            
        DBG("BCMTextButton::applyProperties - mapped Tab: " + tabbedComponentName + ", " + tabName + ", RadioGrp: " + radioGroupTag);
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
        DBG("BCMTextButton::applyProperties - mapping found: " + mapping.toXmlString());
        
        String paramShortDesc;
        parameter->getDescriptions(buttonText, tooltip);
        parameter->getSettings(settings);
        parameter->mapToUIValue(parameterValue);
        
        // Grab the correct mapping type
        mappingType = (MappingType)(int(mapping.getProperty(Ids::type)));
        DBG("BCMTextButton::applyProperties - mappingType: " + String(mappingType));
        
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
                DBG("BCMTextButton::applyProperties - radioGroupId: " + String(radioGroupId));
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
                        DBG("BCMTextButton::applyProperties - Found mapped parameter settings - down: " + settingDown);
                    }
                    else if (settingName == settingUp)
                    {
                        upSettingIdx = i;
                        DBG("BCMTextButton::applyProperties - Found mapped parameter settings - up: " + settingUp);
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
    
    if (getName().equalsIgnoreCase("systemerrormoreinfo"))
    {
        systemErrorDetails.referTo(scopeSyncGUI.getScopeSync().getSystemErrorDetails());
        systemErrorDetails.addListener(this);
        setVisible(systemErrorDetails.toString().isNotEmpty());
    }

    url = properties.url;
    setTooltip (tooltip);
    setButtonText(buttonText);
    setRadioGroupId(radioGroupId);
    
    properties.bounds.copyValues(componentBounds);
    BCM_SET_BOUNDS
    BCM_SET_LOOK_AND_FEEL
}

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

void BCMTextButton::timerCallback()
{
    clicksBlocked = false;
    stopTimer();
}

void BCMTextButton::mouseDown(const MouseEvent& event)
{
    if (event.mods.isPopupMenu())
    {
        showPopupMenu();
    }
    else
    {
        TextButton::mouseDown(event);
    }
}
    
void BCMTextButton::mouseUp(const MouseEvent& event)
{
    if (!(event.mods.isPopupMenu()))
    {
        if (hasParameter() && mappingType == noToggle)
        {
            float valueToSet = (float)upSettingIdx;
            
            if (valueToSet >= 0)
                scopeSyncGUI.getScopeSync().setParameterFromGUI(*(parameter), valueToSet);
        }

        TextButton::mouseUp(event);
    }
}

void BCMTextButton::clicked()
{
    DBG("BCMTextButton::clicked - button clicked: " + getName());

    if (clicksBlocked)
    {
        DBG("BCMTextButton::clicked - clicks blocked");
    }
    else
    {
        if (url.isWellFormed())
            url.launchInDefaultBrowser();

        if (getName().equalsIgnoreCase("snapshot"))
        {
            // force sync of all controls
            scopeSyncGUI.getScopeSync().snapshot();
            clicksBlocked = true;
            startTimer(clickBlockDuration);
        }
        else if (getName().equalsIgnoreCase("showusersettings"))
        {
            scopeSyncGUI.showUserSettings();
        }
        else if (getName().equalsIgnoreCase("showconfigurationmanager"))
        {
            scopeSyncGUI.getScopeSync().showConfigurationManager(scopeSyncGUI.getScreenPosition().getX(), scopeSyncGUI.getScreenPosition().getY());
        }
        else if (getName().equalsIgnoreCase("chooseconfiguration"))
        {
            scopeSyncGUI.chooseConfiguration();
        }
        else if (getName().equalsIgnoreCase("reloadconfiguration"))
        {
            scopeSyncGUI.getScopeSync().applyConfiguration();
            clicksBlocked = true;
            startTimer(clickBlockDuration);
        }
        else if (getName().equalsIgnoreCase("systemerrormoreinfo"))
        {
            showSystemErrorDetails();
        }

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
}

void BCMTextButton::valueChanged(Value& value)
{
    if (value.refersToSameSourceAs(systemErrorDetails))
        setVisible(systemErrorDetails.toString().isNotEmpty());

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

void BCMTextButton::showSystemErrorDetails()
{
    SystemErrorDetailsCallout* errorDetailsBox = new SystemErrorDetailsCallout(scopeSyncGUI.getScopeSync().getSystemErrorDetails().getValue(), *this);
    CallOutBox::launchAsynchronously(errorDetailsBox, getScreenBounds(), nullptr);
}

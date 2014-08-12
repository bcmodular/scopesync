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

const int BCMTextButton::clickBlockDuration = 1000;

BCMTextButton::BCMTextButton(TextButtonProperties& properties, ScopeSyncGUI& owner, String& name) : TextButton(name), scopeSyncGUI(owner)
{
    clicksBlocked = false;

    String tooltip(properties.tooltip);
    String buttonText(properties.text);

    DBG("BCMTextButton::BCMTextButton - setting up button: " + name);

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
            
        DBG("BCMTextButton::BCMTextButton - mapped Tab: " + tabbedComponentName + ", " + tabName + ", RadioGrp: " + radioGroupTag);
    }
        
    if (mapsToTabs)
        radioGroupId = radioGroupTag.hashCode();

    upSettingIdx   = -1;
    downSettingIdx = -1;

    mappingType = noToggle;
    displayType = currentSetting;

    mapsToParameter = false;
    paramIdx = -1;

    ValueTree mapping;
    scopeSyncGUI.getUIMapping(ScopeSyncGUI::mappingTextButtonId, name, mapping, paramIdx);

    if (paramIdx != -1)
    {
        DBG("BCMTextButton::BCMTextButton - mapping found: " + mapping.toXmlString());
        mapsToParameter = true;       
        
        String paramShortDesc;
        scopeSyncGUI.getScopeSync().getParameterDescriptions(paramIdx, buttonText, tooltip);
        scopeSyncGUI.getScopeSync().getParameterSettings(paramIdx, settings);
        scopeSyncGUI.getScopeSync().mapToParameterUIValue(paramIdx, parameterValue);
        
        // Grab the correct mapping type
        String mappingTypeString = mapping.getProperty(ScopeSyncGUI::mappingTypeId);
        DBG("BCMTextButton::BCMTextButton - mappingTypeString: " + mappingTypeString);
        
             if (mappingTypeString == "toggle")  mappingType = toggle;
        else if (mappingTypeString == "inc")     mappingType = inc;
        else if (mappingTypeString == "dec")     mappingType = dec;
        else if (mappingTypeString == "incwrap") mappingType = incWrap;
        else if (mappingTypeString == "decwrap") mappingType = decWrap;
        else                                     mappingType = noToggle;
        
        if (settings.isValid())
        {
            int currentSettingIdx   = roundDoubleToInt(parameterValue.getValue());
            int maxSettingIdx       = settings.getNumChildren() - 1;

            if (mapping.hasProperty(ScopeSyncGUI::mappingRadioGroupId))
            {
                radioGroupId = mapping.getProperty(ScopeSyncGUI::mappingRadioGroupId);
                DBG("BCMTextButton::BCMTextButton - radioGroupId: " + String(radioGroupId));
            }
            
            // Set up the button display type and the initial button text
            String mappingDisplayTypeString = mapping.getProperty(ScopeSyncGUI::mappingDisplayTypeId);
            
            if (mappingDisplayTypeString == "parametername")
            {
                displayType = parameterName;
            }
            else if (mappingDisplayTypeString == "downsetting")
            {
                displayType = downSetting;
                buttonText  = mapping.getProperty(ScopeSyncGUI::mappingSettingDownId);
            }
            else if (mappingDisplayTypeString == "custom")
            {
                displayType = custom;
                buttonText  = mapping.getProperty(ScopeSyncGUI::mappingCustomDisplayId);
            }
            else
            {
                buttonText = settings.getChild(currentSettingIdx).getProperty(ScopeSync::paramTypeSettingNameId, "__NO_NAME__");
            }
        
            if (mappingType == toggle || mappingType == noToggle)
            {
                String settingDown = mapping.getProperty(ScopeSyncGUI::mappingSettingDownId);
                String settingUp   = mapping.getProperty(ScopeSyncGUI::mappingSettingUpId);

                for (int i = 0; i < settings.getNumChildren(); i++)
                {
                    ValueTree setting = settings.getChild(i);
                    String    settingName = setting.getProperty(ScopeSync::paramTypeSettingNameId, "__NO_NAME__");

                    if (settingName == settingDown)
                    {
                        downSettingIdx = i;
                        DBG("BCMTextButton::BCMTextButton - Found mapped parameter settings - down: " + settingDown);
                    }
                    else if (settingName == settingUp)
                    {
                        upSettingIdx = i;
                        DBG("BCMTextButton::BCMTextButton - Found mapped parameter settings - up: " + settingUp);
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

                String settingDown = mapping.getProperty(ScopeSyncGUI::mappingSettingDownId);
                
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
    
    setLookAndFeel(scopeSyncGUI.getScopeSync().getBCMLookAndFeelById(properties.bcmLookAndFeelId));

    setTooltip (tooltip);
    setButtonText(buttonText);
    setRadioGroupId(radioGroupId);
    
    setBounds(
        properties.x,
        properties.y,
        properties.width,
        properties.height
    );
}

BCMTextButton::~BCMTextButton()
{
    stopTimer();
};

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
        if (getName() == "snapshot")
        {
            // force sync of all controls
            scopeSyncGUI.getScopeSync().snapshot();
            clicksBlocked = true;
            startTimer(clickBlockDuration);
        }
        else if (getName() == "showusersettings")
        {
            scopeSyncGUI.showUserSettings();
        }
        else if (getName() == "chooseconfiguration")
        {
            if (!(scopeSyncGUI.getScopeSync().configurationIsLoading()))
            {
                scopeSyncGUI.chooseConfiguration();
            }
        }
        else if (getName() == "reloadconfiguration")
        {
            if (!(scopeSyncGUI.getScopeSync().configurationIsLoading()))
            {
                scopeSyncGUI.getScopeSync().storeParameterValues();    
                scopeSyncGUI.getScopeSync().loadConfiguration(false, true, true);
                clicksBlocked = true;
                startTimer(clickBlockDuration);
            }
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
                scopeSyncGUI.getScopeSync().setParameterFromGUI(getParamIdx(), valueToSet);
        }
    }
}

void BCMTextButton::valueChanged(Value& value)
{
    DBG("BCMTextButton::valueChanged - New value: " + value.toString());

    if (displayType == currentSetting)
    {
        String buttonText = settings.getChild(value.getValue()).getProperty(ScopeSync::paramTypeSettingNameId, "__NO_NAME__");
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

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

#include "BCMTextButton.h"
#include "../Core/ScopeSync.h"
#include "../Core/ScopeSyncGUI.h"
#include "../Properties/TextButtonProperties.h"
#include "../Core/Global.h"
#include "../Configuration/ConfigurationManager.h"
#include "../Parameters/BCMParameterController.h"

BCMTextButton::BCMTextButton(ScopeSyncGUI& owner, String& name)
    : TextButton(name), BCMParameterWidget(owner)
{
    setParentWidget(this);
}

BCMTextButton::~BCMTextButton() {};

void BCMTextButton::applyProperties(TextButtonProperties& props)
{
	// DBG("BCMTextButton::applyProperties - setting up button: " + getName());

    applyWidgetProperties(props);
    mapsToParameter = false;
    isCommandButton = true;
    
	// First see whether it's a command button and set it up if it is
    CommandID commandToTrigger = 0;

    if (getName().equalsIgnoreCase("showusersettings"))
        commandToTrigger = CommandIDs::showUserSettings;
    else if (getName().equalsIgnoreCase("showconfigurationmanager"))
        commandToTrigger = CommandIDs::showConfigurationManager;
    else if (getName().equalsIgnoreCase("newconfiguration"))
        commandToTrigger = CommandIDs::addConfig;
    else if (getName().equalsIgnoreCase("chooseconfiguration"))
        commandToTrigger = CommandIDs::chooseConfiguration;
    else if (getName().equalsIgnoreCase("reloadconfiguration"))
        commandToTrigger = CommandIDs::reloadSavedConfiguration;
    else if (getName().equalsIgnoreCase("showaboutbox"))
        commandToTrigger = CommandIDs::showAboutBox;
    
    if (commandToTrigger != 0)
    {
        setCommandToTrigger(commandManager, commandToTrigger, true);
        setWantsKeyboardFocus(false);
        return;
    }

	// Need to handle two types of snapshot (i.e. regular and snapshot all),
	// so can't use the normal command trigger method
	if (getName().equalsIgnoreCase("snapshot"))
	{
		setTooltip(props.tooltip);
		setButtonText(props.text);
		return;
	}

	// If it's not a command button, it must be a parameter button,
	// so let's do the remaining set up for parameter buttons
	isCommandButton = false;
	
    if (setupFixedButton(props))
        return;
	
    String tooltip(props.tooltip);
    String buttonText(props.text);

    int rgId = props.radioGroupId;
    
    String radioGroupTag = String::empty;

    upSettingIdx   = -1;
    downSettingIdx = -1;

    mappingType = noToggle;
    displayType = currentSetting;

    setupMapping(Ids::textButton, getName(), props.mappingParentType, props.mappingParent);
    
    if (mapsToParameter && parameter->isDiscrete())
    {
        // DBG("BCMTextButton::applyProperties - mapping found: " + mapping.toXmlString());
        
		buttonText = parameter->getShortDescription();
		tooltip    = parameter->getFullDescription(true);
		parameter->getSettings(settings);
        parameter->mapToUIValue(parameterValue);
        
        // Grab the correct mapping type
        mappingType = static_cast<MappingType>(int(mapping.getProperty(Ids::type)));
        // DBG("BCMTextButton::applyProperties - mappingType: " + String(mappingType));
        
        // For mapped buttons, we want them to send their
        // "down" value parameter changes on mouse down
        // except for toggle buttons, where it doesn't
        // make as much sense
        if (mappingType != toggle)
            setTriggeredOnMouseDown(true);
                
        int currentSettingIdx   = roundToInt(parameterValue.getValue());
        int maxSettingIdx       = settings.getNumChildren() - 1;

        if (mapping.getProperty(Ids::radioGroup).toString().isNotEmpty())
        {
			rgId = mapping.getProperty(Ids::radioGroup).toString().hashCode();
            // DBG("BCMTextButton::applyProperties - radioGroupId: " + String(radioGroupId));
        }
                
        // Set up the button display type and the initial button text
        displayType = static_cast<DisplayType>(int(mapping.getProperty(Ids::displayType)));
            
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
                    setToggleState(true, dontSendNotification);
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

    url = props.url;
    setTooltip (tooltip);
    setButtonText(buttonText);
    setRadioGroupId(rgId);
}

bool BCMTextButton::setupFixedButton(TextButtonProperties& props)
{
	if (ScopeSync::fixedParameterNames.contains(props.name))
	{
		BCMParameter* bcmParameter(scopeSync.getParameterController()->getParameterByName(props.name));

		if (bcmParameter != nullptr)
		{
			mappingType = toggle;
			setClickingTogglesState(true);
	
			upSettingIdx   = 0;
			downSettingIdx = 1;
	
			setParameter(bcmParameter);
    
			setToggleState(getParameter()->getUIValue() > 0, dontSendNotification);

			getParameter()->mapToUIValue(parameterValue);

			parameterValue.addListener(this);

			setTooltip(props.tooltip);
			setButtonText(props.text);

			return true;
		}
	}

    return false;
}

const Identifier BCMTextButton::getComponentType() const { return Ids::textButton; };

void BCMTextButton::setNextValues()
{
    int currentSettingIdx   = roundToInt(parameterValue.getValue());
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
        showPopupMenu();
    else
    {
		if (hasParameter())
		{
            if (parameter->isReadOnly())
                return;

			getParameter()->beginChangeGesture();
		}
		else
			DBG("BCMTextButton::mouseDown - button " + getName() + " doesn't have a parameter");
	
        TextButton::mouseDown(event);
    }
}

void BCMTextButton::mouseUp(const MouseEvent& event)
{
    DBG("BCMTextButton::mouseUp - button clicked: " + getName());

    if (!(event.mods.isPopupMenu()))
    {
        if (hasParameter())
		{
            if (parameter->isReadOnly())
                return;

            if (!isCommandButton && mappingType == noToggle)
            {
                float valueToSet = static_cast<float>(upSettingIdx);
            
                if (valueToSet >= 0)
                    scopeSync.getParameterController()->setParameterFromGUI(*(parameter), valueToSet);
            }

			parameter->endChangeGesture();
		}

        TextButton::mouseUp(event);
    }
}

void BCMTextButton::clicked(const ModifierKeys& modifiers)
{
    DBG("BCMTextButton::clicked - button clicked: " + getName());

	if (getName().equalsIgnoreCase("snapshot"))
	{
		if (modifiers.isCommandDown())
			ScopeSync::snapshotAll();
		else
			scopeSync.getParameterController()->snapshot();

		return;
	}

	if (isCommandButton)
    {
        TextButton::clicked();
        return;
    }

    if (url.isWellFormed())
	{
		url.launchInDefaultBrowser();
		return;
	}

    if (hasParameter())
    {
        if (parameter->isReadOnly())
            return;

        float valueToSet;

        if (mappingType == toggle)
        {
            if (getToggleState())
                valueToSet = static_cast<float>(downSettingIdx);
            else
                valueToSet = static_cast<float>(upSettingIdx);
        }
        else
            valueToSet = static_cast<float>(downSettingIdx);
            
        if (valueToSet >= 0)
            parameter->setUIValue(valueToSet);
    }
}

void BCMTextButton::mouseDrag(const MouseEvent& e)
{
	if (hasParameter() && parameter->isReadOnly())
		return;
	else
		TextButton::mouseDrag(e);
}

void BCMTextButton::focusGained(FocusChangeType f)
{
	if (hasParameter() && parameter->isReadOnly())
		return;
	else
		TextButton::focusGained(f);
}

void BCMTextButton::focusLost(FocusChangeType f)
{
	if (hasParameter() && parameter->isReadOnly())
		return;
	else
		TextButton::focusLost(f);
}

void BCMTextButton::valueChanged(Value& value)
{
	if  (  getName().equalsIgnoreCase("presetlist")
		|| getName().equalsIgnoreCase("patchwindow")
		|| getName().equalsIgnoreCase("monoeffect")
		|| getName().equalsIgnoreCase("bypasseffect")
		|| getName().equalsIgnoreCase("shellpresetwindow")
		)
	{
		setToggleState((int(parameterValue.getValue()) > 0), dontSendNotification);
		return;
	}

    if (displayType == currentSetting)
    {
        String buttonText = settings.getChild(value.getValue()).getProperty(Ids::name, "__NO_NAME__");
        setButtonText(buttonText);
    }

    if (roundToInt(value.getValue()) == downSettingIdx && getClickingTogglesState())
        setToggleState(true, dontSendNotification);
    else
        setToggleState(false, dontSendNotification);

    setNextValues();
}

void BCMTextButton::applyLookAndFeel(bool noStyleOverride)
{
    BCMWidget::applyLookAndFeel(noStyleOverride);

    bool useColourOverrides = styleOverride.getProperty(Ids::useColourOverrides);

    if (!noStyleOverride && useColourOverrides)
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
    
    ConfigurationManagerCalloutWindow* configurationManagerCalloutWindow = new ConfigurationManagerCalloutWindow(scopeSyncGUI.getScopeSync(), 550, 165);
    configurationManagerCalloutWindow->setStyleOverridePanel(styleOverride, Ids::textButton, getName(), widgetTemplateId, fillColourString, lineColourString, fillColour2String, lineColour2String);
    configurationManagerCalloutWindow->addChangeListener(this);
    CallOutBox::launchAsynchronously(configurationManagerCalloutWindow, getScreenBounds(), nullptr);
}

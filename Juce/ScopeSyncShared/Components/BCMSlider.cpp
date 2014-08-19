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

BCMSlider::BCMSlider(const String& name) : Slider(name) {}

BCMSlider::~BCMSlider() {}

void BCMSlider::applyProperties(SliderProperties& properties, ScopeSyncGUI& gui)
{
    setComponentID(properties.id);
    
    fontHeight         = properties.fontHeight;
    fontStyleFlags     = properties.fontStyleFlags;
    justificationFlags = properties.justificationFlags;
    
    overrideSliderStyle(gui.getScopeSync().getAppProperties(), properties.style);
    overridePopupEnabled(gui.getScopeSync().getAppProperties(), properties.popupEnabled);
    overrideVelocityBasedMode(gui.getScopeSync().getAppProperties(), properties.velocityBasedMode);

    setTextBoxStyle(properties.textBoxPosition,
                    properties.textBoxReadOnly,
                    properties.textBoxWidth,
                    properties.textBoxHeight);
    
    double rangeMin = properties.rangeMin;
    double rangeMax = properties.rangeMax;
    double rangeInt = properties.rangeInt;
    String tooltip  = properties.name;
    
    mapsToParameter = false;
    
    ValueTree mapping;
    parameter = gui.getUIMapping(ScopeSyncGUI::mappingSliderId, getName(), mapping);

    if (parameter != nullptr)
    {
        mapsToParameter = true;      
        
        String shortDesc;
        parameter->getDescriptions(shortDesc, tooltip);

        ValueTree parameterSettings;
        parameter->getSettings(parameterSettings);

        if (parameterSettings.isValid())
        {
            // We have discrete settings for the mapped parameter, so configure these
            settingsNames.clear();

            for (int i = 0; i < parameterSettings.getNumChildren(); i++)
            {
                ValueTree parameterSetting = parameterSettings.getChild(i);
                String    settingName      = parameterSetting.getProperty(BCMParameter::paramTypeSettingNameId, "__NO_NAME__");

                settingsNames.add(settingName);
            }
            rangeMin = 0;
            rangeMax = parameterSettings.getNumChildren() - 1;

            if (getEncoderSnap(gui.getScopeSync().getAppProperties(), properties.encoderSnap))
                rangeInt = 1;

            setRange(rangeMin, rangeMax, rangeInt);
        }
        else
        {
            String uiSuffix = String::empty;

            parameter->getUIRanges(rangeMin, rangeMax, rangeInt, uiSuffix);

            setRange(rangeMin, rangeMax, rangeInt);

            // Set up a regular slider
            setTextValueSuffix(uiSuffix);

            double uiResetValue = 0.0f;

            if (parameter->getUIResetValue(uiResetValue))
                setDoubleClickReturnValue(true, uiResetValue);

            double uiSkewFactor = 0.0f;

            if (parameter->getUISkewFactor(uiSkewFactor))
            {
                setSkewFactor(uiSkewFactor);
            }
        }

        DBG("BCMSlider::applyProperties - " + getName() + " mapping to parameter: " + parameter->getName());
        parameter->mapToUIValue(getValueObject());
    }
    else
    {
        setRange(rangeMin, rangeMax, rangeInt);
    }

    setTooltip(tooltip);
    
    componentBounds = properties.bounds;
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

void BCMSlider::overrideSliderStyle(PropertiesFile& appProperties, Slider::SliderStyle style)
{
    RotaryMovement rotaryMovementUserSetting = RotaryMovement(appProperties.getIntValue("rotarymovement", 1));

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

void BCMSlider::overridePopupEnabled(PropertiesFile& appProperties, bool popupEnabledFlag)
{
    PopupEnabled popupEnabledUserSetting = PopupEnabled(appProperties.getIntValue("popupenabled", 1));

    if (popupEnabledUserSetting != noOverride_PE)
        popupEnabledFlag = (popupEnabledUserSetting == popupEnabled);
    
    setPopupDisplayEnabled(popupEnabledFlag, 0);
}

void BCMSlider::overrideVelocityBasedMode(PropertiesFile& appProperties, bool velocityBasedMode)
{
    VelocityBasedMode velocityBasedModeUserSetting = VelocityBasedMode(appProperties.getIntValue("velocitybasedmode", 1));

    if (velocityBasedModeUserSetting != noOverride_VBM)
        velocityBasedMode = (velocityBasedModeUserSetting == velocityBasedModeOn);
    
    setVelocityBasedMode(velocityBasedMode);
}

bool BCMSlider::getEncoderSnap(PropertiesFile& appProperties, bool encoderSnap)
{
    EncoderSnap encoderSnapUserSetting = EncoderSnap(appProperties.getIntValue("encodersnap", 1));
    
    if (encoderSnapUserSetting != noOverride_ES)
        encoderSnap = (encoderSnapUserSetting == snap);
    
    return encoderSnap;
}

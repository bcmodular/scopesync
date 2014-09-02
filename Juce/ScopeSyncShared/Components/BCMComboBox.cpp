/**
 * The BCModular version of Juce's ComboBox, which adds the ability
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

#include "BCMComboBox.h"

#include "../Utils/BCMMath.h"
#include "../Components/BCMLookAndFeel.h"
#include "../Core/ScopeSync.h"
#include "../Core/ScopeSyncGUI.h"
#include "../Properties/ComboBoxProperties.h"
#include "../Core/Global.h"

BCMComboBox::BCMComboBox(String& name) : ComboBox(name) {}

BCMComboBox::~BCMComboBox() {}

void BCMComboBox::applyProperties(ScopeSyncGUI& gui, ComboBoxProperties& properties)
{
    setComponentID(properties.id);
    
    fontHeight         = properties.fontHeight;
    fontStyleFlags     = properties.fontStyleFlags;
    
    setEditableText(properties.editableText);
    setJustificationType(Justification(properties.justificationFlags));
    setTextWhenNothingSelected(properties.nothingSelectedText);
    setTextWhenNoChoicesAvailable(properties.noChoicesText);
    
    String tooltip  = properties.tooltip;
    mapsToParameter = false;
    
    // Will ignore any items provided in the layout for now,
    // as there's no functionality associated with them

    mapsToParameter = false;
        
    ValueTree mapping;
    parameter = gui.getUIMapping(Ids::comboBoxes, getName(), mapping);

    if (parameter != nullptr)
    {
        mapsToParameter = true;       
        
        ValueTree parameterSettings;
        parameter->getSettings(parameterSettings);

        if (parameterSettings.isValid())
        {
            clear(juce::dontSendNotification);
            
            parameter->mapToUIValue(parameterValue);
            parameterValue.addListener(this);
            
            String shortDesc;
            parameter->getDescriptions(shortDesc, tooltip);

            for (int i = 0; i < parameterSettings.getNumChildren(); i++)
            {
                ValueTree parameterSetting = parameterSettings.getChild(i);
                String    settingName      = parameterSetting.getProperty(Ids::setting, "__NO_NAME__");

                addItem(settingName, i + 1);
                // DBG("BCMComboBox::BCMComboBox - Added parameter value to drop-down: " + settingName);
            }
            
            setSelectedItemIndex(roundDoubleToInt(parameterValue.getValue()), juce::dontSendNotification);
        }
        else
        {
            // DBG("BCMComboBox::BCMComboBox - No values to list");
            mapsToParameter = false;
        }
    }
    
    setTooltip(tooltip);
    
    properties.bounds.copyValues(componentBounds);
    BCM_SET_BOUNDS
    BCM_SET_LOOK_AND_FEEL
}

void BCMComboBox::valueChanged(Value& value)
{
    if (value.refersToSameSourceAs(parameterValue))
    {
        setSelectedItemIndex(roundDoubleToInt(value.getValue()), juce::dontSendNotification);
    }
    else
    {
        ComboBox::valueChanged(value);
    }
}

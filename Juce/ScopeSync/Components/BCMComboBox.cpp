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
#include "../Core/ScopeSyncGUI.h"
#include "../Properties/ComboBoxProperties.h"
#include "../Core/Global.h"
#include "../Core/BCMParameter.h"

BCMComboBox::BCMComboBox(String& name, ScopeSyncGUI& owner)
    : ComboBox(name), BCMParameterWidget(owner)
{
    setParentWidget(this);
    setWantsKeyboardFocus(true);
}

BCMComboBox::~BCMComboBox() {}

void BCMComboBox::applyProperties(ComboBoxProperties& props)
{
    mapsToParameter = false;
	applyWidgetProperties(props);
    
    fontHeight         = props.fontHeight;
    fontStyleFlags     = props.fontStyleFlags;
    
    setEditableText(props.editableText);
    setJustificationType(Justification(props.justificationFlags));
    setTextWhenNothingSelected(props.nothingSelectedText);
    setTextWhenNoChoicesAvailable(props.noChoicesText);
    
    String tooltip  = props.tooltip;
    
    setupMapping(Ids::comboBox, getName(), props.mappingParentType, props.mappingParent);

    if (mapsToParameter)
    {
		setEditableText(!parameter->isReadOnly() ? props.editableText : false);
		clear(juce::dontSendNotification);

		parameter->mapToUIValue(parameterValue);
        parameterValue.addListener(this);

		if (parameter->isDiscrete())
		{
			ValueTree parameterSettings;
			parameter->getSettings(parameterSettings);
            
			String shortDesc;
			parameter->getDescriptions(shortDesc, tooltip);

			for (int i = 0; i < parameterSettings.getNumChildren(); i++)
			{
				ValueTree parameterSetting = parameterSettings.getChild(i);
				String    settingName      = parameterSetting.getProperty(Ids::name, "__NO_NAME__");

				addItem(settingName, i + 1);
			}
            
			setSelectedItemIndex(roundDoubleToInt(parameterValue.getValue()), juce::dontSendNotification);
		}
    }
    
    setTooltip(tooltip);
}

const Identifier BCMComboBox::getComponentType() const { return Ids::comboBox; };

void BCMComboBox::mouseDown(const MouseEvent& event)
{
    if (event.mods.isPopupMenu())
        showPopupMenu();
    else
		if (hasParameter() && parameter->isReadOnly())
			return;
		else
			ComboBox::mouseDown(event);
}

void BCMComboBox::valueChanged(Value& value)
{
    if (value.refersToSameSourceAs(parameterValue))
        setSelectedItemIndex(roundDoubleToInt(value.getValue()), juce::dontSendNotification);
    else
        ComboBox::valueChanged(value);
}

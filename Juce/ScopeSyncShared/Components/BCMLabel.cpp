/**
 * The BCModular version of Juce's Label, which adds the ability
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

#include "BCMLabel.h"
#include "../Utils/BCMMath.h"
#include "../Components/BCMLookAndFeel.h"
#include "../Core/ScopeSync.h"
#include "../Core/ScopeSyncGUI.h"
#include "../Properties/LabelProperties.h"
#include "../Core/Global.h"

BCMLabel::BCMLabel(String& name, String& text, ScopeSyncGUI& owner)
    : Label(name, text), BCMParameterWidget(owner)
{
    setParentWidget(this);
    setWantsKeyboardFocus(true);
}

BCMLabel::~BCMLabel() {}

void BCMLabel::applyProperties(LabelProperties& properties) 
{
    applyWidgetProperties(properties);
    
    String labelText = getText();
    String tooltip   = getText();
    
    mapsToParameter = false;
    
    if (getName().equalsIgnoreCase("configurationfilepath"))
    {
        labelText = scopeSyncGUI.getScopeSync().getConfigurationFile().getFullPathName();
    }
    else if (getName().equalsIgnoreCase("configurationname"))
    {
        labelText = scopeSyncGUI.getScopeSync().getConfigurationName(true);
    }
    else
    {
        setupMapping(Ids::label, getName(), properties.mappingParentType, properties.mappingParent);

        if (mapsToParameter)
        {
            String shortDescription;
            String fullDescription;
            
            parameter->getDescriptions(shortDescription, fullDescription);
            tooltip = fullDescription;
            
            if (properties.parameterTextDisplay == LabelProperties::parameterName)
                labelText = parameter->getName();
            else if (properties.parameterTextDisplay == LabelProperties::shortDescription)
                labelText = shortDescription;
            else if (properties.parameterTextDisplay == LabelProperties::fullDescription)
                labelText = fullDescription;
            else if (properties.parameterTextDisplay == LabelProperties::scopeCode)
                labelText = parameter->getScopeCodeText();
        }
    }

    // Only relevant if label is editable. Not currently supported
    setEditable(false, false, false);
      
    setText(labelText, dontSendNotification);
    setTooltip(tooltip);
    
    setFont(Font(properties.fontHeight, properties.fontStyleFlags));
    setJustificationType(Justification(properties.justificationFlags));
}

const Identifier BCMLabel::getComponentType() const { return Ids::label; };

void BCMLabel::mouseDown(const MouseEvent& event)
{
    if (event.mods.isPopupMenu())
    {
        grabKeyboardFocus();
        showPopupMenu();
    }
    else
    {
        Label::mouseDown(event);
    }
}

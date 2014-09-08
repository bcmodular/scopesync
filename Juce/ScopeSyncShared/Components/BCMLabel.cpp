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

BCMLabel::BCMLabel(String& name, String& text, ScopeSyncGUI& owner) : Label(name, text), gui(owner)
{
    valueListener = new BCMLabelValueListener(*this);
}

BCMLabel::~BCMLabel()
{
    if (getName().equalsIgnoreCase("SystemError"))
    {
        gui.getScopeSync().getSystemError().removeListener(valueListener);
        gui.getScopeSync().getSystemErrorDetails().removeListener(valueListener);    
    }
}

void BCMLabel::applyProperties(LabelProperties& properties) 
{
    setComponentID(properties.id);
        
    String labelText = getText();
    String tooltip   = getText();
    
    mapsToParameter = false;
    
    if (getName().equalsIgnoreCase("configurationfilepath"))
    {
        labelText = gui.getScopeSync().getConfigurationFile().getFullPathName();
    }
    else if (getName().equalsIgnoreCase("configurationname"))
    {
        labelText = gui.getScopeSync().getConfigurationName(true);
    }
    else
    {
        ValueTree mapping;
        parameter = gui.getUIMapping(Ids::labels, getName(), mapping);

        if (parameter != nullptr)
        {
            mapsToParameter = true;
            parameter->getDescriptions(labelText, tooltip);
        }
    }

    // Only relevant if label is editable. Not currently supported
    setEditable(false, false, false);
    
    if (getName().equalsIgnoreCase("SystemError"))
    {
        labelText = gui.getScopeSync().getSystemError().getValue();

        if (labelText.isNotEmpty())
             labelText += " (hover over text for details)";

        tooltip   = gui.getScopeSync().getSystemErrorDetails().getValue();
        gui.getScopeSync().getSystemError().addListener(valueListener);
        gui.getScopeSync().getSystemErrorDetails().addListener(valueListener);
    }
    
    setText(labelText, dontSendNotification);
    setTooltip(tooltip);
    
    setFont(Font(properties.fontHeight, properties.fontStyleFlags));
    setJustificationType(Justification(properties.justificationFlags));

    properties.bounds.copyValues(componentBounds);
    BCM_SET_BOUNDS
    BCM_SET_LOOK_AND_FEEL
}

void BCMLabel::handleValueChanged(Value& valueThatChanged)
{
    if (valueThatChanged.refersToSameSourceAs(gui.getScopeSync().getSystemError()))
    {
        String text = valueThatChanged.getValue().toString();

        if (text.isNotEmpty())
             text += " (hover over text for details)";

        setText(text, dontSendNotification);
    }

    if (valueThatChanged.refersToSameSourceAs(gui.getScopeSync().getSystemErrorDetails()))
    {
        String tooltip = valueThatChanged.getValue().toString();
        setTooltip(tooltip);   
    }
}

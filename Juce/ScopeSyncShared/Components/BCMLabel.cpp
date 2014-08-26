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

BCMLabel::BCMLabel(String& name, String& text) : Label(name, text) {}

BCMLabel::~BCMLabel() {}

void BCMLabel::applyProperties(LabelProperties& properties, ScopeSyncGUI& gui)
{
    setComponentID(properties.id);
    
    String labelText = getText();
    String tooltip   = getText();
    
    mapsToParameter = false;
    
    if (getName().equalsIgnoreCase("configurationfilepath"))
    {
        labelText = gui.getScopeSync().getConfigurationFilePath();
    }
    else if (getName().equalsIgnoreCase("configurationname"))
    {
        labelText = gui.getScopeSync().getConfigurationName();
    }
    else
    {
        ValueTree mapping;
        parameter = gui.getUIMapping(ScopeSyncGUI::mappingLabelId, getName(), mapping);

        if (parameter != nullptr)
        {
            mapsToParameter = true;
            parameter->getDescriptions(labelText, tooltip);
        }
    }

    // Only relevant if label is editable. Not currently supported
    setEditable(false, false, false);

    setText(labelText, dontSendNotification);
    setTooltip(tooltip);
    
    if (getName().equalsIgnoreCase("SystemError"))
        getTextValue().referTo(gui.getScopeSync().getSystemError());

    setFont(Font(properties.fontHeight, properties.fontStyleFlags));
    setJustificationType(Justification(properties.justificationFlags));

    properties.bounds.copyValues(componentBounds);
    BCM_SET_BOUNDS
    BCM_SET_LOOK_AND_FEEL
}

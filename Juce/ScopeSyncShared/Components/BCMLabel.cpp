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

BCMLabel::BCMLabel(LabelProperties& properties, ScopeSyncGUI& gui, String& name, String& text) : Label(name, text)
{
    String labelText = text;
    String tooltip   = text;
    
    mapsToParameter = false;
    paramIdx        = -1;
    
    if (name.equalsIgnoreCase("configurationfilepath"))
    {
        labelText = gui.getScopeSync().getConfigurationFilePath().getValue();
    }
    else if (name.equalsIgnoreCase("configurationname"))
    {
        labelText = gui.getScopeSync().getConfigurationName().getValue();
    }
    else
    {
        ValueTree mapping;
        gui.getUIMapping(ScopeSyncGUI::mappingLabelId, name, mapping, paramIdx);

        if (paramIdx != -1)
        {
            mapsToParameter = true;
            gui.getScopeSync().getParameterDescriptions(paramIdx, labelText, tooltip);
        }
    }

    // Only relevant if label is editable. Not currently supported
    setEditable(false, false, false);
    //setColour(TextEditor::textColourId, Colours::black);
    //setColour(TextEditor::backgroundColourId, Colour(0x00000000));

    setText(labelText, dontSendNotification);
    setTooltip(tooltip);
    setFont(Font(properties.fontHeight, properties.fontStyleFlags));
    setJustificationType(Justification(properties.justificationFlags));

    if (properties.textColour.isNotEmpty()){
        setColour(Label::textColourId, Colour::fromString(properties.textColour));
    }

    setLookAndFeel(gui.getScopeSync().getBCMLookAndFeelById(properties.bcmLookAndFeelId));

    setBounds(
        properties.x,
        properties.y,
        properties.width,
        properties.height
    );
}

BCMLabel::~BCMLabel() {}

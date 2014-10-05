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

#ifndef BCMCOMBOBOX_H_INCLUDED
#define BCMCOMBOBOX_H_INCLUDED

class ScopeSyncGUI;
class ComboBoxProperties;

#include <JuceHeader.h>
#include "../Core/BCMParameter.h"
#include "../Components/BCMComponentBounds.h"
#include "BCMParameterWidget.h"

class BCMComboBox : public ComboBox,
                    public BCMParameterWidget
{
public:
    BCMComboBox(String& name, ScopeSyncGUI& owner);
    ~BCMComboBox();

    void applyProperties(ComboBoxProperties& properties);
    
    void valueChanged(Value& value) override;

    float                fontHeight;     // Height of font for BCMComboBox's current value (use PopupMenu LookAndFeel for other fonts)
    Font::FontStyleFlags fontStyleFlags; // Style flags for BCMComboBox's current value

private:
    Value              parameterValue;  // Maintains a link to a mapped parameter's UI value
    BCMComponentBounds componentBounds; // Position/Size information

    void mouseDown(const MouseEvent& event) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BCMComboBox);
};

#endif  // BCMCOMBOBOX_H_INCLUDED

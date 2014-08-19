/**
 * Intermediary class for extracting BCMComboBox definitions from
 * XML layout files
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

#ifndef COMBOBOXPROPERTIES_H_INCLUDED
#define COMBOBOXPROPERTIES_H_INCLUDED

#include <JuceHeader.h>
#include "../Components/BCMComponentBounds.h"

class ComboBoxProperties
{
public:
    // Fall-back constructor in case no default XML supplied
    ComboBoxProperties();

    // Constructor for use when setting up defaults
    ComboBoxProperties(XmlElement& comboBoxXML);
    
    // Constructor for use when overriding defaults for a specific ComboBox
    ComboBoxProperties(XmlElement& comboBoxXML, ComboBoxProperties& parentComboBoxProperties);

    ~ComboBoxProperties();

    String               name;
    String               id;
    bool                 editableText;
    String               nothingSelectedText;
    String               noChoicesText;
    StringArray          items;
    String               tooltip;
    BCMComponentBounds   bounds;
    float                fontHeight;
    Font::FontStyleFlags fontStyleFlags;
    Justification::Flags justificationFlags;
    String               bcmLookAndFeelId;

private:
    void initialise();
    void copyProperties(ComboBoxProperties& parentComboBoxProperties);
    void setValuesFromXML(XmlElement& comboBoxXML);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComboBoxProperties);
};

#endif  // COMBOBOXPROPERTIES_H_INCLUDED

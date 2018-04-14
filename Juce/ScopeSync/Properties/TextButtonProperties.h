/**
 * Intermediary class for extracting BCMTextButton definitions from
 * XML layout files
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

#ifndef TEXTBUTTONPROPERTIES_H_INCLUDED
#define TEXTBUTTONPROPERTIES_H_INCLUDED

#include <JuceHeader.h>
#include "WidgetProperties.h"

class TextButtonProperties : public WidgetProperties
{
public:
    // Fall-back constructor in case no default XML supplied
    TextButtonProperties();

    // Constructor for use when setting up defaults
    TextButtonProperties(XmlElement& textButtonXML);

    // Constructor for use when overriding defaults for a specific Label
    TextButtonProperties(XmlElement& textButtonXML, TextButtonProperties& parentTextButtonProperties);

    ~TextButtonProperties();

    String             text;
    String             tooltip;
    int                radioGroupId;
    URL                url;

private:
    void initialise();
    void copyProperties(TextButtonProperties& parentTextButtonProperties);
    void setValuesFromXML(XmlElement& textButtonXML);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextButtonProperties);
};



#endif  // TEXTBUTTONPROPERTIES_H_INCLUDED

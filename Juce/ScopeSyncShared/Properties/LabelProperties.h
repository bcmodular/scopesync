/**
 * Intermediary class for extracting BCMLabel definitions from
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

#ifndef LABELPROPERTIES_H_INCLUDED
#define LABELPROPERTIES_H_INCLUDED

#include <JuceHeader.h>
#include "WidgetProperties.h"

class LabelProperties : public WidgetProperties
{
public:
    // Fall-back constructor in case no default XML supplied
    LabelProperties();

    // Constructor for use when setting up defaults
    LabelProperties(XmlElement& labelXML);

    // Constructor for use when overriding defaults for a specific Label
    LabelProperties(XmlElement& labelXML, LabelProperties& parentLabelProperties);

    ~LabelProperties();

    enum ParameterTextDisplay {parameterName, shortDescription, fullDescription, scopeCode};

    String               text;
    float                fontHeight;
    Font::FontStyleFlags fontStyleFlags;
    Justification::Flags justificationFlags;
    ParameterTextDisplay parameterTextDisplay;

private:
    void initialise();
    void copyProperties(const LabelProperties& parentLabelProperties);
    void setValuesFromXML(const XmlElement& labelXML);

    void getParameterTextDisplayFromXml(const XmlElement& labelXML, ParameterTextDisplay& parameterTextDisplay);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabelProperties);
};

#endif  // LABELPROPERTIES_H_INCLUDED

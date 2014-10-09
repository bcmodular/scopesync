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

#include "LabelProperties.h"
#include "PropertiesHelper.h"

LabelProperties::LabelProperties()
{
    initialise();
}

LabelProperties::LabelProperties(XmlElement& labelXML)
    : WidgetProperties(labelXML)
{
    initialise();
    setValuesFromXML(labelXML);
}

LabelProperties::LabelProperties(XmlElement& labelXML, LabelProperties& parentLabelProperties)
    : WidgetProperties(labelXML, parentLabelProperties)
{
    copyProperties(parentLabelProperties);
    setValuesFromXML(labelXML);
}

LabelProperties::~LabelProperties() {}

void LabelProperties::initialise()
{
    // Ultimate fall-back defaults, in case no defaults supplied in the XML
    fontHeight           = 15.00f;
    fontStyleFlags       = Font::plain;
    justificationFlags   = Justification::centred;
    parameterTextDisplay = shortDescription;
};

void LabelProperties::copyProperties(const LabelProperties& parentLabelProperties)
{
    text                 = parentLabelProperties.text;
    fontHeight           = parentLabelProperties.fontHeight;
    fontStyleFlags       = parentLabelProperties.fontStyleFlags;
    justificationFlags   = parentLabelProperties.justificationFlags;
    parameterTextDisplay = parentLabelProperties.parameterTextDisplay;
};

void LabelProperties::setValuesFromXML(const XmlElement& labelXML)
{
    text = labelXML.getStringAttribute("text", text);
    
    XmlElement* fontXml = labelXML.getChildByName("font");
    if (fontXml != nullptr)
        getFontFromXml(*fontXml, fontHeight, fontStyleFlags);

    XmlElement* justificationXml = labelXML.getChildByName("justification");
    if (justificationXml != nullptr)
        getJustificationFlagsFromXml(*justificationXml, justificationFlags);

    getParameterTextDisplayFromXml(labelXML, parameterTextDisplay);
};

void LabelProperties::getParameterTextDisplayFromXml(const XmlElement& labelXML, ParameterTextDisplay& parameterTextDisplay)
{
    String parameterTextDisplayString = labelXML.getStringAttribute("parametertextdisplay", String::empty);

    if (parameterTextDisplayString.equalsIgnoreCase("name"))
        parameterTextDisplay = parameterName;
    else if (parameterTextDisplayString.equalsIgnoreCase("shortdescription"))
        parameterTextDisplay = shortDescription;
    else if (parameterTextDisplayString.equalsIgnoreCase("fulldescription"))
        parameterTextDisplay = fullDescription;
    else if (parameterTextDisplayString.equalsIgnoreCase("scopecode"))
        parameterTextDisplay = scopeCode;
}

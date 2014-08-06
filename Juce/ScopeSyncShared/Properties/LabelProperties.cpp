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
};

LabelProperties::LabelProperties(XmlElement& labelXML)
{
    initialise();
    setValuesFromXML(labelXML);
};

LabelProperties::LabelProperties(XmlElement& labelXML, LabelProperties& parentLabelProperties)
{
    copyProperties(parentLabelProperties);
    setValuesFromXML(labelXML);
};

LabelProperties::~LabelProperties()
{
};

void LabelProperties::initialise()
{
    // Ultimate fall-back defaults, in case no defaults supplied in the XML
    name               = "def";
    text               = "def";
    width              = 66;
    height             = 20;
    x                  = 18;
    y                  = 26;
    fontHeight         = 15.00f;
    fontStyleFlags     = Font::plain;
    textColour         = String::empty;
    justificationFlags = Justification::centred;
    bcmLookAndFeelId   = String::empty;
};

void LabelProperties::copyProperties(LabelProperties& parentLabelProperties)
{
    name               = parentLabelProperties.name;
    text               = parentLabelProperties.text;
    width              = parentLabelProperties.width;
    height             = parentLabelProperties.height;
    x                  = parentLabelProperties.x;
    y                  = parentLabelProperties.y;
    fontHeight         = parentLabelProperties.fontHeight;
    fontStyleFlags     = parentLabelProperties.fontStyleFlags;
    textColour         = parentLabelProperties.textColour;
    justificationFlags = parentLabelProperties.justificationFlags;
    bcmLookAndFeelId   = parentLabelProperties.bcmLookAndFeelId;
};

void LabelProperties::setValuesFromXML(XmlElement& labelXML)
{
    // Grab values set in XML
    forEachXmlChildElement(labelXML, child)
    {
             if (child->hasTagName("name"))          name       = child->getAllSubText().toLowerCase();
        else if (child->hasTagName("text"))          text       = child->getAllSubText();
        else if (child->hasTagName("textcolour"))    textColour = child->getAllSubText();
        else if (child->hasTagName("position"))      getPositionFromXml(*child, x, y);
        else if (child->hasTagName("size"))          getSizeFromXml(*child, width, height);
        else if (child->hasTagName("font"))          getFontFromXml(*child, fontHeight, fontStyleFlags);
        else if (child->hasTagName("justification")) getJustificationFlagsFromXml(*child, justificationFlags);
    }

    if (labelXML.hasAttribute("lfid")) bcmLookAndFeelId = labelXML.getStringAttribute("lfid").toLowerCase();
};


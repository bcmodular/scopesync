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

#include "ComboBoxProperties.h"
#include "PropertiesHelper.h"

ComboBoxProperties::ComboBoxProperties()
{
    initialise();
};

ComboBoxProperties::ComboBoxProperties(XmlElement& comboBoxXML)
{
    initialise();
    setValuesFromXML(comboBoxXML);
};

ComboBoxProperties::ComboBoxProperties(XmlElement& comboBoxXML, ComboBoxProperties& parentComboBoxProperties)
{
    copyProperties(parentComboBoxProperties);
    setValuesFromXML(comboBoxXML);
};

ComboBoxProperties::~ComboBoxProperties()
{
};

void ComboBoxProperties::initialise()
{
    // Ultimate fall-back defaults, in case no defaults supplied in the XML
    name                = "def";
    tooltip             = "def";
    editableText        = false;
    fontHeight          = 15.00f;
    fontStyleFlags      = Font::plain;
    justificationFlags  = Justification::centred;
    nothingSelectedText = "def";
    noChoicesText       = "def";
    width               = 15;
    height              = 15;
    x                   = 0;
    y                   = 0;
    bcmLookAndFeelId    = String::empty;
};

void ComboBoxProperties::copyProperties(ComboBoxProperties& parentComboBoxProperties)
{
    name                = parentComboBoxProperties.name;
    tooltip             = parentComboBoxProperties.tooltip;
    editableText        = parentComboBoxProperties.editableText;
    fontHeight          = parentComboBoxProperties.fontHeight;
    fontStyleFlags      = parentComboBoxProperties.fontStyleFlags;
    justificationFlags  = parentComboBoxProperties.justificationFlags;
    nothingSelectedText = parentComboBoxProperties.nothingSelectedText;
    for (int i = 0; i < items.size(); i++)
    {
        items.add(parentComboBoxProperties.items[i]);
    }
    noChoicesText       = parentComboBoxProperties.noChoicesText;
    width               = parentComboBoxProperties.width;
    height              = parentComboBoxProperties.height;
    x                   = parentComboBoxProperties.x;
    y                   = parentComboBoxProperties.y;
    bcmLookAndFeelId    = parentComboBoxProperties.bcmLookAndFeelId;
};

void ComboBoxProperties::setValuesFromXML(XmlElement& comboBoxXML)
{
    // Grab values set in XML
    forEachXmlChildElement(comboBoxXML, child)
    {
             if (child->hasTagName("name"))                name = child->getAllSubText().toLowerCase();
        else if (child->hasTagName("tooltip"))             tooltip = child->getAllSubText();
        else if (child->hasTagName("editabletext"))        editableText = child->getAllSubText().equalsIgnoreCase("true");
        else if (child->hasTagName("item"))                items.add(child->getAllSubText());
        else if (child->hasTagName("nothingselectedtext")) nothingSelectedText = child->getAllSubText();
        else if (child->hasTagName("nochoicestext"))       noChoicesText = child->getAllSubText();
        else if (child->hasTagName("justification"))       getJustificationFlagsFromXml(*child, justificationFlags);
        else if (child->hasTagName("position"))            getPositionFromXml(*child, x, y);
        else if (child->hasTagName("size"))                getSizeFromXml(*child, width, height);
        else if (child->hasTagName("font"))                getFontFromXml(*child, fontHeight, fontStyleFlags);
    }

    if (comboBoxXML.hasAttribute("lfid")) bcmLookAndFeelId = comboBoxXML.getStringAttribute("lfid").toLowerCase();
};
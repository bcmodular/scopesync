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

#include "ComboBoxProperties.h"
#include "PropertiesHelper.h"

ComboBoxProperties::ComboBoxProperties()
{
    initialise();
}

ComboBoxProperties::ComboBoxProperties(XmlElement& comboBoxXML)
    : WidgetProperties(comboBoxXML)
{
    initialise();
    setValuesFromXML(comboBoxXML);
}

ComboBoxProperties::ComboBoxProperties(XmlElement& comboBoxXML, ComboBoxProperties& parentComboBoxProperties)
    :WidgetProperties(comboBoxXML, parentComboBoxProperties)
{
    copyProperties(parentComboBoxProperties);
    setValuesFromXML(comboBoxXML);
}

ComboBoxProperties::~ComboBoxProperties() {}

void ComboBoxProperties::initialise()
{
    // Ultimate fall-back defaults, in case no defaults supplied in the XML
    tooltip             = String();
    editableText        = false;
    fontHeight          = 15.00f;
    fontStyleFlags      = Font::plain;
    justificationFlags  = Justification::centred;
    nothingSelectedText = "- Nothing selected -";
    noChoicesText       = "- No choices available -";
};

void ComboBoxProperties::copyProperties(ComboBoxProperties& parentComboBoxProperties)
{
    tooltip             = parentComboBoxProperties.tooltip;
    editableText        = parentComboBoxProperties.editableText;
    fontHeight          = parentComboBoxProperties.fontHeight;
    fontStyleFlags      = parentComboBoxProperties.fontStyleFlags;
    justificationFlags  = parentComboBoxProperties.justificationFlags;
    nothingSelectedText = parentComboBoxProperties.nothingSelectedText;
    noChoicesText       = parentComboBoxProperties.noChoicesText;
};

void ComboBoxProperties::setValuesFromXML(XmlElement& comboBoxXML)
{
    tooltip             = comboBoxXML.getStringAttribute("tooltip",             tooltip);
    nothingSelectedText = comboBoxXML.getStringAttribute("nothingselectedtext", nothingSelectedText);
    noChoicesText       = comboBoxXML.getStringAttribute("nochoicestext",       noChoicesText);
    editableText        = comboBoxXML.getBoolAttribute  ("editabletext",        editableText);
    
    XmlElement* fontXml = comboBoxXML.getChildByName("font");
    if (fontXml != nullptr)
        getFontFromXml(*fontXml, fontHeight, fontStyleFlags);

    XmlElement* justificationXml = comboBoxXML.getChildByName("justification");
    if (justificationXml != nullptr)
        getJustificationFlagsFromXml(*justificationXml, justificationFlags); 
}
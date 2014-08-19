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
    id                  = "def";
    tooltip             = "def";
    editableText        = false;
    fontHeight          = 15.00f;
    fontStyleFlags      = Font::plain;
    justificationFlags  = Justification::centred;
    nothingSelectedText = "def";
    noChoicesText       = "def";
    bounds.width        = 15;
    bounds.height       = 15;
    bounds.x            = 0;
    bounds.y            = 0;
    bcmLookAndFeelId    = String::empty;
};

void ComboBoxProperties::copyProperties(ComboBoxProperties& parentComboBoxProperties)
{
    name                = parentComboBoxProperties.name;
    id                  = parentComboBoxProperties.id;
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
    bounds.width        = parentComboBoxProperties.bounds.width;
    bounds.height       = parentComboBoxProperties.bounds.height;
    bounds.x            = parentComboBoxProperties.bounds.x;
    bounds.y            = parentComboBoxProperties.bounds.y;
    bcmLookAndFeelId    = parentComboBoxProperties.bcmLookAndFeelId;
};

void ComboBoxProperties::setValuesFromXML(XmlElement& comboBoxXML)
{
    name                = comboBoxXML.getStringAttribute("name",                name);
    id                  = comboBoxXML.getStringAttribute("id",                  name); // Default to name if no id set
    tooltip             = comboBoxXML.getStringAttribute("tooltip",             tooltip);
    nothingSelectedText = comboBoxXML.getStringAttribute("nothingselectedtext", nothingSelectedText);
    noChoicesText       = comboBoxXML.getStringAttribute("nochoicestext",       noChoicesText);
    editableText        = comboBoxXML.getBoolAttribute  ("editabletext",        editableText);
    
    XmlElement* boundsXml = comboBoxXML.getChildByName("bounds");
    if (boundsXml != nullptr)
        getBoundsFromXml(*boundsXml, bounds);
    
    XmlElement* fontXml = comboBoxXML.getChildByName("font");
    if (fontXml != nullptr)
        getFontFromXml(*fontXml, fontHeight, fontStyleFlags);

    XmlElement* justificationXml = comboBoxXML.getChildByName("justification");
    if (justificationXml != nullptr)
        getJustificationFlagsFromXml(*justificationXml, justificationFlags);

    forEachXmlChildElementWithTagName(comboBoxXML, child, "item")
    {
        items.add(child->getAllSubText());
    }   

    bcmLookAndFeelId = comboBoxXML.getStringAttribute("lfid", bcmLookAndFeelId);
};
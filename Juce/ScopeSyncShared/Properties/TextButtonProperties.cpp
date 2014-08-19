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

#include "TextButtonProperties.h"
#include "PropertiesHelper.h"

TextButtonProperties::TextButtonProperties()
{
    initialise();
};

TextButtonProperties::TextButtonProperties(XmlElement& textButtonXML)
{
    initialise();
    setValuesFromXML(textButtonXML);
};

TextButtonProperties::TextButtonProperties(XmlElement& textButtonXML, TextButtonProperties& parentTextButtonProperties)
{
    copyProperties(parentTextButtonProperties);
    setValuesFromXML(textButtonXML);
};

TextButtonProperties::~TextButtonProperties()
{
};

void TextButtonProperties::initialise()
{
    // Ultimate fall-back defaults, in case no defaults supplied in the XML
    name             = "def";
    id               = "def";
    text             = "def";
    tooltip          = "def";
    bounds.width     = 15;
    bounds.height    = 15;
    bounds.x         = 0;
    bounds.y         = 0;
    bcmLookAndFeelId = String::empty;
    radioGroupId     = 0;
    tabbedComponents.clear();
    tabNames.clear();
};

void TextButtonProperties::copyProperties(TextButtonProperties& parentTextButtonProperties)
{
    name             = parentTextButtonProperties.name;
    id               = parentTextButtonProperties.id;
    text             = parentTextButtonProperties.text;
    tooltip          = parentTextButtonProperties.tooltip;
    bounds.width     = parentTextButtonProperties.bounds.width;
    bounds.height    = parentTextButtonProperties.bounds.height;
    bounds.x         = parentTextButtonProperties.bounds.x;
    bounds.y         = parentTextButtonProperties.bounds.y;
    radioGroupId     = parentTextButtonProperties.radioGroupId;
    bcmLookAndFeelId = parentTextButtonProperties.bcmLookAndFeelId;
    tabbedComponents = StringArray(parentTextButtonProperties.tabbedComponents);
    tabNames         = StringArray(parentTextButtonProperties.tabNames);
};

void TextButtonProperties::setValuesFromXML(XmlElement& textButtonXML)
{
    name    = textButtonXML.getStringAttribute("name",    name);
    id      = textButtonXML.getStringAttribute("id",      name); // Default to name if no id set
    text    = textButtonXML.getStringAttribute("text",    text);
    tooltip = textButtonXML.getStringAttribute("tooltip", tooltip);
    
    XmlElement* boundsXml = textButtonXML.getChildByName("bounds");
    if (boundsXml != nullptr)
        getBoundsFromXml(*boundsXml, bounds);
    
    XmlElement* chooseTabXml = textButtonXML.getChildByName("choosetab");
    if (chooseTabXml != nullptr)
    {
        String tabbedComponent = chooseTabXml->getStringAttribute("tabbedcomponent", String::empty);
        String tabName         = chooseTabXml->getStringAttribute("tabname", String::empty);
            
        if (tabbedComponent.isNotEmpty() && tabName.isNotEmpty())
        {
            tabbedComponents.add(tabbedComponent);
            tabNames.add(tabName);
        }
    }

    if (textButtonXML.hasAttribute("radiogroup")) radioGroupId = textButtonXML.getStringAttribute("radiogroup").hashCode();
    bcmLookAndFeelId = textButtonXML.getStringAttribute("lfid", bcmLookAndFeelId);
};
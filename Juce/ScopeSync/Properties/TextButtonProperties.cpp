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

#include "TextButtonProperties.h"

TextButtonProperties::TextButtonProperties()
{
    initialise();
}

TextButtonProperties::TextButtonProperties(XmlElement& textButtonXML)
    : WidgetProperties(textButtonXML)
{
    initialise();
    setValuesFromXML(textButtonXML);
}

TextButtonProperties::TextButtonProperties(XmlElement& textButtonXML, TextButtonProperties& parentTextButtonProperties)
    : WidgetProperties(textButtonXML, parentTextButtonProperties)
{
    copyProperties(parentTextButtonProperties);
    setValuesFromXML(textButtonXML);
}

TextButtonProperties::~TextButtonProperties() {}

void TextButtonProperties::initialise()
{
    // Ultimate fall-back defaults, in case no defaults supplied in the XML
    text              = String::empty;
    tooltip           = String::empty;
    url               = URL();
    radioGroupId      = 0;
    tabbedComponents.clear();
    tabNames.clear();
}

void TextButtonProperties::copyProperties(TextButtonProperties& parentTextButtonProperties)
{
    text              = parentTextButtonProperties.text;
    tooltip           = parentTextButtonProperties.tooltip;
    radioGroupId      = parentTextButtonProperties.radioGroupId;
    tabbedComponents  = StringArray(parentTextButtonProperties.tabbedComponents);
    tabNames          = StringArray(parentTextButtonProperties.tabNames);
    url               = parentTextButtonProperties.url;
}

void TextButtonProperties::setValuesFromXML(XmlElement& textButtonXML)
{
    text    = textButtonXML.getStringAttribute("text",    text);
    tooltip = textButtonXML.getStringAttribute("tooltip", tooltip);
    
    String urlString = textButtonXML.getStringAttribute("url");

    if (urlString.isNotEmpty())
        url = URL(urlString);
    
    forEachXmlChildElementWithTagName(textButtonXML, chooseTabXml, "choosetab")
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
}
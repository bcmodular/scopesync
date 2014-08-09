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
    text             = "def";
    tooltip          = "def";
    width            = 15;
    height           = 15;
    x                = 0;
    y                = 0;
    bcmLookAndFeelId = String::empty;
    radioGroupId     = 0;
    tabbedComponents.clear();
    tabNames.clear();
};

void TextButtonProperties::copyProperties(TextButtonProperties& parentTextButtonProperties)
{
    name             = parentTextButtonProperties.name;
    text             = parentTextButtonProperties.text;
    tooltip          = parentTextButtonProperties.tooltip;
    width            = parentTextButtonProperties.width;
    height           = parentTextButtonProperties.height;
    x                = parentTextButtonProperties.x;
    y                = parentTextButtonProperties.y;
    radioGroupId     = parentTextButtonProperties.radioGroupId;
    bcmLookAndFeelId = parentTextButtonProperties.bcmLookAndFeelId;
    tabbedComponents = StringArray(parentTextButtonProperties.tabbedComponents);
    tabNames         = StringArray(parentTextButtonProperties.tabNames);
};

void TextButtonProperties::setValuesFromXML(XmlElement& textButtonXML)
{
    // Grab values set in XML
    forEachXmlChildElement(textButtonXML, child)
    {
             if (child->hasTagName("name"))            name            = child->getAllSubText().toLowerCase();
        else if (child->hasTagName("text"))            text            = child->getAllSubText();
        else if (child->hasTagName("tooltip"))         tooltip         = child->getAllSubText();
        else if (child->hasTagName("position"))
        {
            forEachXmlChildElement(*child, subChild)
            {
                     if (subChild->hasTagName("x")) x = subChild->getAllSubText().getIntValue();
                else if (subChild->hasTagName("y")) y = subChild->getAllSubText().getIntValue();
            }
        }
        else if (child->hasTagName("size"))
        {
            forEachXmlChildElement(*child, subChild)
            {
                     if (subChild->hasTagName("width"))  width  = subChild->getAllSubText().getIntValue();
                else if (subChild->hasTagName("height")) height = subChild->getAllSubText().getIntValue();
            }
        }
        else if (child->hasTagName("choosetab"))
        {
            String tabbedComponent = String::empty;
            String tabName         = String::empty;
            
            forEachXmlChildElement(*child, subChild)
            {
                     if (subChild->hasTagName("tabbedcomponent")) tabbedComponent = subChild->getAllSubText().toLowerCase();
                else if (subChild->hasTagName("tabname"))         tabName         = subChild->getAllSubText().toLowerCase();
            }

            if (tabbedComponent.isNotEmpty() && tabName.isNotEmpty())
            {
                tabbedComponents.add(tabbedComponent);
                tabNames.add(tabName);
            }
        }
    }

    if (textButtonXML.hasAttribute("radiogroup")) radioGroupId = textButtonXML.getStringAttribute("radiogroup").hashCode();
    if (textButtonXML.hasAttribute("lfid"))       bcmLookAndFeelId = textButtonXML.getStringAttribute("lfid").toLowerCase();
};
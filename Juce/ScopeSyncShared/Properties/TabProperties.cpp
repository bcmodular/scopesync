/**
 * Intermediary class for extracting Tab definitions from
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

#include "TabProperties.h"

TabProperties::TabProperties()
{
    initialise();
}

TabProperties::TabProperties(XmlElement& tabXML)
{
    initialise();
    setValuesFromXML(tabXML);
}

TabProperties::TabProperties(XmlElement& tabXML, TabProperties& parentTabProperties)
{
    copyProperties(parentTabProperties);
    setValuesFromXML(tabXML);
}

TabProperties::~TabProperties()
{
}

void TabProperties::initialise()
{
    name              = "def";
    backgroundColour  = "00000000";
    idx               = -1;
    bcmLookAndFeelId  = String::empty;
}

void TabProperties::copyProperties(TabProperties& parentTabProperties)
{
    name              = parentTabProperties.name;
    backgroundColour  = parentTabProperties.backgroundColour;
    idx               = parentTabProperties.idx;
    bcmLookAndFeelId  = parentTabProperties.bcmLookAndFeelId;
}

void TabProperties::setValuesFromXML(XmlElement& tabXML)
{
    // Grab values set in XML
    forEachXmlChildElement(tabXML, child)
    {
             if (child->hasTagName("name"))             name = child->getAllSubText();
        else if (child->hasTagName("backgroundcolour")) backgroundColour = child->getAllSubText();
        else if (child->hasTagName("idx"))              idx = child->getAllSubText().getIntValue();
    }

    if (tabXML.hasAttribute("lfid")) bcmLookAndFeelId = tabXML.getStringAttribute("lfid");
}
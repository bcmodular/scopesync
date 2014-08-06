/**
 * Intermediary class for extracting BCMTabbedComponent definitions from
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

#include "TabbedComponentProperties.h"
#include "PropertiesHelper.h"

TabbedComponentProperties::TabbedComponentProperties()
{
    initialise();
}

TabbedComponentProperties::TabbedComponentProperties(XmlElement& tabbedComponentXML)
{
    initialise();
    setValuesFromXML(tabbedComponentXML);
}

TabbedComponentProperties::TabbedComponentProperties(XmlElement& tabbedComponentXML, TabbedComponentProperties& parentTabbedComponentProperties)
{
    copyProperties(parentTabbedComponentProperties);
    setValuesFromXML(tabbedComponentXML);
}

TabbedComponentProperties::~TabbedComponentProperties()
{
}

void TabbedComponentProperties::initialise()
{
    width             = 0;
    height            = 0;
    x                 = 0;
    y                 = 0;
    tabBarDepth       = 0;
    showDropShadow    = true;
    tabBarOrientation = TabbedButtonBar::TabsAtLeft;
    bcmLookAndFeelId  = String::empty;
    name              = String::empty;
}

void TabbedComponentProperties::copyProperties(TabbedComponentProperties& parentTabbedComponentProperties)
{
    width             = parentTabbedComponentProperties.width;
    height            = parentTabbedComponentProperties.height;
    x                 = parentTabbedComponentProperties.x;
    y                 = parentTabbedComponentProperties.y;
    tabBarDepth       = parentTabbedComponentProperties.tabBarDepth;
    showDropShadow    = parentTabbedComponentProperties.showDropShadow;
    tabBarOrientation = parentTabbedComponentProperties.tabBarOrientation;
    bcmLookAndFeelId  = parentTabbedComponentProperties.bcmLookAndFeelId;
    name              = parentTabbedComponentProperties.name;
}

void TabbedComponentProperties::setValuesFromXML(XmlElement& tabbedComponentXML)
{
    // Grab values set in XML
    forEachXmlChildElement(tabbedComponentXML, child)
    {
             if (child->hasTagName("name"))           name = child->getAllSubText().toLowerCase();
        else if (child->hasTagName("showdropshadow")) showDropShadow = child->getAllSubText().equalsIgnoreCase("true");
        else if (child->hasTagName("position"))       getPositionFromXml(*child, x, y);
        else if (child->hasTagName("size"))           getSizeFromXml(*child, width, height);
        else if (child->hasTagName("tabbar"))
        {
            forEachXmlChildElement(*child, subChild)
            {
                     if (subChild->hasTagName("orientation")) getOrientationFromXml(*subChild, tabBarOrientation);
                else if (subChild->hasTagName("depth"))       tabBarDepth = subChild->getAllSubText().getIntValue();
            }
        }
    }

    if (tabbedComponentXML.hasAttribute("lfid")) bcmLookAndFeelId = tabbedComponentXML.getStringAttribute("lfid").toLowerCase();
}

void TabbedComponentProperties::getOrientationFromXml(const XmlElement& xml, TabbedButtonBar::Orientation& orientation)
{
    String orientationString = xml.getAllSubText();

         if (orientationString.toLowerCase() == "bottom") orientation = TabbedButtonBar::TabsAtBottom;
    else if (orientationString.toLowerCase() == "left")   orientation = TabbedButtonBar::TabsAtLeft;
    else if (orientationString.toLowerCase() == "right")  orientation = TabbedButtonBar::TabsAtRight;
    else if (orientationString.toLowerCase() == "top")    orientation = TabbedButtonBar::TabsAtTop;
}
                
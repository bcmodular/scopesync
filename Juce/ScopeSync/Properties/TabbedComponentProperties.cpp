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
    : WidgetProperties(tabbedComponentXML)
{
    initialise();
    setValuesFromXML(tabbedComponentXML);
}

TabbedComponentProperties::TabbedComponentProperties(XmlElement& tabbedComponentXML, TabbedComponentProperties& parentTabbedComponentProperties)
    : WidgetProperties(tabbedComponentXML, parentTabbedComponentProperties)
{
    copyProperties(parentTabbedComponentProperties);
    setValuesFromXML(tabbedComponentXML);
}

TabbedComponentProperties::~TabbedComponentProperties() {}

void TabbedComponentProperties::initialise()
{
    tabBarDepth       = 0;
    showDropShadow    = true;
    tabBarOrientation = TabbedButtonBar::TabsAtLeft;
}

void TabbedComponentProperties::copyProperties(TabbedComponentProperties& parentTabbedComponentProperties)
{
    tabBarDepth       = parentTabbedComponentProperties.tabBarDepth;
    showDropShadow    = parentTabbedComponentProperties.showDropShadow;
    tabBarOrientation = parentTabbedComponentProperties.tabBarOrientation;
}

void TabbedComponentProperties::setValuesFromXML(XmlElement& tabbedComponentXML)
{
    showDropShadow = tabbedComponentXML.getBoolAttribute  ("showdropshadow", showDropShadow);
    
    XmlElement* boundsXml = tabbedComponentXML.getChildByName("bounds");
    if (boundsXml != nullptr)
        getBoundsFromXml(*boundsXml, bounds);
    
    forEachXmlChildElementWithTagName(tabbedComponentXML, child, "tabbar")
    {
        tabBarDepth = child->getIntAttribute("depth", tabBarDepth);

        String orientationString = child->getStringAttribute("orientation");
        
        if (orientationString.isNotEmpty())
            getOrientationFromString(orientationString, tabBarOrientation);
    }
}

void TabbedComponentProperties::getOrientationFromString(const String& orientationString, TabbedButtonBar::Orientation& orientation)
{
         if (orientationString.equalsIgnoreCase("bottom")) orientation = TabbedButtonBar::TabsAtBottom;
    else if (orientationString.equalsIgnoreCase("left"))   orientation = TabbedButtonBar::TabsAtLeft;
    else if (orientationString.equalsIgnoreCase("right"))  orientation = TabbedButtonBar::TabsAtRight;
    else if (orientationString.equalsIgnoreCase("top"))    orientation = TabbedButtonBar::TabsAtTop;
}
                
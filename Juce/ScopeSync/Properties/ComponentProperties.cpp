/**
 * Intermediary class for extracting BCMComponent definitions from
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

#include "ComponentProperties.h"

ComponentProperties::ComponentProperties()
{
    initialise();
}

ComponentProperties::ComponentProperties(XmlElement& componentXML)
    : WidgetProperties(componentXML)
{
    initialise();
    setValuesFromXML(componentXML);
}

ComponentProperties::ComponentProperties(XmlElement& componentXML, ComponentProperties& parentComponentProperties)
    : WidgetProperties(componentXML, parentComponentProperties)
{
    copyProperties(parentComponentProperties);
    setValuesFromXML(componentXML);
}

ComponentProperties::~ComponentProperties() {}

void ComponentProperties::initialise()
{
    // Ultimate fall-back defaults, in case no defaults supplied in the XML
    backgroundColour         = "00000000";
    backgroundImageFileName  = String::empty;
    backgroundImagePlacement = RectanglePlacement::doNotResize;
}

void ComponentProperties::copyProperties(ComponentProperties& parentComponentProperties)
{
    backgroundColour         = parentComponentProperties.backgroundColour;
    backgroundImageFileName  = parentComponentProperties.backgroundImageFileName;
    backgroundImagePlacement = parentComponentProperties.backgroundImagePlacement;
}

void ComponentProperties::setValuesFromXML(XmlElement& componentXML)
{
    backgroundColour         = componentXML.getStringAttribute("backgroundcolour", backgroundColour);
    backgroundImageFileName  = componentXML.getStringAttribute("backgroundimage", backgroundImageFileName);
    getRectanglePlacementFromString(componentXML.getStringAttribute("backgroundimageplacement"), backgroundImagePlacement);
}

void ComponentProperties::getRectanglePlacementFromString(String string, RectanglePlacement& placement)
{
         if (string == "xleft")              placement = RectanglePlacement::xLeft;
    else if (string == "xright")             placement = RectanglePlacement::xRight;
    else if (string == "xmid")               placement = RectanglePlacement::xMid;
    else if (string == "ytop")               placement = RectanglePlacement::yTop;
    else if (string == "ybottom")            placement = RectanglePlacement::yBottom;
    else if (string == "ymid")               placement = RectanglePlacement::yMid;
    else if (string == "stretchtofit")       placement = RectanglePlacement::stretchToFit;
    else if (string == "filldestination")    placement = RectanglePlacement::fillDestination;
    else if (string == "onlyreduceinsize")   placement = RectanglePlacement::onlyReduceInSize;
    else if (string == "onlyincreaseinsize") placement = RectanglePlacement::onlyIncreaseInSize;
    else if (string == "donotresize")        placement = RectanglePlacement::doNotResize;
    else if (string == "centred")            placement = RectanglePlacement::centred;
}
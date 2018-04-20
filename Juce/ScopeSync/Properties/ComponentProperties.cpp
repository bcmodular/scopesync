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


	String placementString;

	XmlElement* placementXml = componentXML.getChildByName("backgroundimageplacement");
	
	if (placementXml != nullptr)
		placementString = placementXml->getAllSubText();
	
	getRectanglePlacementFromString(placementString);
}

void ComponentProperties::getRectanglePlacementFromString(String string)
{
	if (string.isEmpty())
	{
		backgroundImagePlacement = 0;
		return;
	}

	backgroundImagePlacement = (string.containsIgnoreCase("filldestination") ? RectanglePlacement::fillDestination : 0)
		| (string.containsIgnoreCase("xleft") ? RectanglePlacement::xLeft
			: (string.containsIgnoreCase("xright") ? RectanglePlacement::xRight
				: (string.containsIgnoreCase("xmid") ? RectanglePlacement::xMid : 0)))
		| (string.containsIgnoreCase("ytop") ? RectanglePlacement::yTop
			: (string.containsIgnoreCase("ybottom") ? RectanglePlacement::yBottom
				: (string.containsIgnoreCase("ymid") ? RectanglePlacement::yMid : 0)))
		| (string.containsIgnoreCase("stretchtofit") ? RectanglePlacement::stretchToFit : 0)
		| (string.containsIgnoreCase("filldestination") ? RectanglePlacement::fillDestination : 0)
		| (string.containsIgnoreCase("onlyreduceinsize") ? RectanglePlacement::onlyReduceInSize : 0)
		| (string.containsIgnoreCase("donotresize") ? RectanglePlacement::doNotResize : 0)
		| (string.containsIgnoreCase("centred") ? RectanglePlacement::centred : 0);
}
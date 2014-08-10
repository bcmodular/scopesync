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

#include "ComponentProperties.h"
#include "PropertiesHelper.h"

ComponentProperties::ComponentProperties()
{
    initialise();
}

ComponentProperties::ComponentProperties(XmlElement& componentXML)
{
    initialise();
    setValuesFromXML(componentXML);
}

ComponentProperties::ComponentProperties(XmlElement& componentXML, ComponentProperties& parentComponentProperties)
{
    copyProperties(parentComponentProperties);
    setValuesFromXML(componentXML);
}

ComponentProperties::~ComponentProperties()
{

}

void ComponentProperties::initialise()
{
    // Ultimate fall-back defaults, in case no defaults supplied in the XML
    x                        = 0;
    y                        = 0;
    width                    = 0;
    height                   = 0;
    backgroundColour         = "00000000";
    backgroundImageFileName  = String::empty;
    backgroundImagePlacement = RectanglePlacement::doNotResize;
    bcmLookAndFeelId         = String::empty;
}

void ComponentProperties::copyProperties(ComponentProperties& parentComponentProperties)
{
    x                        = parentComponentProperties.x;
    y                        = parentComponentProperties.y;
    width                    = parentComponentProperties.width;
    height                   = parentComponentProperties.height;
    backgroundColour         = parentComponentProperties.backgroundColour;
    backgroundImageFileName  = parentComponentProperties.backgroundImageFileName;
    backgroundImagePlacement = parentComponentProperties.backgroundImagePlacement;
    bcmLookAndFeelId         = parentComponentProperties.bcmLookAndFeelId;
}

void ComponentProperties::setValuesFromXML(XmlElement& componentXML)
{
    forEachXmlChildElement(componentXML, child)
    {
             if (child->hasTagName("size"))                     getSizeFromXml(*child, width, height);
        else if (child->hasTagName("position"))                 getPositionFromXml(*child, x, y);
        else if (child->hasTagName("backgroundcolour"))         backgroundColour = child->getAllSubText();
        else if (child->hasTagName("backgroundimage"))          backgroundImageFileName = child->getAllSubText();
        else if (child->hasTagName("backgroundimageplacement")) backgroundImagePlacement = getRectanglePlacementFromString(child->getAllSubText().toLowerCase());
    }

    if (componentXML.hasAttribute("lfid")) bcmLookAndFeelId = componentXML.getStringAttribute("lfid");
}

RectanglePlacement ComponentProperties::getRectanglePlacementFromString(String string)
{
         if (string == "xleft")              return RectanglePlacement::xLeft;
    else if (string == "xright")             return RectanglePlacement::xRight;
    else if (string == "xmid")               return RectanglePlacement::xMid;
    else if (string == "ytop")               return RectanglePlacement::yTop;
    else if (string == "ybottom")            return RectanglePlacement::yBottom;
    else if (string == "ymid")               return RectanglePlacement::yMid;
    else if (string == "stretchtofit")       return RectanglePlacement::stretchToFit;
    else if (string == "filldestination")    return RectanglePlacement::fillDestination;
    else if (string == "onlyreduceinsize")   return RectanglePlacement::onlyReduceInSize;
    else if (string == "onlyincreaseinsize") return RectanglePlacement::onlyIncreaseInSize;
    else if (string == "donotresize")        return RectanglePlacement::doNotResize;
    else if (string == "centred")            return RectanglePlacement::centred;
    else                                     return RectanglePlacement::doNotResize;
}
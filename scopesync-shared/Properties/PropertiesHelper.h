/**
 * Utility methods used by the *Properties classes to help extract
 * information from layout XML
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

#ifndef PROPERTIESHELPER_H_INCLUDED
#define PROPERTIESHELPER_H_INCLUDED

#include <JuceHeader.h>
#include "../Components/BCMComponentBounds.h"
#include "../Core/Global.h"

namespace PropertiesHelper
{
    inline void getFontFromXml(const XmlElement& xml, float& height, Font::FontStyleFlags& styleFlags)
    {
        // Reset style flags, so they can be set below
        styleFlags = Font::plain;

        if (xml.getBoolAttribute("bold", false))
            styleFlags = static_cast<Font::FontStyleFlags>(styleFlags | Font::bold);

        if (xml.getBoolAttribute("italic", false))
            styleFlags = static_cast<Font::FontStyleFlags>(styleFlags | Font::italic);

        if (xml.getBoolAttribute("underlined", false))
            styleFlags = static_cast<Font::FontStyleFlags>(styleFlags | Font::underlined);

        height = static_cast<float>(xml.getDoubleAttribute("height", height));
    }

    inline void getJustificationFlagsFromXml(const XmlElement& xml, Justification::Flags& flags)
    {
        // Protection in case of multiple tags
        bool firstFlag = true;

        if (xml.getBoolAttribute("left", false))
        {
            if (firstFlag)
            {
                flags = Justification::left;
                firstFlag = false;
            }
            else
                flags = static_cast<Justification::Flags>(flags | Justification::left);
        }
        
        if (xml.getBoolAttribute("right", false))
        {
            if (firstFlag)
            {
                flags = Justification::right;
                firstFlag = false;
            }
            else
                flags = static_cast<Justification::Flags>(flags | Justification::right);
        }

        if (xml.getBoolAttribute("horizontallycentred", false))
        {
            if (firstFlag)
            {
                flags = Justification::horizontallyCentred;
                firstFlag = false;
            }
            else
                flags = static_cast<Justification::Flags>(flags | Justification::horizontallyCentred);
        }

        if (xml.getBoolAttribute("top", false))
        {
            if (firstFlag)
            {
                flags = Justification::top;
                firstFlag = false;
            }
            else
                flags = static_cast<Justification::Flags>(flags | Justification::top);
        }
        
        if (xml.getBoolAttribute("bottom", false))
        {
            if (firstFlag)
            {
                flags = Justification::bottom;
                firstFlag = false;
            }
            else
                flags = static_cast<Justification::Flags>(flags | Justification::bottom);
        }

        if (xml.getBoolAttribute("verticallycentred", false))
        {
            if (firstFlag)
            {
                flags = Justification::verticallyCentred;
                firstFlag = false;
            }
            else
                flags = static_cast<Justification::Flags>(flags | Justification::verticallyCentred);
        }
        
        if (xml.getBoolAttribute("horizontallyjustified", false))
        {
            if (firstFlag)
            {
                flags = Justification::horizontallyJustified;
            }
            else
                flags = static_cast<Justification::Flags>(flags | Justification::horizontallyJustified);
        }
    }

    inline void getBoundsFromXml(XmlElement& xml, BCMComponentBounds& bounds)
    {
        XmlElement* borderSizeXml = xml.getChildByName("bordersize");
        
		if (xml.getStringAttribute("relativerectangle").isNotEmpty())
		{
			bounds.relativeRectangleString = xml.getStringAttribute("relativerectangle");
			bounds.boundsType = BCMComponentBounds::relativeRectangle;
		}
		else if (borderSizeXml != nullptr)
		{
			bounds.borderSize.setTop(borderSizeXml->getIntAttribute("top", bounds.borderSize.getTop()));
			bounds.borderSize.setLeft(borderSizeXml->getIntAttribute("left", bounds.borderSize.getLeft()));
			bounds.borderSize.setBottom(borderSizeXml->getIntAttribute("bottom", bounds.borderSize.getBottom()));
			bounds.borderSize.setRight(borderSizeXml->getIntAttribute("right", bounds.borderSize.getRight()));

			bounds.boundsType = BCMComponentBounds::inset;
		}
		else if (xml.hasAttribute("x") || xml.hasAttribute("y") || xml.hasAttribute("width") || xml.hasAttribute("height"))
        {
            bounds.x      = xml.getIntAttribute("x",      bounds.x);
            bounds.y      = xml.getIntAttribute("y",      bounds.y);
            bounds.width  = xml.getIntAttribute("width",  bounds.width);
            bounds.height = xml.getIntAttribute("height", bounds.height);

            bounds.boundsType = BCMComponentBounds::standard;
        }
		else
			bounds.boundsType = BCMComponentBounds::fillParent;
	}

    inline void getMappingParentFromXml(XmlElement& xml, Identifier& componentType, String& componentName)
    {
        String componentTypeString = xml.getStringAttribute("componenttype", componentType.toString());

             if (componentTypeString.equalsIgnoreCase("slider"))          componentType = Ids::slider;
        else if (componentTypeString.equalsIgnoreCase("label"))           componentType = Ids::label;
        else if (componentTypeString.equalsIgnoreCase("combobox"))        componentType = Ids::comboBox;
        else if (componentTypeString.equalsIgnoreCase("tabbedcomponent")) componentType = Ids::tabbedComponent;
        else if (componentTypeString.equalsIgnoreCase("textbutton"))      componentType = Ids::textButton;

        componentName = xml.getStringAttribute("componentname", componentName);
    }
};

#ifndef __PROPERTIESHELPER_HEADER_NO_NAMESPACE__
using namespace PropertiesHelper;
#endif

#endif  // PROPERTIESHELPER_H_INCLUDED

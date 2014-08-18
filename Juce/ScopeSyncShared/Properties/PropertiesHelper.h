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

#ifndef PROPERTIESHELPER_H_INCLUDED
#define PROPERTIESHELPER_H_INCLUDED

#include <JuceHeader.h>

namespace PropertiesHelper
{
    inline void getFontFromXml(const XmlElement& xml, float& height, Font::FontStyleFlags& styleFlags)
    {
        // Reset style flags, so they can be set below
        styleFlags = Font::plain;

        if (xml.getBoolAttribute("bold", false))
            styleFlags = (Font::FontStyleFlags)(styleFlags | Font::bold);

        if (xml.getBoolAttribute("italic", false))
            styleFlags = (Font::FontStyleFlags)(styleFlags | Font::italic);

        if (xml.getBoolAttribute("underlined", false))
            styleFlags = (Font::FontStyleFlags)(styleFlags | Font::underlined);

        height = xml.getChildElementAllSubText("height", String(height)).getFloatValue();
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
                flags = (Justification::Flags)(flags | Justification::left);
        }
        
        if (xml.getBoolAttribute("right", false))
        {
            if (firstFlag)
            {
                flags = Justification::right;
                firstFlag = false;
            }
            else
                flags = (Justification::Flags)(flags | Justification::right);
        }

        if (xml.getBoolAttribute("horizontallycentred", false))
        {
            if (firstFlag)
            {
                flags = Justification::horizontallyCentred;
                firstFlag = false;
            }
            else
                flags = (Justification::Flags)(flags | Justification::horizontallyCentred);
        }

        if (xml.getBoolAttribute("top", false))
        {
            if (firstFlag)
            {
                flags = Justification::top;
                firstFlag = false;
            }
            else
                flags = (Justification::Flags)(flags | Justification::top);
        }
        
        if (xml.getBoolAttribute("bottom", false))
        {
            if (firstFlag)
            {
                flags = Justification::bottom;
                firstFlag = false;
            }
            else
                flags = (Justification::Flags)(flags | Justification::bottom);
        }

        if (xml.getBoolAttribute("verticallycentred", false))
        {
            if (firstFlag)
            {
                flags = Justification::verticallyCentred;
                firstFlag = false;
            }
            else
                flags = (Justification::Flags)(flags | Justification::verticallyCentred);
        }
        
        if (xml.getBoolAttribute("horizontallyjustified", false))
        {
            if (firstFlag)
            {
                flags = Justification::horizontallyJustified;
                firstFlag = false;
            }
            else
                flags = (Justification::Flags)(flags | Justification::horizontallyJustified);
        }
    }

    inline void getBoundsFromXml(XmlElement& xml, int& x, int& y, int& width, int& height)
    {
        x      = xml.getIntAttribute("x",      x);
        y      = xml.getIntAttribute("y",      y);
        width  = xml.getIntAttribute("width",  width);
        height = xml.getIntAttribute("height", height);

        DBG("PropertiesHelper::getBoundsFromXml - " + xml.createDocument(""));
    }
};

#ifndef __PROPERTIESHELPER_HEADER_NO_NAMESPACE__
using namespace PropertiesHelper;
#endif

#endif  // PROPERTIESHELPER_H_INCLUDED

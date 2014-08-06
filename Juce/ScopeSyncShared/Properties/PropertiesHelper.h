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

        forEachXmlChildElement(xml, child)
        {
                 if (child->hasTagName("height"))     height     = child->getAllSubText().getFloatValue();
            else if (child->hasTagName("bold"))       styleFlags = (Font::FontStyleFlags)(styleFlags | Font::bold);
            else if (child->hasTagName("italic"))     styleFlags = (Font::FontStyleFlags)(styleFlags | Font::italic);
            else if (child->hasTagName("underlined")) styleFlags = (Font::FontStyleFlags)(styleFlags | Font::underlined);
        }
    }

    inline void getJustificationFlagsFromXml(const XmlElement& xml, Justification::Flags& flags)
    {
        // Protection in case of multiple tags
        bool firstFlag = true;

        forEachXmlChildElement(xml, child)
        {
            if (child->hasTagName("left"))
            {
                if (firstFlag)
                {
                    flags = Justification::left;
                    firstFlag = false;
                }
                else
                {
                    flags = (Justification::Flags)(flags | Justification::left);
                }
            }
            else if (child->hasTagName("right"))
            {
                if (firstFlag)
                {
                    flags = Justification::right;
                    firstFlag = false;
                }
                else
                {
                    flags = (Justification::Flags)(flags | Justification::right);
                }
            }
            else if (child->hasTagName("horizontallycentred"))
            {
                if (firstFlag)
                {
                    flags = Justification::horizontallyCentred;
                    firstFlag = false;
                }
                else
                {
                    flags = (Justification::Flags)(flags | Justification::horizontallyCentred);
                }
            }
            else if (child->hasTagName("top"))
            {
                if (firstFlag)
                {
                    flags = Justification::top;
                    firstFlag = false;
                }
                else
                {
                    flags = (Justification::Flags)(flags | Justification::top);
                }
            }
            else if (child->hasTagName("bottom"))
            {
                if (firstFlag)
                {
                    flags = Justification::bottom;
                    firstFlag = false;
                }
                else
                {
                    flags = (Justification::Flags)(flags | Justification::bottom);
                }
            }
            else if (child->hasTagName("verticallycentred"))
            {
                if (firstFlag)
                {
                    flags = Justification::verticallyCentred;
                    firstFlag = false;
                }
                else
                {
                    flags = (Justification::Flags)(flags | Justification::verticallyCentred);
                }
            }
            else if (child->hasTagName("horizontallyjustified"))
            {
                if (firstFlag)
                {
                    flags = Justification::horizontallyJustified;
                    firstFlag = false;
                }
                else
                {
                    flags = (Justification::Flags)(flags | Justification::horizontallyJustified);
                }
            }
        }
    }

    inline void getSizeFromXml(XmlElement& xml, int& width, int& height)
    {
        forEachXmlChildElement(xml, child)
        {
                 if (child->hasTagName("width"))  width  = child->getAllSubText().getIntValue();
            else if (child->hasTagName("height")) height = child->getAllSubText().getIntValue();
        }
    }

    inline void getPositionFromXml(XmlElement& xml, int& x, int& y)
    {
        forEachXmlChildElement(xml, child)
        {
                 if (child->hasTagName("x")) x = child->getAllSubText().getIntValue();
            else if (child->hasTagName("y")) y = child->getAllSubText().getIntValue();
        }
    }
};

#ifndef __PROPERTIESHELPER_HEADER_NO_NAMESPACE__
using namespace PropertiesHelper;
#endif

#endif  // PROPERTIESHELPER_H_INCLUDED

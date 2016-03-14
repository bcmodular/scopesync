/**
 * A simple Image class, for drawing onto a Component
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

#include "BCMImage.h"

BCMImage::BCMImage(XmlElement& xml) : BCMGraphic(xml)
{
    fileName    = xml.getStringAttribute("filename", String::empty);
    stretchMode = getStretchModeFromXml(xml);
    opacity     = static_cast<float>(xml.getDoubleAttribute("opacity", 1.0f));
}

BCMImage::~BCMImage() {}

BCMImage::StretchMode BCMImage::getStretchModeFromXml(XmlElement& xml)
{
    String stretchModeText = xml.getStringAttribute("stretchmode", "stretchedtofit");

    if (stretchModeText.equalsIgnoreCase("maintainaspect"))
        return maintainAspect;
    else if (stretchModeText.equalsIgnoreCase("maintainaspectonlyreduce"))
        return maintainAspectOnlyReduce;
    else
        return stretchedToFit;
}
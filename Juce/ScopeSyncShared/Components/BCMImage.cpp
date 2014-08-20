/*
  ==============================================================================

    BCMImage.cpp
    Created: 20 Aug 2014 11:09:48am
    Author:  giles

  ==============================================================================
*/

#include "BCMImage.h"

BCMImage::BCMImage(XmlElement& xml) : BCMGraphic(xml)
{
    fileName    = xml.getStringAttribute("filename", String::empty);
    stretchMode = getStretchModeFromXml(xml);
    opacity     = (float)(xml.getDoubleAttribute("opacity", 1.0f));
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
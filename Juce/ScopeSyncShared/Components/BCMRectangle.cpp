/*
  ==============================================================================

    BCMRectangle.cpp
    Created: 20 Aug 2014 10:11:57am
    Author:  giles

  ==============================================================================
*/

#include "BCMRectangle.h"

BCMRectangle::BCMRectangle(XmlElement& xml) : BCMGraphic(xml)
{
    cornerSize       = (float)(xml.getDoubleAttribute("cornersize", 0.0f));
    fillColour       = xml.getStringAttribute("fillcolour", "00000000");
    outlineThickness = (float)(xml.getDoubleAttribute("outlinethickness", 0.0f));
    outlineColour    = xml.getStringAttribute("outlinecolour", "00000000");
}

BCMRectangle::~BCMRectangle() {}
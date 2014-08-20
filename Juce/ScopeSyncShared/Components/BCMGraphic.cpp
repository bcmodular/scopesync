/*
  ==============================================================================

    BCMGraphic.cpp
    Created: 20 Aug 2014 11:55:58am
    Author:  giles

  ==============================================================================
*/

#include "BCMGraphic.h"
#include "../Properties/PropertiesHelper.h"

BCMGraphic::BCMGraphic(XmlElement& xml)
{
    XmlElement* boundsXml = xml.getChildByName("bounds");
    if (boundsXml != nullptr)
        getBoundsFromXml(*boundsXml, bounds);
}

BCMGraphic::~BCMGraphic() {}

/**
 * Intermediary class for extracting BCMSlider definitions from
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

#include "SliderProperties.h"
#include "PropertiesHelper.h"

SliderProperties::SliderProperties()
{
    initialise();
};

SliderProperties::SliderProperties(XmlElement& sliderXML)
{
    initialise();
    setValuesFromXML(sliderXML);
};

SliderProperties::SliderProperties(XmlElement& sliderXML, SliderProperties& parentSliderProperties)
{
    copyProperties(parentSliderProperties);
    setValuesFromXML(sliderXML);
};

SliderProperties::~SliderProperties()
{
};

void SliderProperties::initialise()
{
    // Ultimate fall-back defaults, used when creating default SliderProperties
    name               = "def";
    width              = 72;
    height             = 88;
    x                  = 0;
    y                  = 0;
    rangeMin           = 0;
    rangeMax           = 100;
    rangeInt           = 0.0001;
    style              = Slider::RotaryVerticalDrag;
    textBoxPosition    = Slider::TextBoxBelow;
    textBoxReadOnly    = false;
    textBoxWidth       = 66;
    textBoxHeight      = 16;
    fontHeight         = 15.00f;
    fontStyleFlags     = Font::plain;
    justificationFlags = Justification::centred;
    bcmLookAndFeelId   = String::empty;
    popupEnabled       = false;
    velocityBasedMode  = false;
    encoderSnap        = false;
};

void SliderProperties::copyProperties(SliderProperties& parentSliderProperties)
{
    name               = parentSliderProperties.name;
    width              = parentSliderProperties.width;
    height             = parentSliderProperties.height;
    x                  = parentSliderProperties.x;
    y                  = parentSliderProperties.y;
    rangeMin           = parentSliderProperties.rangeMin;
    rangeMax           = parentSliderProperties.rangeMax;
    rangeInt           = parentSliderProperties.rangeInt;
    style              = parentSliderProperties.style;
    textBoxPosition    = parentSliderProperties.textBoxPosition;
    textBoxReadOnly    = parentSliderProperties.textBoxReadOnly;
    textBoxWidth       = parentSliderProperties.textBoxWidth;
    textBoxHeight      = parentSliderProperties.textBoxHeight;
    fontHeight         = parentSliderProperties.fontHeight;
    fontStyleFlags     = parentSliderProperties.fontStyleFlags;
    justificationFlags = parentSliderProperties.justificationFlags;
    bcmLookAndFeelId   = parentSliderProperties.bcmLookAndFeelId;
    popupEnabled       = parentSliderProperties.popupEnabled;
    velocityBasedMode  = parentSliderProperties.velocityBasedMode;
    encoderSnap        = parentSliderProperties.encoderSnap;
};

void SliderProperties::setValuesFromXML(XmlElement& sliderXML)
{
    // Grab values set in XML
    forEachXmlChildElement(sliderXML, child)
    {
             if (child->hasTagName("name"))              name = child->getAllSubText().toLowerCase();
        else if (child->hasTagName("popupenabled"))      popupEnabled = child->getAllSubText().equalsIgnoreCase("true");
        else if (child->hasTagName("velocitybasedmode")) velocityBasedMode = child->getAllSubText().equalsIgnoreCase("true");
        else if (child->hasTagName("encodersnap"))       encoderSnap = child->getAllSubText().equalsIgnoreCase("true");
        else if (child->hasTagName("position"))          getPositionFromXml(*child, x, y);
        else if (child->hasTagName("size"))              getSizeFromXml(*child, width, height);
        else if (child->hasTagName("style"))             getSliderStyleFromXml(*child, style);
        else if (child->hasTagName("range"))             getRangeFromXml(*child, rangeMin, rangeMax, rangeInt);
        else if (child->hasTagName("textbox"))
        {
            forEachXmlChildElement(*child, subChild)
            {
                     if (subChild->hasTagName("position"))      getTextBoxPositionFromXML(*subChild, textBoxPosition);
                else if (subChild->hasTagName("readonly"))      textBoxReadOnly = subChild->getAllSubText().equalsIgnoreCase("true");
                else if (subChild->hasTagName("size"))          getSizeFromXml(*subChild, textBoxWidth, textBoxHeight);
                else if (subChild->hasTagName("font"))          getFontFromXml(*subChild, fontHeight, fontStyleFlags);
                else if (subChild->hasTagName("justification")) getJustificationFlagsFromXml(*subChild, justificationFlags);
            }
        }
    }

    if (sliderXML.hasAttribute("lfid")) bcmLookAndFeelId = sliderXML.getStringAttribute("lfid").toLowerCase();
};

void SliderProperties::getSliderStyleFromXml(const XmlElement& xml, Slider::SliderStyle& sliderStyle)
{
         if (xml.getAllSubText().toLowerCase() == "linearhorizontal")             sliderStyle = Slider::LinearHorizontal;
    else if (xml.getAllSubText().toLowerCase() == "linearvertical")               sliderStyle = Slider::LinearVertical;
    else if (xml.getAllSubText().toLowerCase() == "linearbar")                    sliderStyle = Slider::LinearBar;
    else if (xml.getAllSubText().toLowerCase() == "linearbarvertical")            sliderStyle = Slider::LinearBarVertical;
    else if (xml.getAllSubText().toLowerCase() == "rotary")                       sliderStyle = Slider::Rotary;
    else if (xml.getAllSubText().toLowerCase() == "rotaryhorizontaldrag")         sliderStyle = Slider::RotaryHorizontalDrag;
    else if (xml.getAllSubText().toLowerCase() == "rotaryverticaldrag")           sliderStyle = Slider::RotaryVerticalDrag;
    else if (xml.getAllSubText().toLowerCase() == "rotaryhorizontalverticaldrag") sliderStyle = Slider::RotaryHorizontalVerticalDrag;
    else if (xml.getAllSubText().toLowerCase() == "incdecbuttons")                sliderStyle = Slider::IncDecButtons;
}

void SliderProperties::getRangeFromXml(const XmlElement& xml, double& min, double& max, double& interval)
{
    forEachXmlChildElement(xml, child)
    {
             if (child->hasTagName("min")) min      = child->getAllSubText().getDoubleValue();
        else if (child->hasTagName("max")) max      = child->getAllSubText().getDoubleValue();
        else if (child->hasTagName("int")) interval = child->getAllSubText().getDoubleValue();
    }
}

void SliderProperties::getTextBoxPositionFromXML(const XmlElement& xml, Slider::TextEntryBoxPosition& position)
{
         if (xml.getAllSubText().toLowerCase() == "none")  position = Slider::NoTextBox;
    else if (xml.getAllSubText().toLowerCase() == "left")  position = Slider::TextBoxLeft;
    else if (xml.getAllSubText().toLowerCase() == "right") position = Slider::TextBoxRight;
    else if (xml.getAllSubText().toLowerCase() == "above") position = Slider::TextBoxAbove;
    else if (xml.getAllSubText().toLowerCase() == "below") position = Slider::TextBoxBelow;
}
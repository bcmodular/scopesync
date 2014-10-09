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
#include "../Core/ScopeSyncGUI.h"

SliderProperties::SliderProperties(ScopeSyncGUI& owner)
    : scopeSyncGUI(owner)
{
    initialise();
}

SliderProperties::SliderProperties(ScopeSyncGUI& owner, XmlElement& sliderXML)
    : scopeSyncGUI(owner), WidgetProperties(sliderXML)
{
    initialise();
    setValuesFromXML(sliderXML);
}

SliderProperties::SliderProperties(ScopeSyncGUI& owner, XmlElement& sliderXML, SliderProperties& parentSliderProperties)
    : scopeSyncGUI(owner), WidgetProperties(sliderXML, parentSliderProperties)
{
    copyProperties(parentSliderProperties);
    setValuesFromXML(sliderXML);
}

SliderProperties::~SliderProperties() {}

void SliderProperties::initialise()
{
    // Ultimate fall-back defaults, used when creating default SliderProperties
    rangeMin           = 0;
    rangeMax           = 100;
    rangeInt           = 0.0001;
    style              = Slider::RotaryVerticalDrag;
    incDecButtonMode   = scopeSyncGUI.settings.incDecButtonMode;
    textBoxPosition    = Slider::TextBoxBelow;
    textBoxReadOnly    = false;
    textBoxWidth       = 66;
    textBoxHeight      = 16;
    fontHeight         = 15.00f;
    fontStyleFlags     = Font::plain;
    justificationFlags = Justification::centred;
    popupEnabled       = (scopeSyncGUI.settings.popupEnabled      == BCMSlider::popupEnabled);
    velocityBasedMode  = (scopeSyncGUI.settings.velocityBasedMode == BCMSlider::velocityBasedModeOn);
    encoderSnap        = (scopeSyncGUI.settings.encoderSnap       == BCMSlider::snap);
    tabbedComponents.clear();
    tabNames.clear();
}

void SliderProperties::copyProperties(SliderProperties& parentSliderProperties)
{
    rangeMin           = parentSliderProperties.rangeMin;
    rangeMax           = parentSliderProperties.rangeMax;
    rangeInt           = parentSliderProperties.rangeInt;
    style              = parentSliderProperties.style;
    incDecButtonMode   = parentSliderProperties.incDecButtonMode;
    textBoxPosition    = parentSliderProperties.textBoxPosition;
    textBoxReadOnly    = parentSliderProperties.textBoxReadOnly;
    textBoxWidth       = parentSliderProperties.textBoxWidth;
    textBoxHeight      = parentSliderProperties.textBoxHeight;
    fontHeight         = parentSliderProperties.fontHeight;
    fontStyleFlags     = parentSliderProperties.fontStyleFlags;
    justificationFlags = parentSliderProperties.justificationFlags;
    popupEnabled       = parentSliderProperties.popupEnabled;
    velocityBasedMode  = parentSliderProperties.velocityBasedMode;
    encoderSnap        = parentSliderProperties.encoderSnap;
    tabbedComponents   = StringArray(parentSliderProperties.tabbedComponents);
    tabNames           = StringArray(parentSliderProperties.tabNames);
}

void SliderProperties::setValuesFromXML(XmlElement& sliderXML)
{
    popupEnabled      = sliderXML.getBoolAttribute  ("popupenabled",      popupEnabled);
    velocityBasedMode = sliderXML.getBoolAttribute  ("velocitybasedmode", velocityBasedMode);
    encoderSnap       = sliderXML.getBoolAttribute  ("encodersnap",       encoderSnap);
    
    forEachXmlChildElementWithTagName(sliderXML, chooseTabXml, "choosetab")
    {
        String tabbedComponent = chooseTabXml->getStringAttribute("tabbedcomponent", String::empty);
        String tabName         = chooseTabXml->getStringAttribute("tabname", String::empty);
            
        if (tabbedComponent.isNotEmpty() && tabName.isNotEmpty())
        {
            tabbedComponents.add(tabbedComponent);
            tabNames.add(tabName);
        }
    }

    getSliderStyleFromXml(sliderXML.getStringAttribute("style"), style);
    getIncDecButtonModeFromXml(sliderXML.getStringAttribute("incdecbuttonsmode"), incDecButtonMode);
    
    XmlElement* rangeXml = sliderXML.getChildByName("range");
    if (rangeXml != nullptr)
        getRangeFromXml(*rangeXml, rangeMin, rangeMax, rangeInt);

    XmlElement* textBoxXml = sliderXML.getChildByName("textbox");
    if (textBoxXml != nullptr)
    {
        getTextBoxPositionFromXML(textBoxXml->getStringAttribute("position"), textBoxPosition);
        
        textBoxReadOnly = textBoxXml->getBoolAttribute("readonly", textBoxReadOnly);
        textBoxWidth    = textBoxXml->getIntAttribute("width", textBoxWidth);
        textBoxHeight   = textBoxXml->getIntAttribute("height", textBoxHeight);

        XmlElement* fontXml = textBoxXml->getChildByName("font");
        if (fontXml != nullptr)
            getFontFromXml(*fontXml, fontHeight, fontStyleFlags);

        XmlElement* justificationXml = textBoxXml->getChildByName("justification");
        if (justificationXml != nullptr)
            getJustificationFlagsFromXml(*justificationXml, justificationFlags);
    }
}

void SliderProperties::getSliderStyleFromXml(const String& styleText, Slider::SliderStyle& sliderStyle)
{
         if (styleText.equalsIgnoreCase("linearhorizontal"))             sliderStyle = Slider::LinearHorizontal;
    else if (styleText.equalsIgnoreCase("linearvertical"))               sliderStyle = Slider::LinearVertical;
    else if (styleText.equalsIgnoreCase("linearbar"))                    sliderStyle = Slider::LinearBar;
    else if (styleText.equalsIgnoreCase("linearbarvertical"))            sliderStyle = Slider::LinearBarVertical;
    else if (styleText.equalsIgnoreCase("rotary"))                       sliderStyle = Slider::Rotary;
    else if (styleText.equalsIgnoreCase("rotaryhorizontaldrag"))         sliderStyle = Slider::RotaryHorizontalDrag;
    else if (styleText.equalsIgnoreCase("rotaryverticaldrag"))           sliderStyle = Slider::RotaryVerticalDrag;
    else if (styleText.equalsIgnoreCase("rotaryhorizontalverticaldrag")) sliderStyle = Slider::RotaryHorizontalVerticalDrag;
    else if (styleText.equalsIgnoreCase("incdecbuttons"))                sliderStyle = Slider::IncDecButtons;
    else if (styleText.equalsIgnoreCase("defaultrotarymovement"))        sliderStyle = scopeSyncGUI.getDefaultRotarySliderStyle();
}

void SliderProperties::getIncDecButtonModeFromXml(const String& buttonModeText, Slider::IncDecButtonMode& incDecButtonMode)
{
         if (buttonModeText.equalsIgnoreCase("notdraggable"))  incDecButtonMode = Slider::incDecButtonsNotDraggable;
    else if (buttonModeText.equalsIgnoreCase("autodirection")) incDecButtonMode = Slider::incDecButtonsDraggable_AutoDirection;
    else if (buttonModeText.equalsIgnoreCase("horizontal"))    incDecButtonMode = Slider::incDecButtonsDraggable_Horizontal;
    else if (buttonModeText.equalsIgnoreCase("vertical"))      incDecButtonMode = Slider::incDecButtonsDraggable_Vertical;
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

void SliderProperties::getTextBoxPositionFromXML(const String& positionText, Slider::TextEntryBoxPosition& position)
{
         if (positionText.equalsIgnoreCase("none"))  position = Slider::NoTextBox;
    else if (positionText.equalsIgnoreCase("left"))  position = Slider::TextBoxLeft;
    else if (positionText.equalsIgnoreCase("right")) position = Slider::TextBoxRight;
    else if (positionText.equalsIgnoreCase("above")) position = Slider::TextBoxAbove;
    else if (positionText.equalsIgnoreCase("below")) position = Slider::TextBoxBelow;
}
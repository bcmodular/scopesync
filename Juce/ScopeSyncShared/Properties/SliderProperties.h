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

#ifndef SLIDERPROPERTIES_H_INCLUDED
#define SLIDERPROPERTIES_H_INCLUDED

#include <JuceHeader.h>

class SliderProperties
{
public:
    // Fall-back constructor in case no default XML supplied
    SliderProperties();

    // Constructor for use when setting up defaults
    SliderProperties(XmlElement& sliderXML);

    // Constructor for use when overriding defaults for a specific Slider
    SliderProperties(XmlElement& sliderXML, SliderProperties& parentSliderProperties);

    ~SliderProperties();

    String name;
    int    width;
    int    height;
    int    x;
    int    y;
    double rangeMin;
    double rangeMax;
    double rangeInt;
    Slider::SliderStyle          style;
    Slider::TextEntryBoxPosition textBoxPosition;
    bool   textBoxReadOnly;
    int    textBoxWidth;
    int    textBoxHeight;
    float  fontHeight;
    Font::FontStyleFlags fontStyleFlags;
    Justification::Flags justificationFlags;
    String bcmLookAndFeelId;
    bool popupEnabled;
    bool velocityBasedMode;
    bool encoderSnap;

private:
    void initialise();
    void copyProperties(SliderProperties& parentSliderProperties);
    void setValuesFromXML(XmlElement& sliderXML);

    static void getSliderStyleFromXml(const XmlElement& xml, Slider::SliderStyle& sliderStyle);
    static void getRangeFromXml(const XmlElement& xml, double& min, double& max, double& interval);
    static void getTextBoxPositionFromXML(const XmlElement& xml, Slider::TextEntryBoxPosition& position);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderProperties);
};

#endif  // SLIDERPROPERTIES_H_INCLUDED

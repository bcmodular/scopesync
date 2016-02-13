/**
 * The BCModular version of Juce's Slider, which adds the ability
 * to be created from an XML definition, as well as being tied into
 * the ScopeSync parameter system
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

#ifndef BCMSLIDER_H_INCLUDED
#define BCMSLIDER_H_INCLUDED

class ScopeSyncGUI;
class SliderProperties;

#include <JuceHeader.h>
#include "../Core/BCMParameter.h"
#include "BCMWidget.h"
#include "BCMComponentBounds.h"

class BCMSlider : public Slider,
                  public BCMParameterWidget
{
public:
    BCMSlider(const String& name, ScopeSyncGUI& owner);
    ~BCMSlider();

    void applyProperties(SliderProperties& properties);
    const Identifier getComponentType() const override;

    // Font and justification variables (made public so they can be accessed by the LookAndFeel)
    float                fontHeight;
    Font::FontStyleFlags fontStyleFlags;
    Justification::Flags justificationFlags;
    
    // Enumerations for User Settings relating to BCMSliders
    enum RotaryMovement
    {
        noOverride_RM,
        rotary,
        vertical,
        horizontal,
        horizontalVertical
    };

    enum IDBMode
    {
        noOverride_IDB,
        idbNotDraggable,
        idbAutoDirection,
        idbHorizontal,
        idbVertical
    };

    enum EncoderSnap
    {
        noOverride_ES,
        dontSnap,
        snap
    };

    enum PopupEnabled
    {
        noOverride_PE,
        popupEnabled,
        popupDisabled
    };

    enum VelocityBasedMode
    {
        noOverride_VBM,
        velocityBasedModeOn,
        velocityBasedModeOff
    };

protected:
    void applyLookAndFeel(bool noStyleOverride) override;

private:
    
    StringArray settingsNames; // Names of discrete settings for the parameter mapped to

    bool        mapsToTabs;           // Does this slider map to one or more tabs in a tabbed component
    StringArray tabbedComponentNames; // Array of tabbed component names that this slider maps to
    StringArray tabNames;             // Names of specific tabs within the mapped tabbed components that this slider maps to
    
	void setupManagedSlider(const String& scopeCode, SliderProperties& props);

    // Returns a formatted string for a given value to be displayed in the Slider's Textbox
    String getTextFromValue(double v) override;

    void mouseDown(const MouseEvent& event) override;
    double valueToProportionOfLength(double value) override;

    // Interprets input typed into a Slider's Textbox and converts it into a value.
    // Includes support for discrete parameter settings
    double getValueFromText(const String& text);

    // Utility methods for applying User Settings that override aspects of Slider handling
    void overrideSliderStyle(Slider::SliderStyle& style);
    void overrideIncDecButtonMode(Slider::IncDecButtonMode& incDecButtonMode);
    void overridePopupEnabled(bool popupEnabledFlag);
    void overrideVelocityBasedMode(bool velocityBasedMode);
    bool getEncoderSnap(bool encoderSnap);

    // Switch any linked TabbedComponent's tabs as appropriate
    void switchToTabs();

    // Returns true if this is a Rotary-style Slider
    bool isRotary() const;

    // Returns true if this is a LinearBar-style Slider
    bool isLinearBar() const;

    // Open the specific Style Override Panel for sliders as appropriate
    void overrideStyle() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BCMSlider);
};

#endif  // BCMSLIDER_H_INCLUDED

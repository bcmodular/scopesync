/**
 * The BCModular version of Juce's TextButton, which adds the ability
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

#ifndef BCMTEXTBUTTON_H_INCLUDED
#define BCMTEXTBUTTON_H_INCLUDED

class ScopeSyncGUI;
class TextButtonProperties;

#include <JuceHeader.h>
#include "BCMTabbedComponent.h"
#include "../Core/BCMParameter.h"

class BCMTextButton : public TextButton,
                      public Value::Listener,
                      public Timer
{
public:
    BCMTextButton(ScopeSyncGUI& owner, String& name);
    ~BCMTextButton();

    void applyProperties(TextButtonProperties& properties);

    // Callback for when the value of a mapped parameter changes
    void valueChanged(Value& value);

    // Indicates whether a BCMComboBox has a parameter mapping
    bool  hasParameter() { return mapsToParameter; };
    
    // Returns parameter a BCMComboBox is mapped to
    BCMParameter* getParameter() { return parameter; };

private:
    Value parameterValue; // Maintains a link to a mapped parameter's UI value

    bool                        mapsToParameter; // Flag for whether BCMComboBox maps to a parameter
    WeakReference<BCMParameter> parameter;       // Pointer to a mapped parameter
    BCMComponentBounds          componentBounds; // Position/Size information

    URL url; // URL launched on clicking the button

    // Timer Callback. Used as part of the click block solution
    void timerCallback();

    bool             clicksBlocked;      // Flag to indicate that clicks should be blocked
    static const int clickBlockDuration; // Length of time (ms) for clicks to be blocked for
    
    bool        mapsToTabs;           // Does this button map to one or more tabs in a tabbed component
    StringArray tabbedComponentNames; // Array of tabbed component names that this button maps to
    StringArray tabNames;             // Names of specific tabs within the mapped tabbed components that this button maps to
    
    int downSettingIdx; // Discrete parameter setting that maps to BCMTextButton's down state
    int upSettingIdx;   // Discrete parameter setting that maps to BCMTextButton's up state
    
    ValueTree settings; // Discrete parameter settings
    
    ScopeSyncGUI& gui; // Reference to main ScopeSyncGUI

    // Switch any linked TabbedComponent's tabs as appropriate
    void switchToTabs();

    // For incrementing and decrementing MappingTypes, update the "next" values to be set
    void setNextValues();
    
    void mouseUp(const MouseEvent& event);
    
    // Callback for when a BCMTextButton is clicked
    void clicked();
    
    // Types of Mapping
    enum MappingType
    {
        noToggle, // Regular button that doesn't toggle
        toggle,   // Button that toggles (e.g. on/off)
        inc,      // Button that increments a value, stopping at maximum
        dec,      // Button that decrements a value, stopping at minimum
        incWrap,  // Button that increments a value, wrapping round to minimum after hitting maximum
        decWrap   // Button that decrements a value, wrapping round to maximum after hitting minimum
    };

    // Types of Button Display
    enum DisplayType
    {
        currentSetting, // Display the current setting for a parameter
        downSetting,    // Display the setting that will be chosen on clicking the button
        parameterName,  // Display a mapped parameter's name
        custom          // Display a custom piece of text
    };
    
    MappingType mappingType;
    DisplayType displayType;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BCMTextButton);
};

#endif  // BCMTEXTBUTTON_H_INCLUDED

/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-13 by Raw Material Software Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_93740D9F37075DA8__
#define __JUCE_HEADER_93740D9F37075DA8__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
#include "../Core/ScopeSyncGUI.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
 * User Settings popup handler.
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
                                                                    //[/Comments]
*/
class UserSettings  : public Component,
                      public ComboBoxListener
{
public:
    //==============================================================================
    UserSettings (ScopeSyncGUI& owner);
    ~UserSettings();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void loadSettings();
    void userTriedToCloseWindow();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    ScopeSyncGUI& scopeSyncGUI;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Label> encoderSnapLabel;
    ScopedPointer<ComboBox> encoderSnapComboBox;
    ScopedPointer<Label> rotaryMovementLabel;
    ScopedPointer<ComboBox> rotaryMovementComboBox;
    ScopedPointer<Label> popupEnabledLabel;
    ScopedPointer<ComboBox> popupEnabledComboBox;
    ScopedPointer<Label> encoderVelocityModeLabel;
    ScopedPointer<ComboBox> encoderVelocityModeComboBox;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UserSettings)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_93740D9F37075DA8__

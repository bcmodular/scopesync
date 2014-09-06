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

//[Headers] You can add your own extra header files here...
/*
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
 */
//[/Headers]

#include "UserSettings.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
UserSettings::UserSettings (ScopeSyncGUI& owner)
    : scopeSyncGUI(owner)
{
    addAndMakeVisible (encoderSnapLabel = new Label ("Encoder Snap Label",
                                                     TRANS("Encoder Snap To Value")));
    encoderSnapLabel->setTooltip (TRANS("Choose whether encoders snap to valid parameter values"));
    encoderSnapLabel->setFont (Font (15.00f, Font::plain));
    encoderSnapLabel->setJustificationType (Justification::centredRight);
    encoderSnapLabel->setEditable (false, false, false);
    encoderSnapLabel->setColour (Label::textColourId, Colours::aliceblue);
    encoderSnapLabel->setColour (TextEditor::textColourId, Colours::black);
    encoderSnapLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (encoderSnapComboBox = new ComboBox ("Encoder Snap ComboBox"));
    encoderSnapComboBox->setTooltip (TRANS("Choose whether encoders snap to valid parameter values"));
    encoderSnapComboBox->setEditableText (false);
    encoderSnapComboBox->setJustificationType (Justification::centredLeft);
    encoderSnapComboBox->setTextWhenNothingSelected (String::empty);
    encoderSnapComboBox->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    encoderSnapComboBox->addItem (TRANS("No Override"), 1);
    encoderSnapComboBox->addItem (TRANS("Don\'t Snap"), 2);
    encoderSnapComboBox->addItem (TRANS("Snap"), 3);
    encoderSnapComboBox->addListener (this);

    addAndMakeVisible (rotaryMovementLabel = new Label ("Rotary Movement Label",
                                                        TRANS("Rotary Encoder Movement")));
    rotaryMovementLabel->setTooltip (TRANS("Choose which mouse movement type to be used by rotary encoders "));
    rotaryMovementLabel->setFont (Font (15.00f, Font::plain));
    rotaryMovementLabel->setJustificationType (Justification::centredRight);
    rotaryMovementLabel->setEditable (false, false, false);
    rotaryMovementLabel->setColour (Label::textColourId, Colours::aliceblue);
    rotaryMovementLabel->setColour (TextEditor::textColourId, Colours::black);
    rotaryMovementLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (rotaryMovementComboBox = new ComboBox ("Rotary Movement ComboBox"));
    rotaryMovementComboBox->setTooltip (TRANS("Choose which mouse movement type to be used by rotary encoders "));
    rotaryMovementComboBox->setEditableText (false);
    rotaryMovementComboBox->setJustificationType (Justification::centredLeft);
    rotaryMovementComboBox->setTextWhenNothingSelected (String::empty);
    rotaryMovementComboBox->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    rotaryMovementComboBox->addItem (TRANS("No Override"), 1);
    rotaryMovementComboBox->addItem (TRANS("Rotary"), 2);
    rotaryMovementComboBox->addItem (TRANS("Vertical"), 3);
    rotaryMovementComboBox->addItem (TRANS("Horizontal"), 4);
    rotaryMovementComboBox->addItem (TRANS("Horizontal & Vertical"), 5);
    rotaryMovementComboBox->addListener (this);

    addAndMakeVisible (popupEnabledLabel = new Label ("Popup Enabled Label",
                                                      TRANS("Encoder Popup Enabled")));
    popupEnabledLabel->setTooltip (TRANS("Choose whether encoders show a popup with current value when dragging"));
    popupEnabledLabel->setFont (Font (15.00f, Font::plain));
    popupEnabledLabel->setJustificationType (Justification::centredRight);
    popupEnabledLabel->setEditable (false, false, false);
    popupEnabledLabel->setColour (Label::textColourId, Colours::aliceblue);
    popupEnabledLabel->setColour (TextEditor::textColourId, Colours::black);
    popupEnabledLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (popupEnabledComboBox = new ComboBox ("Popup Enabled ComboBox"));
    popupEnabledComboBox->setTooltip (TRANS("Choose whether encoders show a popup with current value when dragging"));
    popupEnabledComboBox->setEditableText (false);
    popupEnabledComboBox->setJustificationType (Justification::centredLeft);
    popupEnabledComboBox->setTextWhenNothingSelected (String::empty);
    popupEnabledComboBox->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    popupEnabledComboBox->addItem (TRANS("No Override"), 1);
    popupEnabledComboBox->addItem (TRANS("Enabled"), 2);
    popupEnabledComboBox->addItem (TRANS("Disabled"), 3);
    popupEnabledComboBox->addListener (this);

    addAndMakeVisible (encoderVelocityModeLabel = new Label ("Encoder Velocity Mode Label",
                                                             TRANS("Encoder Velocity Mode")));
    encoderVelocityModeLabel->setTooltip (TRANS("Choose whether Velocity Based Mode is enabled for Encoders"));
    encoderVelocityModeLabel->setFont (Font (15.00f, Font::plain));
    encoderVelocityModeLabel->setJustificationType (Justification::centredRight);
    encoderVelocityModeLabel->setEditable (false, false, false);
    encoderVelocityModeLabel->setColour (Label::textColourId, Colours::aliceblue);
    encoderVelocityModeLabel->setColour (TextEditor::textColourId, Colours::black);
    encoderVelocityModeLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (encoderVelocityModeComboBox = new ComboBox ("Encoder Velocity Mode ComboBox"));
    encoderVelocityModeComboBox->setTooltip (TRANS("Choose whether Velocity Based Mode is enabled for Encoders"));
    encoderVelocityModeComboBox->setEditableText (false);
    encoderVelocityModeComboBox->setJustificationType (Justification::centredLeft);
    encoderVelocityModeComboBox->setTextWhenNothingSelected (String::empty);
    encoderVelocityModeComboBox->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    encoderVelocityModeComboBox->addItem (TRANS("No Override"), 1);
    encoderVelocityModeComboBox->addItem (TRANS("Enabled"), 2);
    encoderVelocityModeComboBox->addItem (TRANS("Disabled"), 3);
    encoderVelocityModeComboBox->addListener (this);


    //[UserPreSize]
    loadSettings();
    //[/UserPreSize]

    setSize (400, 175);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

UserSettings::~UserSettings()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    encoderSnapLabel = nullptr;
    encoderSnapComboBox = nullptr;
    rotaryMovementLabel = nullptr;
    rotaryMovementComboBox = nullptr;
    popupEnabledLabel = nullptr;
    popupEnabledComboBox = nullptr;
    encoderVelocityModeLabel = nullptr;
    encoderVelocityModeComboBox = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //closeWindow();
    //[/Destructor]
}

//==============================================================================
void UserSettings::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

   g.fillAll (Colour (0xff434343));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void UserSettings::resized()
{
    encoderSnapLabel->setBounds (8, 24, 182, 24);
    encoderSnapComboBox->setBounds (199, 24, 185, 24);
    rotaryMovementLabel->setBounds (8, 56, 182, 24);
    rotaryMovementComboBox->setBounds (199, 56, 185, 24);
    popupEnabledLabel->setBounds (8, 88, 182, 24);
    popupEnabledComboBox->setBounds (199, 88, 185, 24);
    encoderVelocityModeLabel->setBounds (8, 122, 182, 24);
    encoderVelocityModeComboBox->setBounds (199, 122, 185, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void UserSettings::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == encoderSnapComboBox)
    {
        //[UserComboBoxCode_encoderSnapComboBox] -- add your combo box handling code here..
        PropertiesFile& properties = scopeSyncGUI.getScopeSync().getAppProperties();
        properties.setValue("encodersnap", comboBoxThatHasChanged->getSelectedId());
        properties.saveIfNeeded();
        //[/UserComboBoxCode_encoderSnapComboBox]
    }
    else if (comboBoxThatHasChanged == rotaryMovementComboBox)
    {
        //[UserComboBoxCode_rotaryMovementComboBox] -- add your combo box handling code here..
        PropertiesFile& properties = scopeSyncGUI.getScopeSync().getAppProperties();
        properties.setValue("rotarymovement", comboBoxThatHasChanged->getSelectedId());
        properties.saveIfNeeded();
        //[/UserComboBoxCode_rotaryMovementComboBox]
    }
    else if (comboBoxThatHasChanged == popupEnabledComboBox)
    {
        //[UserComboBoxCode_popupEnabledComboBox] -- add your combo box handling code here..
        PropertiesFile& properties = scopeSyncGUI.getScopeSync().getAppProperties();
        properties.setValue("popupenabled", comboBoxThatHasChanged->getSelectedId());
        properties.saveIfNeeded();
        //[/UserComboBoxCode_popupEnabledComboBox]
    }
    else if (comboBoxThatHasChanged == encoderVelocityModeComboBox)
    {
        //[UserComboBoxCode_encoderVelocityModeComboBox] -- add your combo box handling code here..
        PropertiesFile& properties = scopeSyncGUI.getScopeSync().getAppProperties();
        properties.setValue("velocitybasedmode", comboBoxThatHasChanged->getSelectedId());
        properties.saveIfNeeded();
        //[/UserComboBoxCode_encoderVelocityModeComboBox]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void UserSettings::loadSettings()
{
    PropertiesFile& properties = scopeSyncGUI.getScopeSync().getAppProperties();

    encoderSnapComboBox->setSelectedId(properties.getIntValue("encodersnap", 1));
    rotaryMovementComboBox->setSelectedId(properties.getIntValue("rotarymovement", 1));
    popupEnabledComboBox->setSelectedId(properties.getIntValue("popupenabled", 1));
    encoderVelocityModeComboBox->setSelectedId(properties.getIntValue("velocitybasedmode", 1));
}

void UserSettings::userTriedToCloseWindow()
{
    scopeSyncGUI.getScopeSync().setGUIReload(true);
    scopeSyncGUI.hideUserSettings();
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="UserSettings" componentName=""
                 parentClasses="public Component" constructorParams="ScopeSync&amp; owner"
                 variableInitialisers="scopeSync(owner)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="400"
                 initialHeight="175">
  <BACKGROUND backgroundColour="ff434343"/>
  <LABEL name="Encoder Snap Label" id="cde0d151d51d9d02" memberName="encoderSnapLabel"
         virtualName="" explicitFocusOrder="0" pos="8 24 182 24" tooltip="Choose whether encoders snap to valid parameter values"
         textCol="fff0f8ff" edTextCol="ff000000" edBkgCol="0" labelText="Encoder Snap To Value"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15" bold="0" italic="0" justification="34"/>
  <COMBOBOX name="Encoder Snap ComboBox" id="74a940e37538ca1b" memberName="encoderSnapComboBox"
            virtualName="" explicitFocusOrder="0" pos="199 24 185 24" tooltip="Choose whether encoders snap to valid parameter values"
            editable="0" layout="33" items="No Override&#10;Don't Snap&#10;Snap"
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="Rotary Movement Label" id="d595993e92d53d8c" memberName="rotaryMovementLabel"
         virtualName="" explicitFocusOrder="0" pos="8 56 182 24" tooltip="Choose which mouse movement type to be used by rotary encoders "
         textCol="fff0f8ff" edTextCol="ff000000" edBkgCol="0" labelText="Rotary Encoder Movement"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15" bold="0" italic="0" justification="34"/>
  <COMBOBOX name="Rotary Movement ComboBox" id="42e918cb54dddfd2" memberName="rotaryMovementComboBox"
            virtualName="" explicitFocusOrder="0" pos="199 56 185 24" tooltip="Choose which mouse movement type to be used by rotary encoders "
            editable="0" layout="33" items="No Override&#10;Rotary&#10;Vertical&#10;Horizontal&#10;Horizontal &amp; Vertical"
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="Popup Enabled Label" id="c5704a7517b37afa" memberName="popupEnabledLabel"
         virtualName="" explicitFocusOrder="0" pos="8 88 182 24" tooltip="Choose whether encoders show a popup with current value when dragging"
         textCol="fff0f8ff" edTextCol="ff000000" edBkgCol="0" labelText="Encoder Popup Enabled"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15" bold="0" italic="0" justification="34"/>
  <COMBOBOX name="Popup Enabled ComboBox" id="6fa875e25592362e" memberName="popupEnabledComboBox"
            virtualName="" explicitFocusOrder="0" pos="199 88 185 24" tooltip="Choose whether encoders show a popup with current value when dragging"
            editable="0" layout="33" items="No Override&#10;Enabled&#10;Disabled"
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="Encoder Velocity Mode Label" id="c5b67e3021d64d08" memberName="encoderVelocityModeLabel"
         virtualName="" explicitFocusOrder="0" pos="8 122 182 24" tooltip="Choose whether Velocity Based Mode is enabled for Encoders"
         textCol="fff0f8ff" edTextCol="ff000000" edBkgCol="0" labelText="Encoder Velocity Mode"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15" bold="0" italic="0" justification="34"/>
  <COMBOBOX name="Encoder Velocity Mode ComboBox" id="d171fd260fb3d2f1" memberName="encoderVelocityModeComboBox"
            virtualName="" explicitFocusOrder="0" pos="199 122 185 24" tooltip="Choose whether Velocity Based Mode is enabled for Encoders"
            editable="0" layout="33" items="No Override&#10;Enabled&#10;Disabled"
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

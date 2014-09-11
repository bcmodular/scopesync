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

#include "UserSettings.h"
#include "../Core/Global.h"
#include "../Core/ScopeSyncApplication.h"
#ifndef __DLL_EFFECT__
    #include "../../ScopeSyncPlugin/Source/PluginProcessor.h"
#else
    #include "../../ScopeSyncFX/Source/ScopeFX.h"
#endif // __DLL_EFFECT__

juce_ImplementSingleton(UserSettings)

UserSettings::UserSettings()
{
    PropertiesFile::Options options;
    options.applicationName     = ProjectInfo::projectName;
    options.folderName          = ProjectInfo::projectName;
    options.filenameSuffix      = "settings";
    options.osxLibrarySubFolder = "Application Support";
    appProperties.setStorageParameters(options);

    properties = appProperties.getUserSettings();

    setName("User Settings");
        
    setupGUIElements();
    loadSettings();
    setSize (400, 175);
}

UserSettings::~UserSettings()
{
    clearSingletonInstance();
}

void UserSettings::setupGUIElements()
{
    addAndMakeVisible (encoderSnapLabel = setupLabel("Encoder Snap Label",
                                                     "Encoder Snap To Value",
                                                     "Choose whether encoders snap to valid parameter values"));
    
    addAndMakeVisible(encoderSnapComboBox = setupComboBox("Encoder Snap ComboBox", "Choose whether encoders snap to valid parameter values"));
    encoderSnapComboBox->addItem (TRANS("No Override"), 1);
    encoderSnapComboBox->addItem (TRANS("Don\'t Snap"), 2);
    encoderSnapComboBox->addItem (TRANS("Snap"), 3);
    
    addAndMakeVisible (rotaryMovementLabel = setupLabel("Rotary Movement Label",
                                                        "Rotary Encoder Movement",
                                                        "Choose which mouse movement type to be used by rotary encoders"));
    
    addAndMakeVisible(rotaryMovementComboBox = setupComboBox("Rotary Movement ComboBox", "Choose which mouse movement type to be used by rotary encoders"));
    rotaryMovementComboBox->addItem (TRANS("No Override"), 1);
    rotaryMovementComboBox->addItem (TRANS("Rotary"), 2);
    rotaryMovementComboBox->addItem (TRANS("Vertical"), 3);
    rotaryMovementComboBox->addItem (TRANS("Horizontal"), 4);
    rotaryMovementComboBox->addItem (TRANS("Horizontal & Vertical"), 5);
    
    addAndMakeVisible (popupEnabledLabel = setupLabel("Popup Enabled Label",
                                                      "Encoder Popup Enabled",
                                                      "Choose whether encoders show a popup with current value when dragging"));
    
    addAndMakeVisible(popupEnabledComboBox = setupComboBox("Popup Enabled ComboBox", "Choose whether encoders show a popup with current value when dragging"));
    popupEnabledComboBox->addItem (TRANS("No Override"), 1);
    popupEnabledComboBox->addItem (TRANS("Enabled"), 2);
    popupEnabledComboBox->addItem (TRANS("Disabled"), 3);
    
    addAndMakeVisible (encoderVelocityModeLabel = setupLabel("Encoder Velocity Mode Label",
                                                             "Encoder Velocity Mode",
                                                             "Choose whether Velocity Based Mode is enabled for Encoders"));
    
    addAndMakeVisible(encoderVelocityModeComboBox = setupComboBox("Encoder Velocity Mode ComboBox", "Choose whether Velocity Based Mode is enabled for Encoders"));
    encoderVelocityModeComboBox->addItem (TRANS("No Override"), 1);
    encoderVelocityModeComboBox->addItem (TRANS("Enabled"), 2);
    encoderVelocityModeComboBox->addItem (TRANS("Disabled"), 3);
}

Label* UserSettings::setupLabel(const String& labelName, const String& labelText, const String& tooltip)
{
    Label* newLabel = new Label(labelName, labelText);
    newLabel->setTooltip(tooltip);
    newLabel->setFont(Font (15.00f, Font::plain));
    newLabel->setJustificationType(Justification::centredRight);
    newLabel->setEditable(false, false, false);
    newLabel->setColour(Label::textColourId, Colours::aliceblue);
    newLabel->setColour(TextEditor::textColourId, Colours::black);
    newLabel->setColour(TextEditor::backgroundColourId, Colour (0x00000000));

    return newLabel;
}

ComboBox* UserSettings::setupComboBox(const String& comboBoxName, const String& tooltip)
{
    ComboBox* newComboBox = new ComboBox (comboBoxName);
    newComboBox->setTooltip(tooltip);
    newComboBox->setEditableText(false);
    newComboBox->setJustificationType(Justification::centredLeft);
    newComboBox->setTextWhenNothingSelected(String::empty);
    newComboBox->setTextWhenNoChoicesAvailable(TRANS("(no choices)"));
    newComboBox->addListener(this);

    return newComboBox;
}

void UserSettings::paint (Graphics& g)
{
   g.fillAll (Colour (0xff434343));
}

void UserSettings::resized()
{
    encoderSnapLabel->setBounds(8, 24, 182, 24);
    encoderSnapComboBox->setBounds(199, 24, 185, 24);
    rotaryMovementLabel->setBounds(8, 56, 182, 24);
    rotaryMovementComboBox->setBounds(199, 56, 185, 24);
    popupEnabledLabel->setBounds(8, 88, 182, 24);
    popupEnabledComboBox->setBounds(199, 88, 185, 24);
    encoderVelocityModeLabel->setBounds(8, 122, 182, 24);
    encoderVelocityModeComboBox->setBounds(199, 122, 185, 24);
}

void UserSettings::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == encoderSnapComboBox)
        properties->setValue("encodersnap", comboBoxThatHasChanged->getSelectedId());
    else if (comboBoxThatHasChanged == rotaryMovementComboBox)
        properties->setValue("rotarymovement", comboBoxThatHasChanged->getSelectedId());
    else if (comboBoxThatHasChanged == popupEnabledComboBox)
        properties->setValue("popupenabled", comboBoxThatHasChanged->getSelectedId());
    else if (comboBoxThatHasChanged == encoderVelocityModeComboBox)
        properties->setValue("velocitybasedmode", comboBoxThatHasChanged->getSelectedId());
    
    properties->saveIfNeeded();
}

void UserSettings::loadSettings()
{
    encoderSnapComboBox->setSelectedId(properties->getIntValue("encodersnap", 1));
    rotaryMovementComboBox->setSelectedId(properties->getIntValue("rotarymovement", 1));
    popupEnabledComboBox->setSelectedId(properties->getIntValue("popupenabled", 1));
    encoderVelocityModeComboBox->setSelectedId(properties->getIntValue("velocitybasedmode", 1));
}

void UserSettings::userTriedToCloseWindow()
{
    ScopeSyncApplication::reloadAllGUIs(); 
    removeFromDesktop();
}

PropertiesFile* UserSettings::getAppProperties()
{
    return properties;
}

void UserSettings::show(int posX, int posY)
{
    setOpaque(true);
    setVisible(true);
    
    setBounds(posX, posY, 400, 200);
    
    addToDesktop(ComponentPeer::windowHasTitleBar | ComponentPeer::windowHasCloseButton | ComponentPeer::windowHasDropShadow, nullptr);
    setAlwaysOnTop(true);
    toFront(true);
}
    
void UserSettings::hide()
{
    removeFromDesktop();
}

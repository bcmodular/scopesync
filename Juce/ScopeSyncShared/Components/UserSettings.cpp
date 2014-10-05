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
#include "../Core/ScopeSync.h"
#include "../Utils/BCMMisc.h"

/* =========================================================================
 * EncoderSnapProperty
 */
class EncoderSnapProperty : public ChoicePropertyComponent
{
public:
    EncoderSnapProperty(UserSettings& userSettings) : ChoicePropertyComponent("Encoder Snap"), owner(userSettings)
    {
        choices.add ("No Override");
        choices.add ("Don\'t Snap");
        choices.add ("Snap");
    }

    void setIndex (int newIndex) override { owner.setPropertyIntValue("encodersnap", newIndex); }
    int  getIndex() const override { return owner.getPropertyIntValue("encodersnap", 1); }

private:
    UserSettings& owner;
};

/* =========================================================================
 * RotaryMovementProperty
 */
class RotaryMovementProperty : public ChoicePropertyComponent
{
public:
    RotaryMovementProperty(UserSettings& userSettings) : ChoicePropertyComponent("Rotary Encoder Movement"), owner(userSettings)
    {
        choices.add ("No Override");
        choices.add ("Rotary");
        choices.add ("Vertical");
        choices.add ("Horizontal");
        choices.add ("Horizontal & Vertical");
    }

    void setIndex (int newIndex) override { owner.setPropertyIntValue("rotarymovement", newIndex); }
    int  getIndex() const override { return owner.getPropertyIntValue("rotarymovement", 1); }

private:
    UserSettings& owner;
};

/* =========================================================================
 * IncDecButtonModeProperty
 */
class IncDecButtonModeProperty : public ChoicePropertyComponent
{
public:
    IncDecButtonModeProperty(UserSettings& userSettings) : ChoicePropertyComponent("Inc/Dec Button Mode"), owner(userSettings)
    {
        choices.add ("No Override");
        choices.add ("Not Draggable");
        choices.add ("Auto Direction");
        choices.add ("Horizontal");
        choices.add ("Vertical");
    }

    void setIndex (int newIndex) override { owner.setPropertyIntValue("incdecbuttonmode", newIndex); }
    int  getIndex() const override { return owner.getPropertyIntValue("incdecbuttonmode", 1); }

private:
    UserSettings& owner;
};

/* =========================================================================
 * PopupEnabledProperty
 */
class PopupEnabledProperty : public ChoicePropertyComponent
{
public:
    PopupEnabledProperty(UserSettings& userSettings) : ChoicePropertyComponent("Encoder Popup Enabled"), owner(userSettings)
    {
        choices.add ("No Override");
        choices.add ("Enabled");
        choices.add ("Disabled");
    }

    void setIndex (int newIndex) override { owner.setPropertyIntValue("popupenabled", newIndex); }
    int  getIndex() const override { return owner.getPropertyIntValue("popupenabled", 1); }

private:
    UserSettings& owner;
};

/* =========================================================================
 * EncoderVelocityModeProperty
 */
class EncoderVelocityModeProperty : public ChoicePropertyComponent
{
public:
    EncoderVelocityModeProperty(UserSettings& userSettings) : ChoicePropertyComponent("Encoder Velocity Mode"), owner(userSettings)
    {
        choices.add ("No Override");
        choices.add ("Enabled");
        choices.add ("Disabled");
    }

    void setIndex (int newIndex) override { owner.setPropertyIntValue("velocitybasedmode", newIndex); }
    int  getIndex() const override { return owner.getPropertyIntValue("velocitybasedmode", 1); }

private:
    UserSettings& owner;
};

/* =========================================================================
 * EnableTooltipsProperty
 */
class EnableTooltipsProperty : public ChoicePropertyComponent
{
public:
    EnableTooltipsProperty(UserSettings& userSettings) : ChoicePropertyComponent("Enable Tooltips"), owner(userSettings)
    {
        choices.add ("No Override");
        choices.add ("Enabled");
        choices.add ("Disabled");
    }

    void setIndex (int newIndex) override { owner.setPropertyIntValue("enabletooltips", newIndex); }
    int  getIndex() const override { return owner.getPropertyIntValue("enabletooltips", 1); }

private:
    UserSettings& owner;
};

juce_ImplementSingleton(UserSettings)

/* =========================================================================
 * UserSettings
 */
UserSettings::UserSettings()
{
    addAndMakeVisible(propertyPanel);
    setWantsKeyboardFocus(true);
    propertyPanel.setWantsKeyboardFocus(true);
    
    setName("User Settings");
    
    tooltipDelayTime.setValue(getPropertyIntValue("tooltipdelaytime", -1));
    tooltipDelayTime.addListener(this);

    setupPanel();
    
    setSize (getLocalBounds().getWidth(), getLocalBounds().getHeight());
}

UserSettings::~UserSettings()
{
    tooltipDelayTime.removeListener(this);
    clearSingletonInstance();
}

void UserSettings::setupPanel()
{
    PropertyListBuilder props;
    
    props.clear();
    props.add(new EnableTooltipsProperty(*this),                                "Choose whether tooltips are displayed when hovering over GUI elements");
    props.add(new IntRangeProperty(tooltipDelayTime, "Tooltip Delay Time", -1), "Enter the number of milliseconds to wait before a tooltip is displayed over GUI elements (-1 to use defaults)");

    propertyPanel.addSection("Tooltip Settings", props.components, true);
    
    props.clear();
    props.add(new EncoderSnapProperty(*this),                                   "Choose whether encoders snap to valid parameter values");
    props.add(new RotaryMovementProperty(*this),                                "Choose which mouse movement type is to be used by rotary encoders");
    props.add(new IncDecButtonModeProperty(*this),                              "Choose the mode for Inc/Dec button style Sliders");
    props.add(new PopupEnabledProperty(*this),                                  "Choose whether encoders show a popup with current value when dragging");
    props.add(new EncoderVelocityModeProperty(*this),                           "Choose whether Velocity Based Mode is enabled for Encoders");
    
    propertyPanel.addSection("Encoder Settings", props.components, true);
}

void UserSettings::paint (Graphics& g)
{
    g.fillAll (Colour (0xff434343));

    g.setColour(Colours::lightgrey);

    Rectangle<int> localBounds(getLocalBounds().reduced(4));
    g.fillRect(localBounds);
}

void UserSettings::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    
    localBounds.removeFromLeft(4);
    localBounds.removeFromRight(8);
    localBounds.removeFromTop(8);

    propertyPanel.setBounds(localBounds);
}

int UserSettings::getPropertyIntValue(const String& propertyName, int defaultValue)
{
    return getAppProperties()->getIntValue(propertyName, defaultValue);
}

void UserSettings::setPropertyIntValue(const String& propertyName, int newValue)
{
    return getAppProperties()->setValue(propertyName, newValue);
}

void UserSettings::valueChanged(Value& valueThatChanged)
{
    if (valueThatChanged.refersToSameSourceAs(tooltipDelayTime))
    {
        setPropertyIntValue("tooltipdelaytime", valueThatChanged.getValue());
    }
}

void UserSettings::userTriedToCloseWindow()
{
    ScopeSync::reloadAllGUIs(); 
    removeFromDesktop();
}

PropertiesFile* UserSettings::getAppProperties()
{
    PropertiesFile::Options options;
    options.applicationName     = ProjectInfo::projectName;
    options.folderName          = ProjectInfo::projectName;
    options.filenameSuffix      = "settings";
    options.osxLibrarySubFolder = "Application Support";
    appProperties.setStorageParameters(options);

    return appProperties.getUserSettings();
}

void UserSettings::show(int posX, int posY)
{
    setOpaque(true);
    setVisible(true);
    
    setBounds(posX, posY, 600, 300);
    
    addToDesktop(ComponentPeer::windowHasTitleBar | ComponentPeer::windowHasCloseButton | ComponentPeer::windowHasDropShadow, nullptr);
    setAlwaysOnTop(true);
    toFront(true);
}
    
void UserSettings::hide()
{
    removeFromDesktop();
}

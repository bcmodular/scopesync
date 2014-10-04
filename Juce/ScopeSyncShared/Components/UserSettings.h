//==============================================================================
/**
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

#ifndef USERSETTINGS_H_INCLUDED
#define USERSETTINGS_H_INCLUDED

#include <JuceHeader.h>

class UserSettings  : public Component
{
public:
    UserSettings ();
    ~UserSettings();

    void show(int posX, int posY);
    void hide();

    PropertiesFile* getAppProperties();
    int  getPropertyIntValue(const String& propertyName, int defaultValue);
    void setPropertyIntValue(const String& propertyName, int newValue);

    juce_DeclareSingleton (UserSettings, false)

private:
    ApplicationProperties appProperties;
    PropertyPanel         propertyPanel;
    
    void userTriedToCloseWindow() override;
    void paint (Graphics& g) override;
    void resized() override;
    
    void setupPanel();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UserSettings)
};

#endif   // USERSETTINGS_H_INCLUDED

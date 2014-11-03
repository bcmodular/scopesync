/**
 * The BCModular version of Juce's TabbedComponent, which adds the
 * ability to be created from an XML definition
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

#ifndef BCMTABBEDCOMPONENT_H_INCLUDED
#define BCMTABBEDCOMPONENT_H_INCLUDED

class TabbedComponentProperties;
class ScopeSyncGUI;

#include <JuceHeader.h>
#include "../Components/BCMLookAndFeel.h"
#include "../Components/BCMComponentBounds.h"
#include "../Core/BCMParameter.h"
#include "BCMWidget.h"

class BCMTabbedComponent : public TabbedComponent,
                           public Value::Listener,
                           public BCMParameterWidget
{
public:
    BCMTabbedComponent(TabbedButtonBar::Orientation orientation, ScopeSyncGUI& owner);
    ~BCMTabbedComponent();
    
    void applyProperties(TabbedComponentProperties& properties);
    const Identifier getComponentType() const override;

    // Returns the name of a BCMTabbedComponent
    //const String& getName() { return name; }
    
    // Sets the name of a BCMTabbedComponent
    //void setName(const String& newName) { name = newName; }

    // Callback for when the value of a mapped parameter changes
    void valueChanged(Value& value) override;

    void currentTabChanged(int newCurrentTabIndex, const String &newCurrentTabName);
    void popupMenuClickOnTab(int tabIndex, const String& tabName);

    // Indicates whether Tabs should show a drop-shadow
    bool shouldShowDropShadow() { return showDropShadow; }

private:
    Value parameterValue;  // Maintains a link to a mapped parameter's UI value

    String name;           // Name of BCMTabbedComponent
    bool   showDropShadow; // Flag as to whether Tabs should display a drop-shadow
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BCMTabbedComponent);
};

#endif  // BCMTABBEDCOMPONENT_H_INCLUDED

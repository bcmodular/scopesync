/**
 * Wrapper class that provides the ScopeSync GUI to the Scope application.
 * Handles things like window title bar etc.
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

#ifndef SCOPEFXGUI_H_INCLUDED
#define SCOPEFXGUI_H_INCLUDED

class ScopeFX;

#include <JuceHeader.h>
#include "../../ScopeSyncShared/Core/ScopeSyncGUI.h"
#include <Windows.h>

class BCMParameter;

class ScopeFXGUI : public Component,
                   public Value::Listener,
				   public Timer
{
public:
    ScopeFXGUI (ScopeFX* owner, HWND scopeWindow);
    ~ScopeFXGUI();

    // Handles resizing the window if the contents have changed in size
    // and updating the window title
    void refreshWindow();

    void valueChanged(Value& valueThatChanged) override;
	void timerCallback() override;
    
private:
    ScopeFX*                    scopeFX;
    ScopedPointer<ScopeSyncGUI> scopeSyncGUI;
    
    Value xPos;
    Value yPos;

    void userTriedToCloseWindow();
    void moved();

    String windowName;
    bool   firstTimeShow; // Little hack to get the window to resize correctly on initial load
    
    void paint(Graphics& g) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopeFXGUI)
};

#endif  // SCOPEFXGUI_H_INCLUDED

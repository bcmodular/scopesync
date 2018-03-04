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
 * the Free Software Foundation, either version 3 of the License, or
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
#include "../Core/ScopeSyncGUI.h"
#include <Windows.h>
#include "../Parameters/BCMParameterController.h"

class BCMParameter;

class ScopeFXGUI : public Component,
                   public Value::Listener,
				   public ComponentListener,
				   public Timer
{
public:
    ScopeFXGUI (ScopeFX* owner);
    ~ScopeFXGUI();

	void open(HWND scopeWindow);

    // Handles resizing the window if the contents have changed in size
    // and updating the window title
    void refreshWindow();

    void valueChanged(Value& valueThatChanged) override;
	void componentMovedOrResized(Component&	component, bool wasMoved, bool wasResized) override;
	    
private:
    ScopeFX*                    scopeFX;
	BCMParameterController*     parameterController;
    ScopedPointer<ScopeSyncGUI> scopeSyncGUI;
    
    Value xPos;
    Value yPos;
	Value configurationName;

    void userTriedToCloseWindow() override;
    void moved() override;

    bool firstTimeShow; // Little hack to get the window to resize correctly on initial load

	bool ignoreXYFromScope; // If the user has recently been moving the window themselves,
						    // then don't respond to incoming Scope values

	void paint(Graphics& g) override;

	void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopeFXGUI)
};

#endif  // SCOPEFXGUI_H_INCLUDED

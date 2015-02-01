/**
 * Wrapper class that hosts the ScopeSync object and its GUI
 * on behalf of the Scope (or Scope SDK) applications, using 
 * the ScopeFX SDK. This requires it to be derived from the Effect
 * class.
 *
 * Also operates as a Timer source for the ScopeSync and ScopeFXGUI
 * objects.
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

#ifndef SCOPEFX_H_INCLUDED
#define SCOPEFX_H_INCLUDED

//#include <vld.h>
#include <JuceHeader.h>
#include "../../ScopeSyncShared/Core/ScopeSync.h"
class ScopeFXGUI;
#include "../Include/SonicCore/effclass.h"
#include "ScopeFXParameterDefinitions.h"

class ScopeFX : public Effect, public Timer
{
public:
    ScopeFX();
    ~ScopeFX();

    // Process a set of Sync data coming in from Scope
    // and fill in outgoing streams as appropriate
    int  syncBlock (PadData** asyncIn,  PadData* syncIn,
                    PadData*  asyncOut, PadData* syncOut, 
                    int       off,      int      cnt);

    // Process new Async values coming in from Scope and pass on
    // updates from within ScopeSync
    int  async (PadData** asyncIn,  PadData* syncIn,
                PadData*  asyncOut, PadData* syncOut);
    
    // Hides the GUI window
    void hideWindow();
    
    // Update current window location values
    void positionChanged(int newPosX, int newPosY);
    
    ScopeSync& getScopeSync() { return *scopeSync; };
    void setGUIEnabled(bool shouldBeEnabled);

private:	
    // Initialise member variables
    void initValues();
   
    // Show the ScopeSync GUI window
    void showWindow();

	// Handle the Timer callback and trigger callbacks in
    // ScopeSync and ScopeFXGUI. Handles window resizing and location.
    void timerCallback();

    ScopedPointer<ScopeSync> scopeSync;	
    
    static const int initPositionX;
	static const int initPositionY;

    static const int windowHandlerDelayMax;
    static const int timerFrequency;
    
    // Async values that are handled directly by ScopeFX
    int positionX;        // Horizontal location of window
	int positionY;        // Vertical location of window
	int configurationUID; // UID of configuration file

	bool requestWindowShow;   // Flag to indicate that window should be shown
    bool windowShown;         // Flag to indicate that window is currently being shown

	int  asyncCounter;        // Counter as part of flip-flop avoidance solution!
    bool processAsyncMessage; // Flag to indicate whether to process the current message
						      // to avoid flip-flopping problems

    int windowHandlerDelay; // Hack to avoid feedback loops when moving window around in Scope

    ScopedPointer<ScopeFXGUI> scopeFXGUI;
};


#endif  // SCOPEFX_H_INCLUDED

/**
 * ScopeSync Class for interfacing with the Sonic|Core Scope Async
 * messaging system. Used within the scopefx AppContext.
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

#ifndef SCOPESYNCASYNC_H_INCLUDED
#define SCOPESYNCASYNC_H_INCLUDED

class ScopeSync;

#include <JuceHeader.h>
#include "../Include/SonicCore/effclass.h"

class ScopeSyncAsync {

public:
    
    ScopeSyncAsync();
    ~ScopeSyncAsync();

    // Processes an incoming array of Async values coming from Scope and passes
    // on any updates from the ScopeSync system
    void handleUpdate(int* asyncValues, bool initialise);
    
    // Passes on the contents of the queue of updates received from Scope
    void getAsyncUpdates(HashMap<int, int, DefaultHashFunctions, CriticalSection>& targetHashMap);

    // Add a new control value change to the queue for processing in the next
    // batch of Async updates
    void setValue(int scopeCode, int newValue);
    
private:

    Array<int, CriticalSection> currentValues;
    
    HashMap<int, int, DefaultHashFunctions, CriticalSection> asyncUpdates;     // Updates received from the async input
	HashMap<int, int, DefaultHashFunctions, CriticalSection> scopeSyncUpdates; // Updates coming from within the ScopeSync code, e.g. GUI or OSC/MIDI changes

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeSyncAsync);
};

#endif  // SCOPESYNCASYNC_H_INCLUDED

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

class ScopeSyncAsync {

public:
    
    ScopeSyncAsync();
    ~ScopeSyncAsync();

    // Processes an incoming array of Async values coming from Scope and passes
    // on any updates from the ScopeSync system
    void handleUpdate(Array<int>& asyncValues, bool initialise);
    
    // Generates a set of async entries that represents current values
    // for all controls
    void createSnapshot();

    // Returns a chunk of the generated snapshot
    void getSnapshot(Array<std::pair<int,int>>& snapshotSubset, int numElements);
    
    // Passes on the contents of the queue of updates received from Scope
    void getAsyncUpdatesArray(Array<std::pair<int, int>, CriticalSection>& asyncUpdateArray);

    // Add a new control value change to the queue for processing in the next
    // batch of Async updates
    void setValue(int scopeCode, int newValue);
    
private:
    static const int maxDeadTimeCounter;

    Array<int, CriticalSection> currentValues;
    Array<int, CriticalSection> deadTimeCounters;
    
    Array<std::pair<int,int>, CriticalSection> asyncUpdates;     // Updates received from the async input
    Array<std::pair<int,int>, CriticalSection> scopeSyncUpdates; // Updates coming from within the ScopeSync code, most likely GUI for now
    Array<std::pair<int,int>, CriticalSection> snapshot;         // Snapshot messages queued up to be sent

    void decDeadTimeCounter(int index);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeSyncAsync);
};

#endif  // SCOPESYNCASYNC_H_INCLUDED

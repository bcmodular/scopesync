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

#include "ScopeSyncAsync.h"
#include "../Core/ScopeSync.h"
#include "../Core/Global.h"

const int ScopeSyncAsync::maxDeadTimeCounter      = 4;

ScopeSyncAsync::ScopeSyncAsync()
{
    for (int i = 0; i < ScopeSyncApplication::numScopeSyncParameters + ScopeSyncApplication::numScopeLocalParameters; i++)
    {
        currentValues.add(0);
        deadTimeCounters.add(0);
    }
}

ScopeSyncAsync::~ScopeSyncAsync() {}

void ScopeSyncAsync::handleUpdate(Array<int>& asyncValues, bool initialise)
{
    Array<int> receivedFromScopeSync;

    int numScopeSyncUpdates = scopeSyncUpdates.size();

    for (int i = numScopeSyncUpdates - 1; i >= 0; i--)
    {
        int scopeCode = scopeSyncUpdates[i].first;

        if (receivedFromScopeSync.contains(scopeCode))
        {
            // Only interested in latest update for a given value
            DBG("ScopeSyncAsync::handleUpdate - Trimming updates for parameter: " + String(scopeCode));
            scopeSyncUpdates.remove(i);
            continue;
        }
        
        int newScopeSyncValue = scopeSyncUpdates[i].second;
        asyncValues.set(scopeCode, newScopeSyncValue);
        deadTimeCounters.set(scopeCode, maxDeadTimeCounter);

        DBG("ScopeSyncAsync::handleUpdate - Received parameter update from ScopeSync - scopeCode:" + String(scopeCode) + ", value: " + String(newScopeSyncValue));
            
        receivedFromScopeSync.add(scopeCode);
        scopeSyncUpdates.remove(i);
    }

    for (int i = 0; i < ScopeSyncApplication::numScopeSyncParameters + ScopeSyncApplication::numScopeLocalParameters; i++)
    {
        if (receivedFromScopeSync.contains(i) || deadTimeCounters[i] > 0)
        {
            // We've just (or recently) processed an update from ScopeSync for this
            // value, so we're going to ignore any async updates for now.
            currentValues.set(i, asyncValues[i]);
            decDeadTimeCounter(i);
            DBG("ScopeSyncAsync::handleUpdate - Ignoring Async update for (" + String(i) + "), new dead time counter: " + String(deadTimeCounters[i]));
            continue;
        }

        // Now check to see if this value has changed since last time we received
        // an update
        int newValue = asyncValues[i];
        
        if (newValue != currentValues[i] || initialise)
        {
            std::pair<int,int> controlUpdate = std::make_pair(i, newValue);
            
            currentValues.set(i, newValue);
            asyncValues.set(i, newValue);
            asyncUpdates.add(controlUpdate);
        }
    }
}

void ScopeSyncAsync::createSnapshot()
{
    if (snapshot.size() == 0)
    {
        for (int i = 0; i < ScopeSyncApplication::numScopeSyncParameters; i++)
        {
            int data = currentValues[i]; 
            int dest = (i + 1 + (2 * (i / 16))) * 8388608;
        
            std::pair<int,int> ctrlMessage = std::make_pair(data, dest);
            snapshot.add(ctrlMessage);
            DBG("ScopeSyncAsync::createSnapshot - Added to snapshot: data: " + String(data) + ", dest: " + String(dest));
        }
    }
}

void ScopeSyncAsync::getSnapshot(Array<std::pair<int,int>>& snapshotSubset, int numElements)
{
    for (int i = 0; i < snapshot.size() && i < numElements; i++)
    {
        snapshotSubset.add(snapshot.getFirst());
        snapshot.remove(0);
    }
}

void ScopeSyncAsync::getAsyncUpdatesArray(Array<std::pair<int, int>, CriticalSection>& asyncUpdateArray)
{
    asyncUpdateArray.swapWith(asyncUpdates);
}

void ScopeSyncAsync::setValue(int scopeCode, int newValue)
{
    std::pair<int,int> ctrlMessage = std::make_pair(scopeCode, newValue);
    scopeSyncUpdates.add(ctrlMessage);
}

void ScopeSyncAsync::decDeadTimeCounter(int index)
{
    if (deadTimeCounters[index] > 0)
        deadTimeCounters.set(index, deadTimeCounters[index] - 1);
}
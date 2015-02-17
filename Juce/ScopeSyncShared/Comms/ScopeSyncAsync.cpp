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
#include "../Core/ScopeSyncApplication.h"

ScopeSyncAsync::ScopeSyncAsync()
{
	for (int i = 0; i < ScopeSyncApplication::numScopeSyncParameters + ScopeSyncApplication::numScopeLocalParameters; i++)
        currentValues.add(0);
}

ScopeSyncAsync::~ScopeSyncAsync() {}

void ScopeSyncAsync::handleUpdate(int* asyncValues, bool initialise)
{
    for (int i = 0; i < ScopeSyncApplication::numScopeSyncParameters + ScopeSyncApplication::numScopeLocalParameters; i++)
    {
        if (scopeSyncUpdates.contains(i))
        {
			// We have an update from the ScopeSync system, so let's process it
			int newValue = scopeSyncUpdates[i];
            
			asyncValues[i] = newValue;
			currentValues.set(i, newValue);

			continue;
        }
		
		if (!initialise && ScopeSyncApplication::getPerformanceMode() == 1 && i < ScopeSyncApplication::numScopeSyncParameters)
		{
			// Reading the async input for ScopeSync parameters is switched off, so just grab
			// the current value
			asyncValues[i] = currentValues[i];
		}
		else
		{
			// There was no update from the ScopeSync system, so let's look to see whether the value
			// has changed since last time
			int newValue = asyncValues[i];
        
			if (newValue != currentValues[i] || initialise)
			{
				// Value has changed, or we're initialising, so let's put it in the set to pass back to
				// the ScopeSync system
				asyncUpdates.set(i, newValue);
				currentValues.set(i, newValue);
			}
		}
    }

	scopeSyncUpdates.clear();
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

void ScopeSyncAsync::getAsyncUpdates(HashMap<int, int, DefaultHashFunctions, CriticalSection>& targetHashMap)
{
    targetHashMap.swapWith(asyncUpdates);
}

void ScopeSyncAsync::setValue(int scopeCode, int newValue)
{
    scopeSyncUpdates.set(scopeCode, newValue);
}

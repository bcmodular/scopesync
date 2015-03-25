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

void ScopeSyncAsync::handleUpdate(int* asyncValues, bool initialise, bool performanceMode)
{
    for (int i = 0; i < ScopeSyncApplication::numScopeSyncParameters + ScopeSyncApplication::numScopeLocalParameters; i++)
    {
		// DBG("ScopeSyncAsync::handleUpdate - current value for param " + String(i) + " is: " + String(currentValues[i]));

        if (scopeSyncUpdates.contains(i))
        {
			// We have an update from the ScopeSync system, so let's process it
			int newValue = scopeSyncUpdates[i];
            // DBG("ScopeSyncAsync::handleUpdate - processing param " + String(i) + " from scopeSyncUpdates, with value: " + String(newValue));
				
			asyncValues[i] = newValue;
			currentValues.set(i, newValue);

			continue;
        }
		
		if (performanceMode && i < ScopeSyncApplication::numScopeSyncParameters)
		{
			// We're in performance mode, so overwrite the inputs with the current values
			asyncValues[i] = currentValues[i];
		}
		
		// There was no update from the ScopeSync system, so let's look to see whether the value
		// has changed since last time, or initialise them all if we've just loaded a configuration
		int newValue = asyncValues[i];
        
		if (newValue != currentValues[i] || initialise)
		{
			// Value has changed, or we're initialising, so let's put it in the set to pass back to
			// the ScopeSync system
			// DBG("ScopeSyncAsync::handleUpdate - adding param " + String(i) + " to asyncUpdates, with value: " + String(newValue) + " (initialising: " + String(initialise) + ")");
			asyncUpdates.set(i, newValue);
			currentValues.set(i, newValue);
		}
    }

	scopeSyncUpdates.clear();
}

void ScopeSyncAsync::getAsyncUpdates(HashMap<int, int, DefaultHashFunctions, CriticalSection>& targetHashMap)
{
	// DBG("ScopeSyncAsync::getAsyncUpdates");
    targetHashMap.swapWith(asyncUpdates);
}

void ScopeSyncAsync::setValue(int scopeCode, int newValue)
{
	// DBG("ScopeSyncAsync::setValue - Adding scopeSyncUpdate for scopeCode: " + String(scopeCode) + " value: " + String(newValue));
    scopeSyncUpdates.set(scopeCode, newValue);
}

void ScopeSyncAsync::snapshot()
{
    for (int i = 0; i < ScopeSyncApplication::numScopeSyncParameters + ScopeSyncApplication::numScopeLocalParameters; i++)
    {
		DBG("ScopeSyncAsync::snapshot Adding param " + String(i) + " to the hashmap with value: " + String(currentValues[i]));
		asyncUpdates.set(i, currentValues[i]);
    }
}
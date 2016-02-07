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

ScopeSyncAsync::ScopeSyncAsync()
{
    initialiseScopeParameters = true;

    for (int i = 0; i < ScopeFXParameterDefinitions::numParameters; i++)
        currentValues[i].store(0);
}

ScopeSyncAsync::~ScopeSyncAsync() {}

void ScopeSyncAsync::handleUpdate(int* asyncValues, int* prevValues, bool performanceMode)
{
    for (int i = 0; i < ScopeFXParameterDefinitions::numParameters; i++)
    {
        if (performanceMode)
        {
            // If we're in performance mode overwrite the inputs with the current values
            asyncValues[i] = currentValues[i];
            prevValues[i]  = currentValues[i];
        }
        else if (asyncValues[i] != prevValues[i] || initialiseScopeParameters)
        {
            // If the value hasn't been changed by ScopeSync in the meantime, update
            if (currentValues[i].compare_exchange_strong(prevValues[i], asyncValues[i]))
            {
                // Successfully changed, so add this into the update map
                DBG("ScopeSyncAsync::handleUpdate - updated value for parameter: " + String(i) + ", new value: " + String(currentValues[i]));
                asyncUpdates.set(i, currentValues[i]);
                prevValues[i] = currentValues[i];
            }
        }
    }

    initialiseScopeParameters = false;
}

void ScopeSyncAsync::getAsyncUpdates(HashMap<int, int, DefaultHashFunctions, CriticalSection>& targetHashMap)
{
	// DBG("ScopeSyncAsync::getAsyncUpdates");
    targetHashMap.swapWith(asyncUpdates);
}

void ScopeSyncAsync::setValue(int scopeCode, int newValue)
{
    currentValues[scopeCode].store(newValue);
}

void ScopeSyncAsync::snapshot()
{
    for (int i = 0; i < ScopeFXParameterDefinitions::numParameters; i++)
    {
		DBG("ScopeSyncAsync::snapshot Adding param " + String(i) + " to the hashmap with value: " + String(currentValues[i]));
		asyncUpdates.set(i, currentValues[i]);
    }
}

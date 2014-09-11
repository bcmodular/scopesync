/**
 * Main ScopeSync Application Singleton class. Handles global/static
 * aspects, such as Application Commands etc.
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

#include "ScopeSyncApplication.h"
#include "Global.h"
#include "ScopeSync.h"

juce_ImplementSingleton (ScopeSyncApplication)

Array<ScopeSync*> ScopeSyncApplication::scopeSyncInstances;

ScopeSyncApplication::ScopeSyncApplication() {}

ScopeSyncApplication::~ScopeSyncApplication() {}

void ScopeSyncApplication::reloadAllGUIs()
{
    for (int i = 0; i < scopeSyncInstances.size(); i++)
    {
        scopeSyncInstances[i]->setGUIReload(true);
    }
}

ScopeSyncApplication& ScopeSyncApplication::getApp()
{
    ScopeSyncApplication* const app = ScopeSyncApplication::getInstance();
    jassert (app != nullptr);
    return *app;
}

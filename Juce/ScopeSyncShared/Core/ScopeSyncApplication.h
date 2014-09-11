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

#ifndef SCOPESYNCAPPLICATION_H_INCLUDED
#define SCOPESYNCAPPLICATION_H_INCLUDED

#include <JuceHeader.h>
class ScopeSync;

class ScopeSyncApplication
{
public:
    ScopeSyncApplication();
    ~ScopeSyncApplication();

    static ScopeSyncApplication& ScopeSyncApplication::getApp();
    
    static const int numScopeSyncParameters = 128;
    static const int numScopeLocalParameters = 16;

    /* ============================ Enumerations ============================== */
    enum AppContext { plugin, scopefx }; // Contexts under which the app may be running

#ifndef __DLL_EFFECT__
    static const AppContext appContext = plugin;
#else
    static const AppContext appContext = scopefx;
#endif // __DLL_EFFECT__

    static const bool inPluginContext()  { return (appContext == plugin) ? true : false; };
    static const bool inScopeFXContext() { return (appContext == scopefx) ? true : false; };

    static void registerScopeSyncInstance(ScopeSync* scopeSync) { scopeSyncInstances.add(scopeSync); };
    static void removeScopeSyncInstance(ScopeSync* scopeSync) { scopeSyncInstances.removeAllInstancesOf(scopeSync); };
    static int  getNumScopeSyncInstances() { return scopeSyncInstances.size(); };
    static void reloadAllGUIs();
    
    juce_DeclareSingleton(ScopeSyncApplication, false)

private:
    
    static Array<ScopeSync*> scopeSyncInstances; // Tracks instances of the DLL, so Juce can be shutdown when no more remain

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeSyncApplication)
};

#endif  // SCOPESYNCAPPLICATION_H_INCLUDED

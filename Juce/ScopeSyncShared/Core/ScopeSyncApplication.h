/**
 * ScopeSync Application namespace. Home for global/static
 * functions etc.
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

namespace ScopeSyncApplication
{
    const int numScopeSyncParameters  = 128;
    const int numScopeLocalParameters = 16;

    /* ============================ Enumerations ============================== */
    enum AppContext { plugin, scopefx }; // Contexts under which the app may be running

#ifndef __DLL_EFFECT__
    const AppContext appContext = plugin;
#else
    const AppContext appContext = scopefx;
#endif // __DLL_EFFECT__

    const bool inPluginContext();
    const bool inScopeFXContext();
};

#endif  // SCOPESYNCAPPLICATION_H_INCLUDED

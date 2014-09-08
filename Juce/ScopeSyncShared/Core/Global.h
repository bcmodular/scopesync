/**
 * Static Identifiers used by ScopeSync
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

#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#include <JuceHeader.h>

namespace Ids
{
    #define DECLARE_ID(name) const Identifier name (#name)

    DECLARE_ID(configuration);
    DECLARE_ID(name);
    DECLARE_ID(readOnly);
    DECLARE_ID(layoutFilename);
    DECLARE_ID(hostParameters);
    DECLARE_ID(scopeParameters);
    DECLARE_ID(mapping);
    DECLARE_ID(parameter);
    DECLARE_ID(shortDescription);
    DECLARE_ID(fullDescription);
    DECLARE_ID(scopeSync);
    DECLARE_ID(scopeLocal);
    DECLARE_ID(uiSuffix);
    DECLARE_ID(scopeRangeMin);
    DECLARE_ID(scopeRangeMax);
    DECLARE_ID(scopeRangeMinFlt);
    DECLARE_ID(scopeRangeMaxFlt);
    DECLARE_ID(scopeDBRef);
    DECLARE_ID(uiRangeMin);
    DECLARE_ID(uiRangeMax);
    DECLARE_ID(uiRangeInterval);
    DECLARE_ID(uiResetValue);
    DECLARE_ID(uiSkewFactor);
    DECLARE_ID(skewUIOnly);
    DECLARE_ID(valueType);
    DECLARE_ID(settings);
    DECLARE_ID(setting);
    DECLARE_ID(value);
    DECLARE_ID(layout);

    DECLARE_ID(parameterName);
    DECLARE_ID(mapTo);
    DECLARE_ID(type);
    DECLARE_ID(displayType);
    DECLARE_ID(customDisplay);
    DECLARE_ID(settingDown);
    DECLARE_ID(settingUp);
    DECLARE_ID(sliders);
    DECLARE_ID(labels);
    DECLARE_ID(comboBoxes);
    DECLARE_ID(textButtons);
    DECLARE_ID(tabbedComponents);
    DECLARE_ID(slider);
    DECLARE_ID(label);
    DECLARE_ID(comboBox);
    DECLARE_ID(textButton);
    DECLARE_ID(tabbedComponent);
    DECLARE_ID(radioGroup);

    #undef DECLARE_ID
}

#include "../Components/UserSettings.h"

namespace ScopeSyncApplication
{
    static const int numScopeSyncParameters  = 128;
    static const int numScopeLocalParameters = 16;

    /* ============================ Enumerations ============================== */
    enum AppContext {plugin, scopefx}; // Contexts under which the app may be running
    
#ifndef __DLL_EFFECT__
    const AppContext appContext = plugin;
#else
    const AppContext appContext = scopefx;
#endif // __DLL_EFFECT__

    inline const bool inPluginContext()  { return (appContext == plugin)  ? true : false; };
    inline const bool inScopeFXContext() { return (appContext == scopefx) ? true : false; };

    inline void showUserSettings(int posX, int posY)
    {
        UserSettings* userSettings = UserSettings::getInstance();
        userSettings->setOpaque(true);
        userSettings->setVisible(true);
        
        userSettings->setBounds(posX, posY, 400, 200);
        
        userSettings->addToDesktop(ComponentPeer::windowHasTitleBar | ComponentPeer::windowHasCloseButton | ComponentPeer::windowHasDropShadow, nullptr);
        userSettings->setAlwaysOnTop(true);
        userSettings->toFront(true);
    }
    
    inline void hideUserSettings()
    {
        UserSettings::getInstance()->removeFromDesktop();
    }
}

#endif // GLOBAL_H_INCLUDED

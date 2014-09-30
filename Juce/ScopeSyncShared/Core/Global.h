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
    DECLARE_ID(uiSkewMidpoint);
    DECLARE_ID(skewUIOnly);
    DECLARE_ID(valueType);
    DECLARE_ID(settings);
    DECLARE_ID(setting);
    DECLARE_ID(value);
    DECLARE_ID(intValue);
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

    DECLARE_ID(ID);
    DECLARE_ID(clipboard);
    DECLARE_ID(parameterClipboard);

    #undef DECLARE_ID
}

namespace CommandIDs
{
    enum
    {
        undo                     = 0x200100,
        redo                     = 0x200110,
        deleteItems              = 0x200120,
        
        saveConfig               = 0x200300,
        saveConfigAs             = 0x200310,
        applyConfigChanges       = 0x200320,
        discardConfigChanges     = 0x200330,
        closeConfig              = 0x200340,
        focusOnPanel             = 0x200350,
        
        copyParameter            = 0x200400,
        pasteParameter           = 0x200410,
        deleteParameter          = 0x200420,
        addParameter             = 0x200430,
        
        copyMapping              = 0x200500,
        pasteMapping             = 0x200510,
        deleteMapping            = 0x200520,
        addMapping               = 0x200530,
        
        addSettings              = 0x200600,
        removeSettings           = 0x200610,
        autoFill                 = 0x200620,
        moveUp                   = 0x200630,
        moveDown                 = 0x200640
    };
}

namespace CommandCategories
{
    static const char* const general   = "General";
    static const char* const configmgr = "Configuration Manager";
}

#endif // GLOBAL_H_INCLUDED

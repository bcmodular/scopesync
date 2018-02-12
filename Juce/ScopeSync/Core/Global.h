/**
 * Static Identifiers used by ScopeSync
 *
 *  (C) Copyright 2014 bcmodular (http://www.bcmodular.co.uk/)
 *
 * This file is part of ScopeSync.
 *
 * ScopeSync is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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
    DECLARE_ID(presets);
    DECLARE_ID(preset);
    DECLARE_ID(name);
    DECLARE_ID(readOnly);
    DECLARE_ID(layoutFilename);
    DECLARE_ID(parameters);
    DECLARE_ID(mapping);
    DECLARE_ID(parameter);
    DECLARE_ID(shortDescription);
    DECLARE_ID(fullDescription);
	DECLARE_ID(uiSuffix);
	DECLARE_ID(scopeDeviceUID);
	DECLARE_ID(scopeParamGroup);
	DECLARE_ID(scopeParamId);
	DECLARE_ID(scopeCode);
    DECLARE_ID(scopeRangeMin);
    DECLARE_ID(scopeRangeMax);
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
    DECLARE_ID(UID);
    DECLARE_ID(clipboard);
    DECLARE_ID(parameterClipboard);

    DECLARE_ID(styleOverrides);
    DECLARE_ID(widgetTemplateId);
    DECLARE_ID(lookAndFeelId);
    DECLARE_ID(useColourOverrides);
    DECLARE_ID(fillColour);
    DECLARE_ID(lineColour);
    DECLARE_ID(fillColour2);
    DECLARE_ID(lineColour2);
    DECLARE_ID(components);
    DECLARE_ID(component);
    DECLARE_ID(tabs);
    DECLARE_ID(none);

    // File Library attributes
    DECLARE_ID(fileLocations);
    DECLARE_ID(layoutLibrary);
    DECLARE_ID(configurationLibrary);
    DECLARE_ID(presetLibrary);
    DECLARE_ID(location);

    // Configuration XML top-level attribute names
    DECLARE_ID(librarySet);
    DECLARE_ID(layoutName);
    DECLARE_ID(layoutLibrarySet);
    DECLARE_ID(excludeFromChooser);
    DECLARE_ID(filePath);
    DECLARE_ID(fileName);
    DECLARE_ID(mruTime);
    
    // Layout XML top-level attribute names
    DECLARE_ID(folder);
    DECLARE_ID(libraryset);
    DECLARE_ID(author);
    DECLARE_ID(excludefromchooser);
    DECLARE_ID(numbuttons);
    DECLARE_ID(numButtons);
    DECLARE_ID(numencoders);
    DECLARE_ID(numEncoders);
    DECLARE_ID(numfaders);
    DECLARE_ID(numFaders);
    DECLARE_ID(panelwidth);
    DECLARE_ID(panelWidth);
    DECLARE_ID(panelheight); 
    DECLARE_ID(panelHeight); 
    DECLARE_ID(numparameters);
    DECLARE_ID(numParameters);
    DECLARE_ID(thumbnail);
    DECLARE_ID(blurb);

    // Preset XML top-level attribute names
    DECLARE_ID(presetFile);
    DECLARE_ID(presetFileName);
    DECLARE_ID(presetFileLibrarySet);
    DECLARE_ID(presetFileAuthor);
    DECLARE_ID(presetFileBlurb);

	// Deprecated in 1.0.2
	DECLARE_ID(scopeSync);
	DECLARE_ID(scopeLocal);
	DECLARE_ID(hostParameters);
	DECLARE_ID(scopeParameters);

    #undef DECLARE_ID
}

namespace CommandIDs
{
    enum
    {
        undo                        = 0x200100,
        redo                        = 0x200110,
        cancel                      = 0x200120,
        showAboutBox                = 0x200130,
                                    
        saveConfig                  = 0x200300,
        saveConfigAs                = 0x200310,
        applyConfigChanges          = 0x200320,
        discardConfigChanges        = 0x200330,
        closeConfig                 = 0x200340,
        focusOnPanel                = 0x200350,
        showHideEditToolbar         = 0x200360,
        addConfig                   = 0x200370,
                                  
        copyItem                    = 0x200400,
        pasteItem                   = 0x200410,
        deleteItems                 = 0x200420,
        addItem                     = 0x200430,
        addItemFromClipboard        = 0x200440,
                                    
        addParameter                = 0x200500,
        addParameterFromClipboard   = 0x200510,
        deleteMapping               = 0x200520,
        editMapping                 = 0x200530,
        copyParameter               = 0x200540,
        pasteParameter              = 0x200550,
        editParameter               = 0x200560,
        deleteParameter             = 0x200570,
        chooseSelectedPreset        = 0x200580,
		copyOSCPath                 = 0x200590,

        addSettings                 = 0x200600,
        removeSettings              = 0x200610,
        autoFill                    = 0x200620,
        moveUp                      = 0x200630,
        moveDown                    = 0x200640,
                                    
        editItem                    = 0x200700,
        editMappedItem              = 0x200710,
                                    
        overrideStyle               = 0x200720,
        clearStyleOverride          = 0x200730,
        copyStyleOverride           = 0x200740,
        copyStyleOverrideToAll      = 0x200745,
        pasteStyleOverride          = 0x200750,
                                  
        editFileLocations           = 0x200800,
        addFileLocation             = 0x200810,
        removeFileLocations         = 0x200820,
        rebuildFileLibrary          = 0x200830,
        chooseSelectedLayout        = 0x200840,
        chooseSelectedConfiguration = 0x200850,
        unloadConfiguration         = 0x200860,
                                  
        snapshot                    = 0x200900,
        snapshotAll                 = 0x200905,
        showUserSettings            = 0x200910,
        showConfigurationManager    = 0x200920,
        chooseConfiguration         = 0x200930,
        reloadSavedConfiguration    = 0x200940,

        addPresetFile               = 0x201000,
        savePresetFile              = 0x201010,
        savePresetFileAs            = 0x201020,
        applyPresetFileChanges      = 0x201030,
        discardPresetFileChanges    = 0x201040,
        closePresetFile             = 0x201050,
        chooseSelectedPresetFile    = 0x201060,
        showPresetManager           = 0x201070,
        choosePresetFile            = 0x201080
    };
}

namespace CommandCategories
{
    static const char* const general      = "General";
    static const char* const configmgr    = "Configuration Manager";
    static const char* const usersettings = "User Settings";
}

#endif // GLOBAL_H_INCLUDED

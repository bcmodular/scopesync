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

#include "ScopeSyncApplication.h"
#include "Global.h"

const bool ScopeSyncApplication::inPluginContext() 
{ 
    return (appContext == plugin) ? true : false;
};

const bool ScopeSyncApplication::inScopeFXContext() 
{ 
    return (appContext == scopefx) ? true : false; 
};

void ScopeSyncApplication::setPerformanceMode(int newValue)
{
	performanceMode = newValue;
}

int ScopeSyncApplication::getPerformanceMode()
{
	int perfModeValue = performanceMode.getValue();

	return perfModeValue;
}

void ScopeSyncApplication::referToPerformanceMode(Value& valueToLink)
{
	valueToLink.referTo(performanceMode);
}

ScopeSyncClipboard::ScopeSyncClipboard()
{
    clipboard = ValueTree();
}

ScopeSyncClipboard::~ScopeSyncClipboard()
{
    clipboard = ValueTree();
}

void ScopeSyncClipboard::copy(const ValueTree& source)
{
    clipboard = source.createCopy();
}

juce_ImplementSingleton (ParameterClipboard)

void ParameterClipboard::paste(ValueTree& target, UndoManager* undoManager)
{
    if (clipboardIsNotEmpty())
    {
        if (target.hasType(Ids::preset))
            pastePreset(target, undoManager);
        else
            pasteParameter(target, undoManager);
    }
}

void ParameterClipboard::pasteParameter(ValueTree& target, UndoManager* undoManager)
{
    // Backup the parameter's identifying values
    String name       = target.getProperty(Ids::name);
    String shortDesc  = target.getProperty(Ids::shortDescription);
    String fullDesc   = target.getProperty(Ids::fullDescription);
    int    scopeSync  = target.getProperty(Ids::scopeSync);
    int    scopeLocal = target.getProperty(Ids::scopeLocal);

    target.copyPropertiesFrom(clipboard, undoManager);

    // Get rid of any stray Preset properties
    target.removeProperty(Ids::blurb, undoManager);
    target.removeProperty(Ids::presetFileName, undoManager);
    target.removeProperty(Ids::presetFileLibrarySet, undoManager);
    target.removeProperty(Ids::presetFileAuthor, undoManager);
    target.removeProperty(Ids::presetFileBlurb, undoManager);
    target.removeProperty(Ids::filePath, undoManager);
    target.removeProperty(Ids::fileName, undoManager);

    target.removeAllChildren(undoManager);

    ValueTree settings = clipboard.getChildWithName(Ids::settings).createCopy();
        
    if (settings.isValid())
        target.addChild(settings, -1, undoManager);

    // Restore the parameter's identifying values
    target.setProperty(Ids::name,             name,       undoManager);
    target.setProperty(Ids::shortDescription, shortDesc,  undoManager);
    target.setProperty(Ids::fullDescription,  fullDesc,   undoManager);
    target.setProperty(Ids::scopeSync,        scopeSync,  undoManager);
    target.setProperty(Ids::scopeLocal,       scopeLocal, undoManager);
}

void ParameterClipboard::pastePreset(ValueTree& target, UndoManager* undoManager)
{
    // Backup the Preset's identifying values
    String name  = target.getProperty(Ids::name);
    String blurb = target.getProperty(Ids::blurb);
    
    target.copyPropertiesFrom(clipboard, undoManager);

    // Get rid of any stray Parameter properties
    target.removeProperty(Ids::shortDescription, undoManager);
    target.removeProperty(Ids::fullDescription, undoManager);
    target.removeProperty(Ids::scopeSync, undoManager);
    target.removeProperty(Ids::scopeLocal, undoManager);

    target.removeAllChildren(undoManager);

    ValueTree settings = clipboard.getChildWithName(Ids::settings).createCopy();
        
    if (settings.isValid())
        target.addChild(settings, -1, undoManager);

    // Restore the parameter's identifying values
    target.setProperty(Ids::name,  name,  undoManager);
    target.setProperty(Ids::blurb, blurb, undoManager);
}

juce_ImplementSingleton (StyleOverrideClipboard)

void StyleOverrideClipboard::paste(ValueTree& target, UndoManager* undoManager)
{
    if (clipboardIsNotEmpty())
    {
        String componentName = target.getProperty(Ids::name);
        
        target.copyPropertiesFrom(clipboard, undoManager);

        target.setProperty(Ids::name, componentName, undoManager);
    }
}
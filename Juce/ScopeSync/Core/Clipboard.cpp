/**
 * Clipboards to support copy/paste features
 *
 *  (C) Copyright 2018 bcmodular (http://www.bcmodular.co.uk/)
 *
 * This file is part of ScopeSync.
 *
 * ScopeSync is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contributors:
 *  Simon Russell
 *  Will Ellis
 *  Jessica Brandt
 */

#include "Clipboard.h"
#include "Global.h"

Clipboard::Clipboard()
{
    clipboard = ValueTree();
}

Clipboard::~Clipboard()
{
    clipboard = ValueTree();
}

void Clipboard::copy(const ValueTree& source)
{
    clipboard = source.createCopy();
}

bool Clipboard::clipboardIsNotEmpty() const
{
    return clipboard.isValid();
}

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

void ParameterClipboard::pasteParameter(ValueTree& target, UndoManager* undoManager) const
{
    // Backup the parameter's identifying values
    String name            = target.getProperty(Ids::name);
    String shortDesc       = target.getProperty(Ids::shortDescription);
    String fullDesc        = target.getProperty(Ids::fullDescription);
    String scopeParamGroup = target.getProperty(Ids::scopeParamGroup);
	String scopeParamId    = target.getProperty(Ids::scopeParamId);
    
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
    target.setProperty(Ids::scopeParamGroup,  scopeParamGroup, undoManager);
	target.setProperty(Ids::scopeParamId,     scopeParamId, undoManager);
}

void ParameterClipboard::pastePreset(ValueTree& target, UndoManager* undoManager) const
{
    // Backup the Preset's identifying values
    String name  = target.getProperty(Ids::name);
    String blurb = target.getProperty(Ids::blurb);
    
    target.copyPropertiesFrom(clipboard, undoManager);

    // Get rid of any stray Parameter properties
    target.removeProperty(Ids::shortDescription, undoManager);
    target.removeProperty(Ids::fullDescription,  undoManager);
    target.removeProperty(Ids::scopeCode,        undoManager);
	target.removeProperty(Ids::scopeParamGroup,  undoManager);
	target.removeProperty(Ids::scopeParamId,     undoManager);
    
    target.removeAllChildren(undoManager);

    ValueTree settings = clipboard.getChildWithName(Ids::settings).createCopy();
        
    if (settings.isValid())
        target.addChild(settings, -1, undoManager);

    // Restore the parameter's identifying values
    target.setProperty(Ids::name,  name,  undoManager);
    target.setProperty(Ids::blurb, blurb, undoManager);
}

void StyleOverrideClipboard::paste(ValueTree& target, UndoManager* undoManager)
{
    if (clipboardIsNotEmpty())
    {
        String componentName = target.getProperty(Ids::name);
        
        target.copyPropertiesFrom(clipboard, undoManager);

        target.setProperty(Ids::name, componentName, undoManager);
    }
}

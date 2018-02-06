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

#ifndef CLIPBOARD_H_INCLUDED
#define CLIPBOARD_H_INCLUDED

#include <JuceHeader.h>

class Clipboard
{
public:
    Clipboard();
    virtual ~Clipboard();

    void copy (const ValueTree& source);
    virtual void paste(ValueTree& target, UndoManager* undoManager) = 0;

    bool clipboardIsNotEmpty() const;;

protected:
    ValueTree clipboard;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Clipboard)
};

class ParameterClipboard : public Clipboard
{
public:
    ParameterClipboard() = default;

    void paste(ValueTree& target, UndoManager* undoManager) override;

private:
    void pastePreset(ValueTree& target, UndoManager* undoManager) const;
    void pasteParameter(ValueTree& target, UndoManager* undoManager) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterClipboard)
};

class StyleOverrideClipboard : public Clipboard
{
public:
    StyleOverrideClipboard() = default;

    void paste(ValueTree& target, UndoManager* undoManager) override;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StyleOverrideClipboard)
};

#endif  // CLIPBOARD_H_INCLUDED

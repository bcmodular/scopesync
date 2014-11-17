/**
 * Settings Table, used within the Parameter Configuration Panels
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

#ifndef SETTINGSTABLE_H_INCLUDED
#define SETTINGSTABLE_H_INCLUDED
#include <JuceHeader.h>
#include "ConfigurationManager.h"
#include "../Core/Global.h"

/* =========================================================================
 * SettingsTable: TableListBox for managing Parameter Settings
 */
class SettingsTable : public  Component,
                      private TableListBoxModel,
                      private ValueTree::Listener,
                      public  ApplicationCommandTarget
{
public:
    SettingsTable(const ValueTree& vt, UndoManager& um, ApplicationCommandManager* acm,
                  ValueTree& parameter, ApplicationCommandTarget* act);
    ~SettingsTable();

    void       resized() override;
    
    int        getNumRows();
    void       paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void       paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;
    void       sortOrderChanged(int newSortColumnId, bool isForwards) override;
    void       selectedRowsChanged(int lastRowSelected) override;
    void       backgroundClicked(const MouseEvent&) override;
    void       deleteKeyPressed(int) override;

	// Overridden methods for ValueTree::Listener
    void valueTreePropertyChanged(ValueTree& /* treeWhosePropertyHasChanged */, const Identifier& /* property */) override { table.updateContent(); };
    void valueTreeChildAdded(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenAdded */) override { table.updateContent(); };
    void valueTreeChildRemoved(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenRemoved */) override { table.updateContent(); };
    void valueTreeChildOrderChanged(ValueTree& /* parentTreeWhoseChildrenHaveMoved */) override { table.updateContent(); };
    void valueTreeParentChanged(ValueTree& /* treeWhoseParentHasChanged */) override { table.updateContent(); };

private:
    TableListBox   table;
    Font           font;
    ValueTree      tree;
    UndoManager&   undoManager;
    ApplicationCommandManager* commandManager;
    Label          numSettingsTextLabel;
    Label          numSettingsToAddLabel;
    TextButton     addSettingsButton;
    TextButton     removeSettingsButton;
    TextButton     autoFillValuesButton;
    TextButton     moveUpButton;
    TextButton     moveDownButton;

    Value        numSettingsToAdd;
    ValueTree    parameter;
	ApplicationCommandTarget* parentCommandTarget;

    class LabelComp;
    friend class LabelComp;

    void textWasEdited();
    
	void undo();
	void redo();
	void addSettings();
    void removeSettings();
    void updateParameterRanges();
    void autoFill();
    void moveSettings(bool moveUp);

    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsTable)
};

#endif  // SETTINGSTABLE_H_INCLUDED

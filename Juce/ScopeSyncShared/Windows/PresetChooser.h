/**
 * Table for choosing a Preset
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

#ifndef PRESETCHOOSER_H_INCLUDED
#define PRESETCHOOSER_H_INCLUDED

#include "../Core/Global.h"
class ScopeSync;

/* =========================================================================
 * PresetChooser: Table view for selecting a Preset
 */
class PresetChooser : public  Component,
                      private TableListBoxModel,
                      private ValueTree::Listener,
                      public  ApplicationCommandTarget,
                      public  ChangeListener,
                      public  ChangeBroadcaster
{
public:
    PresetChooser(ValueTree& param, ScopeSync& ss, ApplicationCommandManager* acm, UndoManager& um);
    ~PresetChooser();

    void changeListenerCallback(ChangeBroadcaster* source) override;
   
    void paint(Graphics& g) override;
    void resized() override;
    
    int  getNumRows();
    void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    void sortOrderChanged(int newSortColumnId, bool isForwards) override;
    void selectedRowsChanged(int lastRowSelected) override;
    void backgroundClicked(const MouseEvent&) override;
    void cellDoubleClicked (int rowNumber, int columnId, const MouseEvent& e);
    void returnKeyPressed(int lastRowSelected);
    
    // Overridden methods for ValueTree::Listener
    void valueTreePropertyChanged(ValueTree& /* treeWhosePropertyHasChanged */, const Identifier& /* property */) override { table.updateContent(); };
    void valueTreeChildAdded(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenAdded */) override { table.updateContent(); };
    void valueTreeChildRemoved(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenRemoved */) override { table.updateContent(); };
    void valueTreeChildOrderChanged(ValueTree& /* parentTreeWhoseChildrenHaveMoved */) override { table.updateContent(); };
    void valueTreeParentChanged(ValueTree& /* treeWhoseParentHasChanged */) override { table.updateContent(); };

private:
    TableListBox table;
    Font         font;
    ValueTree    tree;
    ValueTree    viewTree;
    ScopeSync&   scopeSync;
    UndoManager& undoManager;
    ValueTree    parameter;
    ApplicationCommandManager* commandManager;
    TextButton   chooseButton;
    TextButton   rebuildLibraryButton;
    TextButton   editLocationsButton;
    TextButton   presetManagerButton;
    Label        blurb;
    Label        fileNameLabel;
    
    void editFileLocations();
    void chooseSelectedPreset();
    void showPresetManager();

    void rebuildFileLibrary();
    void removePresetFileEntries();
    void attachToTree();
    
    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetChooser)
};

#endif  // PRESETCHOOSER_H_INCLUDED

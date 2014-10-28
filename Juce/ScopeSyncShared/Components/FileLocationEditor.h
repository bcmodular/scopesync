/**
 * Table for setting up file locations
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

#ifndef FILELOCATIONSEDITOR_H_INCLUDED
#define FILELOCATIONSEDITOR_H_INCLUDED
#include <JuceHeader.h>
#include "../Core/Global.h"

class FileLocationEditorWindow : public DocumentWindow,
                                 public ChangeBroadcaster
{
public:
    FileLocationEditorWindow(int posX, int posY,
                             ApplicationCommandManager* acm, 
                             UndoManager& um);
    ~FileLocationEditorWindow();

    ValueTree getFileLocations();
    bool      locationsHaveChanged();

private:
    void closeButtonPressed() override;
    void restoreWindowPosition(int posX, int posY);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileLocationEditorWindow);
};


class FileLocationEditor : public  Component,
                           private TableListBoxModel,
                           private ValueTree::Listener,
                           public  ApplicationCommandTarget
{
public:
    FileLocationEditor(UndoManager& um, ApplicationCommandManager* acm);
    ~FileLocationEditor();

    ValueTree getFileLocations();
    bool      locationsHaveChanged() { return locationsChanged; }

    void paint(Graphics& g) override;
    void resized() override;
    
    int        getNumRows();
    void       paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void       paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;
    void       sortOrderChanged(int /* newSortColumnId */, bool /* isForwards */) override {};
    void       selectedRowsChanged(int /* lastRowSelected */) override {};
    void       backgroundClicked(const MouseEvent&) override;
    void       deleteKeyPressed(int) override;

    // Overridden methods for ValueTree::Listener
    void valueTreePropertyChanged(ValueTree& /* treeWhosePropertyHasChanged */, const Identifier& /* property */) override
    { 
        table.updateContent(); 
        locationsChanged = true;
    };

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
    Label          sizeWarning;
    TextButton     addFileLocationButton;
    TextButton     removeFileLocationButton;
    //TextButton     moveUpButton;
    //TextButton     moveDownButton;
    TextButton     rebuildButton;
    TextButton     undoButton;
    TextButton     redoButton;

    bool           locationsChanged;
    
    class LabelComp;
    friend class LabelComp;

    class ButtonComp;
    friend class ButtonComp;

    void textWasEdited();
    void addFileLocation();
    void removeFileLocations();
    //void moveFileLocations(bool moveUp);
    void rebuildFileLibrary();

    void undo();
    void redo();

    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileLocationEditor)
};

#endif  // FILELOCATIONSEDITOR_H_INCLUDED

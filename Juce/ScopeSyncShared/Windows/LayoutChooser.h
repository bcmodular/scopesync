/**
 * Table for choosing a Layout to use with a Configuration
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

#ifndef LAYOUTCHOOSER_H_INCLUDED
#define LAYOUTCHOOSER_H_INCLUDED

#include "../Core/Global.h"

/* =========================================================================
 * LayoutChooserWindow: Parent Window for Layout Chooser
 */
class LayoutChooserWindow : public DocumentWindow,
                            public ChangeBroadcaster
{
public:
    LayoutChooserWindow(int posX, int posY,
                        const Value& layoutName,
                        const Value& layoutLocation,
                        ApplicationCommandManager* acm);
    ~LayoutChooserWindow();

private:
    void closeButtonPressed() override;
    void restoreWindowPosition(int posX, int posY);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LayoutChooserWindow);
};

/* =========================================================================
 * LayoutChooser: Table view for selecting a layout to use with a configuration
 */
class LayoutChooser : public  Component,
                      private TableListBoxModel,
                      private ValueTree::Listener,
                      public  ApplicationCommandTarget,
                      public  ChangeListener
{
public:
    LayoutChooser(const Value& layoutName,
                  const Value& layoutLocation,
                  ApplicationCommandManager* acm);
    ~LayoutChooser();

    void changeListenerCallback(ChangeBroadcaster* source);
   
    void paint(Graphics& g) override;
    void resized() override;
    
    int        getNumRows();
    void       paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void       paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    void       sortOrderChanged(int newSortColumnId, bool isForwards) override;
    void       selectedRowsChanged(int lastRowSelected) override;
    void       backgroundClicked(const MouseEvent&) override;
    void       cellDoubleClicked (int rowNumber, int columnId, const MouseEvent& e);
    void       returnKeyPressed(int lastRowSelected);
    
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
    Value        name;
    Value        librarySet;
    ApplicationCommandManager* commandManager;
    TextButton   chooseButton;
    TextButton   rebuildLibraryButton;
    TextButton   editLocationsButton;
    Label        blurb;

    Image  thumbImage;
    bool   useImageCache;

    void editFileLocations();
    void chooseSelectedLayout();
    void rebuildFileLibrary();
    void attachToTree();
    void closeWindow();
    void removeExcludedLayouts();
    void selectCurrentLayout();

    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayoutChooser)
};

#endif  // LAYOUTCHOOSER_H_INCLUDED

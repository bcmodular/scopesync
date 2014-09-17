/*
  ==============================================================================

    ConfigurationTreeItem.h
    Created: 16 Sep 2014 9:38:20am
    Author:  giles

  ==============================================================================
*/

#ifndef CONFIGURATIONTREEITEM_H_INCLUDED
#define CONFIGURATIONTREEITEM_H_INCLUDED

#include <JuceHeader.h>
#include "../Resources/Icons.h"

class ConfigurationTreeItem  : public TreeViewItem,
                               private ValueTree::Listener
{
public:
    ConfigurationTreeItem (const ValueTree& v, UndoManager& um);

    String getUniqueName() const override;
    bool mightContainSubItems() override;
    void paintItem (Graphics& g, int width, int height) override;
    void itemOpennessChanged (bool isNowOpen) override;
    var getDragSourceDescription() override;
    bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    void itemDropped (const DragAndDropTarget::SourceDetails&, int insertIndex) override;
    
    static void moveItems (TreeView& treeView, const Array<ValueTree>& items,
                           ValueTree newParent, int insertIndex, UndoManager& undoManager);
    static Array<ValueTree> getSelectedTreeViewItems (TreeView& treeView);

private:
    ValueTree    tree;
    UndoManager& undoManager;

    void refreshSubItems();
    void valueTreePropertyChanged (ValueTree&, const Identifier&) override;

    void valueTreeChildAdded (ValueTree& parentTree, ValueTree&) override    { treeChildrenChanged(parentTree); }
    void valueTreeChildRemoved (ValueTree& parentTree, ValueTree&) override  { treeChildrenChanged(parentTree); }
    void valueTreeChildOrderChanged (ValueTree& parentTree) override         { treeChildrenChanged(parentTree); }
    void valueTreeParentChanged (ValueTree&) override {}

    void treeChildrenChanged (const ValueTree& parentTree);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConfigurationTreeItem)
};


#endif  // CONFIGURATIONTREEITEM_H_INCLUDED

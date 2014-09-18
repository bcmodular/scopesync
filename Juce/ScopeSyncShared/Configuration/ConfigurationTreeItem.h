

#ifndef CONFIGURATIONTREEITEM_H_INCLUDED
#define CONFIGURATIONTREEITEM_H_INCLUDED

#include <JuceHeader.h>
#include "../Resources/Icons.h"

class ConfigurationTreeItem  : public  TreeViewItem,
                               private ValueTree::Listener
{
public:
    ConfigurationTreeItem (const ValueTree& v, UndoManager& um);

    String getUniqueName() const override;
    bool mightContainSubItems() override;
    Font getFont() const;
    virtual Icon getIcon() const;
    float getIconSize() const;
    bool isIconCrossedOut() const { return false; }
    void paintContent (Graphics& g, const Rectangle<int>& area);
    void paintOpenCloseButton (Graphics&, const Rectangle<float>& area, Colour backgroundColour, bool isMouseOver) override;
    Component* createItemComponent() override;
    void itemOpennessChanged (bool isNowOpen) override;
    virtual var getDragSourceDescription() override;
    virtual bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    void itemDropped (const DragAndDropTarget::SourceDetails&, int insertIndex) override;
    
    virtual String getDisplayName() const;

    static void moveItems (TreeView& treeView, const Array<ValueTree>& items,
                           ValueTree newParent, int insertIndex, UndoManager& undoManager);
    static Array<ValueTree> getSelectedTreeViewItems (TreeView& treeView);
    
    struct WholeTreeOpennessRestorer   : public OpennessRestorer
    {
        WholeTreeOpennessRestorer (TreeViewItem& item)  : OpennessRestorer (getTopLevelItem (item))
        {}

    private:
        static TreeViewItem& getTopLevelItem (TreeViewItem& item)
        {
            if (TreeViewItem* const p = item.getParentItem())
                return getTopLevelItem (*p);

            return item;
        }
    };

    int textX;

protected:
    Colour    getBackgroundColour() const;
    Colour    getContrastingColour (float contrast) const;
    Colour    getContrastingColour (Colour targetColour, float minContrast) const;
    ValueTree tree;
    
private:
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

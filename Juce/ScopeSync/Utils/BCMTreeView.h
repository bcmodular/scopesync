/*
  ==============================================================================

    BCMTreeView.h
    Created: 2 Nov 2014 12:02:26pm
    Author:  giles

  ==============================================================================
*/

#ifndef BCMTREEVIEW_H_INCLUDED
#define BCMTREEVIEW_H_INCLUDED
#include <JuceHeader.h>
#include "../Resources/Icons.h"
class BCMTreeItem;

class BCMTreeView : public Component,
                    public DragAndDropContainer,
                    public Timer
{
public:
    BCMTreeView(UndoManager& um, BCMTreeItem* root, PropertiesFile& properties);
    ~BCMTreeView();

    void paint (Graphics& g) override;
    void resized() override;
    void saveTreeViewState() const;

    void copyItem() const;
    void pasteItem() const;
    bool canPasteItem() const;

    void addItem() const;
    void addItemFromClipboard() const;
    void deleteSelectedItems() const;
    void changePanel() const;

    void storeSelectedItem(int row);
    void moveToSelectedItem();

    enum ColourIds
    {
        mainBackgroundColourId = 0x2340000,
    };    

private:
    TreeView       tree;
    ScopedPointer<BCMTreeItem> rootItem;
    UndoManager&   undoManager;
    PropertiesFile& properties;

    int storedRow;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BCMTreeView);
};

/* =========================================================================
 * BCMTreeItem: Base class for BCM TreeViewItems
 */
class BCMTreeItem  : public  TreeViewItem,
                     ValueTree::Listener
{
public:
    BCMTreeItem(const ValueTree& v, UndoManager& um, ApplicationCommandManager* acm);
    ~BCMTreeItem();

    String         getUniqueName() const override;
    bool		   mightContainSubItems() override;
    virtual Font   getFont() const;
    virtual Icon   getIcon() const = 0;
    virtual float  getIconSize() const;
    static bool    isIconCrossedOut();
    void           paintContent(Graphics& g, const Rectangle<int>& area) const;
    void           paintOpenCloseButton(Graphics&, const Rectangle<float>& area, Colour backgroundColour, bool isMouseOver) override;
    Component*     createItemComponent() override;
    void           itemOpennessChanged(bool isNowOpen) override;
    var			   getDragSourceDescription() override = 0;
    bool		   isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails) override = 0;
    void           itemDropped(const DragAndDropTarget::SourceDetails&, int insertIndex) override;
    void           itemClicked(const MouseEvent& e) override;
    void           itemSelectionChanged(bool isNowSelected) override;
    void           itemDoubleClicked(const MouseEvent&) override;
    virtual void   changePanel() = 0;
    
    virtual void copyItem() = 0;
    virtual void pasteItem() = 0;
    virtual bool canPasteItem() { return false; }

    virtual void deleteItem() = 0;
    virtual void addItem() = 0;
    virtual void addItemFromClipboard() = 0;
    
    virtual void showPopupMenu() = 0;
    virtual void showMultiSelectionPopupMenu();
    
    void         storeSelectionOnAdd() const;
    void         storeSelectionOnDelete() const;

    static int   getMillisecsAllowedForDragGesture();;
    void         cancelDelayedSelectionTimer();
    
    virtual String getDisplayName() const;

    static void moveItems (TreeView& treeView, const Array<ValueTree>& items,
                           ValueTree newParent, int insertIndex, UndoManager& undoManager);
    static Array<ValueTree> getSelectedTreeViewItems (TreeView& treeView);
    
    struct WholeTreeOpennessRestorer : OpennessRestorer
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
    ApplicationCommandManager* commandManager;
    UndoManager& undoManager;
	SharedResourcePointer<Icons> icons;

private:
    class        ItemSelectionTimer;
    friend class ItemSelectionTimer;
    ScopedPointer<Timer> delayedSelectionTimer;

    WeakReference<BCMTreeItem>::Master masterReference;
    friend class WeakReference<BCMTreeItem>;

    virtual void refreshSubItems() = 0;
    
    /* ================= ValueTree::Listener overrides ================= */
    void valueTreePropertyChanged (ValueTree&, const Identifier&) override;
    void valueTreeChildAdded (ValueTree& parentTree, ValueTree&) override { treeChildrenChanged(parentTree); }
    void valueTreeChildRemoved (ValueTree& parentTree, ValueTree&, int /* oldIndex */) override { treeChildrenChanged(parentTree); }
    void valueTreeChildOrderChanged (ValueTree& parentTree, int /* oldIndex */, int /* newIndex */) override { treeChildrenChanged(parentTree); }
    void valueTreeParentChanged (ValueTree&) override {}

    void treeChildrenChanged (const ValueTree& parentTree);
    void invokeChangePanel();

    void deleteAllSelectedItems() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BCMTreeItem)
};




#endif  // BCMTREEVIEW_H_INCLUDED

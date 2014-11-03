/*
  ==============================================================================

    BCMTreeView.cpp
    Created: 2 Nov 2014 12:02:26pm
    Author:  giles

  ==============================================================================
*/

#include "BCMTreeView.h"
#include "../Core/Global.h"
#include "../Resources/Icons.h"

BCMTreeView::BCMTreeView(UndoManager& um, BCMTreeItem* root, PropertiesFile& props)
    : undoManager(um),
      storedRow(-1),
      properties(props)
{
    addAndMakeVisible(tree);

    tree.setDefaultOpenness(true);
    tree.setMultiSelectEnabled(true);
    rootItem = root;
    tree.setRootItem(rootItem);

    const ScopedPointer<XmlElement> treeOpenness(properties.getXmlValue("openness"));
    
    if (treeOpenness != nullptr)
        tree.restoreOpennessState (*treeOpenness, true);

    tree.setColour(TreeView::backgroundColourId, Colours::darkgrey);
    tree.getViewport()->setScrollBarsShown(true, true, true, true);
}

BCMTreeView::~BCMTreeView()
{
    tree.setRootItem(nullptr);
}

void BCMTreeView::paint(Graphics& g)
{
    g.fillAll(Colour(0xff434343));
}

void BCMTreeView::resized()
{
    Rectangle<int> r(getLocalBounds());

    r.removeFromTop(8);
    r.removeFromLeft(8);
    r.removeFromRight(4);
    tree.setBounds(r);
}

void BCMTreeView::copyItem()
{
    if (tree.getNumSelectedItems() == 1)
        dynamic_cast<BCMTreeItem*>(tree.getSelectedItem(0))->copyItem();
}

void BCMTreeView::pasteItem()
{
    if (tree.getNumSelectedItems() == 1)
        dynamic_cast<BCMTreeItem*>(tree.getSelectedItem(0))->pasteItem();
}

bool BCMTreeView::canPasteItem()
{
    if (tree.getNumSelectedItems() == 1)
        return dynamic_cast<BCMTreeItem*>(tree.getSelectedItem(0))->canPasteItem();
    else
        return false;
}

void BCMTreeView::addItem()
{
    if (tree.getNumSelectedItems() == 1)
        dynamic_cast<BCMTreeItem*>(tree.getSelectedItem(0))->addItem();
}

void BCMTreeView::addItemFromClipboard()
{
    if (tree.getNumSelectedItems() == 1)
        dynamic_cast<BCMTreeItem*>(tree.getSelectedItem(0))->addItemFromClipboard();
}

void BCMTreeView::deleteSelectedItems()
{
    StringArray identifiers;

    for (int i = 0; i < tree.getNumSelectedItems(); i++)
    {
        identifiers.add(tree.getSelectedItem(i)->getItemIdentifierString());
        DBG("BCMTreeView::deleteSelectedItems - added item to delete: " + identifiers[i]);
    }

    for (int i = 0; i < identifiers.size(); i++)
    {
        dynamic_cast<BCMTreeItem*>(tree.findItemFromIdentifierString(identifiers[i]))->deleteItem();
    }
}

void BCMTreeView::changePanel()
{
    BCMTreeItem* treeItem = dynamic_cast<BCMTreeItem*>(tree.getSelectedItem(tree.getNumSelectedItems() - 1));
    
    if (treeItem != nullptr)
        treeItem->changePanel();
}

void BCMTreeView::saveTreeViewState()
{
    const ScopedPointer<XmlElement> opennessState(tree.getOpennessState(true));

    if (opennessState != nullptr)
        properties.setValue("openness", opennessState);
}

void BCMTreeView::storeSelectedItem(int row)
{
    storedRow = row;
}

void BCMTreeView::moveToSelectedItem()
{
    if (storedRow >= 0)
    {
        TreeViewItem* itemToMoveTo = tree.getItemOnRow(storedRow);
            
        if (itemToMoveTo != nullptr)
        {
            itemToMoveTo->setSelected(true, true);     
        }
    }

    storedRow = -1;
}

/* =========================================================================
 * TreeItemComponent: Component to display as the TreeViewItem
 */
class TreeItemComponent : public Component
{
public:
    TreeItemComponent(BCMTreeItem& i) : item (i)
    {
        setInterceptsMouseClicks (false, true);
    }

    void paint (Graphics& g) override
    {
        g.setColour(Colours::black);
        paintIcon(g);
        item.paintContent(g, Rectangle<int>(item.textX, 0, getWidth() - item.textX, getHeight()));
    }

    void paintIcon (Graphics& g)
    {
        item.getIcon().draw(g, Rectangle<float> (4.0f, 2.0f, item.getIconSize(), getHeight() - 4.0f),
                                item.isIconCrossedOut());
    }

    void resized() override { item.textX = (int)item.getIconSize() + 8; }

    BCMTreeItem& item;

private:    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TreeItemComponent);

};

/* =========================================================================
 * BCMTreeItem
 */
BCMTreeItem::BCMTreeItem(const ValueTree& v, UndoManager& um, ApplicationCommandManager* acm)
    : tree(v),
      undoManager(um),
      textX(0),
      commandManager(acm)
{
    tree.addListener(this);
}

BCMTreeItem::~BCMTreeItem()
{
    masterReference.clear();
}

String BCMTreeItem::getDisplayName() const 
{
    String displayName = tree["name"].toString();

    if (displayName.isEmpty())
        displayName = tree.getType().toString();

    return displayName;
}

String BCMTreeItem::getUniqueName() const { return getDisplayName(); }

bool BCMTreeItem::mightContainSubItems() { return tree.getNumChildren() > 0; }

Font BCMTreeItem::getFont() const { return Font (getItemHeight() * 0.7f); }

float BCMTreeItem::getIconSize() const { return jmin (getItemHeight() - 4.0f, 18.0f); }

void BCMTreeItem::paintOpenCloseButton (Graphics& g, const Rectangle<float>& area, Colour backgroundColour, bool isMouseOver)
{
    TreeViewItem::paintOpenCloseButton(g, area, backgroundColour, isMouseOver);
}

Colour BCMTreeItem::getBackgroundColour() const
{
    Colour background(Colours::darkgrey);

    if (isSelected())
        background = background.overlaidWith(getOwnerView()->findColour(TreeView::selectedItemBackgroundColourId));
     
    return background;
}

Colour BCMTreeItem::getContrastingColour(float contrast) const { return getBackgroundColour().contrasting(contrast); }

Colour BCMTreeItem::getContrastingColour(Colour target, float minContrast) const { return getBackgroundColour().contrasting(target, minContrast); }

void BCMTreeItem::paintContent(Graphics& g, const Rectangle<int>& area)
{
    g.setFont(getFont());
    g.setColour(getContrastingColour(0.8f));

    g.drawFittedText(getDisplayName(), area, Justification::centredLeft, 1, 0.8f);
}

void BCMTreeItem::itemOpennessChanged(bool isNowOpen)
{
    if (isNowOpen && getNumSubItems() == 0)
    {
        clearSubItems();
        refreshSubItems();
    }
    else
        clearSubItems();
}

Component* BCMTreeItem::createItemComponent() { return new TreeItemComponent(*this); }

void BCMTreeItem::itemDropped(const DragAndDropTarget::SourceDetails&, int insertIndex)
{
    moveItems(*getOwnerView(),
               getSelectedTreeViewItems(*getOwnerView()),
               tree, insertIndex, undoManager);
}

void BCMTreeItem::moveItems(TreeView& treeView, const Array<ValueTree>& items,
                                  ValueTree newParent, int insertIndex, UndoManager& undoManager)
{
    if (items.size() > 0)
    {
        ScopedPointer<XmlElement> oldOpenness(treeView.getOpennessState (false));

        for (int i = items.size(); --i >= 0;)
        {
            ValueTree& v = items.getReference(i);

            if (v.getParent().isValid() && newParent != v && ! newParent.isAChildOf (v))
            {
                v.getParent().removeChild (v, &undoManager);
                newParent.addChild(v, insertIndex, &undoManager);
            }
        }

        if (oldOpenness != nullptr)
            treeView.restoreOpennessState (*oldOpenness, false);
    }
}

Array<ValueTree> BCMTreeItem::getSelectedTreeViewItems (TreeView& treeView)
{
    Array<ValueTree> items;

    const int numSelected = treeView.getNumSelectedItems();

    for (int i = 0; i < numSelected; ++i)
        if (const BCMTreeItem* vti = dynamic_cast<BCMTreeItem*>(treeView.getSelectedItem (i)))
            items.add (vti->tree);

    return items;
}

void BCMTreeItem::valueTreePropertyChanged (ValueTree&, const Identifier&) { repaintItem(); }

void BCMTreeItem::treeChildrenChanged (const ValueTree& parentTree)
{
    if (parentTree == tree)
    {
        setOpen(true);
        refreshSubItems();

        BCMTreeView* bcmTree = dynamic_cast<BCMTreeView*>(getOwnerView()->getParentComponent());
        bcmTree->moveToSelectedItem();
    }
}
        
void BCMTreeItem::itemClicked(const MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
        if (getOwnerView()->getNumSelectedItems() > 1)
        {
            showMultiSelectionPopupMenu();
        }
        else
        {
            showPopupMenu();
        }
    }
    else if (isSelected())
    {
        itemSelectionChanged(true);
    }
}

void BCMTreeItem::showMultiSelectionPopupMenu()
{
    PopupMenu m;
    m.addCommandItem(commandManager, CommandIDs::deleteItems);
    m.showMenuAsync(PopupMenu::Options(), nullptr);
}

void BCMTreeItem::deleteAllSelectedItems()
{
    TreeView* treeView = getOwnerView();
    StringArray identifiers;

    for (int i = 0; i < treeView->getNumSelectedItems(); i++)
        identifiers.add(treeView->getSelectedItem(i)->getItemIdentifierString());

    for (int i = 0; i < identifiers.size(); i++)
        dynamic_cast<BCMTreeItem*>(treeView->findItemFromIdentifierString(identifiers[i]))->deleteItem();
}

class BCMTreeItem::ItemSelectionTimer : public Timer
{
public:
    ItemSelectionTimer(BCMTreeItem& ci) : owner (ci) {}

    void timerCallback() override { owner.invokeChangePanel(); }

private:
    BCMTreeItem& owner;
    JUCE_DECLARE_NON_COPYABLE (ItemSelectionTimer)
};

void BCMTreeItem::itemSelectionChanged(bool isNowSelected)
{
    DBG("BCMTreeItem::itemSelectionChanged - isNowSelected: " + String(isNowSelected) + ", item: " + getUniqueName());

    if (isNowSelected)
    {
        delayedSelectionTimer = new ItemSelectionTimer(*this);
        delayedSelectionTimer->startTimer(getMillisecsAllowedForDragGesture());
    }
    else
    {
        cancelDelayedSelectionTimer();
    }
}

void BCMTreeItem::invokeChangePanel()
{
    cancelDelayedSelectionTimer();
    changePanel();
}

void BCMTreeItem::itemDoubleClicked(const MouseEvent&)
{
    invokeChangePanel();
}

void BCMTreeItem::cancelDelayedSelectionTimer()
{
    delayedSelectionTimer = nullptr;
}

void BCMTreeItem::storeSelectionOnAdd()
{
    DBG("BCMTreeItem::storeSelectionOnAdd - Row Number: " + String(getRowNumberInTree() + 1));

    BCMTreeView* bcmTree = dynamic_cast<BCMTreeView*>(getOwnerView()->getParentComponent());
    bcmTree->storeSelectedItem(getRowNumberInTree() + 1);
}

void BCMTreeItem::storeSelectionOnDelete()
{
    int rowNumber = getRowNumberInTree();

    if (getIndexInParent() == getParentItem()->getNumSubItems() - 1)
    {
        rowNumber -= 1;
    }

    DBG("BCMTreeItem::storeSelectionOnDelete - Row Number: " + String(rowNumber));

    BCMTreeView* bcmTree = dynamic_cast<BCMTreeView*> (getOwnerView()->getParentComponent());
    bcmTree->storeSelectedItem(rowNumber);
}

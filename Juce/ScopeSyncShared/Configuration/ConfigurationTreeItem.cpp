/*
  ==============================================================================

    ConfigurationTreeItem.cpp
    Created: 16 Sep 2014 9:38:20am
    Author:  giles

  ==============================================================================
*/

#include "ConfigurationTreeItem.h"

ConfigurationTreeItem::ConfigurationTreeItem(const ValueTree& v, UndoManager& um)
        : tree(v), undoManager(um)
{
    tree.addListener(this);
}

String ConfigurationTreeItem::getUniqueName() const
{
    String uniqueName = tree["name"].toString();

    if (uniqueName.isEmpty())
        uniqueName = tree.getType().toString();

    return uniqueName;
}

bool ConfigurationTreeItem::mightContainSubItems()
{
    return tree.getNumChildren() > 0;
}

void ConfigurationTreeItem::paintItem(Graphics& g, int width, int height)
{
    g.setColour (Colours::antiquewhite);
    g.setFont (15.0f);

    String displayText = tree["name"].toString();

    if (displayText.isEmpty())
        displayText = tree.getType().toString();

    g.drawText (displayText,
                4, 0, width - 4, height,
                Justification::centredLeft, true);
}

void ConfigurationTreeItem::itemOpennessChanged(bool isNowOpen)
{
    if (isNowOpen && getNumSubItems() == 0)
        refreshSubItems();
    else
        clearSubItems();
}

var ConfigurationTreeItem::getDragSourceDescription()
{
    return "Drag Demo";
}

bool ConfigurationTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails)
{
    return dragSourceDetails.description == "Drag Demo";
}

void ConfigurationTreeItem::itemDropped(const DragAndDropTarget::SourceDetails&, int insertIndex)
{
    moveItems (*getOwnerView(),
                getSelectedTreeViewItems (*getOwnerView()),
                tree, insertIndex, undoManager);
}

void ConfigurationTreeItem::moveItems(TreeView& treeView, const Array<ValueTree>& items,
                                      ValueTree newParent, int insertIndex, UndoManager& undoManager)
{
    if (items.size() > 0)
    {
        ScopedPointer<XmlElement> oldOpenness (treeView.getOpennessState (false));

        for (int i = items.size(); --i >= 0;)
        {
            ValueTree& v = items.getReference(i);

            if (v.getParent().isValid() && newParent != v && ! newParent.isAChildOf (v))
            {
                v.getParent().removeChild (v, &undoManager);
                newParent.addChild (v, insertIndex, &undoManager);
            }
        }

        if (oldOpenness != nullptr)
            treeView.restoreOpennessState (*oldOpenness, false);
    }
}

Array<ValueTree> ConfigurationTreeItem::getSelectedTreeViewItems (TreeView& treeView)
{
    Array<ValueTree> items;

    const int numSelected = treeView.getNumSelectedItems();

    for (int i = 0; i < numSelected; ++i)
        if (const ConfigurationTreeItem* vti = dynamic_cast<ConfigurationTreeItem*> (treeView.getSelectedItem (i)))
            items.add (vti->tree);

    return items;
}

void ConfigurationTreeItem::refreshSubItems()
{
    clearSubItems();

    for (int i = 0; i < tree.getNumChildren(); ++i)
        addSubItem (new ConfigurationTreeItem (tree.getChild (i), undoManager));
}

void ConfigurationTreeItem::valueTreePropertyChanged (ValueTree&, const Identifier&)
{
    repaintItem();
}

void ConfigurationTreeItem::treeChildrenChanged (const ValueTree& parentTree)
{
    if (parentTree == tree)
    {
        refreshSubItems();
        treeHasChanged();
        setOpen (true);
    }
}
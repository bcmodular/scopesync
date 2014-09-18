

#include "ConfigurationTreeItem.h"
#include "ConfigurationTree.h"
#include "TreeItemComponent.h"
#include "HostParameterTreeItem.h"
#include "ScopeParameterTreeItem.h"
#include "../Core/Global.h"

ConfigurationTreeItem::ConfigurationTreeItem(const ValueTree& v, UndoManager& um)
    : tree(v), undoManager(um), textX(0)
{
    tree.addListener(this);
}

String ConfigurationTreeItem::getDisplayName() const 
{
    String displayName = tree["name"].toString();

    if (displayName.isEmpty())
        displayName = tree.getType().toString();

    return displayName;
}

String ConfigurationTreeItem::getUniqueName() const
{
    return getDisplayName();
}

bool ConfigurationTreeItem::mightContainSubItems()
{
    return tree.getNumChildren() > 0;
}

Font ConfigurationTreeItem::getFont() const
{
    return Font (getItemHeight() * 0.7f);
}

float ConfigurationTreeItem::getIconSize() const
{
    return jmin (getItemHeight() - 4.0f, 18.0f);
}

Icon ConfigurationTreeItem::getIcon() const
{
    if (tree.hasType(Ids::configuration))
        return Icon(Icons::getInstance()->config, Colours::lightgreen);
    else if (tree.hasType(Ids::hostParameters))
        return Icon(Icons::getInstance()->hostparameters, Colours::aliceblue);
    else if (tree.hasType(Ids::scopeParameters))
        return Icon(Icons::getInstance()->scopeparameters, Colours::azure);
    else if (tree.hasType(Ids::mapping))
        return Icon(Icons::getInstance()->mapping, Colours::bisque);
    else
        return Icon(Icons::getInstance()->config, Colours::beige);
    
}

void ConfigurationTreeItem::paintOpenCloseButton (Graphics& g, const Rectangle<float>& area, Colour backgroundColour, bool isMouseOver)
{
    TreeViewItem::paintOpenCloseButton(g, area, backgroundColour, isMouseOver);
}

Colour ConfigurationTreeItem::getBackgroundColour() const
{
    Colour background(Colours::darkgrey);

    if (isSelected())
        background = background.overlaidWith (getOwnerView()->findColour(TreeView::selectedItemBackgroundColourId));
     
    return background;
}

Colour ConfigurationTreeItem::getContrastingColour(float contrast) const
{
    return getBackgroundColour().contrasting(contrast);
}

Colour ConfigurationTreeItem::getContrastingColour(Colour target, float minContrast) const
{
    return getBackgroundColour().contrasting(target, minContrast);
}

void ConfigurationTreeItem::paintContent(Graphics& g, const Rectangle<int>& area)
{
    g.setFont(getFont());
    g.setColour(getContrastingColour(0.8f));

    g.drawFittedText(getDisplayName(), area, Justification::centredLeft, 1, 0.8f);
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
    return "Configuration Root Item";
}

Component* ConfigurationTreeItem::createItemComponent()
{
    return new TreeItemComponent(*this);
}

bool ConfigurationTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails)
{
    if (dragSourceDetails.description.toString() == "Parameter" && (tree.hasType(Ids::hostParameters) || tree.hasType(Ids::scopeParameters)))
        return true;
    else
        return false;
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
    {
        if (tree.hasType(Ids::hostParameters))
        {
            addSubItem (new HostParameterTreeItem(tree.getChild(i), undoManager));
        }
        else if (tree.hasType(Ids::scopeParameters))
        {
            addSubItem (new ScopeParameterTreeItem(tree.getChild(i), undoManager));
        }
        else
            addSubItem (new ConfigurationTreeItem(tree.getChild(i), undoManager));
    }
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
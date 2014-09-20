/**
 * Various TreeViewItem classes that support the Configuration
 * Manager TreeView.
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

#include "ConfigurationTreeItem.h"
#include "ConfigurationTree.h"
#include "ConfigurationManagerMain.h"
#include "ConfigurationPanel.h"
#include "../Core/Global.h"

/* =========================================================================
 * ConfigurationTreeItem
 */
ConfigurationTreeItem::ConfigurationTreeItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : configurationManagerMain(cmm),
      tree(v),
      undoManager(um),
      textX(0)
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
    WholeTreeOpennessRestorer wtor(*this);
    
    clearSubItems();

    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        if (tree.hasType(Ids::hostParameters))
        {
            addSubItem (new HostParameterTreeItem(configurationManagerMain, tree.getChild(i), undoManager));
        }
        else if (tree.hasType(Ids::scopeParameters))
        {
            addSubItem (new ScopeParameterTreeItem(configurationManagerMain, tree.getChild(i), undoManager));
        }
        else
            addSubItem (new ConfigurationTreeItem(configurationManagerMain, tree.getChild(i), undoManager));
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

void ConfigurationTreeItem::itemClicked(const MouseEvent& /* e */)
{
    if (isSelected())
    {
        itemSelectionChanged(true);
    }
}

class ConfigurationTreeItem::ItemSelectionTimer : public Timer
{
public:
    ItemSelectionTimer(ConfigurationTreeItem& cti) : owner (cti) {}

    void timerCallback() override { owner.invokeChangePanel(); }

private:
    ConfigurationTreeItem& owner;
    JUCE_DECLARE_NON_COPYABLE (ItemSelectionTimer)
};

void ConfigurationTreeItem::itemSelectionChanged(bool isNowSelected)
{
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

void ConfigurationTreeItem::invokeChangePanel()
{
    cancelDelayedSelectionTimer();
    changePanel();
}

void ConfigurationTreeItem::itemDoubleClicked(const MouseEvent&)
{
    invokeChangePanel();
}

void ConfigurationTreeItem::cancelDelayedSelectionTimer()
{
    delayedSelectionTimer = nullptr;
}

void ConfigurationTreeItem::changePanel()
{
    configurationManagerMain.changePanel(nullptr);
}

/* =========================================================================
 * ParameterTreeItem
 */
ParameterTreeItem::ParameterTreeItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um) : ConfigurationTreeItem(cmm, v, um) {}

var ParameterTreeItem::getDragSourceDescription()
{
    return "Parameter";
}

bool ParameterTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& /* dragSourceDetails */)
{
    return false;
}

String ParameterTreeItem::getDisplayName() const 
{
    String displayName = tree[Ids::name].toString();
    displayName += " (" + tree[Ids::fullDescription].toString() + ")";
    
    return displayName;
}

/* =========================================================================
 * HostParameterTreeItem
 */
HostParameterTreeItem::HostParameterTreeItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um) : ParameterTreeItem(cmm, v, um) {}

Icon HostParameterTreeItem::getIcon() const
{
    return Icon(Icons::getInstance()->hostparameter, Colours::grey);
}

String HostParameterTreeItem::getDisplayName() const 
{
    String displayName = ParameterTreeItem::getDisplayName();
    
    if (int(tree[Ids::scopeSync]) != -1)
        displayName += " - " + ScopeSync::getScopeSyncCode(int(tree[Ids::scopeSync]));

    return displayName;
}

void HostParameterTreeItem::changePanel()
{
    configurationManagerMain.changePanel(new ParameterPanel(tree));
}

/* =========================================================================
 * ScopeParameterTreeItem
 */
ScopeParameterTreeItem::ScopeParameterTreeItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um) : ParameterTreeItem(cmm, v, um) {}

Icon ScopeParameterTreeItem::getIcon() const
{
    return Icon(Icons::getInstance()->scopeparameter, Colours::grey);
}

String ScopeParameterTreeItem::getDisplayName() const 
{
    String displayName = ParameterTreeItem::getDisplayName();
    
    if (int(tree[Ids::scopeLocal]) != -1)
        displayName += " - " + ScopeSync::getScopeLocalCode(int(tree[Ids::scopeLocal]));

    return displayName;
}

void ScopeParameterTreeItem::changePanel()
{
    configurationManagerMain.changePanel(new ParameterPanel(tree));
}

/* =========================================================================
 * TreeItemComponent
 */
TreeItemComponent::TreeItemComponent(ConfigurationTreeItem& i) : item (i)
{
    setInterceptsMouseClicks (false, true);
}

void TreeItemComponent::paint (Graphics& g)
{
    g.setColour(Colours::black);
    paintIcon(g);
    item.paintContent(g, Rectangle<int>(item.textX, 0, getWidth() - item.textX, getHeight()));
}

void TreeItemComponent::paintIcon (Graphics& g)
{
    item.getIcon().draw(g, Rectangle<float> (4.0f, 2.0f, item.getIconSize(), getHeight() - 4.0f),
                            item.isIconCrossedOut());
}

void TreeItemComponent::resized()
{
    item.textX = (int)item.getIconSize() + 8;
}

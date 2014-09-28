/**
 * The Configuration Manager's TreeView
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

#include "ConfigurationTree.h"
#include "Configuration.h"
#include "ConfigurationTreeItem.h"
#include "ConfigurationManagerMain.h"

ConfigurationTree::ConfigurationTree(ConfigurationManagerMain& cmm, Configuration& config, UndoManager& um)
    : configurationManagerMain(cmm),
      configuration(config),
      undoManager(um)
{
    addAndMakeVisible(tree);

    tree.setDefaultOpenness(true);
    tree.setMultiSelectEnabled(true);
    tree.setRootItem(rootItem = new ConfigurationTreeItem(configurationManagerMain, config.getConfigurationRoot(), undoManager));

    const ScopedPointer<XmlElement> treeOpenness(config.getConfigurationProperties().getXmlValue("openness"));
    
    if (treeOpenness != nullptr)
        tree.restoreOpennessState (*treeOpenness, true);

    tree.setColour(TreeView::backgroundColourId, Colours::darkgrey);
    tree.getViewport()->setScrollBarsShown(true, true, true, true);
}

ConfigurationTree::~ConfigurationTree()
{
    tree.setRootItem(nullptr);
}

void ConfigurationTree::paint(Graphics& g)
{
    g.fillAll(Colour(0xff434343));
}

void ConfigurationTree::resized()
{
    Rectangle<int> r(getLocalBounds());

    r.removeFromTop(8);
    r.removeFromLeft(8);
    r.removeFromRight(4);
    tree.setBounds(r);
}

void ConfigurationTree::deleteSelectedItems()
{
    StringArray identifiers;

    for (int i = 0; i < tree.getNumSelectedItems(); i++)
    {
        identifiers.add(tree.getSelectedItem(i)->getItemIdentifierString());
        DBG("ConfigurationTree::deleteSelectedItems - added item to delete: " + identifiers[i]);
    }

    for (int i = 0; i < identifiers.size(); i++)
    {
        dynamic_cast<ConfigurationTreeItem*>(tree.findItemFromIdentifierString(identifiers[i]))->deleteItem();
    }
}

void ConfigurationTree::saveTreeViewState()
{
    const ScopedPointer<XmlElement> opennessState(tree.getOpennessState(true));

    if (opennessState != nullptr)
        configuration.getConfigurationProperties().setValue("openness", opennessState);
}

void ConfigurationTree::storeSelectedItemMove(String itemId, int delta)
{
    lastSelectedItem      = itemId;
    moveSelectedItemDelta = delta;
}

void ConfigurationTree::moveToSelectedItemDelta()
{
    if (lastSelectedItem.isNotEmpty())
    {
        TreeViewItem* itemToMoveTo = tree.findItemFromIdentifierString(lastSelectedItem);
            
        if (itemToMoveTo != nullptr)
        {
            itemToMoveTo->setSelected(true, true);
            tree.moveSelectedRow(moveSelectedItemDelta);
        }

        lastSelectedItem      = String::empty;
        moveSelectedItemDelta = 0;
    }
}
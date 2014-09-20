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
    Rectangle<int> r(getLocalBounds().reduced(8));

    r.removeFromBottom(4);
    tree.setBounds(r);
}

void ConfigurationTree::deleteSelectedItems()
{
    Array<ValueTree> selectedItems(ConfigurationTreeItem::getSelectedTreeViewItems(tree));

    for (int i = selectedItems.size(); --i >= 0;)
    {
        ValueTree& v = selectedItems.getReference(i);

        if (v.getParent().isValid())
            v.getParent().removeChild (v, &undoManager);
    }
}

void ConfigurationTree::saveTreeViewState()
{
    const ScopedPointer<XmlElement> opennessState(tree.getOpennessState(true));

    if (opennessState != nullptr)
        configuration.getConfigurationProperties().setValue("openness", opennessState);
}
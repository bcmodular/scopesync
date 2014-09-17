/*
  ==============================================================================

    ConfigurationTree.cpp
    Created: 16 Sep 2014 11:51:11am
    Author:  giles

  ==============================================================================
*/

#include "ConfigurationTree.h"
#include "Configuration.h"
#include "ConfigurationTreeItem.h"

ConfigurationTree::ConfigurationTree(ValueTree configRoot, UndoManager& um) : configurationRoot(configRoot),
                                                                              undoManager(um)
{
    addAndMakeVisible(tree);

    tree.setDefaultOpenness(false);
    tree.setMultiSelectEnabled(true);
    tree.setRootItem(rootItem = new ConfigurationTreeItem(configurationRoot, undoManager));
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



#include "ConfigurationTree.h"
#include "Configuration.h"
#include "ConfigurationTreeItem.h"

ConfigurationTree::ConfigurationTree(Configuration& config, UndoManager& um)
    : configuration(config),
      undoManager(um)
{
    addAndMakeVisible(tree);

    tree.setDefaultOpenness(true);
    tree.setMultiSelectEnabled(true);
    tree.setRootItem(rootItem = new ConfigurationTreeItem(config.getConfigurationRoot(), undoManager));

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
/*
  ==============================================================================

    ConfigurationTree.h
    Created: 16 Sep 2014 11:51:11am
    Author:  giles

  ==============================================================================
*/

#ifndef CONFIGURATIONTREE_H_INCLUDED
#define CONFIGURATIONTREE_H_INCLUDED

#include <JuceHeader.h>
class Configuration;
class ConfigurationTreeItem;

class ConfigurationTree : public  Component,
                          public  DragAndDropContainer
{
public:
    ConfigurationTree(ValueTree configRoot, UndoManager& um);
    ~ConfigurationTree();

    void paint (Graphics& g) override;
    void resized() override;

    void deleteSelectedItems();
    
private:
    TreeView       tree;
    ValueTree      configurationRoot;
    ScopedPointer<ConfigurationTreeItem> rootItem;
    UndoManager& undoManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationTree);
};



#endif  // CONFIGURATIONTREE_H_INCLUDED

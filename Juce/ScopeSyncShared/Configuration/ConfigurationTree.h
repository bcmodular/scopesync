

#ifndef CONFIGURATIONTREE_H_INCLUDED
#define CONFIGURATIONTREE_H_INCLUDED

#include <JuceHeader.h>
class Configuration;
class ConfigurationTreeItem;

class ConfigurationTree : public  Component,
                          public  DragAndDropContainer
{
public:
    ConfigurationTree(Configuration& config, UndoManager& um);
    ~ConfigurationTree();

    void paint (Graphics& g) override;
    void resized() override;
    void saveTreeViewState();

    void deleteSelectedItems();

    enum ColourIds
    {
        mainBackgroundColourId = 0x2340000,
    };    

private:
    TreeView       tree;
    ScopedPointer<ConfigurationTreeItem> rootItem;
    UndoManager&   undoManager;
    Configuration& configuration;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationTree);
};



#endif  // CONFIGURATIONTREE_H_INCLUDED

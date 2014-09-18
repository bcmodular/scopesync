/*
  ==============================================================================

    TreeItemComponent.h
    Created: 15 Sep 2014 5:20:53pm
    Author:  giles

  ==============================================================================
*/

#ifndef TREEITEMCOMPONENT_H_INCLUDED
#define TREEITEMCOMPONENT_H_INCLUDED

#include <JuceHeader.h>
class ConfigurationTreeItem;

class TreeItemComponent : public Component
{
public:
    TreeItemComponent (ConfigurationTreeItem& i);

    void paint (Graphics& g) override;
    void paintIcon (Graphics& g);
    void resized() override;

    ConfigurationTreeItem& item;

private:    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TreeItemComponent);

};


#endif  // TREEITEMCOMPONENT_H_INCLUDED

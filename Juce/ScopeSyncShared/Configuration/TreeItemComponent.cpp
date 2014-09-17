/*
  ==============================================================================

    TreeItemComponent.cpp
    Created: 16 Sep 2014 9:39:29am
    Author:  giles

  ==============================================================================
*/

#include "TreeItemComponent.h"
#include "ConfigurationTreeItem.h"

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

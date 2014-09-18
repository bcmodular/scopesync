/*
  ==============================================================================

    HostParameterTreeItem.cpp
    Created: 18 Sep 2014 2:47:42pm
    Author:  giles

  ==============================================================================
*/

#include "HostParameterTreeItem.h"

HostParameterTreeItem::HostParameterTreeItem(const ValueTree& v, UndoManager& um) : ParameterTreeItem(v, um) {}

Icon HostParameterTreeItem::getIcon() const
{
    return Icon(Icons::getInstance()->hostparameter, Colours::grey);
}

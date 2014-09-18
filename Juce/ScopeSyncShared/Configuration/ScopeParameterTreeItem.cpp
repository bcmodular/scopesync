/*
  ==============================================================================

    ScopeParameterTreeItem.cpp
    Created: 18 Sep 2014 2:47:32pm
    Author:  giles

  ==============================================================================
*/

#include "ScopeParameterTreeItem.h"

ScopeParameterTreeItem::ScopeParameterTreeItem(const ValueTree& v, UndoManager& um) : ParameterTreeItem(v, um) {}

Icon ScopeParameterTreeItem::getIcon() const
{
    return Icon(Icons::getInstance()->scopeparameter, Colours::grey);
}
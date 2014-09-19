/*
  ==============================================================================

    ScopeParameterTreeItem.cpp
    Created: 18 Sep 2014 2:47:32pm
    Author:  giles

  ==============================================================================
*/

#include "ScopeParameterTreeItem.h"
#include "../Core/Global.h"
#include "../Core/ScopeSync.h"

ScopeParameterTreeItem::ScopeParameterTreeItem(const ValueTree& v, UndoManager& um) : ParameterTreeItem(v, um) {}

Icon ScopeParameterTreeItem::getIcon() const
{
    return Icon(Icons::getInstance()->scopeparameter, Colours::grey);
}

String ScopeParameterTreeItem::getDisplayName() const 
{
    String displayName = ParameterTreeItem::getDisplayName();
    
    if (int(tree[Ids::scopeLocal]) != -1)
        displayName += " - " + ScopeSync::getScopeLocalCode(int(tree[Ids::scopeLocal]));

    return displayName;
}

/*
  ==============================================================================

    HostParameterTreeItem.cpp
    Created: 18 Sep 2014 2:47:42pm
    Author:  giles

  ==============================================================================
*/

#include "HostParameterTreeItem.h"
#include "../Core/Global.h"
#include "../Core/ScopeSync.h"

HostParameterTreeItem::HostParameterTreeItem(const ValueTree& v, UndoManager& um) : ParameterTreeItem(v, um) {}

Icon HostParameterTreeItem::getIcon() const
{
    return Icon(Icons::getInstance()->hostparameter, Colours::grey);
}

String HostParameterTreeItem::getDisplayName() const 
{
    String displayName = ParameterTreeItem::getDisplayName();
    
    if (int(tree[Ids::scopeSync]) != -1)
        displayName += " - " + ScopeSync::getScopeSyncCode(int(tree[Ids::scopeSync]));

    return displayName;
}

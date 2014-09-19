

#include "ParameterTreeItem.h"
#include "../Resources/Icons.h"
#include "../Core/Global.h"

ParameterTreeItem::ParameterTreeItem(const ValueTree& v, UndoManager& um) : ConfigurationTreeItem(v, um)
{

}

var ParameterTreeItem::getDragSourceDescription()
{
    return "Parameter";
}

bool ParameterTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& /* dragSourceDetails */)
{
    return false;
}

String ParameterTreeItem::getDisplayName() const 
{
    String displayName = tree[Ids::name].toString();
    displayName += " (" + tree[Ids::fullDescription].toString() + ")";
    
    return displayName;
}
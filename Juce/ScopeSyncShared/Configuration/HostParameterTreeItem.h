
#ifndef HOSTPARAMETERTREEITEM_H_INCLUDED
#define HOSTPARAMETERTREEITEM_H_INCLUDED

#include <JuceHeader.h>
#include "ParameterTreeItem.h"

class HostParameterTreeItem  : public ParameterTreeItem
{
public:
    HostParameterTreeItem(const ValueTree& v, UndoManager& um);

    Icon getIcon() const;
};



#endif  // HOSTPARAMETERTREEITEM_H_INCLUDED

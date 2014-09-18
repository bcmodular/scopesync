
#ifndef HOSTPARAMETERTREEITEM_H_INCLUDED
#define HOSTPARAMETERTREEITEM_H_INCLUDED

#include <JuceHeader.h>
#include "ParameterTreeItem.h"

class HostParameterTreeItem  : public ParameterTreeItem
{
public:
    HostParameterTreeItem(const ValueTree& v, UndoManager& um);

    Icon getIcon() const;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HostParameterTreeItem);
};



#endif  // HOSTPARAMETERTREEITEM_H_INCLUDED

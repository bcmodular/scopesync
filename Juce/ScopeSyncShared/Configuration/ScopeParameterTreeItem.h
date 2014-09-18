/*
  ==============================================================================

    ScopeParameterTreeItem.h
    Created: 18 Sep 2014 2:47:32pm
    Author:  giles

  ==============================================================================
*/

#ifndef SCOPEPARAMETERTREEITEM_H_INCLUDED
#define SCOPEPARAMETERTREEITEM_H_INCLUDED

#include <JuceHeader.h>
#include "ParameterTreeItem.h"

class ScopeParameterTreeItem  : public ParameterTreeItem
{
public:
    ScopeParameterTreeItem(const ValueTree& v, UndoManager& um);

    Icon getIcon() const;

private:    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeParameterTreeItem);
};



#endif  // SCOPEPARAMETERTREEITEM_H_INCLUDED

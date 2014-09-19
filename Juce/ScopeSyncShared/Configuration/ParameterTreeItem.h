

#ifndef PARAMETERTREEITEM_H_INCLUDED
#define PARAMETERTREEITEM_H_INCLUDED

#include <JuceHeader.h>
#include "ConfigurationTreeItem.h"

class ParameterTreeItem  : public ConfigurationTreeItem
{
public:
    ParameterTreeItem(const ValueTree& v, UndoManager& um);

    var  getDragSourceDescription() override;
    bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    
    virtual Icon getIcon() const override { return Icon(); };

    virtual String getDisplayName() const override;

private:    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterTreeItem);
};

#endif  // PARAMETERTREEITEM_H_INCLUDED

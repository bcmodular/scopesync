

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
    
    virtual Icon getIcon() const { return Icon(); };

    String getDisplayName() const;

};

#endif  // PARAMETERTREEITEM_H_INCLUDED

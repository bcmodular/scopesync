/*
  ==============================================================================

    PresetItem.h
    Created: 2 Nov 2014 10:31:41am
    Author:  giles

  ==============================================================================
*/

#ifndef PRESETITEM_H_INCLUDED
#define PRESETITEM_H_INCLUDED

#include <JuceHeader.h>
#include "../Resources/Icons.h"
#include "../Utils/BCMTreeView.h"
class PresetManager;
class PresetFile;

/* =========================================================================
 * PresetRootItem: Base class for Preset Manager TreeViewItems
 */
class PresetRootItem : public BCMTreeItem
{
public:
    PresetRootItem(PresetFile& pf, PresetManager& pm, const ValueTree& v, UndoManager& um);
    ~PresetRootItem();

    virtual bool mightContainSubItems() override;
    Font         getFont() const override;
    virtual Icon getIcon() const override;
    float        getIconSize() const override;
    virtual var  getDragSourceDescription() override;
    virtual bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    virtual void changePanel() override;
    
    virtual void copyItem() override {}
    virtual void pasteItem() override {}
    virtual bool canPasteItem() override;

    virtual void deleteItem() override {};
    virtual void addItem() override;
    virtual void addItemFromClipboard() override;
    
    virtual void showPopupMenu() override;
    
    virtual String getDisplayName() const override;
    
protected:
    PresetManager& presetManager;
    PresetFile&    presetFile;
    
private:
    virtual void refreshSubItems() override;

    void addPreset(const ValueTree& definition);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetRootItem)
};

#endif  // PRESETITEM_H_INCLUDED

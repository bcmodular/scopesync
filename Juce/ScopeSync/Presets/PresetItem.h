/**
 * Classes used to support Preset Manager TreeViewItems
 *
 *  (C) Copyright 2014 bcmodular (http://www.bcmodular.co.uk/)
 *
 * This file is part of ScopeSync.
 *
 * ScopeSync is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * ScopeSync is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ScopeSync.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contributors:
 *  Simon Russell
 *  Will Ellis
 *  Jessica Brandt
 */

#ifndef PRESETITEM_H_INCLUDED
#define PRESETITEM_H_INCLUDED

#include <JuceHeader.h>
#include "../Resources/Icons.h"
#include "../Utils/BCMTreeView.h"
#include "../Core/Clipboard.h"

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
	SharedResourcePointer<ParameterClipboard> parameterClipboard;

private:
    virtual void refreshSubItems() override;

    void addPreset(const ValueTree& definition);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetRootItem)
};

#endif  // PRESETITEM_H_INCLUDED

/**
 * Classes used to support Preset Manager TreeViewItems
 *
 *  (C) Copyright 2014 bcmodular (http://www.bcmodular.co.uk/)
 *
 * This file is part of ScopeSync.
 *
 * ScopeSync is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include "PresetItem.h"
#include "PresetFile.h"
#include "../Utils/BCMTreeView.h"
#include "PresetManager.h"
#include "../Core/Global.h"

/* =========================================================================
 * PresetItem: Preset TreeViewItems
 */
class PresetItem : public PresetRootItem
{
public:
    PresetItem(PresetFile& pf, PresetManager& pm, const ValueTree& v, UndoManager& um)
         : PresetRootItem(pf, pm, v, um) {}

    bool mightContainSubItems() override { return false; }
    var  getDragSourceDescription() override { return "Preset"; }
    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails& /* dragSourceDetails */) override { return false; }
    
    virtual Icon getIcon() const override { return Icon(icons->parameter, Colours::grey); }
    virtual String getDisplayName() const override;

    void copyItem() override;
    void pasteItem() override;
    bool canPasteItem() override { return parameterClipboard->clipboardIsNotEmpty(); }

    void deleteItem() override;
    void addItem() override;
    void addItemFromClipboard() override;

    void insertPresetAt(const ValueTree& definition, int index);

    void changePanel() override
    {
        presetManager.changePanel(new PresetPanel(tree, undoManager, presetFile, commandManager));
    }
private:
	void refreshSubItems() override;
    void showPopupMenu() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetItem);
};

String PresetItem::getDisplayName() const 
{
    String displayName = tree[Ids::name].toString();
    
    return displayName;
}

void PresetItem::showPopupMenu()
{
    PopupMenu m;
    m.addCommandItem(commandManager, CommandIDs::addItem, "Add Preset");
    m.addCommandItem(commandManager, CommandIDs::addItemFromClipboard, "Add Preset from clipboard");
    m.addSeparator();
    m.addCommandItem(commandManager, CommandIDs::copyItem, "Copy Preset");
    m.addCommandItem(commandManager, CommandIDs::pasteItem, "Paste Preset");
    m.addSeparator();
    m.addCommandItem(commandManager, CommandIDs::deleteItems, "Delete Preset");
    
    m.showMenuAsync (PopupMenu::Options(), nullptr);
}

void PresetItem::refreshSubItems() {}

void PresetItem::deleteItem()
{
    storeSelectionOnDelete();
    tree.getParent().removeChild(tree, &undoManager);
}

void PresetItem::addItem()
{
    ValueTree definition;
    insertPresetAt(definition, tree.getParent().indexOf(tree) + 1);
}

void PresetItem::addItemFromClipboard()
{
    ValueTree definition;
    
    if (parameterClipboard->clipboardIsNotEmpty())
    {
        definition = ValueTree(Ids::preset);
        parameterClipboard->paste(definition, nullptr);
    }
    
    insertPresetAt(definition, tree.getParent().indexOf(tree) + 1);
}

void PresetItem::insertPresetAt(const ValueTree& definition, int index)
{
    storeSelectionOnAdd();
    
    ValueTree newPreset;

    presetFile.addNewPreset(newPreset, definition, index, &undoManager);
}

void PresetItem::copyItem()
{
    parameterClipboard->copy(tree);
}

void PresetItem::pasteItem()
{
    storeSelectionOnDelete();
    parameterClipboard->paste(tree, &undoManager);
    changePanel();
}


/* =========================================================================
 * PresetRootItem
 */
PresetRootItem::PresetRootItem(PresetFile& pf, PresetManager& pm, const ValueTree& v, UndoManager& um)
    : BCMTreeItem(v, um, pm.getCommandManager()),
      presetManager(pm),
      presetFile(pf)
{}

PresetRootItem::~PresetRootItem() {}

String PresetRootItem::getDisplayName() const 
{
    String displayName = tree["name"].toString();

    if (displayName.isEmpty())
        displayName = tree.getType().toString();

    return displayName;
}

bool PresetRootItem::mightContainSubItems() { return tree.getNumChildren() > 0; }

Font PresetRootItem::getFont() const { return Font (getItemHeight() * 0.7f); }

float PresetRootItem::getIconSize() const { return jmin (getItemHeight() - 4.0f, 18.0f); }

Icon PresetRootItem::getIcon() const { return Icon(icons->parameters, Colours::aliceblue); }

var PresetRootItem::getDragSourceDescription() { return "Preset File Root Item"; }

bool PresetRootItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails)
{
    return dragSourceDetails.description.toString() == "Preset";
}

void PresetRootItem::showPopupMenu()
{
    PopupMenu m;
    m.addCommandItem(commandManager, CommandIDs::addItem, "Add Preset");
    m.addCommandItem(commandManager, CommandIDs::addItemFromClipboard, "Add Preset from clipboard");
    
    m.showMenuAsync (PopupMenu::Options(), nullptr);
}

void PresetRootItem::refreshSubItems()
{
    WholeTreeOpennessRestorer wtor(*this);
    
    clearSubItems();

    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        ValueTree child = tree.getChild(i);

        if (child.hasType(Ids::preset))
            addSubItem (new PresetItem(presetFile, presetManager, child, undoManager));
    }
}

void PresetRootItem::changePanel()
{
    if (tree.hasType(Ids::presets))
        presetManager.changePanel(new PresetFilePanel(tree, undoManager, commandManager));
    else
        presetManager.changePanel(new EmptyPanel(tree, undoManager, commandManager));
}

bool PresetRootItem::canPasteItem()
{ 
    return parameterClipboard->clipboardIsNotEmpty();
}

void PresetRootItem::addItem()
{
    ValueTree definition;
    addPreset(definition);
}

void PresetRootItem::addItemFromClipboard()
{
    ValueTree definition;

    if (parameterClipboard->clipboardIsNotEmpty())
    {
        definition = ValueTree(Ids::preset);
        parameterClipboard->paste(definition, nullptr);
    }

    addPreset(definition);
}

void PresetRootItem::addPreset(const ValueTree& definition)
{
    storeSelectionOnAdd();
    
    ValueTree newPreset;

    presetFile.addNewPreset(newPreset, definition, 0, &undoManager);
}

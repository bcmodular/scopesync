/**
 * Various TreeViewItem classes that support the Configuration
 * Manager TreeView.
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

#ifndef CONFIGURATIONTREEITEM_H_INCLUDED
#define CONFIGURATIONTREEITEM_H_INCLUDED

#include <JuceHeader.h>
#include "../Resources/Icons.h"
class ConfigurationManagerMain;
class Configuration;

/* =========================================================================
 * ConfigurationItem: Base class for Configuration Manager TreeViewItems
 */
class ConfigurationItem  : public  TreeViewItem,
                           private ValueTree::Listener
{
public:
    ConfigurationItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um);
    ~ConfigurationItem();

    String       getUniqueName() const override;
    virtual bool mightContainSubItems() override;
    Font         getFont() const;
    virtual Icon getIcon() const;
    float        getIconSize() const;
    bool         isIconCrossedOut() const { return false; }
    void         paintContent(Graphics& g, const Rectangle<int>& area);
    void         paintOpenCloseButton(Graphics&, const Rectangle<float>& area, Colour backgroundColour, bool isMouseOver) override;
    Component*   createItemComponent() override;
    void         itemOpennessChanged(bool isNowOpen) override;
    virtual var  getDragSourceDescription() override;
    virtual bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    void         itemDropped(const DragAndDropTarget::SourceDetails&, int insertIndex) override;
    void         itemClicked(const MouseEvent& e) override;
    void         itemSelectionChanged(bool isNowSelected) override;
    void         itemDoubleClicked(const MouseEvent&) override;
    virtual void changePanel();
    
    virtual void copyItem() {};
    virtual void pasteItem() {};
    virtual void deleteItem() {};
    virtual void addItem() {};
    virtual void addItemFromClipboard() {};
    
    virtual void showPopupMenu() {};
    virtual void showMultiSelectionPopupMenu();
    
    void         storeSelectionOnAdd();
    void         storeSelectionOnDelete();

    virtual int  getMillisecsAllowedForDragGesture() { return 120; };
    void         cancelDelayedSelectionTimer();
    
    virtual String getDisplayName() const;

    static void moveItems (TreeView& treeView, const Array<ValueTree>& items,
                           ValueTree newParent, int insertIndex, UndoManager& undoManager);
    static Array<ValueTree> getSelectedTreeViewItems (TreeView& treeView);
    
    struct WholeTreeOpennessRestorer   : public OpennessRestorer
    {
        WholeTreeOpennessRestorer (TreeViewItem& item)  : OpennessRestorer (getTopLevelItem (item))
        {}

    private:
        static TreeViewItem& getTopLevelItem (TreeViewItem& item)
        {
            if (TreeViewItem* const p = item.getParentItem())
                return getTopLevelItem (*p);

            return item;
        }
    };

    int textX;

protected:
    Colour    getBackgroundColour() const;
    Colour    getContrastingColour (float contrast) const;
    Colour    getContrastingColour (Colour targetColour, float minContrast) const;
    ValueTree tree;
    ConfigurationManagerMain&  configurationManagerMain;
    Configuration&             configuration;
    ApplicationCommandManager* commandManager;
    UndoManager& undoManager;

    static void treeViewMultiSelectItemChosen(int resultCode, ConfigurationItem* item);
    
private:
    class        ItemSelectionTimer;
    friend class ItemSelectionTimer;
    ScopedPointer<Timer> delayedSelectionTimer;

    WeakReference<ConfigurationItem>::Master masterReference;
    friend class WeakReference<ConfigurationItem>;

    virtual void refreshSubItems();
    
    /* ================= ValueTree::Listener overrides ================= */
    void valueTreePropertyChanged (ValueTree&, const Identifier&) override;
    void valueTreeChildAdded (ValueTree& parentTree, ValueTree&) override    { treeChildrenChanged(parentTree); }
    void valueTreeChildRemoved (ValueTree& parentTree, ValueTree&) override  { treeChildrenChanged(parentTree); }
    void valueTreeChildOrderChanged (ValueTree& parentTree) override         { treeChildrenChanged(parentTree); }
    void valueTreeParentChanged (ValueTree&) override {}

    void treeChildrenChanged (const ValueTree& parentTree);
    void invokeChangePanel();

    void deleteAllSelectedItems();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConfigurationItem)
};

/* =========================================================================
 * ParameterRootItem: Parameter root node TreeViewItems
 */
class ParameterRootItem : public ConfigurationItem
{
public:
    ParameterRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um);

    var  getDragSourceDescription() override;
    bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    
    Icon getIcon() const override;
    
    void addItem() override;
    void addItemFromClipboard() override;
    
protected:
    void addNewSubItem();
    
private:
    void refreshSubItems() override;
    void showPopupMenu() override;

    void addParameter(const ValueTree& definition);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterRootItem);
};

/* =========================================================================
 * ParameterItem: Base class for Parameter instance TreeViewItems
 */
class ParameterItem : public ConfigurationItem
{
public:
    ParameterItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um);

    bool mightContainSubItems() override { return false; };
    var  getDragSourceDescription() override;
    bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    
    virtual Icon getIcon() const override = 0;
    virtual String getDisplayName() const override;

    void copyItem() override;
    void pasteItem() override;
    void deleteItem() override;
    void addItem() override;
    void addItemFromClipboard() override;

    void insertParameterAt(const ValueTree& definition, int index);

private:
    void refreshSubItems() override;
    
    void showPopupMenu() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterItem);
};

/* =========================================================================
 * HostParameterItem: Host Parameter instance TreeViewItems
 */
class HostParameterItem  : public ParameterItem
{
public:
    HostParameterItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um);

    Icon getIcon() const override;
    String getDisplayName() const override;
    void changePanel() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HostParameterItem);
};

/* =========================================================================
 * ScopeParameterItem: Scope Parameter instance TreeViewItems
 */
class ScopeParameterItem  : public ParameterItem
{
public:
    ScopeParameterItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um);

    Icon getIcon() const override;
    String getDisplayName() const override;
    void changePanel() override;

private:    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeParameterItem);
};

/* =========================================================================
 * MappingRootTreeItem: Mapping root node TreeViewItems
 */
class MappingRootItem : public ConfigurationItem
{
public:
    MappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um);

    virtual var  getDragSourceDescription() override;
    virtual Icon getIcon() const override;

    void addGenericMapping(const Identifier& mappingType);

private:
    virtual void refreshSubItems() override;

    void showPopupMenu() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MappingRootItem);
};

/* =========================================================================
 * SliderMappingRootItem: Slider Mapping root node TreeViewItem
 */
class SliderMappingRootItem : public MappingRootItem
{
public:
    SliderMappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um);
    
    bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    Icon getIcon() const override;
    void addItem() override;
    
private:
    void refreshSubItems() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderMappingRootItem);
};

/* =========================================================================
 * LabelMappingRootItem: Label Mapping root node TreeViewItem
 */
class LabelMappingRootItem : public MappingRootItem
{
public:
    LabelMappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um);
    
    bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    Icon getIcon() const override;
    void addItem() override;
    
private:
    void refreshSubItems() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabelMappingRootItem);
};

/* =========================================================================
 * ComboBoxMappingRootItem: ComboBox Mapping root node TreeViewItem
 */
class ComboBoxMappingRootItem : public MappingRootItem
{
public:
    ComboBoxMappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um);
    
    bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    Icon getIcon() const override;
    void addItem() override;
    
private:
    void refreshSubItems() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComboBoxMappingRootItem);
};

/* =========================================================================
 * TabbedComponentMappingRootItem: TabbedComponent Mapping root node TreeViewItem
 */
class TabbedComponentMappingRootItem : public MappingRootItem
{
public:
    TabbedComponentMappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um);
    
    bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    Icon getIcon() const override;
    void addItem() override;
    
private:
    void refreshSubItems() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabbedComponentMappingRootItem);
};

/* =========================================================================
 * TextButtonMappingRootItem: TextButton Mapping root node TreeViewItem
 */
class TextButtonMappingRootItem : public MappingRootItem
{
public:
    TextButtonMappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um);
    
    bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    Icon getIcon() const override;
    void addItem() override;
    
private:
    void refreshSubItems() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextButtonMappingRootItem);
};

/* =========================================================================
 * MappingItem: Base class for Mapping-based TreeViewItems
 */
class MappingItem : public ConfigurationItem
{
public:
    MappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um);

    bool mightContainSubItems() override { return false; };
    virtual var getDragSourceDescription() override;
    virtual bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override;

    void deleteItem() override;
    void addItem() override;
    
    virtual Icon getIcon() const override { return Icon(); };
    virtual String getDisplayName() const override;

private:
    void refreshSubItems() override;
    
    void showPopupMenu() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MappingItem);
};

/* =========================================================================
 * SliderMappingItem: TreeViewItem for Slider Mappings
 */
class SliderMappingItem : public MappingItem
{
public:
    SliderMappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um);

    var  getDragSourceDescription() override;
    Icon getIcon() const override;
    void changePanel() override;

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderMappingItem);
};

/* =========================================================================
 * LabelMappingItem: TreeViewItem for Label Mappings
 */
class LabelMappingItem : public MappingItem
{
public:
    LabelMappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um);

    var  getDragSourceDescription() override;
    Icon getIcon() const override;
    void changePanel() override;

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabelMappingItem);
};

/* =========================================================================
 * ComboBoxMappingItem: TreeViewItem for ComboBox Mappings
 */
class ComboBoxMappingItem : public MappingItem
{
public:
    ComboBoxMappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um);

    var  getDragSourceDescription() override;
    Icon getIcon() const override;
    void changePanel() override;

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComboBoxMappingItem);
};

/* =========================================================================
 * TabbedComponentMappingItem: TreeViewItem for TabbedComponent Mappings
 */
class TabbedComponentMappingItem : public MappingItem
{
public:
    TabbedComponentMappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um);

    var  getDragSourceDescription() override;
    Icon getIcon() const override;
    void changePanel() override;

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabbedComponentMappingItem);
};

/* =========================================================================
 * TextButtonMappingItem: TreeViewItem for TextButton Mappings
 */
class TextButtonMappingItem : public MappingItem
{
public:
    TextButtonMappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um);

    var  getDragSourceDescription() override;
    Icon getIcon() const override;
    void changePanel() override;

private:
    String getDisplayName() const override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextButtonMappingItem);
};

/* =========================================================================
 * TreeItemComponent: Component to display as the TreeViewItem
 */
class TreeItemComponent : public Component
{
public:
    TreeItemComponent(ConfigurationItem& i);

    void paint (Graphics& g) override;
    void paintIcon (Graphics& g);
    void resized() override;

    ConfigurationItem& item;

private:    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TreeItemComponent);

};

#endif  // CONFIGURATIONTREEITEM_H_INCLUDED

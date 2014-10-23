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

#include "ConfigurationTreeItem.h"
#include "ConfigurationTree.h"
#include "ConfigurationManagerMain.h"
#include "Configuration.h"
#include "ConfigurationPanel.h"
#include "../Core/Global.h"
#include "../Core/ScopeSyncApplication.h"

/* =========================================================================
 * TreeItemComponent: Component to display as the TreeViewItem
 */
class TreeItemComponent : public Component
{
public:
    TreeItemComponent(ConfigurationItem& i) : item (i)
    {
        setInterceptsMouseClicks (false, true);
    }

    void paint (Graphics& g) override
    {
        g.setColour(Colours::black);
        paintIcon(g);
        item.paintContent(g, Rectangle<int>(item.textX, 0, getWidth() - item.textX, getHeight()));
    }

    void paintIcon (Graphics& g)
    {
        item.getIcon().draw(g, Rectangle<float> (4.0f, 2.0f, item.getIconSize(), getHeight() - 4.0f),
                                item.isIconCrossedOut());
    }

    void resized() override { item.textX = (int)item.getIconSize() + 8; }

    ConfigurationItem& item;

private:    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TreeItemComponent);

};

/* =========================================================================
 * ParameterRootItem: Parameter root node TreeViewItems
 */
class ParameterRootItem : public ConfigurationItem
{
public:
    ParameterRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : ConfigurationItem(cmm, v, um) {}

    var  getDragSourceDescription() override { return "Parameter Root Item"; }

    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails) override
    {
        return dragSourceDetails.description.toString() == "Parameter";
    }
    
    Icon getIcon() const override;

    void addItem() override;
    void addItemFromClipboard() override;

    bool canPasteItem() override { return ParameterClipboard::getInstance()->clipboardIsNotEmpty(); }

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
    ParameterItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
         : ConfigurationItem(cmm, v, um) {}

    bool mightContainSubItems() override { return false; }
    var  getDragSourceDescription() override { return "Parameter"; }
    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails& /* dragSourceDetails */) override { return false; }
    
    virtual Icon getIcon() const override = 0;
    virtual String getDisplayName() const override;

    void copyItem() override;
    void pasteItem() override;
    bool canPasteItem() override { return ParameterClipboard::getInstance()->clipboardIsNotEmpty(); }

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
    HostParameterItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : ParameterItem(cmm, v, um) {}

    Icon getIcon() const override { return Icon(Icons::getInstance()->hostparameter, Colours::grey); }
    
    String getDisplayName() const override
    {
        String displayName = ParameterItem::getDisplayName();
    
        if (int(tree[Ids::scopeSync]) != -1)
            displayName += " - " + ScopeSync::getScopeSyncCode(int(tree[Ids::scopeSync]));

        return displayName;
    }

    void changePanel() override
    {
        configurationManagerMain.changePanel(new ParameterPanel(tree, undoManager, BCMParameter::hostParameter, configurationManagerMain.getScopeSync(), commandManager));
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HostParameterItem);
};

/* =========================================================================
 * ScopeParameterItem: Scope Parameter instance TreeViewItems
 */
class ScopeParameterItem  : public ParameterItem
{
public:
    ScopeParameterItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : ParameterItem(cmm, v, um) {}

    Icon getIcon() const override { return Icon(Icons::getInstance()->scopeparameter, Colours::grey); }
    
    String getDisplayName() const override
    {
        String displayName = ParameterItem::getDisplayName();
    
        if (int(tree[Ids::scopeLocal]) != -1)
            displayName += " - " + ScopeSync::getScopeLocalCode(int(tree[Ids::scopeLocal]));

        return displayName;
    }

    void changePanel() override
    {
        configurationManagerMain.changePanel(new ParameterPanel(tree, undoManager, BCMParameter::scopeLocal, configurationManagerMain.getScopeSync(), commandManager));
    }

private:    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeParameterItem);
};

/* =========================================================================
 * MappingItem: Base class for Mapping-based TreeViewItems
 */
class MappingItem : public ConfigurationItem
{
public:
    MappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : ConfigurationItem(cmm, v, um) {}

    bool mightContainSubItems() override { return false; }
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
    SliderMappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : MappingItem(cmm, v, um) {}

    var  getDragSourceDescription() override { return "SliderMapping"; }
    
    Icon getIcon() const override { return Icon(Icons::getInstance()->slider, Colours::grey); }
    
    void changePanel() override
    {
        configurationManagerMain.changePanel(new MappingPanel(tree, undoManager, configurationManagerMain.getScopeSync(), commandManager, Ids::slider));
    }

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderMappingItem);
};

/* =========================================================================
 * LabelMappingItem: TreeViewItem for Label Mappings
 */
class LabelMappingItem : public MappingItem
{
public:
    LabelMappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : MappingItem(cmm, v, um) {}

    var  getDragSourceDescription() override { return "LabelMapping"; }

    Icon getIcon() const override { return Icon(Icons::getInstance()->label, Colours::grey); }
    
    void changePanel() override
    {
        configurationManagerMain.changePanel(new MappingPanel(tree, undoManager, configurationManagerMain.getScopeSync(), commandManager, Ids::label));
    }

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabelMappingItem);
};

/* =========================================================================
 * ComboBoxMappingItem: TreeViewItem for ComboBox Mappings
 */
class ComboBoxMappingItem : public MappingItem
{
public:
    ComboBoxMappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : MappingItem(cmm, v, um) {}

    var  getDragSourceDescription() override { return "ComboBoxMapping"; }

    Icon getIcon() const override { return Icon(Icons::getInstance()->combobox, Colours::grey); }
    
    void changePanel() override
    {
        configurationManagerMain.changePanel(new MappingPanel(tree, undoManager, configurationManagerMain.getScopeSync(), commandManager, Ids::comboBox));
    }

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComboBoxMappingItem);
};

/* =========================================================================
 * TabbedComponentMappingItem: TreeViewItem for TabbedComponent Mappings
 */
class TabbedComponentMappingItem : public MappingItem
{
public:
    TabbedComponentMappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : MappingItem(cmm, v, um) {}

    var  getDragSourceDescription() override { return "TabbedComponentMapping"; }

    Icon getIcon() const override { return Icon(Icons::getInstance()->tabbedcomponent, Colours::grey); }
    
    void changePanel() override
    {
        configurationManagerMain.changePanel(new MappingPanel(tree, undoManager, configurationManagerMain.getScopeSync(), commandManager, Ids::tabbedComponent));
    }

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabbedComponentMappingItem);
};

/* =========================================================================
 * TextButtonMappingItem: TreeViewItem for TextButton Mappings
 */
class TextButtonMappingItem : public MappingItem
{
public:
    TextButtonMappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : MappingItem(cmm, v, um) {}

    var  getDragSourceDescription() override { return "TextButtonMapping"; }
    Icon getIcon() const override { return Icon(Icons::getInstance()->textbutton, Colours::grey); }
    void changePanel() override
    {
        configurationManagerMain.changePanel(new TextButtonMappingPanel(tree, undoManager, configurationManagerMain.getScopeSync(), commandManager));
    }

private:
    String getDisplayName() const override
    {
        String source = tree[Ids::name].toString();
        String target = tree[Ids::mapTo].toString();
        String displayName;

        if (source.isEmpty() && target.isEmpty())
        {
            displayName = "Empty Mapping (" + String(getRowNumberInTree()) + ")";
        }
        else
        {
            displayName = source;
            String settings = tree[Ids::settingDown].toString();

            if (tree.hasProperty(Ids::settingUp))
            {
                settings += "/" + tree[Ids::settingUp].toString();
            }

            displayName += " => " + target + " (" + settings + ")";
        }
    
        return displayName;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextButtonMappingItem);
};

/* =========================================================================
 * MappingRootItem: Mapping root node TreeViewItems
 */
class MappingRootItem : public ConfigurationItem
{
public:
    MappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : ConfigurationItem(cmm, v, um) {}

    virtual var  getDragSourceDescription() override { return "Mapping Root Item"; }
    virtual Icon getIcon() const override { return Icon(Icons::getInstance()->mapping, Colours::bisque); }

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
    SliderMappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : MappingRootItem(cmm, v, um) {}
    
    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails) override
    { 
        return dragSourceDetails.description.toString() == "SliderMapping";
    }

    Icon getIcon() const override { return Icon(Icons::getInstance()->sliders, Colours::bisque); }
    void addItem() override { addGenericMapping(Ids::slider); }
    
private:
    void refreshSubItems() override
    {
        WholeTreeOpennessRestorer wtor(*this);
        clearSubItems();

        for (int i = 0; i < tree.getNumChildren(); ++i)
            addSubItem(new SliderMappingItem(configurationManagerMain, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderMappingRootItem);
};

/* =========================================================================
 * LabelMappingRootItem: Label Mapping root node TreeViewItem
 */
class LabelMappingRootItem : public MappingRootItem
{
public:
    LabelMappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : MappingRootItem(cmm, v, um) {}
    
    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails) override
    {
        return dragSourceDetails.description.toString() == "LabelMapping";
    }

    Icon getIcon() const override { return Icon(Icons::getInstance()->labels, Colours::bisque); }
    void addItem() override { addGenericMapping(Ids::label); }
    
private:
    void refreshSubItems() override
    {
        WholeTreeOpennessRestorer wtor(*this);
        clearSubItems();

        for (int i = 0; i < tree.getNumChildren(); ++i)
            addSubItem(new LabelMappingItem(configurationManagerMain, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabelMappingRootItem);
};

/* =========================================================================
 * ComboBoxMappingRootItem: ComboBox Mapping root node TreeViewItem
 */
class ComboBoxMappingRootItem : public MappingRootItem
{
public:
    ComboBoxMappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : MappingRootItem(cmm, v, um) {}

    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails) override
    {
        return dragSourceDetails.description.toString() == "ComboBoxMapping";
    }

    Icon getIcon() const override { return Icon(Icons::getInstance()->comboboxes, Colours::bisque); }
    void addItem() override { addGenericMapping(Ids::comboBox); }
    
private:
    void refreshSubItems() override
    {
        WholeTreeOpennessRestorer wtor(*this);
        clearSubItems();

        for (int i = 0; i < tree.getNumChildren(); ++i)
            addSubItem(new ComboBoxMappingItem(configurationManagerMain, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComboBoxMappingRootItem);
};

/* =========================================================================
 * TabbedComponentMappingRootItem: TabbedComponent Mapping root node TreeViewItem
 */
class TabbedComponentMappingRootItem : public MappingRootItem
{
public:
    TabbedComponentMappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : MappingRootItem(cmm, v, um) {}    

    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails) override
    {
        return dragSourceDetails.description.toString() == "TabbedComponentMapping";
    }

    Icon getIcon() const override { return Icon(Icons::getInstance()->tabbedcomponents, Colours::bisque); }
    void addItem() override { addGenericMapping(Ids::tabbedComponent); }
    
private:
    void refreshSubItems() override
    {
        WholeTreeOpennessRestorer wtor(*this);
        clearSubItems();

        for (int i = 0; i < tree.getNumChildren(); ++i)
            addSubItem(new TabbedComponentMappingItem(configurationManagerMain, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabbedComponentMappingRootItem);
};

/* =========================================================================
 * TextButtonMappingRootItem: TextButton Mapping root node TreeViewItem
 */
class TextButtonMappingRootItem : public MappingRootItem
{
public:
    TextButtonMappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : MappingRootItem(cmm, v, um) {}
    
    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails) override
    {
        return dragSourceDetails.description.toString() == "TextButtonMapping";
    }

    Icon getIcon() const override { return Icon(Icons::getInstance()->textbuttons, Colours::bisque); }
    void addItem() override { addGenericMapping(Ids::textButton); }
    
private:
    void refreshSubItems() override
    {
        WholeTreeOpennessRestorer wtor(*this);
        clearSubItems();

        for (int i = 0; i < tree.getNumChildren(); ++i)
            addSubItem(new TextButtonMappingItem(configurationManagerMain, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextButtonMappingRootItem);
};

/* =========================================================================
 * StyleOverrideItem: Base class for Style Override TreeViewItems
 */
class StyleOverrideItem : public ConfigurationItem
{
public:
    StyleOverrideItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : ConfigurationItem(cmm, v, um) {}

    bool mightContainSubItems() override { return false; }
    virtual var getDragSourceDescription() override;
    virtual bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override;

    void copyItem() override;
    void pasteItem() override;
    bool canPasteItem() override { return StyleOverrideClipboard::getInstance()->clipboardIsNotEmpty(); }

    void deleteItem() override;
    void addItem() override;
    void addItemFromClipboard() override;
    
    virtual Icon getIcon() const override { return Icon(); }
    virtual String getDisplayName() const override;

private:
    void refreshSubItems() override;
    
    void showPopupMenu() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StyleOverrideItem);
};

/* =========================================================================
 * SliderStyleOverrideItem: TreeViewItem for Slider Style Overrides
 */
class SliderStyleOverrideItem : public StyleOverrideItem
{
public:
    SliderStyleOverrideItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : StyleOverrideItem(cmm, v, um) {}

    var  getDragSourceDescription() override { return "SliderStyleOverride"; };
    
    Icon getIcon() const override { return Icon(Icons::getInstance()->slider, Colours::grey); }
    
    void changePanel() override
    {
        configurationManagerMain.changePanel(new StyleOverridePanel(tree, undoManager, configurationManagerMain.getScopeSync(), commandManager, Ids::slider));
    }

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderStyleOverrideItem);
};

/* =========================================================================
 * LabelStyleOverrideItem: TreeViewItem for Label Style Overrides
 */
class LabelStyleOverrideItem : public StyleOverrideItem
{
public:
    LabelStyleOverrideItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : StyleOverrideItem(cmm, v, um) {}

    var  getDragSourceDescription() override { return "LabelStyleOverride"; };
    
    Icon getIcon() const override { return Icon(Icons::getInstance()->label, Colours::grey); }
    
    void changePanel() override
    {
        configurationManagerMain.changePanel(new StyleOverridePanel(tree, undoManager, configurationManagerMain.getScopeSync(), commandManager, Ids::label));
    }

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabelStyleOverrideItem);
};

/* =========================================================================
 * ComboBoxStyleOverrideItem: TreeViewItem for ComboBox Style Overrides
 */
class ComboBoxStyleOverrideItem : public StyleOverrideItem
{
public:
    ComboBoxStyleOverrideItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : StyleOverrideItem(cmm, v, um) {}

    var  getDragSourceDescription() override { return "ComboBoxStyleOverride"; };
    
    Icon getIcon() const override { return Icon(Icons::getInstance()->combobox, Colours::grey); }
    
    void changePanel() override
    {
        configurationManagerMain.changePanel(new StyleOverridePanel(tree, undoManager, configurationManagerMain.getScopeSync(), commandManager, Ids::comboBox));
    }

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComboBoxStyleOverrideItem);
};

/* =========================================================================
 * TabbedComponentStyleOverrideItem: TreeViewItem for TabbedComponent Style Overrides
 */
class TabbedComponentStyleOverrideItem : public StyleOverrideItem
{
public:
    TabbedComponentStyleOverrideItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : StyleOverrideItem(cmm, v, um) {}

    var  getDragSourceDescription() override { return "TabbedComponentStyleOverride"; };
    
    Icon getIcon() const override { return Icon(Icons::getInstance()->tabbedcomponent, Colours::grey); }
    
    void changePanel() override
    {
        configurationManagerMain.changePanel(new StyleOverridePanel(tree, undoManager, configurationManagerMain.getScopeSync(), commandManager, Ids::tabbedComponent));
    }

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabbedComponentStyleOverrideItem);
};

/* =========================================================================
 * TextButtonStyleOverrideItem: TreeViewItem for TextButton Style Overrides
 */
class TextButtonStyleOverrideItem : public StyleOverrideItem
{
public:
    TextButtonStyleOverrideItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : StyleOverrideItem(cmm, v, um) {}

    var  getDragSourceDescription() override { return "TextButtonStyleOverride"; };
    
    Icon getIcon() const override { return Icon(Icons::getInstance()->textbutton, Colours::grey); }
    
    void changePanel() override
    {
        configurationManagerMain.changePanel(new StyleOverridePanel(tree, undoManager, configurationManagerMain.getScopeSync(), commandManager, Ids::textButton));
    }

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextButtonStyleOverrideItem);
};

/* =========================================================================
 * ComponentStyleOverrideItem: TreeViewItem for Component Style Overrides
 */
class ComponentStyleOverrideItem : public StyleOverrideItem
{
public:
    ComponentStyleOverrideItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : StyleOverrideItem(cmm, v, um) {}

    var  getDragSourceDescription() override { return "ComponentStyleOverride"; };
    
    Icon getIcon() const override { return Icon(Icons::getInstance()->component, Colours::grey); }
    
    void changePanel() override
    {
        configurationManagerMain.changePanel(new StyleOverridePanel(tree, undoManager, configurationManagerMain.getScopeSync(), commandManager, Ids::component));
    }

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComponentStyleOverrideItem);
};

/* =========================================================================
 * StyleOverrideRootItem: Style Override root node TreeViewItem
 */
class StyleOverrideRootItem : public ConfigurationItem
{
public:
    StyleOverrideRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : ConfigurationItem(cmm, v, um) {}

    virtual var  getDragSourceDescription() override { return "StyleOverrideRoot"; }
    virtual bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& /* dragSourceDetails */) override { return false; }
    
    Icon getIcon() const override { return Icon(Icons::getInstance()->styleoverrides, Colours::lightblue); }
    
    bool canPasteItem() override { return StyleOverrideClipboard::getInstance()->clipboardIsNotEmpty(); }

protected:
    void addGenericStyleOverride(const Identifier& componentType);
    void addGenericItemFromClipboard(const Identifier& componentType);

private:
    virtual void refreshSubItems() override;
    
    void showPopupMenu() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StyleOverrideRootItem);
};

/* =========================================================================
 * SliderStyleOverrideRootItem: Slider Style Override root node TreeViewItem
 */
class SliderStyleOverrideRootItem : public StyleOverrideRootItem
{
public:
    SliderStyleOverrideRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : StyleOverrideRootItem(cmm, v, um) {}
    
    bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override
    { 
        return dragSourceDetails.description.toString() == "SliderStyleOverride";
    }

    Icon getIcon() const override { return Icon(Icons::getInstance()->sliders, Colours::lightblue); }
    void addItem() override { addGenericStyleOverride(Ids::slider); }
    void addItemFromClipboard() override { addGenericItemFromClipboard(Ids::slider); }
    
private:
    void refreshSubItems() override
    {
        WholeTreeOpennessRestorer wtor(*this);
        clearSubItems();

        for (int i = 0; i < tree.getNumChildren(); ++i)
            addSubItem(new SliderStyleOverrideItem(configurationManagerMain, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderStyleOverrideRootItem);
};

/* =========================================================================
 * LabelStyleOverrideRootItem: Label Style Override root node TreeViewItem
 */
class LabelStyleOverrideRootItem : public StyleOverrideRootItem
{
public:
    LabelStyleOverrideRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : StyleOverrideRootItem(cmm, v, um) {}
    
    bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override
    { 
        return dragSourceDetails.description.toString() == "LabelStyleOverride";
    }

    Icon getIcon() const override { return Icon(Icons::getInstance()->labels, Colours::lightblue); }
    void addItem() override { addGenericStyleOverride(Ids::label); }
    void addItemFromClipboard() override { addGenericItemFromClipboard(Ids::label); }
    
private:
    void refreshSubItems() override
    {
        WholeTreeOpennessRestorer wtor(*this);
        clearSubItems();

        for (int i = 0; i < tree.getNumChildren(); ++i)
            addSubItem(new LabelStyleOverrideItem(configurationManagerMain, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabelStyleOverrideRootItem);
};

/* =========================================================================
 * ComboBoxStyleOverrideRootItem: ComboBox Style Override root node TreeViewItem
 */
class ComboBoxStyleOverrideRootItem : public StyleOverrideRootItem
{
public:
    ComboBoxStyleOverrideRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : StyleOverrideRootItem(cmm, v, um) {}
    
    bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override
    { 
        return dragSourceDetails.description.toString() == "ComboBoxStyleOverride";
    }

    Icon getIcon() const override { return Icon(Icons::getInstance()->comboboxes, Colours::lightblue); }
    void addItem() override { addGenericStyleOverride(Ids::comboBox); }
    void addItemFromClipboard() override { addGenericItemFromClipboard(Ids::comboBox); }
    
private:
    void refreshSubItems() override
    {
        WholeTreeOpennessRestorer wtor(*this);
        clearSubItems();

        for (int i = 0; i < tree.getNumChildren(); ++i)
            addSubItem(new ComboBoxStyleOverrideItem(configurationManagerMain, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComboBoxStyleOverrideRootItem);
};

/* =========================================================================
 * TabbedComponentStyleOverrideRootItem: TabbedComponent Style Override root node TreeViewItem
 */
class TabbedComponentStyleOverrideRootItem : public StyleOverrideRootItem
{
public:
    TabbedComponentStyleOverrideRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : StyleOverrideRootItem(cmm, v, um) {}
    
    bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override
    { 
        return dragSourceDetails.description.toString() == "TabbedComponentStyleOverride";
    }

    Icon getIcon() const override { return Icon(Icons::getInstance()->tabbedcomponents, Colours::lightblue); }
    void addItem() override { addGenericStyleOverride(Ids::tabbedComponent); }
    void addItemFromClipboard() override { addGenericItemFromClipboard(Ids::tabbedComponent); }
    
private:
    void refreshSubItems() override
    {
        WholeTreeOpennessRestorer wtor(*this);
        clearSubItems();

        for (int i = 0; i < tree.getNumChildren(); ++i)
            addSubItem(new TabbedComponentStyleOverrideItem(configurationManagerMain, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabbedComponentStyleOverrideRootItem);
};

/* =========================================================================
 * TextButtonStyleOverrideRootItem: TextButton Style Override root node TreeViewItem
 */
class TextButtonStyleOverrideRootItem : public StyleOverrideRootItem
{
public:
    TextButtonStyleOverrideRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : StyleOverrideRootItem(cmm, v, um) {}
    
    bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override
    { 
        return dragSourceDetails.description.toString() == "TextButtonStyleOverride";
    }

    Icon getIcon() const override { return Icon(Icons::getInstance()->textbuttons, Colours::lightblue); }
    void addItem() override { addGenericStyleOverride(Ids::textButton); }
    void addItemFromClipboard() override { addGenericItemFromClipboard(Ids::textButton); }
    
private:
    void refreshSubItems() override
    {
        WholeTreeOpennessRestorer wtor(*this);
        clearSubItems();

        for (int i = 0; i < tree.getNumChildren(); ++i)
            addSubItem(new TextButtonStyleOverrideItem(configurationManagerMain, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextButtonStyleOverrideRootItem);
};

/* =========================================================================
 * ComponentStyleOverrideRootItem: Component Style Override root node TreeViewItem
 */
class ComponentStyleOverrideRootItem : public StyleOverrideRootItem
{
public:
    ComponentStyleOverrideRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
        : StyleOverrideRootItem(cmm, v, um) {}
    
    bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override
    { 
        return dragSourceDetails.description.toString() == "ComponentStyleOverride";
    }

    Icon getIcon() const override { return Icon(Icons::getInstance()->components, Colours::lightblue); }
    void addItem() override { addGenericStyleOverride(Ids::component); }
    void addItemFromClipboard() override { addGenericItemFromClipboard(Ids::component); }
    
private:
    void refreshSubItems() override
    {
        WholeTreeOpennessRestorer wtor(*this);
        clearSubItems();

        for (int i = 0; i < tree.getNumChildren(); ++i)
            addSubItem(new ComponentStyleOverrideItem(configurationManagerMain, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComponentStyleOverrideRootItem);
};

/* =========================================================================
 * ConfigurationTreeItem
 */
ConfigurationItem::ConfigurationItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : configurationManagerMain(cmm),
      configuration(configurationManagerMain.getConfiguration()),
      tree(v),
      undoManager(um),
      textX(0),
      commandManager(configurationManagerMain.getCommandManager())
{
    tree.addListener(this);
}

ConfigurationItem::~ConfigurationItem()
{
    masterReference.clear();
}

String ConfigurationItem::getDisplayName() const 
{
    String displayName = tree["name"].toString();

    if (displayName.isEmpty())
        displayName = tree.getType().toString();

    return displayName;
}

String ConfigurationItem::getUniqueName() const { return getDisplayName(); }

bool ConfigurationItem::mightContainSubItems() { return tree.getNumChildren() > 0; }

Font ConfigurationItem::getFont() const { return Font (getItemHeight() * 0.7f); }

float ConfigurationItem::getIconSize() const { return jmin (getItemHeight() - 4.0f, 18.0f); }

Icon ConfigurationItem::getIcon() const { return Icon(Icons::getInstance()->config, Colours::lightgreen); }

void ConfigurationItem::paintOpenCloseButton (Graphics& g, const Rectangle<float>& area, Colour backgroundColour, bool isMouseOver)
{
    TreeViewItem::paintOpenCloseButton(g, area, backgroundColour, isMouseOver);
}

Colour ConfigurationItem::getBackgroundColour() const
{
    Colour background(Colours::darkgrey);

    if (isSelected())
        background = background.overlaidWith(getOwnerView()->findColour(TreeView::selectedItemBackgroundColourId));
     
    return background;
}

Colour ConfigurationItem::getContrastingColour(float contrast) const { return getBackgroundColour().contrasting(contrast); }

Colour ConfigurationItem::getContrastingColour(Colour target, float minContrast) const { return getBackgroundColour().contrasting(target, minContrast); }

void ConfigurationItem::paintContent(Graphics& g, const Rectangle<int>& area)
{
    g.setFont(getFont());
    g.setColour(getContrastingColour(0.8f));

    g.drawFittedText(getDisplayName(), area, Justification::centredLeft, 1, 0.8f);
}

void ConfigurationItem::itemOpennessChanged(bool isNowOpen)
{
    if (isNowOpen && getNumSubItems() == 0)
    {
        clearSubItems();
        refreshSubItems();
    }
    else
        clearSubItems();
}

var ConfigurationItem::getDragSourceDescription() { return "Configuration Root Item"; }

Component* ConfigurationItem::createItemComponent() { return new TreeItemComponent(*this); }

bool ConfigurationItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& /* dragSourceDetails */) { return false; }

void ConfigurationItem::itemDropped(const DragAndDropTarget::SourceDetails&, int insertIndex)
{
    moveItems(*getOwnerView(),
               getSelectedTreeViewItems(*getOwnerView()),
               tree, insertIndex, undoManager);
}

void ConfigurationItem::moveItems(TreeView& treeView, const Array<ValueTree>& items,
                                  ValueTree newParent, int insertIndex, UndoManager& undoManager)
{
    if (items.size() > 0)
    {
        ScopedPointer<XmlElement> oldOpenness(treeView.getOpennessState (false));

        for (int i = items.size(); --i >= 0;)
        {
            ValueTree& v = items.getReference(i);

            if (v.getParent().isValid() && newParent != v && ! newParent.isAChildOf (v))
            {
                v.getParent().removeChild (v, &undoManager);
                newParent.addChild(v, insertIndex, &undoManager);
            }
        }

        if (oldOpenness != nullptr)
            treeView.restoreOpennessState (*oldOpenness, false);
    }
}

Array<ValueTree> ConfigurationItem::getSelectedTreeViewItems (TreeView& treeView)
{
    Array<ValueTree> items;

    const int numSelected = treeView.getNumSelectedItems();

    for (int i = 0; i < numSelected; ++i)
        if (const ConfigurationItem* vti = dynamic_cast<ConfigurationItem*>(treeView.getSelectedItem (i)))
            items.add (vti->tree);

    return items;
}

void ConfigurationItem::refreshSubItems()
{
    WholeTreeOpennessRestorer wtor(*this);
    
    clearSubItems();

    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        ValueTree child = tree.getChild(i);

        if (child.hasType(Ids::hostParameters) || child.hasType(Ids::scopeParameters))
        {
            addSubItem (new ParameterRootItem(configurationManagerMain, child, undoManager));
        }
        else if (child.hasType(Ids::mapping))
        {
            addSubItem (new MappingRootItem(configurationManagerMain, child, undoManager));
        }
        else if (child.hasType(Ids::styleOverrides))
        {
            addSubItem (new StyleOverrideRootItem(configurationManagerMain, child, undoManager));
        }
    }
}

void ConfigurationItem::valueTreePropertyChanged (ValueTree&, const Identifier&) { repaintItem(); }

void ConfigurationItem::treeChildrenChanged (const ValueTree& parentTree)
{
    if (parentTree == tree)
    {
        setOpen(true);
        refreshSubItems();

        ConfigurationTree* configTree = dynamic_cast<ConfigurationTree*>(getOwnerView()->getParentComponent());
        configTree->moveToSelectedItem();
    }
}
        
void ConfigurationItem::itemClicked(const MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
        if (getOwnerView()->getNumSelectedItems() > 1)
        {
            showMultiSelectionPopupMenu();
        }
        else
        {
            showPopupMenu();
        }
    }
    else if (isSelected())
    {
        itemSelectionChanged(true);
    }
}

void ConfigurationItem::showMultiSelectionPopupMenu()
{
    PopupMenu m;
    m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::deleteItems);
    m.showMenuAsync(PopupMenu::Options(), nullptr);
}

void ConfigurationItem::deleteAllSelectedItems()
{
    TreeView* treeView = getOwnerView();
    StringArray identifiers;

    for (int i = 0; i < treeView->getNumSelectedItems(); i++)
        identifiers.add(treeView->getSelectedItem(i)->getItemIdentifierString());

    for (int i = 0; i < identifiers.size(); i++)
        dynamic_cast<ConfigurationItem*>(treeView->findItemFromIdentifierString(identifiers[i]))->deleteItem();
}

class ConfigurationItem::ItemSelectionTimer : public Timer
{
public:
    ItemSelectionTimer(ConfigurationItem& ci) : owner (ci) {}

    void timerCallback() override { owner.invokeChangePanel(); }

private:
    ConfigurationItem& owner;
    JUCE_DECLARE_NON_COPYABLE (ItemSelectionTimer)
};

void ConfigurationItem::itemSelectionChanged(bool isNowSelected)
{
    DBG("ConfigurationItem::itemSelectionChanged - isNowSelected: " + String(isNowSelected) + ", item: " + getUniqueName());

    if (isNowSelected)
    {
        delayedSelectionTimer = new ItemSelectionTimer(*this);
        delayedSelectionTimer->startTimer(getMillisecsAllowedForDragGesture());
    }
    else
    {
        cancelDelayedSelectionTimer();
    }
}

void ConfigurationItem::invokeChangePanel()
{
    cancelDelayedSelectionTimer();
    changePanel();
}

void ConfigurationItem::itemDoubleClicked(const MouseEvent&)
{
    invokeChangePanel();
}

void ConfigurationItem::cancelDelayedSelectionTimer()
{
    delayedSelectionTimer = nullptr;
}

void ConfigurationItem::changePanel()
{
    if (tree.hasType(Ids::configuration))
        configurationManagerMain.changePanel(new ConfigurationPanel(tree, undoManager, configurationManagerMain.getScopeSync(), commandManager));
    else
        configurationManagerMain.changePanel(new EmptyPanel(tree, undoManager, configurationManagerMain.getScopeSync(), commandManager));
}

void ConfigurationItem::storeSelectionOnAdd()
{
    DBG("ConfigurationItem::storeSelectionOnAdd - Row Number: " + String(getRowNumberInTree() + 1));

    ConfigurationTree* configTree = dynamic_cast<ConfigurationTree*> (getOwnerView()->getParentComponent());
    configTree->storeSelectedItem(getRowNumberInTree() + 1);
}

void ConfigurationItem::storeSelectionOnDelete()
{
    int rowNumber = getRowNumberInTree();

    if (getIndexInParent() == getParentItem()->getNumSubItems() - 1)
    {
        rowNumber -= 1;
    }

    DBG("ConfigurationItem::storeSelectionOnDelete - Row Number: " + String(rowNumber));

    ConfigurationTree* configTree = dynamic_cast<ConfigurationTree*> (getOwnerView()->getParentComponent());
    configTree->storeSelectedItem(rowNumber);
}

/* =========================================================================
 * ParameterRootItem
 */
Icon ParameterRootItem::getIcon() const
{
    if (tree.hasType(Ids::hostParameters))
        return Icon(Icons::getInstance()->hostparameters, Colours::aliceblue);
    else if (tree.hasType(Ids::scopeParameters))
        return Icon(Icons::getInstance()->scopeparameters, Colours::azure);
    else
        return Icon();
}

void ParameterRootItem::showPopupMenu()
{
    PopupMenu m;
    m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::addItem, "Add Parameter");
    m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::addItemFromClipboard, "Add Parameter from clipboard");
    
    m.showMenuAsync (PopupMenu::Options(), nullptr);
}

void ParameterRootItem::refreshSubItems()
{
    WholeTreeOpennessRestorer wtor(*this);
    
    clearSubItems();

    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        if (tree.hasType(Ids::hostParameters))
        {
            addSubItem(new HostParameterItem(configurationManagerMain, tree.getChild(i), undoManager));
        }
        else if (tree.hasType(Ids::scopeParameters))
        {
            addSubItem(new ScopeParameterItem(configurationManagerMain, tree.getChild(i), undoManager));
        }
    }
}

void ParameterRootItem::addItem()
{
    ValueTree definition;
    addParameter(definition);
}

void ParameterRootItem::addItemFromClipboard()
{
    ValueTree definition;

    if (ParameterClipboard::getInstance()->clipboardIsNotEmpty())
    {
        definition = ValueTree(Ids::parameter);
        ParameterClipboard::getInstance()->paste(definition, nullptr);
    }

    addParameter(definition);
}

void ParameterRootItem::addParameter(const ValueTree& definition)
{
    storeSelectionOnAdd();
    
    Configuration::ParameterTarget parameterTarget;

    if (tree.hasType(Ids::hostParameters))
        parameterTarget = Configuration::host;
    else
        parameterTarget = Configuration::scopeLocal;
    
    configurationManagerMain.getConfiguration().addNewParameter(definition, 0, parameterTarget, &undoManager);
}

/* =========================================================================
 * ParameterItem
 */
String ParameterItem::getDisplayName() const 
{
    String displayName = tree[Ids::name].toString();
    displayName += " (" + tree[Ids::fullDescription].toString() + ")";
    
    return displayName;
}

void ParameterItem::showPopupMenu()
{
    PopupMenu m;
    m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::addItem, "Add Parameter");
    m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::addItemFromClipboard, "Add Parameter from clipboard");
    m.addSeparator();
    m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::copyItem, "Copy Parameter Definition");
    m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::pasteItem, "Paste Parameter Definition");
    m.addSeparator();
    m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::deleteItems, "Delete Parameter");
    
    m.showMenuAsync (PopupMenu::Options(), nullptr);
}

void ParameterItem::refreshSubItems() {}

void ParameterItem::deleteItem()
{
    storeSelectionOnDelete();
    tree.getParent().removeChild(tree, &undoManager);
}

void ParameterItem::addItem()
{
    ValueTree definition;
    insertParameterAt(definition, tree.getParent().indexOf(tree) + 1);
}

void ParameterItem::addItemFromClipboard()
{
    ValueTree definition;
    
    if (ParameterClipboard::getInstance()->clipboardIsNotEmpty())
    {
        definition = ValueTree(Ids::parameter);
        ParameterClipboard::getInstance()->paste(definition, nullptr);
    }
    
    insertParameterAt(definition, tree.getParent().indexOf(tree) + 1);
}

void ParameterItem::insertParameterAt(const ValueTree& definition, int index)
{
    storeSelectionOnAdd();
    
    Configuration::ParameterTarget parameterTarget;

    if (tree.getParent().hasType(Ids::hostParameters))
        parameterTarget = Configuration::host;
    else
        parameterTarget = Configuration::scopeLocal;
    
    configurationManagerMain.getConfiguration().addNewParameter(definition, index, parameterTarget, &undoManager);
}

void ParameterItem::copyItem()
{
    ParameterClipboard::getInstance()->copy(tree);
}

void ParameterItem::pasteItem()
{
    storeSelectionOnDelete();
    ParameterClipboard::getInstance()->paste(tree, &undoManager);
    changePanel();
}

/* =========================================================================
 * MappingRootItem
 */
void MappingRootItem::refreshSubItems()
{
    WholeTreeOpennessRestorer wtor(*this);
    
    clearSubItems();

    for (int i = 0; i < tree.getNumChildren(); i++)
    {
        ValueTree child = tree.getChild(i);

        if (child.hasType(Ids::sliders))
            addSubItem(new SliderMappingRootItem(configurationManagerMain, child, undoManager));
        else if (child.hasType(Ids::labels))
            addSubItem(new LabelMappingRootItem(configurationManagerMain, child, undoManager));
        else if (child.hasType(Ids::comboBoxes))
            addSubItem(new ComboBoxMappingRootItem(configurationManagerMain, child, undoManager));
        else if (child.hasType(Ids::tabbedComponents))
            addSubItem(new TabbedComponentMappingRootItem(configurationManagerMain, child, undoManager));
        else if (child.hasType(Ids::textButtons))
            addSubItem(new TextButtonMappingRootItem(configurationManagerMain, child, undoManager));
    }
}

void MappingRootItem::showPopupMenu()
{
    if (!(tree.hasType(Ids::mapping)))
    {
        PopupMenu m;
        m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::addItem, "Add Mapping");
    
        m.showMenuAsync (PopupMenu::Options(), nullptr);
    }
}

void MappingRootItem::addGenericMapping(const Identifier& mappingType)
{
    storeSelectionOnAdd();
    
    ValueTree newMapping;
    configuration.addNewMapping(mappingType, String::empty, String::empty, newMapping, 0, &undoManager);
}

/* =========================================================================
 * MappingItem
 */
var MappingItem::getDragSourceDescription()
{
    return "Mapping";
}

bool MappingItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& /* dragSourceDetails */)
{
    return false;
}

String MappingItem::getDisplayName() const 
{
    String source = tree[Ids::name].toString();
    String target = tree[Ids::mapTo].toString();
    String displayName;

    if (source.isEmpty() && target.isEmpty())
    {
        displayName = "Empty Mapping (" + String(getRowNumberInTree()) + ")";
    }
    else
    {
        displayName = tree[Ids::name].toString();
        displayName += " => " + tree[Ids::mapTo].toString();
    }
    
    return displayName;
}

void MappingItem::showPopupMenu()
{
    PopupMenu m;
    m.addCommandItem(commandManager, CommandIDs::addItem, "Add Mapping");
    m.addSeparator();
    m.addCommandItem(commandManager, CommandIDs::deleteItems, "Delete Mapping");
    m.showMenuAsync (PopupMenu::Options(), nullptr);
}

void MappingItem::refreshSubItems() {}

void MappingItem::deleteItem()
{
    storeSelectionOnDelete();
    tree.getParent().removeChild(tree, &undoManager);
}

void MappingItem::addItem()
{
    storeSelectionOnAdd();

    ValueTree newMapping;
    configuration.addNewMapping(tree.getType(), String::empty, String::empty, newMapping, tree.getParent().indexOf(tree) + 1, &undoManager);
}

/* =========================================================================
 * StyleOverrideRootItem
 */
void StyleOverrideRootItem::refreshSubItems()
{
    WholeTreeOpennessRestorer wtor(*this);
    
    clearSubItems();

    for (int i = 0; i < tree.getNumChildren(); i++)
    {
        ValueTree child = tree.getChild(i);

        if (child.hasType(Ids::labels))
            addSubItem(new LabelStyleOverrideRootItem(configurationManagerMain, child, undoManager));
        else if (child.hasType(Ids::sliders))
            addSubItem(new SliderStyleOverrideRootItem(configurationManagerMain, child, undoManager));
        else if (child.hasType(Ids::comboBoxes))
            addSubItem(new ComboBoxStyleOverrideRootItem(configurationManagerMain, child, undoManager));
        else if (child.hasType(Ids::tabbedComponents))
            addSubItem(new TabbedComponentStyleOverrideRootItem(configurationManagerMain, child, undoManager));
        else if (child.hasType(Ids::textButtons))
            addSubItem(new TextButtonStyleOverrideRootItem(configurationManagerMain, child, undoManager));
        else if (child.hasType(Ids::components))
            addSubItem(new ComponentStyleOverrideRootItem(configurationManagerMain, child, undoManager));
    }
}

void StyleOverrideRootItem::showPopupMenu()
{
    if (!tree.hasType(Ids::styleOverrides))
    {
        PopupMenu m;
        m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::addItem, "Add Style Override");
        m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::addItemFromClipboard, "Add Style Override from clipboard");
    
        m.showMenuAsync (PopupMenu::Options(), nullptr);
    }
}

void StyleOverrideRootItem::addGenericStyleOverride(const Identifier& componentType)
{
    storeSelectionOnAdd();
    
    ValueTree newStyleOverride;
    configuration.addStyleOverride(componentType, String::empty, newStyleOverride, 0, &undoManager);
}

void StyleOverrideRootItem::addGenericItemFromClipboard(const Identifier& componentType)
{
    ValueTree styleOverride;
    
    if (StyleOverrideClipboard::getInstance()->clipboardIsNotEmpty())
    {
        styleOverride = ValueTree(componentType);
        StyleOverrideClipboard::getInstance()->paste(styleOverride, nullptr);
    }
    
    storeSelectionOnAdd();
    configuration.addStyleOverride(componentType, String::empty, styleOverride, 0, &undoManager);
}

/* =========================================================================
 * StyleOverrideItem
 */
var StyleOverrideItem::getDragSourceDescription()
{
    return "Style Override";
}

bool StyleOverrideItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& /* dragSourceDetails */)
{
    return false;
}

String StyleOverrideItem::getDisplayName() const 
{
    return tree[Ids::name].toString();
}

void StyleOverrideItem::showPopupMenu()
{
    PopupMenu m;
    m.addCommandItem(commandManager, CommandIDs::addItem, "Add Style Override");
    m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::addItemFromClipboard, "Add Style Override from clipboard");
    m.addSeparator();
    m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::copyItem, "Copy Style Override");
    m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::pasteItem, "Paste Style Override");
    m.addSeparator();
    m.addCommandItem(commandManager, CommandIDs::deleteItems, "Delete Style Override");
    m.showMenuAsync (PopupMenu::Options(), nullptr);
}

void StyleOverrideItem::refreshSubItems() {}

void StyleOverrideItem::deleteItem()
{
    storeSelectionOnDelete();
    tree.getParent().removeChild(tree, &undoManager);
}

void StyleOverrideItem::addItem()
{
    storeSelectionOnAdd();

    ValueTree newStyleOverride;
    configuration.addStyleOverride(tree.getType(), String::empty, newStyleOverride, tree.getParent().indexOf(tree) + 1, &undoManager);
}

void StyleOverrideItem::addItemFromClipboard()
{
    ValueTree styleOverride;
    
    if (StyleOverrideClipboard::getInstance()->clipboardIsNotEmpty())
    {
        styleOverride = ValueTree(tree.getType());
        StyleOverrideClipboard::getInstance()->paste(styleOverride, nullptr);
    }
    
    storeSelectionOnAdd();
    configuration.addStyleOverride(tree.getType(), String::empty, styleOverride, tree.getParent().indexOf(tree) + 1, &undoManager);
}

void StyleOverrideItem::copyItem()
{
    StyleOverrideClipboard::getInstance()->copy(tree);
}

void StyleOverrideItem::pasteItem()
{
    storeSelectionOnDelete();
    StyleOverrideClipboard::getInstance()->paste(tree, &undoManager);
    changePanel();
}
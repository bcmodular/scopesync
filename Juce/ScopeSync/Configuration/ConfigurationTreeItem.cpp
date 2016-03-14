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
#include "ConfigurationManager.h"
#include "Configuration.h"
#include "ConfigurationPanel.h"
#include "../Core/Global.h"
#include "../Core/ScopeSyncApplication.h"

/* =========================================================================
 * ParameterRootItem: Parameter root node TreeViewItems
 */
class ParameterRootItem : public ConfigurationItem
{
public:
    ParameterRootItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : ConfigurationItem(cm, v, um) {}

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
    void addNewSubItem() = delete;
    
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
    ParameterItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
         : ConfigurationItem(cm, v, um) {}

    bool mightContainSubItems() override { return false; }
    var  getDragSourceDescription() override { return "Parameter"; }
    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails& /* dragSourceDetails */) override { return false; }
    
    Icon getIcon() const override { return Icon(Icons::getInstance()->parameter, Colours::grey); }
    virtual String getDisplayName() const override;

    void copyItem() override;
    void pasteItem() override;
    bool canPasteItem() override { return ParameterClipboard::getInstance()->clipboardIsNotEmpty(); }

    void deleteItem() override;
    void addItem() override;
    void addItemFromClipboard() override;

    void insertParameterAt(const ValueTree& definition, int index);

	void changePanel() override;

private:
    void refreshSubItems() override;
    void showPopupMenu() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterItem);
};

/* =========================================================================
 * MappingItem: Base class for Mapping-based TreeViewItems
 */
class MappingItem : public ConfigurationItem
{
public:
    MappingItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : ConfigurationItem(cm, v, um) {}

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
    SliderMappingItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : MappingItem(cm, v, um) {}

    var  getDragSourceDescription() override { return "SliderMapping"; }
    
    Icon getIcon() const override { return Icon(Icons::getInstance()->slider, Colours::grey); }
    
    void changePanel() override
    {
        configurationManager.changePanel(new MappingPanel(tree, undoManager, configurationManager.getScopeSync(), commandManager, Ids::slider));
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
    LabelMappingItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : MappingItem(cm, v, um) {}

    var  getDragSourceDescription() override { return "LabelMapping"; }

    Icon getIcon() const override { return Icon(Icons::getInstance()->label, Colours::grey); }
    
    void changePanel() override
    {
        configurationManager.changePanel(new MappingPanel(tree, undoManager, configurationManager.getScopeSync(), commandManager, Ids::label));
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
    ComboBoxMappingItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : MappingItem(cm, v, um) {}

    var  getDragSourceDescription() override { return "ComboBoxMapping"; }

    Icon getIcon() const override { return Icon(Icons::getInstance()->combobox, Colours::grey); }
    
    void changePanel() override
    {
        configurationManager.changePanel(new MappingPanel(tree, undoManager, configurationManager.getScopeSync(), commandManager, Ids::comboBox));
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
    TabbedComponentMappingItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : MappingItem(cm, v, um) {}

    var  getDragSourceDescription() override { return "TabbedComponentMapping"; }

    Icon getIcon() const override { return Icon(Icons::getInstance()->tabbedcomponent, Colours::grey); }
    
    void changePanel() override
    {
        configurationManager.changePanel(new MappingPanel(tree, undoManager, configurationManager.getScopeSync(), commandManager, Ids::tabbedComponent));
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
    TextButtonMappingItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : MappingItem(cm, v, um) {}

    var  getDragSourceDescription() override { return "TextButtonMapping"; }
    Icon getIcon() const override { return Icon(Icons::getInstance()->textbutton, Colours::grey); }
    void changePanel() override
    {
        configurationManager.changePanel(new TextButtonMappingPanel(tree, undoManager, configurationManager.getScopeSync(), commandManager));
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
    MappingRootItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : ConfigurationItem(cm, v, um) {}

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
    SliderMappingRootItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : MappingRootItem(cm, v, um) {}
    
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
            addSubItem(new SliderMappingItem(configurationManager, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderMappingRootItem);
};

/* =========================================================================
 * LabelMappingRootItem: Label Mapping root node TreeViewItem
 */
class LabelMappingRootItem : public MappingRootItem
{
public:
    LabelMappingRootItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : MappingRootItem(cm, v, um) {}
    
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
            addSubItem(new LabelMappingItem(configurationManager, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabelMappingRootItem);
};

/* =========================================================================
 * ComboBoxMappingRootItem: ComboBox Mapping root node TreeViewItem
 */
class ComboBoxMappingRootItem : public MappingRootItem
{
public:
    ComboBoxMappingRootItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : MappingRootItem(cm, v, um) {}

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
            addSubItem(new ComboBoxMappingItem(configurationManager, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComboBoxMappingRootItem);
};

/* =========================================================================
 * TabbedComponentMappingRootItem: TabbedComponent Mapping root node TreeViewItem
 */
class TabbedComponentMappingRootItem : public MappingRootItem
{
public:
    TabbedComponentMappingRootItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : MappingRootItem(cm, v, um) {}    

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
            addSubItem(new TabbedComponentMappingItem(configurationManager, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabbedComponentMappingRootItem);
};

/* =========================================================================
 * TextButtonMappingRootItem: TextButton Mapping root node TreeViewItem
 */
class TextButtonMappingRootItem : public MappingRootItem
{
public:
    TextButtonMappingRootItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : MappingRootItem(cm, v, um) {}
    
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
            addSubItem(new TextButtonMappingItem(configurationManager, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextButtonMappingRootItem);
};

/* =========================================================================
 * StyleOverrideItem: Base class for Style Override TreeViewItems
 */
class StyleOverrideItem : public ConfigurationItem
{
public:
    StyleOverrideItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : ConfigurationItem(cm, v, um) {}

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
    SliderStyleOverrideItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : StyleOverrideItem(cm, v, um) {}

    var  getDragSourceDescription() override { return "SliderStyleOverride"; };
    
    Icon getIcon() const override { return Icon(Icons::getInstance()->slider, Colours::grey); }
    
    void changePanel() override
    {
        configurationManager.changePanel(new StyleOverridePanel(tree, undoManager, configurationManager.getScopeSync(), commandManager, Ids::slider));
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
    LabelStyleOverrideItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : StyleOverrideItem(cm, v, um) {}

    var  getDragSourceDescription() override { return "LabelStyleOverride"; };
    
    Icon getIcon() const override { return Icon(Icons::getInstance()->label, Colours::grey); }
    
    void changePanel() override
    {
        configurationManager.changePanel(new StyleOverridePanel(tree, undoManager, configurationManager.getScopeSync(), commandManager, Ids::label));
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
    ComboBoxStyleOverrideItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : StyleOverrideItem(cm, v, um) {}

    var  getDragSourceDescription() override { return "ComboBoxStyleOverride"; };
    
    Icon getIcon() const override { return Icon(Icons::getInstance()->combobox, Colours::grey); }
    
    void changePanel() override
    {
        configurationManager.changePanel(new StyleOverridePanel(tree, undoManager, configurationManager.getScopeSync(), commandManager, Ids::comboBox));
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
    TabbedComponentStyleOverrideItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : StyleOverrideItem(cm, v, um) {}

    var  getDragSourceDescription() override { return "TabbedComponentStyleOverride"; };
    
    Icon getIcon() const override { return Icon(Icons::getInstance()->tabbedcomponent, Colours::grey); }
    
    void changePanel() override
    {
        configurationManager.changePanel(new StyleOverridePanel(tree, undoManager, configurationManager.getScopeSync(), commandManager, Ids::tabbedComponent));
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
    TextButtonStyleOverrideItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : StyleOverrideItem(cm, v, um) {}

    var  getDragSourceDescription() override { return "TextButtonStyleOverride"; };
    
    Icon getIcon() const override { return Icon(Icons::getInstance()->textbutton, Colours::grey); }
    
    void changePanel() override
    {
        configurationManager.changePanel(new StyleOverridePanel(tree, undoManager, configurationManager.getScopeSync(), commandManager, Ids::textButton));
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
    ComponentStyleOverrideItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : StyleOverrideItem(cm, v, um) {}

    var  getDragSourceDescription() override { return "ComponentStyleOverride"; };
    
    Icon getIcon() const override { return Icon(Icons::getInstance()->component, Colours::grey); }
    
    void changePanel() override
    {
        configurationManager.changePanel(new StyleOverridePanel(tree, undoManager, configurationManager.getScopeSync(), commandManager, Ids::component));
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
    StyleOverrideRootItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : ConfigurationItem(cm, v, um) {}

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
    SliderStyleOverrideRootItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : StyleOverrideRootItem(cm, v, um) {}
    
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
            addSubItem(new SliderStyleOverrideItem(configurationManager, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderStyleOverrideRootItem);
};

/* =========================================================================
 * LabelStyleOverrideRootItem: Label Style Override root node TreeViewItem
 */
class LabelStyleOverrideRootItem : public StyleOverrideRootItem
{
public:
    LabelStyleOverrideRootItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : StyleOverrideRootItem(cm, v, um) {}
    
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
            addSubItem(new LabelStyleOverrideItem(configurationManager, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabelStyleOverrideRootItem);
};

/* =========================================================================
 * ComboBoxStyleOverrideRootItem: ComboBox Style Override root node TreeViewItem
 */
class ComboBoxStyleOverrideRootItem : public StyleOverrideRootItem
{
public:
    ComboBoxStyleOverrideRootItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : StyleOverrideRootItem(cm, v, um) {}
    
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
            addSubItem(new ComboBoxStyleOverrideItem(configurationManager, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComboBoxStyleOverrideRootItem);
};

/* =========================================================================
 * TabbedComponentStyleOverrideRootItem: TabbedComponent Style Override root node TreeViewItem
 */
class TabbedComponentStyleOverrideRootItem : public StyleOverrideRootItem
{
public:
    TabbedComponentStyleOverrideRootItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : StyleOverrideRootItem(cm, v, um) {}
    
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
            addSubItem(new TabbedComponentStyleOverrideItem(configurationManager, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabbedComponentStyleOverrideRootItem);
};

/* =========================================================================
 * TextButtonStyleOverrideRootItem: TextButton Style Override root node TreeViewItem
 */
class TextButtonStyleOverrideRootItem : public StyleOverrideRootItem
{
public:
    TextButtonStyleOverrideRootItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : StyleOverrideRootItem(cm, v, um) {}
    
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
            addSubItem(new TextButtonStyleOverrideItem(configurationManager, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextButtonStyleOverrideRootItem);
};

/* =========================================================================
 * ComponentStyleOverrideRootItem: Component Style Override root node TreeViewItem
 */
class ComponentStyleOverrideRootItem : public StyleOverrideRootItem
{
public:
    ComponentStyleOverrideRootItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
        : StyleOverrideRootItem(cm, v, um) {}
    
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
            addSubItem(new ComponentStyleOverrideItem(configurationManager, tree.getChild(i), undoManager));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComponentStyleOverrideRootItem);
};

/* =========================================================================
 * ConfigurationItem
 */
ConfigurationItem::ConfigurationItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um)
    : BCMTreeItem(v, um, cm.getCommandManager()),
      configurationManager(cm),
      configuration(cm.getConfiguration())
{}

ConfigurationItem::~ConfigurationItem() {}

String ConfigurationItem::getDisplayName() const 
{
    String displayName = tree["name"].toString();

    if (displayName.isEmpty())
        displayName = tree.getType().toString();

    return displayName;
}

bool ConfigurationItem::mightContainSubItems() { return tree.getNumChildren() > 0; }

Font ConfigurationItem::getFont() const { return Font (getItemHeight() * 0.7f); }

float ConfigurationItem::getIconSize() const { return jmin (getItemHeight() - 4.0f, 18.0f); }

Icon ConfigurationItem::getIcon() const { return Icon(Icons::getInstance()->config, Colours::lightgreen); }

var ConfigurationItem::getDragSourceDescription() { return "Configuration Root Item"; }

bool ConfigurationItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& /* dragSourceDetails */) { return false; }

void ConfigurationItem::refreshSubItems()
{
    WholeTreeOpennessRestorer wtor(*this);
    
    clearSubItems();

    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        ValueTree child = tree.getChild(i);

        if (child.hasType(Ids::parameters))
            addSubItem (new ParameterRootItem(configurationManager, child, undoManager));
        else if (child.hasType(Ids::mapping))
            addSubItem (new MappingRootItem(configurationManager, child, undoManager));
        else if (child.hasType(Ids::styleOverrides))
            addSubItem (new StyleOverrideRootItem(configurationManager, child, undoManager));
    }
}

void ConfigurationItem::changePanel()
{
    if (tree.hasType(Ids::configuration))
        configurationManager.changePanel(new ConfigurationPanel(tree, undoManager, configurationManager.getScopeSync(), commandManager, false));
    else
        configurationManager.changePanel(new EmptyPanel(tree, undoManager, commandManager));
}

/* =========================================================================
 * ParameterRootItem
 */
Icon ParameterRootItem::getIcon() const
{
    if (tree.hasType(Ids::parameters))
        return Icon(Icons::getInstance()->parameters, Colours::aliceblue);
    else
        return Icon();
}

void ParameterRootItem::showPopupMenu()
{
    PopupMenu m;
    m.addCommandItem(commandManager, CommandIDs::addItem, "Add Parameter");
    m.addCommandItem(commandManager, CommandIDs::addItemFromClipboard, "Add Parameter from clipboard");
    
    m.showMenuAsync (PopupMenu::Options(), nullptr);
}

void ParameterRootItem::refreshSubItems()
{
    WholeTreeOpennessRestorer wtor(*this);
    
    clearSubItems();

    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        if (tree.hasType(Ids::parameters))
            addSubItem(new ParameterItem(configurationManager, tree.getChild(i), undoManager));
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
    
    ValueTree newParameter;

    configurationManager.getConfiguration().addNewParameter(newParameter, definition, 0, &undoManager);
}

/* =========================================================================
 * ParameterItem
 */

String ParameterItem::getDisplayName() const 
{
    String displayName = tree[Ids::name].toString();
    displayName += " (" + tree[Ids::fullDescription].toString() + ")";

	if (tree[Ids::scopeCode].toString().isNotEmpty())
        displayName += " - " + tree[Ids::scopeCode].toString();

    return displayName;
}

void ParameterItem::changePanel()
{
    configurationManager.changePanel(configurationManager.createParameterPanelComponent(tree));
}

void ParameterItem::showPopupMenu()
{
    PopupMenu m;
    m.addCommandItem(configurationManager.getCommandManager(), CommandIDs::addItem, "Add Parameter");
    m.addCommandItem(configurationManager.getCommandManager(), CommandIDs::addItemFromClipboard, "Add Parameter from clipboard");
    m.addSeparator();
    m.addCommandItem(configurationManager.getCommandManager(), CommandIDs::copyItem, "Copy Parameter Definition");
    m.addCommandItem(configurationManager.getCommandManager(), CommandIDs::pasteItem, "Paste Parameter Definition");
    m.addSeparator();
    m.addCommandItem(configurationManager.getCommandManager(), CommandIDs::deleteItems, "Delete Parameter");
    
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
    
    ValueTree newParameter;

    configurationManager.getConfiguration().addNewParameter(newParameter, definition, index, &undoManager);
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
            addSubItem(new SliderMappingRootItem(configurationManager, child, undoManager));
        else if (child.hasType(Ids::labels))
            addSubItem(new LabelMappingRootItem(configurationManager, child, undoManager));
        else if (child.hasType(Ids::comboBoxes))
            addSubItem(new ComboBoxMappingRootItem(configurationManager, child, undoManager));
        else if (child.hasType(Ids::tabbedComponents))
            addSubItem(new TabbedComponentMappingRootItem(configurationManager, child, undoManager));
        else if (child.hasType(Ids::textButtons))
            addSubItem(new TextButtonMappingRootItem(configurationManager, child, undoManager));
    }
}

void MappingRootItem::showPopupMenu()
{
    if (!(tree.hasType(Ids::mapping)))
    {
        PopupMenu m;
        m.addCommandItem(configurationManager.getCommandManager(), CommandIDs::addItem, "Add Mapping");
    
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
            addSubItem(new LabelStyleOverrideRootItem(configurationManager, child, undoManager));
        else if (child.hasType(Ids::sliders))
            addSubItem(new SliderStyleOverrideRootItem(configurationManager, child, undoManager));
        else if (child.hasType(Ids::comboBoxes))
            addSubItem(new ComboBoxStyleOverrideRootItem(configurationManager, child, undoManager));
        else if (child.hasType(Ids::tabbedComponents))
            addSubItem(new TabbedComponentStyleOverrideRootItem(configurationManager, child, undoManager));
        else if (child.hasType(Ids::textButtons))
            addSubItem(new TextButtonStyleOverrideRootItem(configurationManager, child, undoManager));
        else if (child.hasType(Ids::components))
            addSubItem(new ComponentStyleOverrideRootItem(configurationManager, child, undoManager));
    }
}

void StyleOverrideRootItem::showPopupMenu()
{
    if (!tree.hasType(Ids::styleOverrides))
    {
        PopupMenu m;
        m.addCommandItem(configurationManager.getCommandManager(), CommandIDs::addItem, "Add Style Override");
        m.addCommandItem(configurationManager.getCommandManager(), CommandIDs::addItemFromClipboard, "Add Style Override from clipboard");
    
        m.showMenuAsync (PopupMenu::Options(), nullptr);
    }
}

void StyleOverrideRootItem::addGenericStyleOverride(const Identifier& componentType)
{
    storeSelectionOnAdd();
    
    ValueTree newStyleOverride;
    configuration.addStyleOverride(componentType, String::empty, String::empty, newStyleOverride, 0, &undoManager);
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
    configuration.addStyleOverride(componentType, String::empty, String::empty, styleOverride, 0, &undoManager);
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
    m.addCommandItem(configurationManager.getCommandManager(), CommandIDs::addItemFromClipboard, "Add Style Override from clipboard");
    m.addSeparator();
    m.addCommandItem(configurationManager.getCommandManager(), CommandIDs::copyItem, "Copy Style Override");
    m.addCommandItem(configurationManager.getCommandManager(), CommandIDs::pasteItem, "Paste Style Override");
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
    configuration.addStyleOverride(tree.getType(), String::empty, String::empty, newStyleOverride, tree.getParent().indexOf(tree) + 1, &undoManager);
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
    configuration.addStyleOverride(tree.getType(), String::empty, String::empty, styleOverride, tree.getParent().indexOf(tree) + 1, &undoManager);
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
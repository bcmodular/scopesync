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
#include "ConfigurationPanel.h"
#include "../Core/Global.h"
#include "../Core/ScopeSyncApplication.h"

/* =========================================================================
 * ConfigurationTreeItem
 */
ConfigurationItem::ConfigurationItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : configurationManagerMain(cmm),
      tree(v),
      undoManager(um),
      textX(0),
      commandManager(configurationManagerMain.getCommandManager())
{
    commandManager->registerAllCommandsForTarget(this);
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
            commandManager->setFirstCommandTarget(&configurationManagerMain);
            showMultiSelectionPopupMenu();
        }
        else
        {
            commandManager->setFirstCommandTarget(this);
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
    commandManager->setFirstCommandTarget(this);
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
        configurationManagerMain.changePanel(new ConfigurationPanel(tree, undoManager, configurationManagerMain));
    else
        configurationManagerMain.changePanel(new EmptyPanel(tree, undoManager, configurationManagerMain));
}

void ConfigurationItem::storeSelectionOnAdd()
{
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

    ConfigurationTree* configTree = dynamic_cast<ConfigurationTree*> (getOwnerView()->getParentComponent());

    configTree->storeSelectedItem(rowNumber);
}

ApplicationCommandTarget* ConfigurationItem::getNextCommandTarget()
{
    return &configurationManagerMain;
}

/* =========================================================================
 * ParameterRootItem
 */
ParameterRootItem::ParameterRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um) : ConfigurationItem(cmm, v, um)
{
    commandManager->registerAllCommandsForTarget(this);
}

var ParameterRootItem::getDragSourceDescription()
{
    return "Parameter Root Item";
}

bool ParameterRootItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails)
{
    if (dragSourceDetails.description.toString() == "Parameter")
        return true;
    else
        return false;
}

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
    m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::addParameter);
    
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

void ParameterRootItem::getAllCommands(Array<CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::addParameter
                            };

    commands.addArray (ids, numElementsInArray (ids));
}

void ParameterRootItem::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::addParameter:
        result.setInfo ("Add parameter", "Adds a new parameter", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('n', ModifierKeys::commandModifier, 0));
        break;
    }
}

bool ParameterRootItem::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::addParameter:         addItem(); break;
        default:                               return false;
    }

    return true;
}

void ParameterRootItem::addItem()
{
    storeSelectionOnAdd();
    
    if (tree.hasType(Ids::hostParameters))
    {
        ValueTree tmp;
        configurationManagerMain.getConfiguration().addNewParameter(tmp, 0, Configuration::host, &undoManager);
    }
    else if (tree.hasType(Ids::scopeParameters))
    {
        ValueTree tmp;
        configurationManagerMain.getConfiguration().addNewParameter(tmp, 0, Configuration::scopeLocal, &undoManager);
    }   
}

/* =========================================================================
 * ParameterItem
 */
ParameterItem::ParameterItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um) : ConfigurationItem(cmm, v, um)
{
    commandManager->registerAllCommandsForTarget(this);
}

var ParameterItem::getDragSourceDescription()
{
    return "Parameter";
}

bool ParameterItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& /* dragSourceDetails */)
{
    return false;
}

String ParameterItem::getDisplayName() const 
{
    String displayName = tree[Ids::name].toString();
    displayName += " (" + tree[Ids::fullDescription].toString() + ")";
    
    return displayName;
}

void ParameterItem::showPopupMenu()
{
    PopupMenu m;
    m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::addParameter);
    m.addSeparator();
    m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::copyParameter);
    m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::pasteParameter);
    m.addSeparator();
    m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::deleteParameter);
    
    m.showMenuAsync (PopupMenu::Options(), nullptr);
}

void ParameterItem::refreshSubItems() {}

void ParameterItem::deleteItem()
{
    storeSelectionOnDelete();
    commandManager->setFirstCommandTarget(&configurationManagerMain);
    tree.getParent().removeChild(tree, &undoManager);
}

void ParameterItem::addItem()
{
    storeSelectionOnAdd();
    insertParameterAt(tree.getParent().indexOf(tree) + 1);
}

void ParameterItem::insertParameterAt(int index)
{
    if (tree.getParent().hasType(Ids::hostParameters))
    {
        ValueTree tmp;
        configurationManagerMain.getConfiguration().addNewParameter(tmp, index, Configuration::host, &undoManager);
    }
    else if (tree.getParent().hasType(Ids::scopeParameters))
    {
        ValueTree tmp;
        configurationManagerMain.getConfiguration().addNewParameter(tmp, index, Configuration::scopeLocal, &undoManager);
    }        
}

void ParameterItem::copyParameter()
{
    ParameterClipboard::getInstance()->copy(tree);
}

void ParameterItem::pasteParameter()
{
    ParameterClipboard::getInstance()->paste(tree, &undoManager);
    changePanel();
}

void ParameterItem::getAllCommands(Array<CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::copyParameter,
                              CommandIDs::pasteParameter,
                              CommandIDs::deleteParameter,
                              CommandIDs::addParameter
                            };

    commands.addArray (ids, numElementsInArray (ids));
}

void ParameterItem::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::copyParameter:
        result.setInfo ("Copy parameter definition", "Copies a parameter's definition to the clipboard", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('c', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::pasteParameter:
        result.setInfo ("Paste parameter definition", "Overwrites a parameter's definition with values from the clipboard", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('v', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::deleteParameter:
        result.setInfo ("Delete parameter", "Deletes the selected parameter", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress (KeyPress::deleteKey, ModifierKeys::noModifiers, 0));
        result.defaultKeypresses.add (KeyPress (KeyPress::backspaceKey, 0, 0));
        break;
    case CommandIDs::addParameter:
        result.setInfo ("Add parameter", "Adds a new parameter", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('n', ModifierKeys::commandModifier, 0));
        break;
    }
}

bool ParameterItem::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::copyParameter:        copyParameter(); break;
        case CommandIDs::pasteParameter:       pasteParameter(); break;
        case CommandIDs::deleteParameter:      deleteItem(); break;
        case CommandIDs::addParameter:         addItem(); break;
        default:                               return false;
    }

    return true;
}

/* =========================================================================
 * HostParameterItem
 */
HostParameterItem::HostParameterItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um) : ParameterItem(cmm, v, um) {}

Icon HostParameterItem::getIcon() const
{
    return Icon(Icons::getInstance()->hostparameter, Colours::grey);
}

String HostParameterItem::getDisplayName() const 
{
    String displayName = ParameterItem::getDisplayName();
    
    if (int(tree[Ids::scopeSync]) != -1)
        displayName += " - " + ScopeSync::getScopeSyncCode(int(tree[Ids::scopeSync]));

    return displayName;
}

void HostParameterItem::changePanel()
{
    configurationManagerMain.changePanel(new ParameterPanel(tree, undoManager, ParameterPanel::hostParameter, configurationManagerMain));
}

/* =========================================================================
 * ScopeParameterItem
 */
ScopeParameterItem::ScopeParameterItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um) : ParameterItem(cmm, v, um) {}

Icon ScopeParameterItem::getIcon() const
{
    return Icon(Icons::getInstance()->scopeparameter, Colours::grey);
}

String ScopeParameterItem::getDisplayName() const 
{
    String displayName = ParameterItem::getDisplayName();
    
    if (int(tree[Ids::scopeLocal]) != -1)
        displayName += " - " + ScopeSync::getScopeLocalCode(int(tree[Ids::scopeLocal]));

    return displayName;
}

void ScopeParameterItem::changePanel()
{
    configurationManagerMain.changePanel(new ParameterPanel(tree, undoManager, ParameterPanel::scopeLocal, configurationManagerMain));
}

/* =========================================================================
 * MappingRootItem
 */
MappingRootItem::MappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um) : ConfigurationItem(cmm, v, um) {}

Icon MappingRootItem::getIcon() const
{
    return Icon(Icons::getInstance()->mapping, Colours::bisque);
}

var MappingRootItem::getDragSourceDescription()
{
    return "Mapping Root Item";
}

void MappingRootItem::refreshSubItems()
{
    WholeTreeOpennessRestorer wtor(*this);
    
    clearSubItems();

    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        ValueTree child = tree.getChild(i);

        if (child.hasType(Ids::sliders))
            addSubItem(new SliderMappingRootItem(configurationManagerMain, tree.getChild(i), undoManager));
        else if (child.hasType(Ids::labels))
            addSubItem(new LabelMappingRootItem(configurationManagerMain, tree.getChild(i), undoManager));
        else if (child.hasType(Ids::comboBoxes))
            addSubItem(new ComboBoxMappingRootItem(configurationManagerMain, tree.getChild(i), undoManager));
        else if (child.hasType(Ids::tabbedComponents))
            addSubItem(new TabbedComponentMappingRootItem(configurationManagerMain, tree.getChild(i), undoManager));
        else if (tree.hasType(Ids::textButtons))
            addSubItem (new TextButtonMappingRootItem(configurationManagerMain, tree.getChild(i), undoManager));
    }
}

void MappingRootItem::getAllCommands(Array<CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::addMapping
                            };

    commands.addArray (ids, numElementsInArray (ids));
}

void MappingRootItem::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::addMapping:
        result.setInfo ("Add mapping", "Adds a new mapping", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('n', ModifierKeys::commandModifier, 0));
        break;
    }
}

bool MappingRootItem::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::addMapping:           addItem(); break;
        default:                               return false;
    }

    return true;
}

void MappingRootItem::showPopupMenu()
{
    PopupMenu m;
    m.addCommandItem(configurationManagerMain.getCommandManager(), CommandIDs::addMapping);
    
    m.showMenuAsync (PopupMenu::Options(), nullptr);
}

void MappingRootItem::addGenericMapping(const Identifier& mappingType)
{
    storeSelectionOnAdd();
    ValueTree newMapping(mappingType);
    tree.addChild(newMapping, 0, &undoManager);
}

/* =========================================================================
 * SliderMappingRootItem
 */
SliderMappingRootItem::SliderMappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : MappingRootItem(cmm, v, um)
{
    commandManager->registerAllCommandsForTarget(this);
}

bool SliderMappingRootItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails)
{   
    if (dragSourceDetails.description.toString() == "SliderMapping")
        return true;
    else
        return false;
}
 
Icon SliderMappingRootItem::getIcon() const { return Icon(Icons::getInstance()->sliders, Colours::bisque); }

void SliderMappingRootItem::refreshSubItems()
{
    WholeTreeOpennessRestorer wtor(*this);
    clearSubItems();

    for (int i = 0; i < tree.getNumChildren(); ++i)
        addSubItem(new SliderMappingItem(configurationManagerMain, tree.getChild(i), undoManager));
}

void SliderMappingRootItem::addItem()
{
    addGenericMapping(Ids::slider);
}

/* =========================================================================
 * LabelMappingRootItem
 */
LabelMappingRootItem::LabelMappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : MappingRootItem(cmm, v, um)
{
    commandManager->registerAllCommandsForTarget(this);
}

bool LabelMappingRootItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails)
{   
    if (dragSourceDetails.description.toString() == "LabelMapping")
        return true;
    else
        return false;
}
 
Icon LabelMappingRootItem::getIcon() const { return Icon(Icons::getInstance()->labels, Colours::bisque); }

void LabelMappingRootItem::refreshSubItems()
{
    WholeTreeOpennessRestorer wtor(*this);
    clearSubItems();

    for (int i = 0; i < tree.getNumChildren(); ++i)
        addSubItem(new LabelMappingItem(configurationManagerMain, tree.getChild(i), undoManager));
}

void LabelMappingRootItem::addItem()
{
    addGenericMapping(Ids::label);
}

/* =========================================================================
 * ComboBoxMappingRootItem
 */
ComboBoxMappingRootItem::ComboBoxMappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : MappingRootItem(cmm, v, um)
{
    commandManager->registerAllCommandsForTarget(this);
}

bool ComboBoxMappingRootItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails)
{   
    if (dragSourceDetails.description.toString() == "ComboBoxMapping")
        return true;
    else
        return false;
}
 
Icon ComboBoxMappingRootItem::getIcon() const { return Icon(Icons::getInstance()->comboboxes, Colours::bisque); }

void ComboBoxMappingRootItem::refreshSubItems()
{
    WholeTreeOpennessRestorer wtor(*this);
    clearSubItems();

    for (int i = 0; i < tree.getNumChildren(); ++i)
        addSubItem(new ComboBoxMappingItem(configurationManagerMain, tree.getChild(i), undoManager));
}

void ComboBoxMappingRootItem::addItem()
{
    addGenericMapping(Ids::comboBox);
}

/* =========================================================================
 * TabbedComponentMappingRootItem
 */
TabbedComponentMappingRootItem::TabbedComponentMappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : MappingRootItem(cmm, v, um)
{
    commandManager->registerAllCommandsForTarget(this);
}

bool TabbedComponentMappingRootItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails)
{   
    if (dragSourceDetails.description.toString() == "TabbedComponentMapping")
        return true;
    else
        return false;
}
 
Icon TabbedComponentMappingRootItem::getIcon() const { return Icon(Icons::getInstance()->tabbedcomponents, Colours::bisque); }

void TabbedComponentMappingRootItem::refreshSubItems()
{
    WholeTreeOpennessRestorer wtor(*this);
    clearSubItems();

    for (int i = 0; i < tree.getNumChildren(); ++i)
        addSubItem(new TabbedComponentMappingItem(configurationManagerMain, tree.getChild(i), undoManager));
}

void TabbedComponentMappingRootItem::addItem()
{
    addGenericMapping(Ids::tabbedComponent);
}

/* =========================================================================
 * TextButtonMappingRootItem
 */
TextButtonMappingRootItem::TextButtonMappingRootItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : MappingRootItem(cmm, v, um)
{
    commandManager->registerAllCommandsForTarget(this);
}

bool TextButtonMappingRootItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails)
{   
    if (dragSourceDetails.description.toString() == "TextButtonMapping")
        return true;
    else
        return false;
}
 
Icon TextButtonMappingRootItem::getIcon() const { return Icon(Icons::getInstance()->textbuttons, Colours::bisque); }

void TextButtonMappingRootItem::refreshSubItems()
{
    WholeTreeOpennessRestorer wtor(*this);
    clearSubItems();

    for (int i = 0; i < tree.getNumChildren(); ++i)
        addSubItem(new TextButtonMappingItem(configurationManagerMain, tree.getChild(i), undoManager));
}

void TextButtonMappingRootItem::addItem()
{
    addGenericMapping(Ids::textButton);
}

/* =========================================================================
 * MappingItem
 */
MappingItem::MappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um) : ConfigurationItem(cmm, v, um) {}

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
    String displayName = tree[Ids::name].toString();
    displayName += " => " + tree[Ids::mapTo].toString();
    
    return displayName;
}

void MappingItem::showPopupMenu()
{
    PopupMenu m;
    m.addCommandItem(commandManager, CommandIDs::addMapping);
    m.addSeparator();
    m.addCommandItem(commandManager, CommandIDs::deleteMapping);
    m.showMenuAsync (PopupMenu::Options(), nullptr);
}

void MappingItem::refreshSubItems() {}

void MappingItem::deleteItem()
{
    storeSelectionOnDelete();
    commandManager->setFirstCommandTarget(&configurationManagerMain);
    tree.getParent().removeChild(tree, &undoManager);
}

void MappingItem::addItem()
{
    storeSelectionOnAdd();
    ValueTree newMapping(tree.getType());
    tree.getParent().addChild(newMapping, tree.getParent().indexOf(tree) + 1, &undoManager);
}

void MappingItem::getAllCommands(Array<CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::addMapping,
                              CommandIDs::deleteMapping
                            };

    commands.addArray (ids, numElementsInArray (ids));
}

void MappingItem::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::addMapping:
        result.setInfo ("Add mapping", "Adds a new mapping", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('n', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::deleteMapping:
        result.setInfo ("Delete mapping", "Deletes the selected mapping", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress (KeyPress::deleteKey, ModifierKeys::noModifiers, 0));
        result.defaultKeypresses.add (KeyPress (KeyPress::backspaceKey, 0, 0));
        break;

    }
}

bool MappingItem::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::addMapping:           addItem(); break;
        case CommandIDs::deleteMapping:        deleteItem(); break;
        default:                               return false;
    }

    return true;
}

/* =========================================================================
 * SliderMappingItem
 */
SliderMappingItem::SliderMappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : MappingItem(cmm, v, um) {}

var SliderMappingItem::getDragSourceDescription()
{
    return "SliderMapping";
}

Icon SliderMappingItem::getIcon() const
{
    return Icon(Icons::getInstance()->slider, Colours::grey);
}

void SliderMappingItem::changePanel()
{
    configurationManagerMain.changePanel(new MappingPanel(tree, undoManager, configurationManagerMain, "Slider"));
}

/* =========================================================================
 * LabelMappingTreeItem
 */
LabelMappingItem::LabelMappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : MappingItem(cmm, v, um) {}

var LabelMappingItem::getDragSourceDescription()
{
    return "LabelMapping";
}

Icon LabelMappingItem::getIcon() const
{
    return Icon(Icons::getInstance()->label, Colours::grey);
}

void LabelMappingItem::changePanel()
{
    configurationManagerMain.changePanel(new MappingPanel(tree, undoManager, configurationManagerMain, "Label"));
}

/* =========================================================================
 * ComboBoxMappingTreeItem
 */
ComboBoxMappingItem::ComboBoxMappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : MappingItem(cmm, v, um) {}

var ComboBoxMappingItem::getDragSourceDescription()
{
    return "ComboBoxMapping";
}

Icon ComboBoxMappingItem::getIcon() const
{
    return Icon(Icons::getInstance()->combobox, Colours::grey);
}

void ComboBoxMappingItem::changePanel()
{
    configurationManagerMain.changePanel(new MappingPanel(tree, undoManager, configurationManagerMain, "ComboBox"));
}

/* =========================================================================
 * TabbedComponentMappingItem
 */
TabbedComponentMappingItem::TabbedComponentMappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : MappingItem(cmm, v, um) {}

var TabbedComponentMappingItem::getDragSourceDescription()
{
    return "TabbedComponentMapping";
}

Icon TabbedComponentMappingItem::getIcon() const
{
    return Icon(Icons::getInstance()->tabbedcomponent, Colours::grey);
}

void TabbedComponentMappingItem::changePanel()
{
    configurationManagerMain.changePanel(new MappingPanel(tree, undoManager, configurationManagerMain, "TabbedComponent"));
}

/* =========================================================================
 * TextButtonMappingTreeItem
 */
TextButtonMappingItem::TextButtonMappingItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : MappingItem(cmm, v, um) {}

var TextButtonMappingItem::getDragSourceDescription()
{
    return "TextButtonMapping";
}

Icon TextButtonMappingItem::getIcon() const
{
    return Icon(Icons::getInstance()->textbutton, Colours::grey);
}

String TextButtonMappingItem::getDisplayName() const
{
    String displayName = tree[Ids::name].toString();
    String settings = tree[Ids::settingDown].toString();

    if (tree.hasProperty(Ids::settingUp))
    {
        settings += "/" + tree[Ids::settingUp].toString();
    }

    displayName += " => " + tree[Ids::mapTo].toString() + " (" + settings + ")";
    
    return displayName;
}

void TextButtonMappingItem::changePanel()
{
    configurationManagerMain.changePanel(new TextButtonMappingPanel(tree, undoManager, configurationManagerMain));
}

/* =========================================================================
 * TreeItemComponent
 */
TreeItemComponent::TreeItemComponent(ConfigurationItem& i) : item (i)
{
    setInterceptsMouseClicks (false, true);
}

void TreeItemComponent::paint (Graphics& g)
{
    g.setColour(Colours::black);
    paintIcon(g);
    item.paintContent(g, Rectangle<int>(item.textX, 0, getWidth() - item.textX, getHeight()));
}

void TreeItemComponent::paintIcon (Graphics& g)
{
    item.getIcon().draw(g, Rectangle<float> (4.0f, 2.0f, item.getIconSize(), getHeight() - 4.0f),
                            item.isIconCrossedOut());
}

void TreeItemComponent::resized()
{
    item.textX = (int)item.getIconSize() + 8;
}

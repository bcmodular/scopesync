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

/* =========================================================================
 * ConfigurationTreeItem
 */
ConfigurationTreeItem::ConfigurationTreeItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : configurationManagerMain(cmm),
      tree(v),
      undoManager(um),
      textX(0)
{
    tree.addListener(this);
}

ConfigurationTreeItem::~ConfigurationTreeItem()
{
    masterReference.clear();
}

String ConfigurationTreeItem::getDisplayName() const 
{
    String displayName = tree["name"].toString();

    if (displayName.isEmpty())
        displayName = tree.getType().toString();

    return displayName;
}

String ConfigurationTreeItem::getUniqueName() const
{
    return getDisplayName();
}

bool ConfigurationTreeItem::mightContainSubItems()
{
    return tree.getNumChildren() > 0;
}

Font ConfigurationTreeItem::getFont() const
{
    return Font (getItemHeight() * 0.7f);
}

float ConfigurationTreeItem::getIconSize() const
{
    return jmin (getItemHeight() - 4.0f, 18.0f);
}

Icon ConfigurationTreeItem::getIcon() const
{
    if (tree.hasType(Ids::configuration))
        return Icon(Icons::getInstance()->config, Colours::lightgreen);
    else if (tree.hasType(Ids::hostParameters))
        return Icon(Icons::getInstance()->hostparameters, Colours::aliceblue);
    else if (tree.hasType(Ids::scopeParameters))
        return Icon(Icons::getInstance()->scopeparameters, Colours::azure);
    else if (tree.hasType(Ids::mapping))
        return Icon(Icons::getInstance()->mapping, Colours::bisque);
    else if (tree.hasType(Ids::sliders))
        return Icon(Icons::getInstance()->sliders, Colours::bisque);
    else if (tree.hasType(Ids::textButtons))
        return Icon(Icons::getInstance()->textbuttons, Colours::bisque);
    else if (tree.hasType(Ids::labels))
        return Icon(Icons::getInstance()->labels, Colours::bisque);
    else if (tree.hasType(Ids::tabbedComponents))
        return Icon(Icons::getInstance()->tabbedcomponents, Colours::bisque);
    else if (tree.hasType(Ids::comboBoxes))
        return Icon(Icons::getInstance()->comboboxes, Colours::bisque);
    else
        return Icon(Icons::getInstance()->config, Colours::beige);
    
}

void ConfigurationTreeItem::paintOpenCloseButton (Graphics& g, const Rectangle<float>& area, Colour backgroundColour, bool isMouseOver)
{
    TreeViewItem::paintOpenCloseButton(g, area, backgroundColour, isMouseOver);
}

Colour ConfigurationTreeItem::getBackgroundColour() const
{
    Colour background(Colours::darkgrey);

    if (isSelected())
        background = background.overlaidWith (getOwnerView()->findColour(TreeView::selectedItemBackgroundColourId));
     
    return background;
}

Colour ConfigurationTreeItem::getContrastingColour(float contrast) const
{
    return getBackgroundColour().contrasting(contrast);
}

Colour ConfigurationTreeItem::getContrastingColour(Colour target, float minContrast) const
{
    return getBackgroundColour().contrasting(target, minContrast);
}

void ConfigurationTreeItem::paintContent(Graphics& g, const Rectangle<int>& area)
{
    g.setFont(getFont());
    g.setColour(getContrastingColour(0.8f));

    g.drawFittedText(getDisplayName(), area, Justification::centredLeft, 1, 0.8f);
}

void ConfigurationTreeItem::itemOpennessChanged(bool isNowOpen)
{
    if (isNowOpen && getNumSubItems() == 0)
    {
        clearSubItems();
        refreshSubItems();
    }
    else
        clearSubItems();
}

var ConfigurationTreeItem::getDragSourceDescription()
{
    return "Configuration Root Item";
}

Component* ConfigurationTreeItem::createItemComponent()
{
    return new TreeItemComponent(*this);
}

bool ConfigurationTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails)
{
    if (
           (dragSourceDetails.description.toString() == "Parameter"              && (tree.hasType(Ids::hostParameters) || tree.hasType(Ids::scopeParameters)))
        || (dragSourceDetails.description.toString() == "SliderMapping"          && tree.hasType(Ids::sliders))
        || (dragSourceDetails.description.toString() == "LabelMapping"           && tree.hasType(Ids::labels))
        || (dragSourceDetails.description.toString() == "TextButtonMapping"      && tree.hasType(Ids::textButtons))
        || (dragSourceDetails.description.toString() == "TabbedComponentMapping" && tree.hasType(Ids::tabbedComponents))
        || (dragSourceDetails.description.toString() == "ComboBoxMapping"        && tree.hasType(Ids::comboBoxes))
       )
        return true;
    else
        return false;
}

void ConfigurationTreeItem::itemDropped(const DragAndDropTarget::SourceDetails&, int insertIndex)
{
    moveItems (*getOwnerView(),
                getSelectedTreeViewItems (*getOwnerView()),
                tree, insertIndex, undoManager);
}

void ConfigurationTreeItem::moveItems(TreeView& treeView, const Array<ValueTree>& items,
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

Array<ValueTree> ConfigurationTreeItem::getSelectedTreeViewItems (TreeView& treeView)
{
    Array<ValueTree> items;

    const int numSelected = treeView.getNumSelectedItems();

    for (int i = 0; i < numSelected; ++i)
        if (const ConfigurationTreeItem* vti = dynamic_cast<ConfigurationTreeItem*> (treeView.getSelectedItem (i)))
            items.add (vti->tree);

    return items;
}

void ConfigurationTreeItem::refreshSubItems()
{
    WholeTreeOpennessRestorer wtor(*this);
    
    clearSubItems();

    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        if (tree.hasType(Ids::hostParameters))
        {
            addSubItem (new HostParameterTreeItem(configurationManagerMain, tree.getChild(i), undoManager));
        }
        else if (tree.hasType(Ids::scopeParameters))
        {
            addSubItem (new ScopeParameterTreeItem(configurationManagerMain, tree.getChild(i), undoManager));
        }
        else if (tree.hasType(Ids::sliders))
        {
            addSubItem (new SliderMappingTreeItem(configurationManagerMain, tree.getChild(i), undoManager));
        }
        else if (tree.hasType(Ids::labels))
        {
            addSubItem (new LabelMappingTreeItem(configurationManagerMain, tree.getChild(i), undoManager));
        }
        else if (tree.hasType(Ids::comboBoxes))
        {
            addSubItem (new ComboBoxMappingTreeItem(configurationManagerMain, tree.getChild(i), undoManager));
        }
        else if (tree.hasType(Ids::tabbedComponents))
        {
            addSubItem (new TabbedComponentMappingTreeItem(configurationManagerMain, tree.getChild(i), undoManager));
        }
        else if (tree.hasType(Ids::textButtons))
        {
            addSubItem (new TextButtonMappingTreeItem(configurationManagerMain, tree.getChild(i), undoManager));
        }
        else
            addSubItem (new ConfigurationTreeItem(configurationManagerMain, tree.getChild(i), undoManager));
    }
}

void ConfigurationTreeItem::valueTreePropertyChanged (ValueTree&, const Identifier&)
{
    repaintItem();
}

void ConfigurationTreeItem::treeChildrenChanged (const ValueTree& parentTree)
{
    if (parentTree == tree)
    {
        setOpen(true);
        refreshSubItems();
        
        ConfigurationTree* configTree = dynamic_cast<ConfigurationTree*> (getOwnerView()->getParentComponent());
        configTree->moveToSelectedItemDelta();
    }
}
        
void ConfigurationTreeItem::itemClicked(const MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
        if (getOwnerView()->getNumSelectedItems() > 1)
            showMultiSelectionPopupMenu();
        else
            showPopupMenu();
    }
    else if (isSelected())
    {
        itemSelectionChanged(true);
    }
}

void ConfigurationTreeItem::treeViewMenuItemChosen(int resultCode, WeakReference<ConfigurationTreeItem> item)
{
    if (item != nullptr)
        item->handlePopupMenuResult(resultCode);
}

void ConfigurationTreeItem::handlePopupMenuResult (int resultCode)
{
    if (resultCode == 1)
        deleteItem();
    else if (resultCode == 2)
        addNewSubItem();
    else if (resultCode == 3)
        insertItemBefore();
    else if (resultCode == 4)
        insertItemAfter();
}

void ConfigurationTreeItem::launchPopupMenu(PopupMenu& m)
{
    m.showMenuAsync (PopupMenu::Options(),
                     ModalCallbackFunction::create(treeViewMenuItemChosen, WeakReference<ConfigurationTreeItem>(this)));
}

void ConfigurationTreeItem::showPopupMenu()
{
    PopupMenu menu;
    String addSubItemText = String::empty;

    if (tree.hasType(Ids::hostParameters) || tree.hasType(Ids::scopeParameters))
    {
        addSubItemText = "Add new parameter";
    }
    else if (tree.hasType(Ids::sliders) || tree.hasType(Ids::textButtons) || tree.hasType(Ids::labels) || tree.hasType(Ids::tabbedComponents) || tree.hasType(Ids::comboBoxes))
    {
        addSubItemText = "Add new mapping";
    }
    
    if (addSubItemText.isNotEmpty())
    {
        menu.addItem(2, addSubItemText);
        launchPopupMenu(menu);
    }
}

void ConfigurationTreeItem::addNewSubItem()
{
    if (tree.hasType(Ids::hostParameters))
    {
        storeSelectionMove(1);
        ValueTree tmp;
        configurationManagerMain.getConfiguration().addNewParameter(tmp, 0, Configuration::host, &undoManager);
    }
    else if (tree.hasType(Ids::scopeParameters))
    {
        storeSelectionMove(1);
        ValueTree tmp;
        configurationManagerMain.getConfiguration().addNewParameter(tmp, 0, Configuration::scopeLocal, &undoManager);
    }
    else if (tree.hasType(Ids::sliders))
    {
        storeSelectionMove(1);
        ValueTree newMapping(Ids::slider);
        tree.addChild(newMapping, 0, &undoManager);
    }
    else if (tree.hasType(Ids::textButtons))
    {
        storeSelectionMove(1);
        ValueTree newMapping(Ids::textButton);
        tree.addChild(newMapping, 0, &undoManager);
    }
    else if (tree.hasType(Ids::labels))
    {
        storeSelectionMove(1);
        ValueTree newMapping(Ids::label);
        tree.addChild(newMapping, 0, &undoManager);
    }
    else if (tree.hasType(Ids::tabbedComponents))
    {
        storeSelectionMove(1);
        ValueTree newMapping(Ids::tabbedComponent);
        tree.addChild(newMapping, 0, &undoManager);
    }
    else if (tree.hasType(Ids::comboBoxes))
    {
        storeSelectionMove(1);
        ValueTree newMapping(Ids::comboBox);
        tree.addChild(newMapping, 0, &undoManager);
    }
}

void ConfigurationTreeItem::deleteAllSelectedItems()
{
    TreeView* treeView = getOwnerView();
    StringArray identifiers;

    for (int i = 0; i < treeView->getNumSelectedItems(); i++)
    {
        identifiers.add(treeView->getSelectedItem(i)->getItemIdentifierString());
        DBG("ConfigurationTreeItem::deleteAllSelectedItems - added item to delete: " + identifiers[i]);
    }

    for (int i = 0; i < identifiers.size(); i++)
    {
        dynamic_cast<ConfigurationTreeItem*>(treeView->findItemFromIdentifierString(identifiers[i]))->deleteItem();
    }
}

void ConfigurationTreeItem::treeViewMultiSelectItemChosen(int resultCode, ConfigurationTreeItem* item)
{
    switch (resultCode)
    {
        case 1:     item->deleteAllSelectedItems(); break;
        default:    break;
    }
}

class ConfigurationTreeItem::ItemSelectionTimer : public Timer
{
public:
    ItemSelectionTimer(ConfigurationTreeItem& cti) : owner (cti) {}

    void timerCallback() override { owner.invokeChangePanel(); }

private:
    ConfigurationTreeItem& owner;
    JUCE_DECLARE_NON_COPYABLE (ItemSelectionTimer)
};

void ConfigurationTreeItem::itemSelectionChanged(bool isNowSelected)
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

void ConfigurationTreeItem::invokeChangePanel()
{
    cancelDelayedSelectionTimer();
    changePanel();
}

void ConfigurationTreeItem::itemDoubleClicked(const MouseEvent&)
{
    invokeChangePanel();
}

void ConfigurationTreeItem::cancelDelayedSelectionTimer()
{
    delayedSelectionTimer = nullptr;
}

void ConfigurationTreeItem::changePanel()
{
    if (tree.hasType(Ids::configuration))
        configurationManagerMain.changePanel(new ConfigurationPanel(tree, undoManager, configurationManagerMain));
    else
        configurationManagerMain.changePanel(new EmptyPanel(tree, undoManager, configurationManagerMain));
}

void ConfigurationTreeItem::storeSelectionMove(int delta)
{
    String lastSelectedItem = getItemIdentifierString();
       
    ConfigurationTree* configTree = dynamic_cast<ConfigurationTree*> (getOwnerView()->getParentComponent());

    configTree->storeSelectedItemMove(lastSelectedItem, delta);
}

void ConfigurationTreeItem::storeSelectionMove()
{
    TreeViewItem* nearbyItem = getParentItem()->getSubItem(getIndexInParent() + 1);

    if (nearbyItem == nullptr)
    {
        nearbyItem = getParentItem()->getSubItem(getIndexInParent() - 1);
    }

    if (nearbyItem != nullptr)
    {
        String lastSelectedItem = nearbyItem->getItemIdentifierString();
        ConfigurationTree* configTree = dynamic_cast<ConfigurationTree*> (getOwnerView()->getParentComponent());

        configTree->storeSelectedItemMove(lastSelectedItem, 0);
    }
}

/* =========================================================================
 * ParameterTreeItem
 */
ParameterTreeItem::ParameterTreeItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um) : ConfigurationTreeItem(cmm, v, um) {}

var ParameterTreeItem::getDragSourceDescription()
{
    return "Parameter";
}

bool ParameterTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& /* dragSourceDetails */)
{
    return false;
}

String ParameterTreeItem::getDisplayName() const 
{
    String displayName = tree[Ids::name].toString();
    displayName += " (" + tree[Ids::fullDescription].toString() + ")";
    
    return displayName;
}

void ParameterTreeItem::showMultiSelectionPopupMenu()
{
    PopupMenu m;
    m.addItem(1, "Delete");
    
    m.showMenuAsync (PopupMenu::Options(),
                     ModalCallbackFunction::create(treeViewMultiSelectItemChosen, (ConfigurationTreeItem*)this));
}

void ParameterTreeItem::handlePopupMenuResult (int resultCode)
{
    if (resultCode == 1)
        deleteItem();
    else if (resultCode == 2)
        addNewSubItem();
    else if (resultCode == 3)
        insertItemBefore();
    else if (resultCode == 4)
        insertItemAfter();
    else if (resultCode == 5)
        copyParameter();
    else if (resultCode == 6)
        pasteParameter();
}

void ParameterTreeItem::showPopupMenu()
{
    PopupMenu m;
    m.addItem(3, "Insert new parameter before");
    m.addItem(4, "Insert new parameter after");
    m.addSeparator();
    m.addItem(5, "Copy parameter definition");
    m.addItem(6, "Paste parameter definition");
    m.addSeparator();
    m.addItem(1, "Remove this parameter");
    
    launchPopupMenu(m);
}

void ParameterTreeItem::refreshSubItems() {}

void ParameterTreeItem::deleteItem()
{
    storeSelectionMove();
    tree.getParent().removeChild(tree, &undoManager);
}

void ParameterTreeItem::insertItemBefore()
{
    storeSelectionMove(-1);
    insertParameterAt(tree.getParent().indexOf(tree));
}

void ParameterTreeItem::insertItemAfter()
{
    storeSelectionMove(1);
    insertParameterAt(tree.getParent().indexOf(tree) + 1);
}

void ParameterTreeItem::insertParameterAt(int index)
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

void ParameterTreeItem::copyParameter()
{
    Configuration::copyParameterDefinition(tree);
}

void ParameterTreeItem::pasteParameter()
{
    Configuration::pasteParameterDefinition(tree, &undoManager);
    changePanel();
}

/* =========================================================================
 * HostParameterTreeItem
 */
HostParameterTreeItem::HostParameterTreeItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um) : ParameterTreeItem(cmm, v, um) {}

Icon HostParameterTreeItem::getIcon() const
{
    return Icon(Icons::getInstance()->hostparameter, Colours::grey);
}

String HostParameterTreeItem::getDisplayName() const 
{
    String displayName = ParameterTreeItem::getDisplayName();
    
    if (int(tree[Ids::scopeSync]) != -1)
        displayName += " - " + ScopeSync::getScopeSyncCode(int(tree[Ids::scopeSync]));

    return displayName;
}

void HostParameterTreeItem::changePanel()
{
    configurationManagerMain.changePanel(new ParameterPanel(tree, undoManager, ParameterPanel::hostParameter, configurationManagerMain));
}

/* =========================================================================
 * ScopeParameterTreeItem
 */
ScopeParameterTreeItem::ScopeParameterTreeItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um) : ParameterTreeItem(cmm, v, um) {}

Icon ScopeParameterTreeItem::getIcon() const
{
    return Icon(Icons::getInstance()->scopeparameter, Colours::grey);
}

String ScopeParameterTreeItem::getDisplayName() const 
{
    String displayName = ParameterTreeItem::getDisplayName();
    
    if (int(tree[Ids::scopeLocal]) != -1)
        displayName += " - " + ScopeSync::getScopeLocalCode(int(tree[Ids::scopeLocal]));

    return displayName;
}

void ScopeParameterTreeItem::changePanel()
{
    configurationManagerMain.changePanel(new ParameterPanel(tree, undoManager, ParameterPanel::scopeLocal, configurationManagerMain));
}

/* =========================================================================
 * MappingTreeItem
 */
MappingTreeItem::MappingTreeItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um) : ConfigurationTreeItem(cmm, v, um) {}

var MappingTreeItem::getDragSourceDescription()
{
    return "Mapping";
}

bool MappingTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& /* dragSourceDetails */)
{
    return false;
}

String MappingTreeItem::getDisplayName() const 
{
    String displayName = tree[Ids::name].toString();
    displayName += " => " + tree[Ids::mapTo].toString();
    
    return displayName;
}

void MappingTreeItem::showMultiSelectionPopupMenu()
{
    PopupMenu m;
    m.addItem(1, "Delete");
    m.showMenuAsync (PopupMenu::Options(),
                     ModalCallbackFunction::create(treeViewMultiSelectItemChosen, (ConfigurationTreeItem*)this));
}

void MappingTreeItem::showPopupMenu()
{
    PopupMenu m;
    m.addItem(3, "Insert new mapping before");
    m.addItem(4, "Insert new mapping after");
    m.addSeparator();
    m.addItem (1, "Remove this mapping");
    launchPopupMenu(m);
}

void MappingTreeItem::refreshSubItems() {}

void MappingTreeItem::deleteItem()
{
    storeSelectionMove();
    tree.getParent().removeChild(tree, &undoManager);
}

void MappingTreeItem::insertItemBefore()
{
    storeSelectionMove(-1);
    ValueTree newMapping(tree.getType());
    tree.getParent().addChild(newMapping, tree.getParent().indexOf(tree), &undoManager);
}

void MappingTreeItem::insertItemAfter()
{
    storeSelectionMove(1);
    ValueTree newMapping(tree.getType());
    tree.getParent().addChild(newMapping, tree.getParent().indexOf(tree) + 1, &undoManager);
}

/* =========================================================================
 * SliderMappingTreeItem
 */
SliderMappingTreeItem::SliderMappingTreeItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : MappingTreeItem(cmm, v, um) {}

var SliderMappingTreeItem::getDragSourceDescription()
{
    return "SliderMapping";
}

Icon SliderMappingTreeItem::getIcon() const
{
    return Icon(Icons::getInstance()->slider, Colours::grey);
}

void SliderMappingTreeItem::changePanel()
{
    configurationManagerMain.changePanel(new MappingPanel(tree, undoManager, configurationManagerMain, "Slider"));
}

/* =========================================================================
 * LabelMappingTreeItem
 */
LabelMappingTreeItem::LabelMappingTreeItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : MappingTreeItem(cmm, v, um) {}

var LabelMappingTreeItem::getDragSourceDescription()
{
    return "LabelMapping";
}

Icon LabelMappingTreeItem::getIcon() const
{
    return Icon(Icons::getInstance()->label, Colours::grey);
}

void LabelMappingTreeItem::changePanel()
{
    configurationManagerMain.changePanel(new MappingPanel(tree, undoManager, configurationManagerMain, "Label"));
}

/* =========================================================================
 * ComboBoxMappingTreeItem
 */
ComboBoxMappingTreeItem::ComboBoxMappingTreeItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : MappingTreeItem(cmm, v, um) {}

var ComboBoxMappingTreeItem::getDragSourceDescription()
{
    return "ComboBoxMapping";
}

Icon ComboBoxMappingTreeItem::getIcon() const
{
    return Icon(Icons::getInstance()->combobox, Colours::grey);
}

void ComboBoxMappingTreeItem::changePanel()
{
    configurationManagerMain.changePanel(new MappingPanel(tree, undoManager, configurationManagerMain, "ComboBox"));
}

/* =========================================================================
 * TabbedComponentMappingTreeItem
 */
TabbedComponentMappingTreeItem::TabbedComponentMappingTreeItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : MappingTreeItem(cmm, v, um) {}

var TabbedComponentMappingTreeItem::getDragSourceDescription()
{
    return "TabbedComponentMapping";
}

Icon TabbedComponentMappingTreeItem::getIcon() const
{
    return Icon(Icons::getInstance()->tabbedcomponent, Colours::grey);
}

void TabbedComponentMappingTreeItem::changePanel()
{
    configurationManagerMain.changePanel(new MappingPanel(tree, undoManager, configurationManagerMain, "TabbedComponent"));
}

/* =========================================================================
 * TextButtonMappingTreeItem
 */
TextButtonMappingTreeItem::TextButtonMappingTreeItem(ConfigurationManagerMain& cmm, const ValueTree& v, UndoManager& um)
    : MappingTreeItem(cmm, v, um) {}

var TextButtonMappingTreeItem::getDragSourceDescription()
{
    return "TextButtonMapping";
}

Icon TextButtonMappingTreeItem::getIcon() const
{
    return Icon(Icons::getInstance()->textbutton, Colours::grey);
}

String TextButtonMappingTreeItem::getDisplayName() const
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

void TextButtonMappingTreeItem::changePanel()
{
    configurationManagerMain.changePanel(new TextButtonMappingPanel(tree, undoManager, configurationManagerMain));
}

/* =========================================================================
 * TreeItemComponent
 */
TreeItemComponent::TreeItemComponent(ConfigurationTreeItem& i) : item (i)
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

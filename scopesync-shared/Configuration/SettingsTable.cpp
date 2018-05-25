/**
 * Settings Table, used within the Parameter Configuration Panels
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

#include "SettingsTable.h"
#include "ConfigurationManager.h"
#include "ConfigurationPanel.h"
#include "../Core/Global.h"

//==============================================================================
// A comparator used to sort our data when the user clicks a column header
class SettingsSorter
{
public:
    SettingsSorter (const int columnIdToSort, bool forwards)
        : columnId(columnIdToSort),
          direction (forwards ? 1 : -1)
    {
    }

    int compareElements(const ValueTree& first, const ValueTree& second) const
    {
        int result = 0;

        if (columnId == 2)
        {
            String firstString  = first.getProperty(Ids::name, String());
            String secondString = second.getProperty(Ids::name, String());
            result = firstString.compareNatural(secondString);
        }
        else if (columnId == 3)
        {
            int firstInt  = first.getProperty(Ids::intValue);
            int secondInt = second.getProperty(Ids::intValue);
            result = (firstInt < secondInt) ? -1 : ((secondInt < firstInt) ? 1 : 0);
        }
        
        return direction * result;
    }

private:
    int columnId;
    int direction;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsSorter)
};

/* =========================================================================
 * SettingsTable
 */
class SettingsTable::LabelComp : public Component
{
public:
    LabelComp(Value& valueToEdit)
        : label(String(), String()),
          value(valueToEdit)
    {
        addAndMakeVisible(label);
        label.getTextValue().referTo(value);
        label.setEditable(true, true, false);  
    }

    void resized() override
    {
        label.setBoundsInset(BorderSize<int> (2));
    }

    void setLabelValue(Value& valueToReferTo)
    {
        label.getTextValue().referTo(valueToReferTo);
    }

private:
    Label label;
    Value value;
    int maxChars;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabelComp)
};

SettingsTable::SettingsTable(const ValueTree& valueTree, UndoManager& um, ApplicationCommandManager* acm,
                             ValueTree& param, ApplicationCommandTarget* act)
    : font(14.0f), tree(valueTree), undoManager(um), commandManager(acm),
      numSettingsTextLabel(String(), "Num to add:"),
      addSettingsButton("Add"),
      removeSettingsButton("Remove"),
      autoFillValuesButton("Auto-fill"),
      moveUpButton("Move Up"),
      moveDownButton("Move Down"),
      parameter(param),
	  parentCommandTarget(act)
{
    commandManager->registerAllCommandsForTarget(this);

    addAndMakeVisible(table);
    
    table.setModel(this);
    
    table.setColour(ListBox::outlineColourId, Colours::grey);
    table.setOutlineThickness (1);
    
    table.getHeader().addColumn(String(),     1, 30,  30, 30,  TableHeaderComponent::notResizableOrSortable);
    table.getHeader().addColumn("Name",            2, 150, 50, 400, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Scope Int Value", 3, 150, 50, 400, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    
    table.getHeader().setStretchToFitActive(true);
    table.setMultipleSelectionEnabled (true);
    
    tree.addListener(this);

    addAndMakeVisible(numSettingsTextLabel);

    addAndMakeVisible(numSettingsToAddLabel);
    numSettingsToAddLabel.setEditable(true);
    numSettingsToAddLabel.getTextValue().referTo(numSettingsToAdd);
    
    numSettingsToAdd = 1;

    addAndMakeVisible(addSettingsButton);
    addSettingsButton.setCommandToTrigger(commandManager, CommandIDs::addSettings, true);
    
    addAndMakeVisible(removeSettingsButton);
    removeSettingsButton.setCommandToTrigger(commandManager, CommandIDs::removeSettings, true);
    
    addAndMakeVisible(autoFillValuesButton);
    autoFillValuesButton.setCommandToTrigger(commandManager, CommandIDs::autoFill, true);

    addAndMakeVisible(moveUpButton);
    moveUpButton.setCommandToTrigger(commandManager, CommandIDs::moveUp, true);

    addAndMakeVisible(moveDownButton);
    moveDownButton.setCommandToTrigger(commandManager, CommandIDs::moveDown, true);

	addKeyListener(commandManager->getKeyMappings());
}

SettingsTable::~SettingsTable()
{
    tree.removeListener(this);
	removeKeyListener(commandManager->getKeyMappings());
}

void SettingsTable::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    Rectangle<int> buttonBar(localBounds.removeFromBottom(30).reduced(4, 4));

    numSettingsTextLabel.setBounds(buttonBar.removeFromLeft(100));
    numSettingsToAddLabel.setBounds(buttonBar.removeFromLeft(35));
    addSettingsButton.setBounds(buttonBar.removeFromLeft(70));
    removeSettingsButton.setBounds(buttonBar.removeFromLeft(70));
    buttonBar.removeFromLeft(10);
    autoFillValuesButton.setBounds(buttonBar.removeFromLeft(70));
    buttonBar.removeFromLeft(10);
    moveUpButton.setBounds(buttonBar.removeFromLeft(70));
    moveDownButton.setBounds(buttonBar.removeFromLeft(70));

    DBG("SettingsTable::resized - Local Bounds: " + String(localBounds.getWidth()) + ", " + String(localBounds.getHeight()));
    table.setBounds(localBounds.reduced(4, 4));
}
    
int SettingsTable::getNumRows()
{
    return tree.getNumChildren();
}

Component* SettingsTable::refreshComponentForCell(int rowNumber, int columnId, bool /* isRowSelected */, Component* existingComponentToUpdate)
{
    Identifier propertyId;

    if (columnId == 2)
    {
        propertyId = Ids::name;
    }
    else if (columnId == 3)
    {
        propertyId = Ids::intValue;
    }
    else
    {
        return nullptr;
    }
    
    Value valueToEdit(tree.getChild(rowNumber).getPropertyAsValue(propertyId, &undoManager));
    
    LabelComp* labelComp = static_cast<LabelComp*>(existingComponentToUpdate);

    if (labelComp == nullptr)
        labelComp = new LabelComp(valueToEdit);
    else
        labelComp->setLabelValue(valueToEdit);
    
    return labelComp;
}

void SettingsTable::sortOrderChanged(int newSortColumnId, bool isForwards)
{
    SettingsSorter sorter(newSortColumnId, isForwards);
    tree.sort(sorter, &undoManager, true);
}

void SettingsTable::selectedRowsChanged(int lastRowSelected)
{
    (void)lastRowSelected;
}

void SettingsTable::paintRowBackground(Graphics& g, int /* rowNumber */, int /* width */, int /* height */, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll (findColour (TextEditor::highlightColourId));
}

void SettingsTable::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool /* rowIsSelected */)
{
    g.setColour(Colours::black);
    g.setFont(font);

    if (columnId == 1)
    {
        const String text(rowNumber + 1);

        g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);
    }

    g.setColour(Colours::black.withAlpha(0.2f));
    g.fillRect(width - 1, 0, 1, height);
}

void SettingsTable::backgroundClicked(const MouseEvent&)
{
    table.deselectAllRows();
}

void SettingsTable::deleteKeyPressed(int)
{
    removeSettings();
}

void SettingsTable::getAllCommands(Array <CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::undo,
                              CommandIDs::redo,
                              CommandIDs::addSettings,
                              CommandIDs::removeSettings,
                              CommandIDs::autoFill,
                              CommandIDs::moveUp,
                              CommandIDs::moveDown
                            };

    commands.addArray(ids, numElementsInArray (ids));
}

void SettingsTable::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::undo:
        result.setInfo("Undo", "Undo latest change", CommandCategories::general, !(undoManager.canUndo()));
        result.defaultKeypresses.add(KeyPress ('z', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::redo:
        result.setInfo("Redo", "Redo latest change", CommandCategories::general, !(undoManager.canRedo()));
        result.defaultKeypresses.add(KeyPress ('y', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::addSettings:
        result.setInfo("Add", "Add new Settings", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress('n', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::removeSettings:
        result.setInfo ("Remove", "Remove selected Settings", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress(KeyPress::deleteKey));
        break;
    case CommandIDs::autoFill:
        result.setInfo ("Auto-fill", "Automatically fill all Setting values (evenly spread from min to max)", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress('f', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::moveUp:
        result.setInfo ("Move Up", "Move all selected Settings up one position", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress(KeyPress::upKey, ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::moveDown:
        result.setInfo ("Move Down", "Move all selected Settings down one position", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress(KeyPress::downKey, ModifierKeys::commandModifier, 0));
        break;
    }
}

bool SettingsTable::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::undo:                 undo(); break;
        case CommandIDs::redo:                 redo(); break;
        case CommandIDs::addSettings:          addSettings(); break;
        case CommandIDs::removeSettings:       removeSettings(); break;
        case CommandIDs::autoFill:             autoFill(); break;
        case CommandIDs::moveUp:               moveSettings(true); break;
        case CommandIDs::moveDown:             moveSettings(false); break;
        default:                               return false;
    }

    return true;
}

void SettingsTable::undo() const
{
    undoManager.undo();
}

void SettingsTable::redo() const
{
    undoManager.redo();
}

void SettingsTable::addSettings()
{
    for (int i = 0; i < int(numSettingsToAdd.getValue()); i++)
    {
        ValueTree newSetting(Ids::setting);
        String newSettingName;

        int settingNum = 1;

        for(;;)
        {
            newSettingName = "New Setting " + String(settingNum);

            if (tree.getChildWithProperty(Ids::name, newSettingName).isValid())
            {
                settingNum++;
                continue;
            }
            else
                break;
        }

        newSetting.setProperty(Ids::name, newSettingName, &undoManager);
        newSetting.setProperty(Ids::intValue, 0, &undoManager);

        tree.addChild(newSetting, -1, &undoManager);
    }

    updateParameterRanges();
	commandManager->commandStatusChanged();
}

void SettingsTable::removeSettings()
{
    SparseSet<int> selectedRows(table.getSelectedRows());
    Array<int> settingsToRemove;
    
    for (int i = 0; i < selectedRows.size(); i++)
    {
        settingsToRemove.add(selectedRows[i]);
    }

    DefaultElementComparator<int> sorter;
    settingsToRemove.sort(sorter);
    
    for (int i = settingsToRemove.size() - 1; i > -1; i--)
    {
        tree.removeChild(settingsToRemove[i], &undoManager);
    }
    
    updateParameterRanges();
	commandManager->commandStatusChanged();
}

void SettingsTable::updateParameterRanges()
{
    int maxValue = jmax(tree.getNumChildren() - 1, 1);
    ParameterPanel::setParameterUIRanges(0, maxValue, 0, undoManager, tree);
}

void SettingsTable::autoFill() const
{
    int    numSettings = tree.getNumChildren();
    double scopeRangeMin = parameter.getProperty(Ids::scopeRangeMin);
    double scopeRangeMax = parameter.getProperty(Ids::scopeRangeMax);

    for (int i = 0; i < numSettings; i++)
    {
        int newValue = roundToInt((i * ((scopeRangeMax - scopeRangeMin) / (numSettings - 1))) + scopeRangeMin);

        tree.getChild(i).setProperty(Ids::intValue, newValue, &undoManager);
    }

	commandManager->commandStatusChanged();
}

void SettingsTable::moveSettings(bool moveUp)
{
    SparseSet<int> selectedRows(table.getSelectedRows());
    Array<int> settingsToMove;
    
    for (int i = 0; i < selectedRows.size(); i++)
    {
        settingsToMove.add(selectedRows[i]);
    }

    DefaultElementComparator<int> sorter;
    settingsToMove.sort(sorter);
    
    if (moveUp)
    {
        for (int i = 0; i < settingsToMove.size(); i++)
        {
            int currentIndex = settingsToMove[i];
            int newIndex     = jmax(settingsToMove[i] - 1, 0);

            table.deselectRow(currentIndex);

            tree.moveChild(currentIndex, newIndex, &undoManager);
            table.selectRow(newIndex, false, false);
        }
    }
    else
    {
        for (int i = settingsToMove.size() - 1; i > -1; i--)
        {
            int currentIndex = settingsToMove[i];
            int newIndex     = jmin(settingsToMove[i] + 1, tree.getNumChildren());
            
            table.deselectRow(currentIndex);

            tree.moveChild(currentIndex, newIndex, &undoManager);
            table.selectRow(newIndex, false, false);
        }
    }  

	commandManager->commandStatusChanged();
}

ApplicationCommandTarget* SettingsTable::getNextCommandTarget()
{
    return parentCommandTarget;
}

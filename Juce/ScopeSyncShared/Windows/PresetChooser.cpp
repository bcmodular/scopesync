/**
 * Table for choosing a Preset
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

#include "PresetChooser.h"
#include "../Resources/ImageLoader.h"
#include "../Windows/UserSettings.h"
#include "../Core/ScopeSync.h"
#include "../Windows/FileLocationEditor.h"

/* =========================================================================
 * PresetSorter - A comparator used to sort our data when the user clicks a column header
 */
class PresetSorter
{
public:
    PresetSorter (const int columnIdToSort, bool forwards)
        : columnId(columnIdToSort),
          direction (forwards ? 1 : -1)
    {
    }

    int compareElements(const ValueTree& first, const ValueTree& second) const
    {
        int result = 0;

        // Sort by name
        if (columnId == 2)
        {
            String firstString  = first.getProperty(Ids::name, String::empty);
            String secondString = second.getProperty(Ids::name, String::empty);
            result = firstString.compareNatural(secondString);
        }
        // Sort by presetFileName
        else if (columnId == 3)
        {
            String firstString  = first.getProperty(Ids::presetFileName, String::empty);
            String secondString = second.getProperty(Ids::presetFileName, String::empty);
            result = firstString.compareNatural(secondString);
        }
        // Sort by presetFileLibrarySet
        else if (columnId == 4)
        {
            String firstString  = first.getProperty(Ids::presetFileLibrarySet, String::empty);
            String secondString = second.getProperty(Ids::presetFileLibrarySet, String::empty);
            result = firstString.compareNatural(secondString);
        }
        // Sort by presetFileAuthor
        else if (columnId == 4)
        {
            String firstString  = first.getProperty(Ids::presetFileAuthor, String::empty);
            String secondString = second.getProperty(Ids::presetFileAuthor, String::empty);
            result = firstString.compareNatural(secondString);
        }

        return direction * result;
    }

private:
    int columnId;
    int direction;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetSorter)
};

/* =========================================================================
 * PresetChooser
 */
PresetChooser::PresetChooser(ValueTree& param, ScopeSync& ss, ApplicationCommandManager* acm, UndoManager& um)
    : undoManager(um),
      parameter(param),
      scopeSync(ss),
      font(14.0f), 
      commandManager(acm),
      chooseButton("Choose Preset"),
      rebuildLibraryButton("Rebuild Library"),
      editLocationsButton("File Locations..."),
      presetManagerButton("Preset Manager..."),
      blurb(String::empty),
      fileNameLabel(String::empty)
{
    attachToTree();

    commandManager->registerAllCommandsForTarget(this);

    chooseButton.setCommandToTrigger(commandManager, CommandIDs::chooseSelectedPreset, true);
    addAndMakeVisible(chooseButton);

    rebuildLibraryButton.setCommandToTrigger(commandManager, CommandIDs::rebuildFileLibrary, true);
    addAndMakeVisible(rebuildLibraryButton);

    editLocationsButton.setCommandToTrigger(commandManager, CommandIDs::editFileLocations, true);
    addAndMakeVisible(editLocationsButton);
    
    presetManagerButton.setCommandToTrigger(commandManager, CommandIDs::showPresetManager, true);
    addAndMakeVisible(presetManagerButton);
    
    blurb.setJustificationType(Justification::topLeft);
    blurb.setMinimumHorizontalScale(1.0f);
    addAndMakeVisible(blurb);

    fileNameLabel.setMinimumHorizontalScale(1.0f);
    fileNameLabel.setJustificationType(Justification::topLeft);
    fileNameLabel.setFont(Font(12.0f, Font::bold));
    addAndMakeVisible(fileNameLabel);

    addAndMakeVisible(table);
    
    table.setModel(this);
    
    table.setColour(ListBox::outlineColourId, Colours::darkgrey);
    table.setOutlineThickness(4);
    
    table.getHeader().addColumn(String::empty,        1, 10,  10, 10, TableHeaderComponent::notResizableOrSortable & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Name",               2, 120, 40, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Preset File",        3, 120, 40, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Library Set",        4, 120, 40, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Author",             5, 100, 40, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
        
    table.getHeader().setStretchToFitActive(true);
    
    viewTree.addListener(this);

    addKeyListener(commandManager->getKeyMappings());

    setBounds(0, 0, 1000, 600);
}

PresetChooser::~PresetChooser()
{
    removeKeyListener(commandManager->getKeyMappings());
    viewTree.removeListener(this);
    UserSettings::getInstance()->removeChangeListener(this);
}

void PresetChooser::changeListenerCallback(ChangeBroadcaster* /* source */)
{
    attachToTree();
}

void PresetChooser::paint(Graphics& g)
{
    g.fillAll(Colours::lightgrey);
}
    
void PresetChooser::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    Rectangle<int> headerBounds(localBounds.removeFromTop(100));
    Rectangle<int> buttonBar(headerBounds.removeFromBottom(30));
    
    chooseButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    rebuildLibraryButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    editLocationsButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    presetManagerButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    headerBounds.removeFromLeft(4);
    headerBounds.removeFromTop(10);
    fileNameLabel.setBounds(headerBounds.removeFromTop(20).reduced(2, 2));
    blurb.setBounds(headerBounds.reduced(2, 2));
    table.setBounds(localBounds.reduced(2, 2));
}
    
int PresetChooser::getNumRows()
{
    return viewTree.getNumChildren();
}

void PresetChooser::sortOrderChanged(int newSortColumnId, bool isForwards)
{
    PresetSorter sorter(newSortColumnId, isForwards);
    viewTree.sort(sorter, nullptr, true);
}

void PresetChooser::paintRowBackground(Graphics& g, int /* rowNumber */, int /* width */, int /* height */, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll (findColour(TextEditor::highlightColourId));
}

void PresetChooser::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool /* rowIsSelected */)
{
    g.setColour(Colours::black);
    g.setFont(font);

    String text;

    switch (columnId)
    {
        case 1: text = String::empty; break;
        case 2: text = viewTree.getChild(rowNumber).getProperty(Ids::name); break;
        case 3: text = viewTree.getChild(rowNumber).getProperty(Ids::presetFileName); break;
        case 4: text = viewTree.getChild(rowNumber).getProperty(Ids::presetFileLibrarySet); break;
        case 5: text = viewTree.getChild(rowNumber).getProperty(Ids::presetFileAuthor); break;
    }

    g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);
    
    g.setColour(Colours::black.withAlpha(0.2f));
    g.fillRect(width - 1, 0, 1, height);
}

void PresetChooser::backgroundClicked(const MouseEvent&)
{
    table.deselectAllRows();
}

void PresetChooser::cellDoubleClicked(int /* rowNumber */, int /* columnId */, const MouseEvent& /* e */)
{
    chooseSelectedPreset();
}

void PresetChooser::selectedRowsChanged(int lastRowSelected)
{
    blurb.setText(viewTree.getChild(lastRowSelected).getProperty(Ids::blurb), dontSendNotification);
    
    String filePath = viewTree.getChild(lastRowSelected).getProperty(Ids::filePath).toString();
    fileNameLabel.setText("File path: " + filePath, dontSendNotification);
    fileNameLabel.setTooltip(filePath);
    
    commandManager->commandStatusChanged();
}

void PresetChooser::getAllCommands(Array <CommandID>& commands)
{
    const CommandID ids[] = {CommandIDs::chooseSelectedPreset,
                             CommandIDs::rebuildFileLibrary,
                             CommandIDs::editFileLocations,
                             CommandIDs::showPresetManager};
    
    commands.addArray(ids, numElementsInArray (ids));
}

void PresetChooser::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::chooseSelectedPreset:
        result.setInfo("Choose Selected Preset", "Applies selected Preset to Parameter", CommandCategories::configmgr, !table.getNumSelectedRows());
        result.defaultKeypresses.add(KeyPress(KeyPress::returnKey));
        break;
    case CommandIDs::rebuildFileLibrary:
        result.setInfo("Rebuild library", "Rebuild File Library", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('r', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    case CommandIDs::editFileLocations:
        result.setInfo("File Locations...", "Open File Locations edit window", CommandCategories::usersettings, 0);
        result.defaultKeypresses.add(KeyPress('f', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::showPresetManager:
        result.setInfo("Preset Manager...", "Open Preset Manager window", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress('p', ModifierKeys::commandModifier, 0));
        break;
    }
}

void PresetChooser::returnKeyPressed(int /* lastRowSelected */)
{
    chooseSelectedPreset();
}

bool PresetChooser::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::chooseSelectedPreset:  chooseSelectedPreset(); break;
        case CommandIDs::rebuildFileLibrary:    rebuildFileLibrary(); break;
        case CommandIDs::editFileLocations:     editFileLocations(); break;
        case CommandIDs::showPresetManager:     showPresetManager(); break;
        default:                                return false;
    }

    return true;
}

void PresetChooser::showPresetManager()
{
    UserSettings::getInstance()->addChangeListener(this);
    UserSettings::getInstance()->showPresetManagerWindow(getParentMonitorArea().getCentreX(), getParentMonitorArea().getCentreY());
}

void PresetChooser::editFileLocations()
{
    UserSettings::getInstance()->addChangeListener(this);
    UserSettings::getInstance()->editFileLocations(getParentMonitorArea().getCentreX(), getParentMonitorArea().getCentreY());
}

void PresetChooser::chooseSelectedPreset()
{
    int selectedRow = table.getSelectedRow();
    
    if (selectedRow > -1)
    {
        scopeSync.getConfiguration().updateParameterFromPreset(parameter, viewTree.getChild(selectedRow), false, &undoManager);
        sendChangeMessage();
    }
}

void PresetChooser::rebuildFileLibrary()
{
    UserSettings::getInstance()->rebuildFileLibrary();
    attachToTree();
}


void PresetChooser::removePresetFileEntries()
{
    for (int i = viewTree.getNumChildren() - 1; i >= 0; i--)
    {
        if (viewTree.getChild(i).hasType(Ids::presetFile))
            viewTree.removeChild(i, nullptr);
    }
}

void PresetChooser::attachToTree()
{
    viewTree.removeListener(this);
    
    tree = UserSettings::getInstance()->getPresetLibrary();
    viewTree = tree.createCopy();
    
    removePresetFileEntries();
    
    PresetSorter sorter(2, false);
    viewTree.sort(sorter, nullptr, true);
    
    viewTree.addListener(this);
    table.updateContent();
}

ApplicationCommandTarget* PresetChooser::getNextCommandTarget()
{
    return nullptr;
}

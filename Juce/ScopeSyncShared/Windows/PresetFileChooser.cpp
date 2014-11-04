/**
 * Table for choosing a Preset File
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

#include "PresetFileChooser.h"
#include "../Resources/ImageLoader.h"
#include "../Windows/UserSettings.h"
#include "../Core/ScopeSync.h"
#include "../Windows/FileLocationEditor.h"

/* =========================================================================
 * PresetFileSorter - A comparator used to sort our data when the user clicks a column header
 */
class PresetFileSorter 
{
public:
    PresetFileSorter (const int columnIdToSort, bool forwards)
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
        // Sort by librarySet
        else if (columnId == 3)
        {
            String firstString  = first.getProperty(Ids::librarySet, String::empty);
            String secondString = second.getProperty(Ids::librarySet, String::empty);
            result = firstString.compareNatural(secondString);
        }
        // Sort by author
        else if (columnId == 4)
        {
            String firstString  = first.getProperty(Ids::author, String::empty);
            String secondString = second.getProperty(Ids::author, String::empty);
            result = firstString.compareNatural(secondString);
        }
        // Sort by fileName
        else if (columnId == 5)
        {
            String firstString  = first.getProperty(Ids::fileName, String::empty);
            String secondString = second.getProperty(Ids::fileName, String::empty);
            result = firstString.compareNatural(secondString);
        }
        
        return direction * result;
    }

private:
    int columnId;
    int direction;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetFileSorter)
};

/* =========================================================================
 * PresetFileChooser
 */
PresetFileChooser::PresetFileChooser(File& pf, ApplicationCommandManager* acm, UndoManager& um)
    : undoManager(um),
      presetFile(pf),
      font(14.0f), 
      commandManager(acm),
      chooseButton("Choose Preset File"),
      rebuildLibraryButton("Rebuild Library"),
      editLocationsButton("File Locations..."),
      blurb(String::empty),
      fileNameLabel(String::empty)
{
    attachToTree();

    commandManager->registerAllCommandsForTarget(this);

    chooseButton.setCommandToTrigger(commandManager, CommandIDs::chooseSelectedPresetFile, true);
    addAndMakeVisible(chooseButton);

    rebuildLibraryButton.setCommandToTrigger(commandManager, CommandIDs::rebuildFileLibrary, true);
    addAndMakeVisible(rebuildLibraryButton);

    editLocationsButton.setCommandToTrigger(commandManager, CommandIDs::editFileLocations, true);
    addAndMakeVisible(editLocationsButton);
    
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
    table.getHeader().addColumn("Library Set",        3, 120, 40, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Author",             4, 100, 40, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("File name",          5, 120, 40, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
        
    table.getHeader().setStretchToFitActive(true);
    
    viewTree.addListener(this);

    addKeyListener(commandManager->getKeyMappings());

    setBounds(0, 0, 1000, 600);
}

PresetFileChooser::~PresetFileChooser()
{
    removeKeyListener(commandManager->getKeyMappings());
    viewTree.removeListener(this);
    UserSettings::getInstance()->removeChangeListener(this);
}

void PresetFileChooser::changeListenerCallback(ChangeBroadcaster* /* source */)
{
    attachToTree();
}

void PresetFileChooser::paint(Graphics& g)
{
    g.fillAll(Colours::lightgrey);
}
    
void PresetFileChooser::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    Rectangle<int> headerBounds(localBounds.removeFromTop(100));
    Rectangle<int> buttonBar(headerBounds.removeFromBottom(30));
    
    chooseButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    rebuildLibraryButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    editLocationsButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    headerBounds.removeFromLeft(4);
    headerBounds.removeFromTop(10);
    fileNameLabel.setBounds(headerBounds.removeFromTop(20).reduced(2, 2));
    blurb.setBounds(headerBounds.reduced(2, 2));
    table.setBounds(localBounds.reduced(2, 2));
}
    
int PresetFileChooser::getNumRows()
{
    return viewTree.getNumChildren();
}

void PresetFileChooser::sortOrderChanged(int newSortColumnId, bool isForwards)
{
    PresetFileSorter sorter(newSortColumnId, isForwards);
    viewTree.sort(sorter, nullptr, true);
}

void PresetFileChooser::paintRowBackground(Graphics& g, int /* rowNumber */, int /* width */, int /* height */, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll (findColour(TextEditor::highlightColourId));
}

void PresetFileChooser::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool /* rowIsSelected */)
{
    g.setColour(Colours::black);
    g.setFont(font);

    String text;

    switch (columnId)
    {
        case 1: text = String::empty; break;
        case 2: text = viewTree.getChild(rowNumber).getProperty(Ids::name); break;
        case 3: text = viewTree.getChild(rowNumber).getProperty(Ids::librarySet); break;
        case 4: text = viewTree.getChild(rowNumber).getProperty(Ids::author); break;
        case 5: text = viewTree.getChild(rowNumber).getProperty(Ids::fileName); break;
    }

    g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);
    
    g.setColour(Colours::black.withAlpha(0.2f));
    g.fillRect(width - 1, 0, 1, height);
}

void PresetFileChooser::backgroundClicked(const MouseEvent&)
{
    table.deselectAllRows();
}

void PresetFileChooser::cellDoubleClicked(int /* rowNumber */, int /* columnId */, const MouseEvent& /* e */)
{
    chooseSelectedPresetFile();
}

void PresetFileChooser::selectedRowsChanged(int lastRowSelected)
{
    blurb.setText(viewTree.getChild(lastRowSelected).getProperty(Ids::blurb), dontSendNotification);
    
    String filePath = viewTree.getChild(lastRowSelected).getProperty(Ids::filePath).toString();
    fileNameLabel.setText("File path: " + filePath, dontSendNotification);
    fileNameLabel.setTooltip(filePath);
    
    commandManager->commandStatusChanged();
}

void PresetFileChooser::getAllCommands(Array <CommandID>& commands)
{
    const CommandID ids[] = {CommandIDs::chooseSelectedPreset,
                             CommandIDs::rebuildFileLibrary,
                             CommandIDs::editFileLocations};
    
    commands.addArray(ids, numElementsInArray (ids));
}

void PresetFileChooser::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::chooseSelectedPresetFile:
        result.setInfo("Choose Preset File", "Loads selected Preset File for editing", CommandCategories::configmgr, !table.getNumSelectedRows());
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
    }
}

void PresetFileChooser::returnKeyPressed(int /* lastRowSelected */)
{
    chooseSelectedPresetFile();
}

bool PresetFileChooser::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::chooseSelectedPreset:  chooseSelectedPresetFile(); break;
        case CommandIDs::rebuildFileLibrary:    rebuildFileLibrary(); break;
        case CommandIDs::editFileLocations:     editFileLocations(); break;
        default:                                return false;
    }

    return true;
}

void PresetFileChooser::editFileLocations()
{
    UserSettings::getInstance()->addChangeListener(this);
    UserSettings::getInstance()->editFileLocations(getParentMonitorArea().getCentreX(), getParentMonitorArea().getCentreY());
}

void PresetFileChooser::chooseSelectedPresetFile()
{
    int selectedRow = table.getSelectedRow();
    
    if (selectedRow > -1)
    {
        presetFile = File(viewTree.getChild(selectedRow).getProperty(Ids::filePath));
        sendChangeMessage();
    }
}

void PresetFileChooser::rebuildFileLibrary()
{
    UserSettings::getInstance()->rebuildFileLibrary();
    attachToTree();
}


void PresetFileChooser::removePresetEntries()
{
    for (int i = viewTree.getNumChildren() - 1; i >= 0; i--)
    {
        if (viewTree.getChild(i).hasType(Ids::preset))
            viewTree.removeChild(i, nullptr);
    }
}

void PresetFileChooser::attachToTree()
{
    viewTree.removeListener(this);
    
    tree = UserSettings::getInstance()->getPresetLibrary();
    viewTree = tree.createCopy();
    
    removePresetEntries();
    
    PresetFileSorter sorter(2, false);
    viewTree.sort(sorter, nullptr, true);
    
    viewTree.addListener(this);
    table.updateContent();
}

ApplicationCommandTarget* PresetFileChooser::getNextCommandTarget()
{
    return nullptr;
}

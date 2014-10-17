/**
 * Table for setting up layout locations
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

#include "LayoutLocationEditor.h"
#include "../Core/Global.h"
#include "UserSettings.h"

LayoutLocationEditorWindow::LayoutLocationEditorWindow(int posX, int posY, 
                                                       const ValueTree& vt, ApplicationCommandManager* acm, 
                                                       UndoManager& um)
    : DocumentWindow("Layout Locations",
                     Colour::greyLevel(0.6f),
                     DocumentWindow::allButtons,
                     true)
{
    setUsingNativeTitleBar (true);
    
    setContentOwned(new LayoutLocationEditor(vt, um, acm), true);
    
    restoreWindowPosition(posX, posY);
    
    setVisible(true);
    setResizable(true, false);

    setWantsKeyboardFocus (false);

    setResizeLimits(400, 200, 32000, 32000);
}

LayoutLocationEditorWindow::~LayoutLocationEditorWindow() {}

void LayoutLocationEditorWindow::closeButtonPressed()
{
    sendChangeMessage();
}

void LayoutLocationEditorWindow::restoreWindowPosition(int posX, int posY)
{
    setBounds(posX, posY, getWidth(), getHeight());
}

class LayoutLocationEditor::LabelComp : public Component
{
public:
    LabelComp(Value& valueToEdit)
        : label(String::empty, String::empty),
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

class LayoutLocationEditor::ButtonComp : public Component,
                                         public Button::Listener
{
public:
    ButtonComp(Value& valueToEdit)
        : textButton("...", "Open Directory Chooser..."),
          value(valueToEdit)
    {
        addAndMakeVisible(textButton);
        value.referTo(value);
        textButton.addListener(this);
    }

    void resized() override
    {
        textButton.setBoundsInset(BorderSize<int> (2));
    }

    void buttonClicked(Button* /* button */) override
    {
        String currentPath = value.toString();

        FileChooser fileChooser("Please select the base folder for your layout location...", File(currentPath));
    
        if (fileChooser.browseForDirectory())
            value = fileChooser.getResult().getFullPathName();
    }

    void setButtonValue(Value& valueToReferTo)
    {
        value.referTo(valueToReferTo);
    }

private:
    TextButton textButton;
    Value value;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonComp)
};

LayoutLocationEditor::LayoutLocationEditor(const ValueTree& valueTree, UndoManager& um, ApplicationCommandManager* acm)
    : tree(valueTree), undoManager(um), font(14.0f), commandManager(acm),
      addLayoutLocationButton("Add"),
      removeLayoutLocationButton("Remove"),
      moveUpButton("Move Up"),
      moveDownButton("Move Down"),
      rebuildButton("Rebuild library"),
      undoButton("Undo"),
      redoButton("Redo")
{
    DBG("LayoutLocationEditor::LayoutLocationEditor");
    
    commandManager->registerAllCommandsForTarget(this);

    addAndMakeVisible(table);
    
    table.setModel(this);
    
    table.setColour(ListBox::outlineColourId, Colours::grey);
    table.setOutlineThickness (1);
    
    table.getHeader().addColumn(String::empty, 1, 30,  30,  30, TableHeaderComponent::notResizableOrSortable & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Location",    2, 200, 100, -1, TableHeaderComponent::defaultFlags & TableHeaderComponent::notSortable & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn(String::empty, 3, 30,  30,  30, TableHeaderComponent::notResizableOrSortable & ~TableHeaderComponent::draggable);
    
    table.getHeader().setStretchToFitActive(true);
    table.setMultipleSelectionEnabled (true);
    
    tree.addListener(this);

    addAndMakeVisible(addLayoutLocationButton);
    addLayoutLocationButton.setCommandToTrigger(commandManager, CommandIDs::addLayoutLocation, true);
    
    addAndMakeVisible(removeLayoutLocationButton);
    removeLayoutLocationButton.setCommandToTrigger(commandManager, CommandIDs::removeLayoutLocations, true);
    
    addAndMakeVisible(moveUpButton);
    moveUpButton.setCommandToTrigger(commandManager, CommandIDs::moveUp, true);

    addAndMakeVisible(moveDownButton);
    moveDownButton.setCommandToTrigger(commandManager, CommandIDs::moveDown, true);

    addAndMakeVisible(undoButton);
    undoButton.setCommandToTrigger(commandManager, CommandIDs::undo, true);

    addAndMakeVisible(redoButton);
    redoButton.setCommandToTrigger(commandManager, CommandIDs::redo, true);

    addAndMakeVisible(rebuildButton);
    rebuildButton.setCommandToTrigger(commandManager, CommandIDs::rebuildLayoutLibrary, true);

    addKeyListener(commandManager->getKeyMappings());

    setBounds(0, 0, 600, 300);
    //setSize(getLocalBounds().getWidth(), getLocalBounds().getHeight());
}

LayoutLocationEditor::~LayoutLocationEditor()
{
    removeKeyListener(commandManager->getKeyMappings());
    tree.removeListener(this);
}

void LayoutLocationEditor::paint(Graphics& g)
{
    g.fillAll(Colours::darkgrey);
}
    
void LayoutLocationEditor::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    Rectangle<int> buttonBar(localBounds.removeFromBottom(30).reduced(4, 4));

    addLayoutLocationButton.setBounds(buttonBar.removeFromLeft(70));
    removeLayoutLocationButton.setBounds(buttonBar.removeFromLeft(70));
    buttonBar.removeFromLeft(20);
    moveUpButton.setBounds(buttonBar.removeFromLeft(70));
    moveDownButton.setBounds(buttonBar.removeFromLeft(70));
    buttonBar.removeFromLeft(20);
    rebuildButton.setBounds(buttonBar.removeFromLeft(90));
    buttonBar.removeFromLeft(20);
    undoButton.setBounds(buttonBar.removeFromLeft(70));
    redoButton.setBounds(buttonBar.removeFromLeft(70));

    DBG("LayoutLocations::resized - Local Bounds: " + String(localBounds.getWidth()) + ", " + String(localBounds.getHeight()));
    table.setBounds(localBounds.reduced(4, 4));
}
    
int LayoutLocationEditor::getNumRows()
{
    return tree.getNumChildren();
}

Component* LayoutLocationEditor::refreshComponentForCell(int rowNumber, int columnId, bool /* isRowSelected */, Component* existingComponentToUpdate)
{
    Identifier propertyId;

    if (columnId == 2 || columnId == 3)
    {
        propertyId = Ids::folder;
    }
    else
    {
        return nullptr;
    }
    
    Value valueToEdit(tree.getChild(rowNumber).getPropertyAsValue(propertyId, &undoManager));
    
    if (columnId == 2)
    {
        LabelComp* labelComp = (LabelComp*)existingComponentToUpdate;

        if (labelComp == nullptr)
            labelComp = new LabelComp(valueToEdit);
        else
            labelComp->setLabelValue(valueToEdit);
    
        return labelComp;
    }
    else
    {
        ButtonComp* buttonComp = (ButtonComp*)existingComponentToUpdate;

        if (buttonComp == nullptr)
            buttonComp = new ButtonComp(valueToEdit);
        else
            buttonComp->setButtonValue(valueToEdit);
    
        return buttonComp;
    }
}

void LayoutLocationEditor::paintRowBackground(Graphics& g, int /* rowNumber */, int /* width */, int /* height */, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll (findColour (TextEditor::highlightColourId));
}

void LayoutLocationEditor::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool /* rowIsSelected */)
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

void LayoutLocationEditor::backgroundClicked(const MouseEvent&)
{
    table.deselectAllRows();
}

void LayoutLocationEditor::deleteKeyPressed(int)
{
    removeLayoutLocations();
}

void LayoutLocationEditor::getAllCommands(Array <CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::addLayoutLocation,
                              CommandIDs::removeLayoutLocations,
                              CommandIDs::moveUp,
                              CommandIDs::moveDown,
                              CommandIDs::undo,
                              CommandIDs::redo,
                              CommandIDs::rebuildLayoutLibrary
                            };

    commands.addArray(ids, numElementsInArray (ids));
}

void LayoutLocationEditor::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::addLayoutLocation:
        result.setInfo("Add", "Add new Layout Location", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress('n', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::removeLayoutLocations:
        result.setInfo ("Remove", "Remove selected Layout Location", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress(KeyPress::deleteKey));
        break;
    case CommandIDs::moveUp:
        result.setInfo ("Move Up", "Move all selected Layout Locations up one position", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress(KeyPress::upKey, ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::moveDown:
        result.setInfo ("Move Down", "Move all selected Layout Locations down one position", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress(KeyPress::downKey, ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::undo:
        result.setInfo("Undo", "Undo latest change", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('z', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::redo:
        result.setInfo("Redo", "Redo latest change", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('y', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::rebuildLayoutLibrary:
        result.setInfo("Rebuild library", "Rebuild Layout Library", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('r', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    }
}

bool LayoutLocationEditor::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::addLayoutLocation:     addLayoutLocation(); break;
        case CommandIDs::removeLayoutLocations: removeLayoutLocations(); break;
        case CommandIDs::moveUp:                moveLayoutLocations(true); break;
        case CommandIDs::moveDown:              moveLayoutLocations(false); break;
        case CommandIDs::undo:                  undo(); break;
        case CommandIDs::redo:                  redo(); break;
        case CommandIDs::rebuildLayoutLibrary:  UserSettings::getInstance()->rebuildLayoutLibrary(); break;
        default:                                return false;
    }

    return true;
}

void LayoutLocationEditor::undo()
{
    undoManager.undo();
}

void LayoutLocationEditor::redo()
{
    undoManager.redo();
}

void LayoutLocationEditor::addLayoutLocation()
{
    ValueTree newLayoutLocation(Ids::location);
    String newLocationName;

    newLayoutLocation.setProperty(Ids::folder, String::empty, &undoManager);

    tree.addChild(newLayoutLocation, -1, &undoManager);
}

void LayoutLocationEditor::removeLayoutLocations()
{
    SparseSet<int> selectedRows(table.getSelectedRows());
    Array<int> itemsToRemove;
    
    for (int i = 0; i < selectedRows.size(); i++)
    {
        itemsToRemove.add(selectedRows[i]);
    }

    DefaultElementComparator<int> sorter;
    itemsToRemove.sort(sorter);
    
    for (int i = itemsToRemove.size() - 1; i > -1; i--)
    {
        tree.removeChild(itemsToRemove[i], &undoManager);
    }
}

void LayoutLocationEditor::moveLayoutLocations(bool moveUp)
{
    SparseSet<int> selectedRows(table.getSelectedRows());
    Array<int> itemsToMove;
    
    for (int i = 0; i < selectedRows.size(); i++)
    {
        itemsToMove.add(selectedRows[i]);
    }

    DefaultElementComparator<int> sorter;
    itemsToMove.sort(sorter);
    
    if (moveUp)
    {
        for (int i = 0; i < itemsToMove.size(); i++)
        {
            int currentIndex = itemsToMove[i];
            int newIndex     = jmax(itemsToMove[i] - 1, 0);

            table.deselectRow(currentIndex);

            tree.moveChild(currentIndex, newIndex, &undoManager);
            table.selectRow(newIndex, false, false);
        }
    }
    else
    {
        for (int i = itemsToMove.size() - 1; i > -1; i--)
        {
            int currentIndex = itemsToMove[i];
            int newIndex     = jmin(itemsToMove[i] + 1, tree.getNumChildren());
            
            table.deselectRow(currentIndex);

            tree.moveChild(currentIndex, newIndex, &undoManager);
            table.selectRow(newIndex, false, false);
        }
    }  
}

ApplicationCommandTarget* LayoutLocationEditor::getNextCommandTarget()
{
    return nullptr;
}

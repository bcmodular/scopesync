/**
 * Table for setting up File locations
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

#include "FileLocationEditor.h"
#include "../Core/Global.h"

FileLocationEditorWindow::FileLocationEditorWindow(int posX, int posY, 
                                                   ApplicationCommandManager* acm, 
                                                   UndoManager& um)
    : DocumentWindow("File Locations",
                     Colour::greyLevel(0.6f),
                     DocumentWindow::allButtons,
                     true)
{
    setUsingNativeTitleBar (true);
    
    setContentOwned(new FileLocationEditor(um, acm), true);
    
    restoreWindowPosition(posX, posY);

    Component::setVisible(true);
    setResizable(true, false);

    setWantsKeyboardFocus (false);

    setResizeLimits(400, 200, 32000, 32000);
}

FileLocationEditorWindow::~FileLocationEditorWindow() {}

ValueTree FileLocationEditorWindow::getFileLocations() const
{
    FileLocationEditor* fileLocationEditor = dynamic_cast<FileLocationEditor*>(getContentComponent());
    return fileLocationEditor->getFileLocations();
}

bool FileLocationEditorWindow::locationsHaveChanged() const
{
    FileLocationEditor* fileLocationEditor = dynamic_cast<FileLocationEditor*>(getContentComponent());
    return fileLocationEditor->locationsHaveChanged();
}

void FileLocationEditorWindow::closeButtonPressed()
{
    userSettings->hideFileLocationsWindow();
}

void FileLocationEditorWindow::restoreWindowPosition(int posX, int posY)
{
    setCentrePosition(posX, posY);
}

class FileLocationEditor::LabelComp : public Component
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

class FileLocationEditor::ButtonComp : public Component,
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

        FileChooser fileChooser("Please select the base folder for your File location...", File(currentPath));
    
        if (fileChooser.browseForDirectory())
        {
            File result = fileChooser.getResult();
            value = result.getFullPathName();
        }
    }

    void setButtonValue(Value& valueToReferTo)
    {
        value.referTo(valueToReferTo);
    }

private:
    TextButton textButton;
    Value      value;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonComp)
};

FileLocationEditor::FileLocationEditor(UndoManager& um, ApplicationCommandManager* acm)
    : font(14.0f), undoManager(um), commandManager(acm),
      sizeWarning("Size Warning"),
      addFileLocationButton("Add"),
      removeFileLocationButton("Remove"),
      moveUpButton("Move Up"),
      moveDownButton("Move Down"),
      rebuildButton("Rebuild library"),
      undoButton("Undo"),
      redoButton("Redo")
{
    DBG("FileLocationEditor::FileLocationEditor");
    locationsChanged = false;

    tree = userSettings->getFileLocations();
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

    addAndMakeVisible(addFileLocationButton);
    addFileLocationButton.setCommandToTrigger(commandManager, CommandIDs::addFileLocation, true);
    
    addAndMakeVisible(removeFileLocationButton);
    removeFileLocationButton.setCommandToTrigger(commandManager, CommandIDs::removeFileLocations, true);
    
    addAndMakeVisible(moveUpButton);
    moveUpButton.setCommandToTrigger(commandManager, CommandIDs::moveUp, true);
    
    addAndMakeVisible(moveDownButton);
    moveDownButton.setCommandToTrigger(commandManager, CommandIDs::moveDown, true);

    addAndMakeVisible(undoButton);
    undoButton.setCommandToTrigger(commandManager, CommandIDs::undo, true);

    addAndMakeVisible(redoButton);
    redoButton.setCommandToTrigger(commandManager, CommandIDs::redo, true);

    addAndMakeVisible(rebuildButton);
    rebuildButton.setCommandToTrigger(commandManager, CommandIDs::rebuildFileLibrary, true);

    sizeWarning.setText("Note: Only the first 100,000 files in a location are read in a library rebuild, so make sure not to add very large directories",
                        dontSendNotification);
    sizeWarning.setColour(Label::textColourId, Colours::white);
    sizeWarning.setJustificationType(Justification::topLeft);
    sizeWarning.setMinimumHorizontalScale(1.0f);
    addAndMakeVisible(sizeWarning);
    
    addKeyListener(commandManager->getKeyMappings());

    setBounds(0, 0, 600, 300);

	startTimer(100);
}

FileLocationEditor::~FileLocationEditor()
{
	stopTimer();
    removeKeyListener(commandManager->getKeyMappings());
    tree.removeListener(this);
}

void FileLocationEditor::timerCallback()
{
	stopTimer();

#if JUCE_WINDOWS
	String stockFileName = WindowsRegistry::getValue("HKEY_CURRENT_USER\\Software\\ScopeSync\\InstallLocation");

	if (stockFileName.isNotEmpty() && tree.getNumChildren() == 0)
	{
		AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
			"Add stock location?",
			"Would you like to add the stock library location (" + stockFileName + ")?",
			String::empty,
			String::empty,
			this,
			ModalCallbackFunction::forComponent(alertBoxAddStockLocationConfirm, this, stockFileName));
	}
#endif
}

void FileLocationEditor::alertBoxAddStockLocationConfirm(int result,
														 FileLocationEditor* fileLocationEditor, 
														 String stockFileName)
{
	if (result)
		fileLocationEditor->addFileLocation(stockFileName);
}

ValueTree FileLocationEditor::getFileLocations() const
{
    return tree;
}

bool FileLocationEditor::locationsHaveChanged() const
{
    return locationsChanged;
}

void FileLocationEditor::paint(Graphics& g)
{
    g.fillAll(Colours::darkgrey);
}
    
void FileLocationEditor::resized()
{
    Rectangle<int> localBounds(getLocalBounds());

    sizeWarning.setBounds(localBounds.removeFromBottom(50).reduced(4, 4));

    Rectangle<int> buttonBar(localBounds.removeFromBottom(30).reduced(4, 4));

    addFileLocationButton.setBounds(buttonBar.removeFromLeft(70));
    removeFileLocationButton.setBounds(buttonBar.removeFromLeft(70));
    buttonBar.removeFromLeft(20);
    moveUpButton.setBounds(buttonBar.removeFromLeft(70));
    moveDownButton.setBounds(buttonBar.removeFromLeft(70));
    buttonBar.removeFromLeft(20);
    rebuildButton.setBounds(buttonBar.removeFromLeft(90));
    buttonBar.removeFromLeft(20);
    undoButton.setBounds(buttonBar.removeFromLeft(70));
    redoButton.setBounds(buttonBar.removeFromLeft(70));

    table.setBounds(localBounds.reduced(4, 4));
}
    
int FileLocationEditor::getNumRows()
{
    return tree.getNumChildren();
}

Component* FileLocationEditor::refreshComponentForCell(int rowNumber, int columnId, bool /* isRowSelected */, Component* existingComponentToUpdate)
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
        LabelComp* labelComp = static_cast<LabelComp*>(existingComponentToUpdate);

        if (labelComp == nullptr)
            labelComp = new LabelComp(valueToEdit);
        else
            labelComp->setLabelValue(valueToEdit);
    
        return labelComp;
    }
    else
    {
        ButtonComp* buttonComp = static_cast<ButtonComp*>(existingComponentToUpdate);

        if (buttonComp == nullptr)
            buttonComp = new ButtonComp(valueToEdit);
        else
            buttonComp->setButtonValue(valueToEdit);
    
        return buttonComp;
    }
}

void FileLocationEditor::paintRowBackground(Graphics& g, int /* rowNumber */, int /* width */, int /* height */, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll (findColour (TextEditor::highlightColourId));
}

void FileLocationEditor::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool /* rowIsSelected */)
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

void FileLocationEditor::backgroundClicked(const MouseEvent&)
{
    table.deselectAllRows();
}

void FileLocationEditor::deleteKeyPressed(int)
{
    removeFileLocations();
}

void FileLocationEditor::valueTreePropertyChanged(ValueTree&, const Identifier&)
{
    table.updateContent();
    locationsChanged = true;
}

void FileLocationEditor::valueTreeChildAdded(ValueTree&, ValueTree&)
{
    table.updateContent();
}

void FileLocationEditor::valueTreeChildRemoved(ValueTree&, ValueTree&, int)
{
    table.updateContent();
}

void FileLocationEditor::valueTreeChildOrderChanged(ValueTree&, int, int)
{
    table.updateContent();
}

void FileLocationEditor::valueTreeParentChanged(ValueTree&)
{
    table.updateContent();
}

void FileLocationEditor::getAllCommands(Array <CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::addFileLocation,
                              CommandIDs::removeFileLocations,
                              CommandIDs::moveUp,
                              CommandIDs::moveDown,
                              CommandIDs::undo,
                              CommandIDs::redo,
                              CommandIDs::rebuildFileLibrary
                            };

    commands.addArray(ids, numElementsInArray (ids));
}

void FileLocationEditor::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::addFileLocation:
        result.setInfo("Add", "Add new File Location", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress('n', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::removeFileLocations:
        result.setInfo ("Remove", "Remove selected File Location", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress(KeyPress::deleteKey));
        break;
    case CommandIDs::moveUp:
        result.setInfo ("Move Up", "Move all selected File Locations up one position", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress(KeyPress::upKey, ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::moveDown:
        result.setInfo ("Move Down", "Move all selected File Locations down one position", CommandCategories::configmgr, 0);
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
    case CommandIDs::rebuildFileLibrary:
        result.setInfo("Rebuild library", "Rebuild File Library", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('r', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    }
}

bool FileLocationEditor::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::addFileLocation:     addFileLocation(); break;
        case CommandIDs::removeFileLocations: removeFileLocations(); break;
        case CommandIDs::moveUp:              moveFileLocations(true); break;
        case CommandIDs::moveDown:            moveFileLocations(false); break;
        case CommandIDs::undo:                undo(); break;
        case CommandIDs::redo:                redo(); break;
        case CommandIDs::rebuildFileLibrary:  rebuildFileLibrary(); break;
        default:                              return false;
    }

    return true;
}

void FileLocationEditor::undo() const
{
    undoManager.undo();
}

void FileLocationEditor::redo() const
{
    undoManager.redo();
}

void FileLocationEditor::addFileLocation()
{
	FileChooser fileChooser("Please select the base folder for your File location...");

	if (fileChooser.browseForDirectory())
	{
		String result = fileChooser.getResult().getFullPathName();
		addFileLocation(result);
	}
}

UndoManager& FileLocationEditor::getUndoManager() const
{
    return undoManager;
}

void FileLocationEditor::addFileLocation(const String& newFileLocation)
{
	ValueTree newLocation(Ids::location);
	String newLocationName;

	newLocation.setProperty(Ids::folder, newFileLocation, &undoManager);

	tree.addChild(newLocation, -1, &undoManager);
	locationsChanged = true;
}

void FileLocationEditor::removeFileLocations()
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

    locationsChanged = true;
}

void FileLocationEditor::moveFileLocations(bool moveUp)
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
    
    locationsChanged = true;
}

void FileLocationEditor::rebuildFileLibrary()
{
    userSettings->updateFileLocations(tree);
    locationsChanged = false;
}

ApplicationCommandTarget* FileLocationEditor::getNextCommandTarget()
{
    return nullptr;
}

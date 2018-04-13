/**
 * Table for choosing a Configuration to use
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

#include "ConfigurationChooser.h"
#include "../Core/ScopeSync.h"
#include "../Core/ScopeSyncGUI.h"

/* =========================================================================
 * ConfigurationChooserWindow
 */
ConfigurationChooserWindow::ConfigurationChooserWindow(int posX, int posY, 
                                                       ScopeSync& ss,
                                                       ScopeSyncGUI& ssg,
                                                       ApplicationCommandManager* acm)
    : DocumentWindow("Configuration Chooser",
                     Colour::greyLevel(0.6f),
                     DocumentWindow::allButtons,
                     true),
      scopeSyncGUI(ssg)
{
    setUsingNativeTitleBar (true);
    
    setContentOwned(new ConfigurationChooser(ss, acm), true);
    
    restoreWindowPosition(posX, posY);

    Component::setVisible(true);
    setResizable(true, false);

    setWantsKeyboardFocus (false);

    setResizeLimits(400, 200, 32000, 32000);
}

ConfigurationChooserWindow::~ConfigurationChooserWindow() {}

void ConfigurationChooserWindow::closeButtonPressed()
{
    closeWindow();
}

void ConfigurationChooserWindow::closeWindow() const
{
    scopeSyncGUI.hideConfigurationChooserWindow();
}

void ConfigurationChooserWindow::restoreWindowPosition(int posX, int posY)
{
    setCentrePosition(posX, posY);
}

/* =========================================================================
 * ConfigurationSorter - A comparator used to sort our data when the user clicks a column header
 */
class ConfigurationSorter
{
public:
    ConfigurationSorter (const int columnIdToSort, bool forwards)
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
        // Sort by layoutName
        if (columnId == 5)
        {
            String firstString  = first.getProperty(Ids::layoutName, String::empty);
            String secondString = second.getProperty(Ids::layoutName, String::empty);
            result = firstString.compareNatural(secondString);
        }
        // Sort by layoutLibrarySet
        else if (columnId == 6)
        {
            String firstString  = first.getProperty(Ids::layoutLibrarySet, String::empty);
            String secondString = second.getProperty(Ids::layoutLibrarySet, String::empty);
            result = firstString.compareNatural(secondString);
        }
        // Sort by fileName
        else if (columnId == 7)
        {
            String firstString  = first.getProperty(Ids::fileName, String::empty);
            String secondString = second.getProperty(Ids::fileName, String::empty);
            result = firstString.compareNatural(secondString);
        }
        // Sort by mruTime
        else if (columnId == 8)
        {
            String firstString  = first.getProperty(Ids::mruTime, String::empty);
            String secondString = second.getProperty(Ids::mruTime, String::empty);
            result = firstString.compareNatural(secondString);
        }

        return direction * result;
    }

private:
    int columnId;
    int direction;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConfigurationSorter)
};

/* =========================================================================
 * ConfigurationChooser
 */
ConfigurationChooser::ConfigurationChooser(ScopeSync& ss,
                                           ApplicationCommandManager* acm)
    : font(14.0f), 
      scopeSync(ss),
      commandManager(acm),
      chooseButton("Choose Configuration"),
      rebuildLibraryButton("Rebuild Library"),
      unloadConfigButton("Unload Configuration"),
      editLocationsButton("File Locations..."),
      blurb(String::empty),
      fileNameLabel(String::empty)
{
    userSettings->addActionListener(this);
    attachToTree();

    commandManager->registerAllCommandsForTarget(this);

    chooseButton.setCommandToTrigger(commandManager, CommandIDs::chooseSelectedConfiguration, true);
    addAndMakeVisible(chooseButton);

    rebuildLibraryButton.setCommandToTrigger(commandManager, CommandIDs::rebuildFileLibrary, true);
    addAndMakeVisible(rebuildLibraryButton);

    unloadConfigButton.setCommandToTrigger(commandManager, CommandIDs::unloadConfiguration, true);
    addAndMakeVisible(unloadConfigButton);
    
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
    table.getHeader().addColumn("Layout Name",        5, 120, 40, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Layout Library Set", 6, 120, 40, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("File Name",          7, 120, 40, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Last Used",          8, 120, 40, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
        
    table.getHeader().setStretchToFitActive(true);
    
    selectCurrentConfiguration(scopeSync.getConfigurationFile().getFullPathName());

    viewTree.addListener(this);

    addKeyListener(commandManager->getKeyMappings());

    setBounds(0, 0, 1000, 600);

	startTimer(100);
}

ConfigurationChooser::~ConfigurationChooser()
{
	stopTimer();
    removeKeyListener(commandManager->getKeyMappings());
    viewTree.removeListener(this);
    userSettings->removeActionListener(this);
}

void ConfigurationChooser::timerCallback()
{
	stopTimer();

	if (viewTree.getNumChildren() == 0)
		editFileLocations();
	else if (userSettings->getPropertyBoolValue("autorebuildlibrary", false))
		rebuildFileLibrary();
}

void ConfigurationChooser::actionListenerCallback(const String& message)
{
    DBG("ConfigurationChooser::actionListenerCallback");
    
    if (message == "configurationlibraryupdated")
        attachToTree();
}

void ConfigurationChooser::removeExcludedConfigurations()
{
    for (int i = viewTree.getNumChildren() - 1; i >= 0; i--)
    {
        bool excludeFromChooser = viewTree.getChild(i).getProperty(Ids::excludeFromChooser, false);
        
        if (excludeFromChooser)
            viewTree.removeChild(i, nullptr);
    }
}

void ConfigurationChooser::selectCurrentConfiguration(const String& filePath)
{
    for (int i = 0; i < viewTree.getNumChildren(); i++)
    {
        String itemFilePath = viewTree.getChild(i).getProperty(Ids::filePath);
        
        if (itemFilePath.equalsIgnoreCase(filePath))
            table.selectRow(i);
    }
}

void ConfigurationChooser::paint(Graphics& g)
{
    g.fillAll(Colours::lightgrey);
}
    
void ConfigurationChooser::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    Rectangle<int> headerBounds(localBounds.removeFromTop(100));
    Rectangle<int> buttonBar(headerBounds.removeFromBottom(30));
    
    chooseButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    rebuildLibraryButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    unloadConfigButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    editLocationsButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    headerBounds.removeFromLeft(4);
    headerBounds.removeFromTop(10);
    fileNameLabel.setBounds(headerBounds.removeFromTop(20).reduced(2, 2));
    blurb.setBounds(headerBounds.reduced(2, 2));
    table.setBounds(localBounds.reduced(2, 2));
}
    
int ConfigurationChooser::getNumRows()
{
    return viewTree.getNumChildren();
}

void ConfigurationChooser::sortOrderChanged(int newSortColumnId, bool isForwards)
{
    ConfigurationSorter sorter(newSortColumnId, isForwards);
    viewTree.sort(sorter, nullptr, true);
}

void ConfigurationChooser::paintRowBackground(Graphics& g, int /* rowNumber */, int /* width */, int /* height */, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll (findColour(TextEditor::highlightColourId));
}

void ConfigurationChooser::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool /* rowIsSelected */)
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
        case 5: text = viewTree.getChild(rowNumber).getProperty(Ids::layoutName).toString(); break;
        case 6: text = viewTree.getChild(rowNumber).getProperty(Ids::layoutLibrarySet); break;
        case 7: text = viewTree.getChild(rowNumber).getProperty(Ids::fileName); break;
        case 8: text = viewTree.getChild(rowNumber).getProperty(Ids::mruTime); break;
    }

    g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);
    
    g.setColour(Colours::black.withAlpha(0.2f));
    g.fillRect(width - 1, 0, 1, height);
}

void ConfigurationChooser::backgroundClicked(const MouseEvent&)
{
    table.deselectAllRows();
}

void ConfigurationChooser::cellDoubleClicked(int rowNumber, int /* columnId */, const MouseEvent& /* e */)
{
    String newFileName = viewTree.getChild(rowNumber).getProperty(Ids::filePath).toString();

    chooseConfiguration(newFileName);
}

void ConfigurationChooser::selectedRowsChanged(int lastRowSelected)
{
    blurb.setText(viewTree.getChild(lastRowSelected).getProperty(Ids::blurb), dontSendNotification);
    
    String filePath = viewTree.getChild(lastRowSelected).getProperty(Ids::filePath).toString();
    fileNameLabel.setText("File path: " + filePath, dontSendNotification);
    fileNameLabel.setTooltip(filePath);
    
    commandManager->commandStatusChanged();
}

void ConfigurationChooser::getAllCommands(Array <CommandID>& commands)
{
    const CommandID ids[] = {CommandIDs::chooseSelectedConfiguration,
                             CommandIDs::rebuildFileLibrary,
                             CommandIDs::unloadConfiguration,
                             CommandIDs::editFileLocations};
    
    commands.addArray(ids, numElementsInArray (ids));
}

void ConfigurationChooser::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::chooseSelectedConfiguration:
        result.setInfo("Choose Selected Configuration", "Changes the loaded Configuration", CommandCategories::configmgr, !table.getNumSelectedRows());
        result.defaultKeypresses.add(KeyPress(KeyPress::returnKey));
        break;
    case CommandIDs::rebuildFileLibrary:
        result.setInfo("Rebuild library", "Rebuild File Library", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('r', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    case CommandIDs::unloadConfiguration:
        result.setInfo("Unload Configuration", "Reverts to the loader configuration", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('u', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::editFileLocations:
        result.setInfo("File Locations...", "Open File Locations edit window", CommandCategories::usersettings, 0);
        result.defaultKeypresses.add(KeyPress('f', ModifierKeys::commandModifier, 0));
        break;
    }
}

void ConfigurationChooser::returnKeyPressed(int /* lastRowSelected */)
{
    chooseSelectedConfiguration();
}

bool ConfigurationChooser::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::chooseSelectedConfiguration:  chooseSelectedConfiguration(); break;
        case CommandIDs::rebuildFileLibrary:           rebuildFileLibrary(); break;
        case CommandIDs::unloadConfiguration:          unloadConfiguration(); break;
        case CommandIDs::editFileLocations:            editFileLocations(); break;
        default:                                       return false;
    }

    return true;
}

void ConfigurationChooser::editFileLocations()
{
    userSettings->addActionListener(this);
    userSettings->editFileLocations(getParentMonitorArea().getCentreX(), getParentMonitorArea().getCentreY());
}

void ConfigurationChooser::chooseConfiguration(const String& newFileName)
{
    if (scopeSync.getConfigurationFile().getFullPathName().equalsIgnoreCase(newFileName))
    {
        if (scopeSync.configurationHasUnsavedChanges())
        {
            AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
                                         "Are you sure?",
                                         "There are unsaved changes. This will discard them and clear the undo history.",
                                         String::empty,
                                         String::empty,
                                         this,
                                         ModalCallbackFunction::forComponent(alertBoxReloadConfirm, this));

            return;
        }
        else
            scopeSync.reloadSavedConfiguration();
    }
    else
        scopeSync.changeConfiguration(newFileName);

    closeWindow();
}

void ConfigurationChooser::alertBoxReloadConfirm(int result, ConfigurationChooser* configurationChooser)
{
    if (result)
    {
        configurationChooser->scopeSync.reloadSavedConfiguration();
        configurationChooser->closeWindow();
    }
}

void ConfigurationChooser::chooseSelectedConfiguration()
{
    String newFileName = viewTree.getChild(table.getSelectedRow()).getProperty(Ids::filePath).toString();

    chooseConfiguration(newFileName);
}

void ConfigurationChooser::closeWindow() const
{
    ConfigurationChooserWindow* window = static_cast<ConfigurationChooserWindow*>(getParentComponent());
    window->closeWindow();
}

void ConfigurationChooser::rebuildFileLibrary()
{
    userSettings->rebuildFileLibrary(true, false, false);
}

void ConfigurationChooser::attachToTree()
{
    viewTree.removeListener(this);
    
    tree = userSettings->getConfigurationLibrary();
    viewTree = tree.createCopy();
    
    removeExcludedConfigurations();
    
    ConfigurationSorter sorter(8, false);
    viewTree.sort(sorter, nullptr, true);
    
    viewTree.addListener(this);
    table.updateContent();
}

void ConfigurationChooser::unloadConfiguration() const
{
    scopeSync.unloadConfiguration();
    closeWindow();
}

ApplicationCommandTarget* ConfigurationChooser::getNextCommandTarget()
{
    return nullptr;
}

/**
 * Table for choosing a Layout to use with a Configuration
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

#include "LayoutChooser.h"
#include "../Configuration/ConfigurationPanel.h"

/* =========================================================================
 * LayoutChooserWindow
 */
LayoutChooserWindow::LayoutChooserWindow(int posX, int posY, 
                                         const Value& layoutName,
                                         const Value& layoutLibrarySet,
                                         ConfigurationPanel& cp,
                                         ApplicationCommandManager* acm)
    : DocumentWindow("Layout Chooser",
                     Colour::greyLevel(0.6f),
                     DocumentWindow::allButtons,
                     true),
      configurationPanel(cp)
{
    setUsingNativeTitleBar (true);
    
    setContentOwned(new LayoutChooser(layoutName, layoutLibrarySet, acm), true);
    
    restoreWindowPosition(posX, posY);

    Component::setVisible(true);
    setResizable(true, false);

    setWantsKeyboardFocus (false);

    setResizeLimits(400, 200, 32000, 32000);
}

LayoutChooserWindow::~LayoutChooserWindow() {}

void LayoutChooserWindow::closeWindow() const
{
    configurationPanel.updateLayout();
    configurationPanel.hideLayoutChooser();
}

void LayoutChooserWindow::closeButtonPressed()
{
    closeWindow();
}

void LayoutChooserWindow::restoreWindowPosition(int posX, int posY)
{
    setCentrePosition(posX, posY);
}

/* =========================================================================
 * LayoutSorter - A comparator used to sort our data when the user clicks a column header
 */
class LayoutSorter
{
public:
    LayoutSorter (const int columnIdToSort, bool forwards)
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
        // Sort by panelWidth
        else if (columnId == 5)
        {
            int firstPixelCount  = int(first.getProperty(Ids::panelWidth, 0))  * int(first.getProperty(Ids::panelHeight, 0));
            int secondPixelCount = int(second.getProperty(Ids::panelWidth, 0)) * int(second.getProperty(Ids::panelHeight, 0));
            result = (firstPixelCount < secondPixelCount) ? -1 : ((secondPixelCount < firstPixelCount) ? 1 : 0);
        }
        // Sort by numEncoders
        else if (columnId == 6)
        {
            int firstCount  = first.getProperty(Ids::numEncoders, 0);
            int secondCount = second.getProperty(Ids::numEncoders, 0);
            result = (firstCount < secondCount) ? -1 : ((secondCount < firstCount) ? 1 : 0);
        }
        // Sort by numFaders
        else if (columnId == 7)
        {
            int firstCount  = first.getProperty(Ids::numFaders, 0);
            int secondCount = second.getProperty(Ids::numFaders, 0);
            result = (firstCount < secondCount) ? -1 : ((secondCount < firstCount) ? 1 : 0);
        }
        // Sort by numButtons
        else if (columnId == 8)
        {
            int firstCount  = first.getProperty(Ids::numButtons, 0);
            int secondCount = second.getProperty(Ids::numButtons, 0);
            result = (firstCount < secondCount) ? -1 : ((secondCount < firstCount) ? 1 : 0);
        }
        // Sort by numParameters
        else if (columnId == 9)
        {
            int firstCount  = first.getProperty(Ids::numParameters, 0);
            int secondCount = second.getProperty(Ids::numParameters, 0);
            result = (firstCount < secondCount) ? -1 : ((secondCount < firstCount) ? 1 : 0);
        }
        // Sort by mruTime
        else if (columnId == 10)
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayoutSorter)
};

/* =========================================================================
 * LayoutChooser
 */
LayoutChooser::LayoutChooser(const Value& layoutName,
                             const Value& layoutLibrarySet,
                             ApplicationCommandManager* acm)
    : font(14.0f), 
      commandManager(acm),
      chooseButton("Choose Layout"),
      rebuildLibraryButton("Rebuild Library"),
      editLocationsButton("File Locations..."),
      blurb(String::empty)
{
    userSettings->addActionListener(this);
    
    useImageCache = userSettings->getPropertyBoolValue("useimagecache", true);

    attachToTree();

    commandManager->registerAllCommandsForTarget(this);

    chooseButton.setCommandToTrigger(commandManager, CommandIDs::chooseSelectedLayout, true);
    addAndMakeVisible(chooseButton);

    rebuildLibraryButton.setCommandToTrigger(commandManager, CommandIDs::rebuildFileLibrary, true);
    addAndMakeVisible(rebuildLibraryButton);

    editLocationsButton.setCommandToTrigger(commandManager, CommandIDs::editFileLocations, true);
    addAndMakeVisible(editLocationsButton);
    
    blurb.setJustificationType(Justification::topLeft);
    addAndMakeVisible(blurb);

    addAndMakeVisible(table);
    
    table.setModel(this);
    
    table.setColour(ListBox::outlineColourId, Colours::darkgrey);
    table.setOutlineThickness (4);
    
    table.getHeader().addColumn(String::empty, 1,  10,  10, 10, TableHeaderComponent::notResizableOrSortable & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Name",        2,  120, 40, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Library Set", 3,  120, 40, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Author",      4,  100, 40, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Dimensions",  5,  80,  40, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Encoders",    6,  50,  30, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Faders",      7,  50,  30, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Buttons",     8,  50,  30, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Parameters",  9,  50,  30, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Last Used",   10, 120, 40, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    
    table.getHeader().setStretchToFitActive(true);
    
    name.referTo(layoutName);
    librarySet.referTo(layoutLibrarySet);

    selectCurrentLayout();

    viewTree.addListener(this);

    addKeyListener(commandManager->getKeyMappings());

    setBounds(0, 0, 1000, 600);

	startTimer(100);
}

LayoutChooser::~LayoutChooser()
{
	stopTimer();
	removeKeyListener(commandManager->getKeyMappings());
    viewTree.removeListener(this);
    userSettings->removeActionListener(this);
}

void LayoutChooser::timerCallback()
{
	stopTimer();

	if (viewTree.getNumChildren() == 0)
		editFileLocations();
	else if (userSettings->getPropertyBoolValue("autorebuildlibrary", false))
		rebuildFileLibrary();
}

void LayoutChooser::actionListenerCallback(const String& message)
{
    if (message == "layoutlibraryupdated")
        attachToTree();
}

void LayoutChooser::removeExcludedLayouts()
{
    for (int i = 0; i < viewTree.getNumChildren(); i++)
    {
        bool excludeFromChooser = viewTree.getChild(i).getProperty(Ids::excludeFromChooser, false);
        
        if (excludeFromChooser)
            viewTree.removeChild(i, nullptr);
    }
}

void LayoutChooser::selectCurrentLayout()
{
    for (int i = 0; i < viewTree.getNumChildren(); i++)
    {
        String itemName       = viewTree.getChild(i).getProperty(Ids::name);
        String itemLibrarySet = viewTree.getChild(i).getProperty(Ids::librarySet);
        
        if (itemName == name && itemLibrarySet == librarySet)
        {
            table.selectRow(i);
        }
    }
}

void LayoutChooser::paint(Graphics& g)
{
    g.fillAll(Colours::lightgrey);

    if (thumbImage.isValid())
        g.drawImageWithin(thumbImage, 4, 4, 196, 106, RectanglePlacement::centred);
}
    
void LayoutChooser::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    Rectangle<int> headerBounds(localBounds.removeFromTop(140));
    Rectangle<int> buttonBar(headerBounds.removeFromBottom(30));
    
    Rectangle<int> headerLeftSection(headerBounds.removeFromLeft(200).removeFromBottom(30));

    chooseButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    rebuildLibraryButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    editLocationsButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    headerBounds.removeFromLeft(4);
    headerBounds.removeFromTop(10);
    blurb.setBounds(headerBounds.reduced(4, 4));
    table.setBounds(localBounds.reduced(4, 4));
}
    
int LayoutChooser::getNumRows()
{
    return viewTree.getNumChildren();
}

void LayoutChooser::sortOrderChanged(int newSortColumnId, bool isForwards)
{
    LayoutSorter sorter(newSortColumnId, isForwards);
    viewTree.sort(sorter, nullptr, true);
}

void LayoutChooser::paintRowBackground(Graphics& g, int /* rowNumber */, int /* width */, int /* height */, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll (findColour(TextEditor::highlightColourId));
}

void LayoutChooser::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool /* rowIsSelected */)
{
    g.setColour(Colours::black);
    g.setFont(font);

    String text;

    switch (columnId)
    {
        case 1:  text = String::empty; break;
        case 2:  text = viewTree.getChild(rowNumber).getProperty(Ids::name); break;
        case 3:  text = viewTree.getChild(rowNumber).getProperty(Ids::librarySet); break;
        case 4:  text = viewTree.getChild(rowNumber).getProperty(Ids::author); break;
        case 5:  text = viewTree.getChild(rowNumber).getProperty(Ids::panelWidth).toString() 
                        + "x" 
                        + viewTree.getChild(rowNumber).getProperty(Ids::panelHeight).toString(); break;
        case 6:  text = viewTree.getChild(rowNumber).getProperty(Ids::numEncoders); break;
        case 7:  text = viewTree.getChild(rowNumber).getProperty(Ids::numFaders); break;
        case 8:  text = viewTree.getChild(rowNumber).getProperty(Ids::numButtons); break;
        case 9:  text = viewTree.getChild(rowNumber).getProperty(Ids::numParameters); break;
        case 10: text = viewTree.getChild(rowNumber).getProperty(Ids::mruTime); break;
    }

    g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);
    
    g.setColour(Colours::black.withAlpha(0.2f));
    g.fillRect(width - 1, 0, 1, height);
}

void LayoutChooser::backgroundClicked(const MouseEvent&)
{
    table.deselectAllRows();
}

void LayoutChooser::cellDoubleClicked(int rowNumber, int /* columnId */, const MouseEvent& /* e */)
{
    name       = viewTree.getChild(rowNumber).getProperty(Ids::name);
    librarySet = viewTree.getChild(rowNumber).getProperty(Ids::librarySet);
    
    closeWindow();
}

void LayoutChooser::selectedRowsChanged(int lastRowSelected)
{
    String thumbFileName = viewTree.getChild(lastRowSelected).getProperty(Ids::thumbnail);
  
    if (thumbFileName.isNotEmpty())
    {
        File   layoutFile(viewTree.getChild(lastRowSelected).getProperty(Ids::filePath));
        String layoutDirectory(layoutFile.getParentDirectory().getFullPathName());
            
        thumbImage = imageLoader->loadImage(thumbFileName, layoutDirectory);
        repaint();
    }
    else
    {
        thumbImage = Image();
        repaint();
    }

    blurb.setText(viewTree.getChild(lastRowSelected).getProperty(Ids::blurb), dontSendNotification);
    commandManager->commandStatusChanged();
}

void LayoutChooser::getAllCommands(Array <CommandID>& commands)
{
    const CommandID ids[] = {CommandIDs::chooseSelectedLayout,
                             CommandIDs::rebuildFileLibrary,
                             CommandIDs::editFileLocations
                             };
    
    commands.addArray(ids, numElementsInArray (ids));
}

void LayoutChooser::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::chooseSelectedLayout:
        result.setInfo("Choose Selected Layout", "Changes the layout in the current Configuration to the one currently select", CommandCategories::configmgr, !table.getNumSelectedRows());
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

void LayoutChooser::returnKeyPressed(int /* lastRowSelected */)
{
    chooseSelectedLayout();
}

bool LayoutChooser::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::chooseSelectedLayout:  chooseSelectedLayout(); break;
        case CommandIDs::rebuildFileLibrary:    rebuildFileLibrary(); break;
        case CommandIDs::editFileLocations:     editFileLocations(); break;
        default:                                return false;
    }

    return true;
}


void LayoutChooser::editFileLocations() const
{
    userSettings->editFileLocations(getParentMonitorArea().getCentreX(), getParentMonitorArea().getCentreY());
}

void LayoutChooser::chooseSelectedLayout()
{
    name       = viewTree.getChild(table.getSelectedRow()).getProperty(Ids::name);
    librarySet = viewTree.getChild(table.getSelectedRow()).getProperty(Ids::librarySet);
    
    closeWindow();
}

void LayoutChooser::rebuildFileLibrary()
{
    userSettings->rebuildFileLibrary(false, true, false);
    attachToTree();
}

void LayoutChooser::attachToTree()
{
    viewTree.removeListener(this);
    
    tree = userSettings->getLayoutLibrary();
    viewTree = tree.createCopy();
    
    ScopedPointer<XmlElement> xml(viewTree.createXml());

    DBG("LayoutChooser::attachToTree - new tree: " + xml->createDocument(String::empty));

    LayoutSorter sorter(10, false);
    viewTree.sort(sorter, nullptr, true);
    
    viewTree.addListener(this);
    table.updateContent();
}

void LayoutChooser::closeWindow() const
{
    LayoutChooserWindow* window = static_cast<LayoutChooserWindow*>(getParentComponent());
    window->closeWindow();
}

ApplicationCommandTarget* LayoutChooser::getNextCommandTarget()
{
    return nullptr;
}

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
#include "../Resources/ImageLoader.h"
#include "../Components/UserSettings.h"

/* =========================================================================
 * LayoutChooserWindow
 */
LayoutChooserWindow::LayoutChooserWindow(int posX, int posY, 
                                         const ValueTree& vt,
                                         const Value& layoutName,
                                         const Value& layoutLocation,
                                         ApplicationCommandManager* acm)
    : DocumentWindow("Layout Chooser",
                     Colour::greyLevel(0.6f),
                     DocumentWindow::allButtons,
                     true)
{
    setUsingNativeTitleBar (true);
    
    setContentOwned(new LayoutChooser(vt, layoutName, layoutLocation, acm), true);
    
    restoreWindowPosition(posX, posY);
    
    setVisible(true);
    setResizable(true, false);

    setWantsKeyboardFocus (false);

    setResizeLimits(400, 200, 32000, 32000);
}

LayoutChooserWindow::~LayoutChooserWindow() {}

void LayoutChooserWindow::closeButtonPressed()
{
    sendChangeMessage();
}

void LayoutChooserWindow::restoreWindowPosition(int posX, int posY)
{
    setBounds(posX, posY, getWidth(), getHeight());
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

        if (columnId == 2)
        {
            String firstString  = first.getProperty(Ids::name, String::empty);
            String secondString = second.getProperty(Ids::name, String::empty);
            result = firstString.compareNatural(secondString);
        }
        else if (columnId == 3)
        {
            String firstString  = first.getProperty(Ids::location, String::empty);
            String secondString = second.getProperty(Ids::location, String::empty);
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
 * LayoutChooser::ImageComp - Used for handling the thumbnail and screenshot display
 */
class LayoutChooser::ImageComp : public Component
{
public:
    ImageComp(const String& thumb, const String& screenshot, const String& layoutDir)
        : layoutDirectory(layoutDir)
    {
        useImageCache = UserSettings::getInstance()->getPropertyBoolValue("useimagecache", true);
        setImage(thumb, screenshot);
    }

    void resized() override {}

    void paint(Graphics& g) override
    {
        g.fillAll(Colours::white);
        
        if (thumbImage.isValid())
            g.drawImageWithin(thumbImage, 0, 0, getWidth() - 2, getHeight() - 2, RectanglePlacement::centred);
    }

    void mouseDown(const MouseEvent& /* e */) override
    {
        Image screenshotImage = ImageLoader::getInstance()->loadImage(screenshotFileName, useImageCache, layoutDirectory);

        if (screenshotImage.isValid())
        {
            ImageComponent* screenshotComponent = new ImageComponent(String::empty);
            screenshotComponent->setImage(screenshotImage);
            screenshotComponent->setSize(screenshotImage.getWidth(), screenshotImage.getHeight());
            screenshotComponent->setAlwaysOnTop(true);
            DBG("screenshotImage - " + String(screenshotComponent->getWidth()) + "/" + String(screenshotComponent->getHeight()));
            CallOutBox::launchAsynchronously(screenshotComponent, getScreenBounds(), nullptr);
        }
    }

    void setImage(const String& thumb, const String& screenshot)
    {
        thumbImage = ImageLoader::getInstance()->loadImage(thumb, useImageCache, layoutDirectory);
        screenshotFileName = screenshot;
    }

private:
    String screenshotFileName;
    String layoutDirectory;
    Image  thumbImage;
    bool   useImageCache;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImageComp)
};

/* =========================================================================
 * LayoutChooser
 */
LayoutChooser::LayoutChooser(const ValueTree& valueTree,
                             const Value& layoutName,
                             const Value& layoutLocation,
                             ApplicationCommandManager* acm)
    : viewTree(valueTree.createCopy()), 
      tree(valueTree), 
      font(14.0f), 
      commandManager(acm),
      chooseButton("Choose Layout"),
      blurb(String::empty)
{
    commandManager->registerAllCommandsForTarget(this);

    chooseButton.setCommandToTrigger(commandManager, CommandIDs::chooseSelectedLayout, true);
    addAndMakeVisible(chooseButton);

    addAndMakeVisible(blurb);

    addAndMakeVisible(table);
    
    table.setModel(this);
    
    table.setColour(ListBox::outlineColourId, Colours::darkgrey);
    table.setOutlineThickness (4);
    table.setRowHeight(50);
    
    table.getHeader().addColumn(String::empty, 1, 50,  50,  50, TableHeaderComponent::notResizableOrSortable & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Name",        2, 100, 40,  -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Location",    3, 200, 100, -1, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    
    table.getHeader().setStretchToFitActive(true);
    
    name.referTo(layoutName);
    location.referTo(layoutLocation);

    viewTree.addListener(this);

    addKeyListener(commandManager->getKeyMappings());

    setBounds(0, 0, 600, 500);
}

LayoutChooser::~LayoutChooser()
{
    removeKeyListener(commandManager->getKeyMappings());
    viewTree.removeListener(this);
}

void LayoutChooser::paint(Graphics& g)
{
    g.fillAll(Colours::lightgrey);
}
    
void LayoutChooser::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    Rectangle<int> buttonBar(localBounds.removeFromTop(100));

    chooseButton.setBounds(buttonBar.removeFromLeft(100).reduced(4, 35));
    buttonBar.removeFromLeft(4);
    blurb.setBounds(buttonBar.reduced(4, 4));
    table.setBounds(localBounds.reduced(4, 4));
}
    
int LayoutChooser::getNumRows()
{
    return viewTree.getNumChildren();
}

Component* LayoutChooser::refreshComponentForCell(int rowNumber, int columnId, bool /* isRowSelected */, Component* existingComponentToUpdate)
{
    if (columnId == 1)
    {
        String thumbFileName = viewTree.getChild(rowNumber).getProperty(Ids::thumbnail);
        ImageComp* imageComp = (ImageComp*)existingComponentToUpdate;

        if (thumbFileName.isNotEmpty())
        {
            File   layoutFile(viewTree.getChild(rowNumber).getProperty(Ids::filePath));
            String layoutDirectory    = layoutFile.getParentDirectory().getFullPathName();
            String screenshotFileName = viewTree.getChild(rowNumber).getProperty(Ids::screenshot);
            
            if (imageComp == nullptr)
                imageComp = new ImageComp(thumbFileName, screenshotFileName, layoutDirectory);
            else
                imageComp->setImage(thumbFileName, screenshotFileName);
    
            return imageComp;
        }
        else if (imageComp != nullptr)
        {
            delete imageComp;
        }
    }
    return nullptr;
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
        case 1: text = String::empty; break;
        case 2: text = viewTree.getChild(rowNumber).getProperty(Ids::name); break;
        case 3: text = viewTree.getChild(rowNumber).getProperty(Ids::location); break;
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
    name     = viewTree.getChild(rowNumber).getProperty(Ids::name);
    location = viewTree.getChild(rowNumber).getProperty(Ids::location);
    
    closeWindow();
}

void LayoutChooser::selectedRowsChanged(int lastRowSelected)
{
    blurb.setText(viewTree.getChild(lastRowSelected).getProperty(Ids::blurb), dontSendNotification);
    commandManager->commandStatusChanged();
}

void LayoutChooser::getAllCommands(Array <CommandID>& commands)
{
    const CommandID ids[] = {CommandIDs::chooseSelectedLayout};
    
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
        default:                                return false;
    }

    return true;
}

void LayoutChooser::chooseSelectedLayout()
{
    name     = viewTree.getChild(table.getSelectedRow()).getProperty(Ids::name);
    location = viewTree.getChild(table.getSelectedRow()).getProperty(Ids::location);
    
    closeWindow();
}

void LayoutChooser::closeWindow()
{
    LayoutChooserWindow* window = (LayoutChooserWindow*)getParentComponent();
    window->sendChangeMessage();
}

ApplicationCommandTarget* LayoutChooser::getNextCommandTarget()
{
    return nullptr;
}

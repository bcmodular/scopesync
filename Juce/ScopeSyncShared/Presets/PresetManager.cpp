/*
  ==============================================================================

    PresetManager.cpp
    Created: 2 Nov 2014 10:12:07am
    Author:  giles

  ==============================================================================
*/

#include "PresetManager.h"
#include "PresetItem.h"
#include "../Utils/BCMTreeView.h"
#include "../Resources/ImageLoader.h"
#include "../Windows/PresetFileChooser.h"
#include "../Windows/UserSettings.h"

/* =========================================================================
 * PresetManager
 */
PresetManager::PresetManager(PresetManagerWindow& parent) 
    : parentWindow(parent),
      addButton("New"),
      openButton("Open"),
      saveButton("Save"),
      saveAsButton("Save As..."),
      applyChangesButton("Apply Changes"),
      discardChangesButton("Discard All Unsaved Changes"),
      undoButton("Undo"),
      redoButton("Redo"),
      presetFile(parent.getPresetFile())
{
    UserSettings::getInstance()->addActionListener(this);
    
    initialised = false;

    commandManager = parentWindow.getCommandManager();
    
    lookAndFeel.setColour(TreeView::backgroundColourId,             Colours::darkgrey);
    lookAndFeel.setColour(TreeView::linesColourId,                  Colours::white);
    lookAndFeel.setColour(TreeView::dragAndDropIndicatorColourId,   Colours::red);
    lookAndFeel.setColour(TreeView::selectedItemBackgroundColourId, Colours::slategrey);
    setLookAndFeel(&lookAndFeel);

    commandManager->registerAllCommandsForTarget(this);
    
    setButtonImages(addButton, "newConfigOff", "newConfigOver", "newConfigOn", Colours::transparentBlack);
    addButton.setCommandToTrigger(commandManager, CommandIDs::addPresetFile, true);
    addAndMakeVisible(addButton);

    setButtonImages(openButton, "loadConfigButtonOff", "loadConfigButtonOver", "loadConfigButtonOn", Colours::transparentBlack);
    openButton.setCommandToTrigger(commandManager, CommandIDs::choosePresetFile, true);
    addAndMakeVisible(openButton);
    
    setButtonImages(saveButton, "saveOff", "saveOver", "saveOn", Colours::transparentBlack);
    saveButton.setCommandToTrigger(commandManager, CommandIDs::savePresetFile, true);
    addAndMakeVisible(saveButton);

    setButtonImages(saveAsButton, "saveAsOff", "saveAsOver", "saveAsOn", Colours::transparentBlack);
    saveAsButton.setCommandToTrigger(commandManager, CommandIDs::savePresetFileAs, true);
    addAndMakeVisible(saveAsButton);

    setButtonImages(applyChangesButton, "confirmOff", "confirmOver", "confirmOn", Colours::transparentBlack);
    applyChangesButton.setCommandToTrigger(commandManager, CommandIDs::applyPresetFileChanges, true);
    addAndMakeVisible(applyChangesButton);

    setButtonImages(discardChangesButton, "closeOff", "closeOver", "closeOn", Colours::transparentBlack);
    discardChangesButton.setCommandToTrigger(commandManager, CommandIDs::discardPresetFileChanges, true);
    addAndMakeVisible(discardChangesButton);

    setButtonImages(undoButton, "undoOff", "undoOver", "undoOn", Colours::transparentBlack);
    undoButton.setCommandToTrigger(commandManager, CommandIDs::undo, true);
    addAndMakeVisible(undoButton);

    setButtonImages(redoButton, "redoOff", "redoOver", "redoOn", Colours::transparentBlack);
    redoButton.setCommandToTrigger(commandManager, CommandIDs::redo, true);
    addAndMakeVisible(redoButton);

    treeSizeConstrainer.setMinimumWidth(200);
    treeSizeConstrainer.setMaximumWidth(700);

    
    PresetRootItem* rootItem = new PresetRootItem(presetFile, *this, presetFile.getPresetFileRoot(), undoManager);
    treeView = new BCMTreeView(undoManager, rootItem, presetFile.getPresetProperties());
    
    fileNameLabel.setText("File path: " + presetFile.getFile().getFullPathName(), dontSendNotification);
    fileNameLabel.setTooltip(presetFile.getFile().getFullPathName());
    fileNameLabel.setColour(Label::textColourId, Colours::lightgrey);
    fileNameLabel.setMinimumHorizontalScale(1.0f);
    addAndMakeVisible(fileNameLabel);

    int lastTreeWidth = presetFile.getPresetProperties().getIntValue("lastPresetTreeWidth", 300);
    treeView->setBounds(0, 0, lastTreeWidth, getHeight());
    addAndMakeVisible(treeView);
    
    addAndMakeVisible (resizerBar = new ResizableEdgeComponent(treeView, &treeSizeConstrainer,
                                                               ResizableEdgeComponent::rightEdge));
    resizerBar->setAlwaysOnTop (true);

    int lastConfigMgrWidth  = presetFile.getPresetProperties().getIntValue("lastPresetMgrWidth", 600);
    int lastConfigMgrHeight = presetFile.getPresetProperties().getIntValue("lastPresetMgrHeight", 500);
    
    initialised = true;
    setSize(lastConfigMgrWidth, lastConfigMgrHeight);
    
    startTimer(500);
}

void PresetManager::setButtonImages(ImageButton& button, const String& normalImage, const String& overImage, const String& downImage, const Colour& overlayColour)
{
    button.setImages(true, true, true,
                     ImageLoader::getInstance()->loadImage(normalImage, true, ""), 1.0f, overlayColour,
                     ImageLoader::getInstance()->loadImage(overImage,   true, ""), 1.0f, overlayColour,
                     ImageLoader::getInstance()->loadImage(downImage,   true, ""), 1.0f, overlayColour, 0);
}

PresetManager::~PresetManager()
{
    unload();
}

void PresetManager::changePanel(Component* newComponent)
{
    panel = newComponent;
    addAndMakeVisible(panel);
    resized();
}

void PresetManager::unload()
{
    UserSettings::getInstance()->removeActionListener(this);
    stopTimer();
    saveTreeViewState();
    presetFile.getPresetProperties().setValue("lastPresetMgrWidth", getWidth());
    presetFile.getPresetProperties().setValue("lastPresetMgrHeight", getHeight());
    presetFile.getPresetProperties().setValue("lastPresetTreeWidth", treeView->getWidth());
}

void PresetManager::getAllCommands (Array <CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::addPresetFile,
                              CommandIDs::choosePresetFile,
                              CommandIDs::savePresetFile,
                              CommandIDs::savePresetFileAs,
                              CommandIDs::applyPresetFileChanges,
                              CommandIDs::discardPresetFileChanges,
                              CommandIDs::closePresetFile,
                              CommandIDs::undo,
                              CommandIDs::redo,
                              CommandIDs::focusOnPanel,
                              CommandIDs::copyItem,
                              CommandIDs::pasteItem,
                              CommandIDs::deleteItems,
                              CommandIDs::addItem,
                              CommandIDs::addItemFromClipboard
                            };

    commands.addArray (ids, numElementsInArray(ids));
}

void PresetManager::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
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
    case CommandIDs::addPresetFile:
        result.setInfo("Add Preset File", "Create a new Preset File", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('w', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::choosePresetFile:
        result.setInfo("Choose Preset File", "Switch to a different Preset File", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('o', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::savePresetFile:
        result.setInfo("Save Preset File", "Save Preset File", CommandCategories::configmgr, !(presetFile.hasChangedSinceSaved()));
        result.defaultKeypresses.add(KeyPress ('s', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::savePresetFileAs:
        result.setInfo("Save Preset File As...", "Save Preset File as a new file", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('s', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    case CommandIDs::applyPresetFileChanges:
        result.setInfo("Apply Preset File Changes", "Updates Preset Library with latest changes", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress (KeyPress::returnKey, ModifierKeys::altModifier, 0));
        break;
    case CommandIDs::discardPresetFileChanges:
        result.setInfo("Discard Preset File Changes", "Discards all unsaved changes to the current Preset File and rebuilds Preset Library", CommandCategories::configmgr, !(presetFile.hasChangedSinceSaved()));
        result.defaultKeypresses.add(KeyPress ('d', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::closePresetFile:
        result.setInfo("Close Preset Manager", "Closes Preset Manager window", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('q', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::focusOnPanel:
        result.setInfo("Focus on panel", "Switches keyboard focus to edit panel", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress (KeyPress::F2Key, ModifierKeys::noModifiers, 0));
        result.defaultKeypresses.add(KeyPress (KeyPress::returnKey, ModifierKeys::noModifiers, 0));
        break;
    case CommandIDs::copyItem:
        result.setInfo ("Copy item", "Copies an item to the clipboard", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('c', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::pasteItem:
        result.setInfo ("Paste item", "Overwrites an item with values from the clipboard", CommandCategories::configmgr, !canPasteItem());
        result.defaultKeypresses.add(KeyPress ('v', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::deleteItems:
        result.setInfo("Delete", "Delete selected items", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add (KeyPress (KeyPress::deleteKey, 0, 0));
        result.defaultKeypresses.add (KeyPress (KeyPress::backspaceKey, 0, 0));
        break;
    case CommandIDs::addItem:
        result.setInfo("Add item", "Adds a new item", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('n', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::addItemFromClipboard:
        result.setInfo("Add item from clipboard", "Adds a new item using the definition stored in the clipboard", CommandCategories::configmgr, !canPasteItem());
        result.defaultKeypresses.add(KeyPress ('n', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    }
}

bool PresetManager::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::undo:                     undo(); break;
        case CommandIDs::redo:                     redo(); break;
        case CommandIDs::addPresetFile:            parentWindow.addPresetFile(); break;
        case CommandIDs::choosePresetFile:         parentWindow.showPresetFileChooser(); break;
        case CommandIDs::savePresetFile:           save(); break;
        case CommandIDs::savePresetFileAs:         saveAs(); break;
        case CommandIDs::applyPresetFileChanges:   parentWindow.updatePresetLibrary(); break;
        case CommandIDs::discardPresetFileChanges: parentWindow.discardChanges(); break;
        case CommandIDs::closePresetFile:          parentWindow.hidePresetManager(); break;
        case CommandIDs::focusOnPanel:             panel->grabKeyboardFocus(); break;
        case CommandIDs::copyItem:                 copyItem(); break;
        case CommandIDs::pasteItem:                pasteItem(); break;
        case CommandIDs::deleteItems:              deleteItems(); break;
        case CommandIDs::addItem:                  addItem(); break;
        case CommandIDs::addItemFromClipboard:     addItemFromClipboard(); break;
        default:                                   return false;
    }

    return true;
}

void PresetManager::copyItem()
{
    treeView->copyItem();
    parentWindow.incrNumActions();
}

void PresetManager::pasteItem()
{
     treeView->pasteItem();
     parentWindow.incrNumActions();
}
 
void PresetManager::deleteItems()
{
    treeView->deleteSelectedItems();
    parentWindow.incrNumActions();
}

void PresetManager::addItem()
{
    treeView->addItem();
    parentWindow.incrNumActions();
}

void PresetManager::addItemFromClipboard()
{
    treeView->addItemFromClipboard();
    parentWindow.incrNumActions();
}

void PresetManager::undo()
{
    if (undoManager.undo())
    {
        parentWindow.incrNumActions();
        treeView->changePanel();
    }
}

void PresetManager::redo()
{
    if (undoManager.redo())
    {
        parentWindow.incrNumActions();
        treeView->changePanel();
    }
}

bool PresetManager::canPasteItem()
{
    if (treeView != nullptr)
        return treeView->canPasteItem();
    else
        return false;
}

ApplicationCommandTarget* PresetManager::getNextCommandTarget()
{
    return nullptr;
}

void PresetManager::childBoundsChanged(Component* child)
{
    if (child == treeView)
        resized();
}

void PresetManager::saveTreeViewState()
{
    treeView->saveTreeViewState();
}

void PresetManager::resized()
{
    if (initialised)
    {
        Rectangle<int> localBounds(getLocalBounds());
    
        Rectangle<int> toolbar(localBounds.removeFromTop(40).reduced(8, 8));
    
        addButton.setBounds(toolbar.removeFromLeft(40));
        openButton.setBounds(toolbar.removeFromLeft(40));
        saveButton.setBounds(toolbar.removeFromLeft(40));
        saveAsButton.setBounds(toolbar.removeFromLeft(40));
        toolbar.removeFromLeft(16);
        applyChangesButton.setBounds(toolbar.removeFromLeft(40));
        discardChangesButton.setBounds(toolbar.removeFromLeft(40));
        toolbar.removeFromLeft(16);
        undoButton.setBounds(toolbar.removeFromLeft(40));
        redoButton.setBounds(toolbar.removeFromLeft(40));
    
        fileNameLabel.setBounds(localBounds.removeFromBottom(40).reduced(8, 8));
    
        treeView->setBounds(localBounds.removeFromLeft(treeView->getWidth()));
        resizerBar->setBounds(localBounds.withWidth(4));
    
        if (panel != nullptr)
            panel->setBounds(localBounds);
    }
}

void PresetManager::paint(Graphics& g)
{
    g.fillAll (Colour (0xff434343));

    if (initialised)
    {
        g.setColour(Colours::darkgrey);
        g.fillRect(0, 0, getWidth(), 40);
        g.fillRect(0, 0, getWidth(), getHeight() - 40);

        g.drawImageAt(ImageLoader::getInstance()->loadImage("divider", true, String::empty), 174, 8);
        g.drawImageAt(ImageLoader::getInstance()->loadImage("divider", true, String::empty), 268, 8);
    }
}

void PresetManager::paintOverChildren(Graphics& g)
{
    if (initialised)
    {
        const int shadowSize = 15;
    
        const int resizerX = resizerBar->getX();

        ColourGradient resizerCG (Colours::black.withAlpha (0.25f), (float) resizerX, 0,
                                  Colours::transparentBlack,        (float) (resizerX - shadowSize), 0, false);
        resizerCG.addColour (0.4, Colours::black.withAlpha (0.07f));
        resizerCG.addColour (0.6, Colours::black.withAlpha (0.02f));

        g.setGradientFill(resizerCG);
        g.fillRect (resizerX - shadowSize, resizerBar->getY(), shadowSize, resizerBar->getHeight());
    }
}

void PresetManager::timerCallback()
{
    undoManager.beginNewTransaction();
}

void PresetManager::actionListenerCallback(const String& message)
{
    if (message == "presetlibraryupdated")
        parentWindow.showPresetManager();
}

void PresetManager::save()
{
    presetFile.save(true, true);
}

void PresetManager::saveAs()
{
    File presetFileDirectory = presetFile.getFile().getParentDirectory();
    
    FileChooser fileChooser("Save Preset File As...",
                            presetFileDirectory,
                            "*.presets");
    
    if (fileChooser.browseForFileToSave(true))
    {
        String fileName = fileChooser.getResult().getFullPathName();

        if (File::isAbsolutePath(fileName))
        {
            presetFile.setFile(File(fileName));
            save();
            parentWindow.incrNumActions();
            parentWindow.showPresetManager();
        }
    }
}

/* =========================================================================
 * PresetMenuBarModel
 */
PresetMenuBarModel::PresetMenuBarModel(PresetManagerWindow& owner)
    : presetManagerWindow(owner)
{
    setApplicationCommandManagerToWatch(presetManagerWindow.getCommandManager());
}

StringArray PresetMenuBarModel::getMenuBarNames()
{
    return presetManagerWindow.getMenuNames();
}

PopupMenu PresetMenuBarModel::getMenuForIndex (int /*topLevelMenuIndex*/, const String& menuName)
{
    PopupMenu menu;
    presetManagerWindow.createMenu(menu, menuName);
    return menu;
}

/* =========================================================================
 * PresetManagerWindow
 */
PresetManagerWindow::PresetManagerWindow(const String& filePath, 
                                         ApplicationCommandManager* acm,
                                         UndoManager& um, 
                                         int posX, 
                                         int posY)
    : DocumentWindow("Parameter Preset Manager",
                     Colour::greyLevel(0.6f),
                     DocumentWindow::allButtons,
                     true),
      undoManager(um),
      numActions(0),
      offerToSaveOnExit(true)
{
    commandManager = acm;
    setUsingNativeTitleBar (true);
    
    menuModel = new PresetMenuBarModel(*this);
    
    newPresetFile = File(filePath);

    if (filePath.isNotEmpty() && newPresetFile.existsAsFile())
    {
        Result result = presetFile.loadFrom(newPresetFile, true);

        if (!result.wasOk())
        {
            showPresetFileChooser();
        }
        else
        {
            showPresetManager();
        }
    }
    else
        showPresetFileChooser();
    
    addKeyListener(commandManager->getKeyMappings());

    restoreWindowPosition(posX, posY);
    
    setVisible(true);
    setResizable(true, false);

    setWantsKeyboardFocus (false);

    setResizeLimits(600, 500, 32000, 32000);
}

PresetManagerWindow::~PresetManagerWindow()
{
    unload();
}

void PresetManagerWindow::showPresetFileChooser()
{
    if (presetFile.saveIfNeededAndUserAgrees(true) == FileBasedDocument::savedOk)
    {
        presetFile.setChangedFlag(false);
        setMenuBar(nullptr);
        clearContentComponent();

        presetFileChooser = new PresetFileChooser(newPresetFile, commandManager, undoManager, *this);
        setContentOwned(presetFileChooser, true);
    }
}

void PresetManagerWindow::showPresetManager()
{
    if (newPresetFile.existsAsFile())
    {
        Result result = presetFile.loadFrom(newPresetFile, true);

        if (!result.wasOk())
            showPresetFileChooser();
        else
        {
            setMenuBar(menuModel);
            clearContentComponent();

            presetManager = new PresetManager(*this);
            setContentOwned(presetManager, true);
        }
    }
    else
        showPresetFileChooser();
}

bool PresetManagerWindow::presetsHaveChanged()
{
    return numActions > 0;
}

StringArray PresetManagerWindow::getMenuNames()
{
    const char* const names[] = { "File", "Edit", nullptr };
    return StringArray (names);
}

void PresetManagerWindow::createMenu(PopupMenu& menu, const String& menuName)
{
         if (menuName == "File") createFileMenu(menu);
    else if (menuName == "Edit") createEditMenu(menu);
    else                         jassertfalse; // names have changed?
}

void PresetManagerWindow::createFileMenu(PopupMenu& menu)
{
    menu.addCommandItem(commandManager, CommandIDs::addPresetFile);
    menu.addCommandItem(commandManager, CommandIDs::savePresetFile);
    menu.addCommandItem(commandManager, CommandIDs::savePresetFileAs);
    menu.addSeparator();
    menu.addCommandItem(commandManager, CommandIDs::applyPresetFileChanges);   
    menu.addCommandItem(commandManager, CommandIDs::discardPresetFileChanges);   
    menu.addSeparator();
    menu.addCommandItem(commandManager, CommandIDs::closePresetFile);   
}

void PresetManagerWindow::createEditMenu(PopupMenu& menu)
{
    menu.addCommandItem(commandManager, CommandIDs::undo);
    menu.addCommandItem(commandManager, CommandIDs::redo);
}

void PresetManagerWindow::closeButtonPressed()
{
    if (presetFile.saveIfNeededAndUserAgrees(true) == FileBasedDocument::savedOk)
    {
        hidePresetManager(false);
    }
}

void PresetManagerWindow::addPresetFile()
{
    presetFile.createPresetFile();
    newPresetFile = presetFile.getFile();
    showPresetManager();

    // Rebuild the library, so we can check whether the new preset file
    // was put into a File Location. We will get an action callback
    // once the rebuild is complete
    UserSettings::getInstance()->addActionListener(this);
    UserSettings::getInstance()->rebuildFileLibrary(false, false, true);
}

void PresetManagerWindow::actionListenerCallback(const String& message)
{
    if (message == "presetlibraryupdated")
    {
        if (newPresetFileIsInLocation())
        {
            UserSettings::getInstance()->removeActionListener(this);
        }
    }
}


bool PresetManagerWindow::newPresetFileIsInLocation()
{
    ValueTree pf = UserSettings::getInstance()->getPresetFileFromFilePath(presetFile.getFile().getFullPathName(), UserSettings::getInstance()->getPresetLibrary());
    
    if (!pf.isValid())
    {
        AlertWindow::showOkCancelBox(AlertWindow::InfoIcon,
                                    "Check locations",
                                    "Your new Preset File was not automatically added to the library. You probably need to add a new location."
                                    + newLine + "Press OK to launch the File Location Editor or Cancel if you intend to do it later.",
                                    String::empty,
                                    String::empty,
                                    nullptr,
                                    ModalCallbackFunction::withParam(alertBoxLaunchLocationEditor, getParentMonitorArea(), this));
        return false;
    }

    return true;
}

void PresetManagerWindow::alertBoxLaunchLocationEditor(int result, Rectangle<int> newConfigWindowPosition, PresetManagerWindow* pmw)
{
    if (result)
    {
        // User clicked OK, so launch the location editor
        UserSettings::getInstance()->editFileLocations(newConfigWindowPosition.getCentreX(),
                                                       newConfigWindowPosition.getCentreY());    
    }
    else
    {
        // User clicked cancel, so we just give up for now
        UserSettings::getInstance()->removeActionListener(pmw);
    }
}

void PresetManagerWindow::unload()
{
    if (offerToSaveOnExit)
        presetFile.saveIfNeededAndUserAgrees(false);

    clearContentComponent();
    removeKeyListener(commandManager->getKeyMappings());
    setMenuBar(nullptr);
}

void PresetManagerWindow::restoreWindowPosition(int posX, int posY)
{
    String windowState;

    windowState = presetFile.getPresetProperties().getValue("lastConfigMgrPos");

    if (windowState.isEmpty())
        setCentrePosition(posX, posY);
    else
        restoreWindowStateFromString(windowState);
}

void PresetManagerWindow::restoreWindowPosition()
{
    String windowState;

    windowState = presetFile.getPresetProperties().getValue("lastConfigMgrPos");

    if (windowState.isNotEmpty())
        restoreWindowStateFromString(windowState);
}

void PresetManagerWindow::discardChanges()
{
    presetFile.loadFrom(presetFile.getFile(), true);
    showPresetManager();
}

void PresetManagerWindow::updatePresetLibrary()
{
    UserSettings::getInstance()->rebuildFileLibrary(false, false, true);
}

void PresetManagerWindow::hidePresetManager(bool offerToSave)
{
    offerToSaveOnExit = offerToSave;

    UserSettings::getInstance()->hidePresetManagerWindow();
}
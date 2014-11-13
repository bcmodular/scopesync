/**
 * UI Component for managing the current Configuration
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

#include "ConfigurationManager.h"
#include "../Core/Global.h"
#include "../Core/ScopeSyncApplication.h"
#include "../Utils/BCMMisc.h"
#include "../Utils/BCMTreeView.h"
#include "../Core/ScopeSync.h"
#include "ConfigurationTreeItem.h"
#include "../Resources/ImageLoader.h"
#include "../Windows/PresetChooser.h"

/* =========================================================================
 * ConfigurationManager
 */
ConfigurationManager::ConfigurationManager(ScopeSync& ss, ConfigurationManagerWindow& parent) 
    : scopeSync(ss),
      parentWindow(parent),
      addButton("New"),
      saveButton("Save"),
      saveAsButton("Save As..."),
      applyChangesButton("Apply Changes"),
      discardChangesButton("Discard All Unsaved Changes"),
      undoButton("Undo"),
      redoButton("Redo")
{
    commandManager = scopeSync.getCommandManager();
    commandManager->setFirstCommandTarget(nullptr);

    lookAndFeel.setColour(TreeView::backgroundColourId,             Colours::darkgrey);
    lookAndFeel.setColour(TreeView::linesColourId,                  Colours::white);
    lookAndFeel.setColour(TreeView::dragAndDropIndicatorColourId,   Colours::red);
    lookAndFeel.setColour(TreeView::selectedItemBackgroundColourId, Colours::slategrey);
    setLookAndFeel(&lookAndFeel);

    commandManager->registerAllCommandsForTarget(this);
    
    fileNameLabel.setText("File path: " + scopeSync.getConfigurationFile().getFullPathName(), dontSendNotification);
    fileNameLabel.setTooltip(scopeSync.getConfigurationFile().getFullPathName());
    fileNameLabel.setColour(Label::textColourId, Colours::lightgrey);
    fileNameLabel.setMinimumHorizontalScale(1.0f);
    addAndMakeVisible(fileNameLabel);

    setButtonImages(addButton, "newConfigOff", "newConfigOver", "newConfigOn", Colours::transparentBlack);
    addButton.setCommandToTrigger(commandManager, CommandIDs::addConfig, true);
    addAndMakeVisible(addButton);

    setButtonImages(saveButton, "saveOff", "saveOver", "saveOn", Colours::transparentBlack);
    saveButton.setCommandToTrigger(commandManager, CommandIDs::saveConfig, true);
    addAndMakeVisible(saveButton);

    setButtonImages(saveAsButton, "saveAsOff", "saveAsOver", "saveAsOn", Colours::transparentBlack);
    saveAsButton.setCommandToTrigger(commandManager, CommandIDs::saveConfigAs, true);
    addAndMakeVisible(saveAsButton);

    setButtonImages(applyChangesButton, "confirmOff", "confirmOver", "confirmOn", Colours::transparentBlack);
    applyChangesButton.setCommandToTrigger(commandManager, CommandIDs::applyConfigChanges, true);
    addAndMakeVisible(applyChangesButton);

    setButtonImages(discardChangesButton, "closeOff", "closeOver", "closeOn", Colours::transparentBlack);
    discardChangesButton.setCommandToTrigger(commandManager, CommandIDs::discardConfigChanges, true);
    addAndMakeVisible(discardChangesButton);

    setButtonImages(undoButton, "undoOff", "undoOver", "undoOn", Colours::transparentBlack);
    undoButton.setCommandToTrigger(commandManager, CommandIDs::undo, true);
    addAndMakeVisible(undoButton);

    setButtonImages(redoButton, "redoOff", "redoOver", "redoOn", Colours::transparentBlack);
    redoButton.setCommandToTrigger(commandManager, CommandIDs::redo, true);
    addAndMakeVisible(redoButton);

    treeSizeConstrainer.setMinimumWidth(200);
    treeSizeConstrainer.setMaximumWidth(700);

    ConfigurationItem* rootItem = new ConfigurationItem(*this, scopeSync.getConfigurationRoot(), undoManager);
    treeView = new BCMTreeView(undoManager, rootItem, scopeSync.getConfiguration().getConfigurationProperties());

    int lastTreeWidth = scopeSync.getConfiguration().getConfigurationProperties().getIntValue("lastConfigTreeWidth", 300);
    treeView->setBounds(0, 0, lastTreeWidth, getHeight());
    addAndMakeVisible(treeView);
    
    addAndMakeVisible (resizerBar = new ResizableEdgeComponent(treeView, &treeSizeConstrainer,
                                                               ResizableEdgeComponent::rightEdge));
    resizerBar->setAlwaysOnTop (true);

    int lastConfigMgrWidth  = scopeSync.getConfiguration().getConfigurationProperties().getIntValue("lastConfigMgrWidth", 600);
    int lastConfigMgrHeight = scopeSync.getConfiguration().getConfigurationProperties().getIntValue("lastConfigMgrHeight", 500);
    setSize(lastConfigMgrWidth, lastConfigMgrHeight);
    
    startTimer(500);
}

void ConfigurationManager::setButtonImages(ImageButton& button, const String& normalImage, const String& overImage, const String& downImage, const Colour& overlayColour)
{
    button.setImages(true, true, true,
                     ImageLoader::getInstance()->loadImage(normalImage, true, ""), 1.0f, overlayColour,
                     ImageLoader::getInstance()->loadImage(overImage,   true, ""), 1.0f, overlayColour,
                     ImageLoader::getInstance()->loadImage(downImage,   true, ""), 1.0f, overlayColour, 0);
}

ConfigurationManager::~ConfigurationManager()
{
    stopTimer();
}

void ConfigurationManager::changePanel(Component* newComponent)
{
    panel = newComponent;
    addAndMakeVisible(panel);
    resized();
}

Component* ConfigurationManager::createParameterPanelComponent(ValueTree& tree, BCMParameter::ParameterType parameterType)
{
    return createParameterPanelComponent(tree, parameterType, scopeSync, undoManager, commandManager, this);
}

Component* ConfigurationManager::createParameterPanelComponent(ValueTree& tree, 
                                                               BCMParameter::ParameterType parameterType, 
                                                               ScopeSync& scopeSync, 
                                                               UndoManager& undoManager, 
                                                               ApplicationCommandManager* commandManager,
                                                               ChangeListener* listener)
{
    ParameterPanel* parameterPanel = new ParameterPanel(tree, undoManager, parameterType, scopeSync, commandManager);
    PresetChooser* presetChooser = new PresetChooser(tree, scopeSync, commandManager, undoManager);
    presetChooser->addChangeListener(listener);

    TabbedComponent* tabbedComponent = new TabbedComponent(TabbedButtonBar::TabsAtTop);
    tabbedComponent->setTabBarDepth(25);
    tabbedComponent->addTab("Parameter", Colours::darkgrey, parameterPanel, true, 0);
    tabbedComponent->addTab("Update From Preset", Colours::darkgrey, presetChooser, true, 1);

    return tabbedComponent;
}

void ConfigurationManager::changeListenerCallback(ChangeBroadcaster* /* source */)
{
    TabbedComponent* tabbedComponentPanel = dynamic_cast<TabbedComponent*>(panel.get());
    
	if (tabbedComponentPanel != nullptr)
	{
		tabbedComponentPanel->setCurrentTabIndex(0, false);

		ParameterPanel* parameterPanel = dynamic_cast<ParameterPanel*>(tabbedComponentPanel->getTabContentComponent(0));
		parameterPanel->rebuild();
	}
}

void ConfigurationManager::unload()
{
    stopTimer();
    saveTreeViewState();
    scopeSync.getConfiguration().getConfigurationProperties().setValue("lastConfigMgrWidth", getWidth());
    scopeSync.getConfiguration().getConfigurationProperties().setValue("lastConfigMgrHeight", getHeight());
    scopeSync.getConfiguration().getConfigurationProperties().setValue("lastConfigTreeWidth", treeView->getWidth());
}

void ConfigurationManager::getAllCommands (Array <CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::addConfig,
                              CommandIDs::saveConfig,
                              CommandIDs::saveConfigAs,
                              CommandIDs::applyConfigChanges,
                              CommandIDs::discardConfigChanges,
                              CommandIDs::closeConfig,
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

void ConfigurationManager::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
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
    case CommandIDs::addConfig:
        result.setInfo("Add Configuration", "Create a new Configuration", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('w', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::saveConfig:
        result.setInfo("Save Configuration", "Save Configuration", CommandCategories::configmgr, !(scopeSync.getConfiguration().hasChangedSinceSaved()));
        result.defaultKeypresses.add(KeyPress ('s', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::saveConfigAs:
        result.setInfo("Save Configuration As...", "Save Configuration as a new file", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('s', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    case CommandIDs::applyConfigChanges:
        result.setInfo("Apply Configuration Changes", "Applies changes made in the Configuration Manager to the relevant ScopeSync instance", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress (KeyPress::returnKey, ModifierKeys::altModifier, 0));
        break;
    case CommandIDs::discardConfigChanges:
        result.setInfo("Discard Configuration Changes", "Discards all unsaved changes to the current Configuration", CommandCategories::configmgr, !(scopeSync.getConfiguration().hasChangedSinceSaved()));
        result.defaultKeypresses.add(KeyPress ('d', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::closeConfig:
        result.setInfo("Close Configuration Manager", "Closes Configuration Manager window", CommandCategories::configmgr, 0);
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
        result.setInfo("Add item", "Adds a new configuration item", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('n', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::addItemFromClipboard:
        result.setInfo("Add item from clipboard", "Adds a new configuration item using the definition stored in the clipboard", CommandCategories::configmgr, !canPasteItem());
        result.defaultKeypresses.add(KeyPress ('n', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    }
}

bool ConfigurationManager::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::undo:                 undo(); break;
        case CommandIDs::redo:                 redo(); break;
        case CommandIDs::addConfig:            parentWindow.addConfig(); break;
        case CommandIDs::saveConfig:           parentWindow.save(); break;
        case CommandIDs::saveConfigAs:         parentWindow.saveAs(); break;
        case CommandIDs::applyConfigChanges:   scopeSync.applyConfiguration(); break;
        case CommandIDs::discardConfigChanges: parentWindow.reloadConfiguration(); break;
        case CommandIDs::closeConfig:          scopeSync.hideConfigurationManager(); break;
        case CommandIDs::focusOnPanel:         panel->grabKeyboardFocus(); break;
        case CommandIDs::copyItem:             treeView->copyItem(); break;
        case CommandIDs::pasteItem:            treeView->pasteItem(); break;
        case CommandIDs::deleteItems:          treeView->deleteSelectedItems(); break;
        case CommandIDs::addItem:              treeView->addItem(); break;
        case CommandIDs::addItemFromClipboard: treeView->addItemFromClipboard(); break;
        default:                               return false;
    }

    return true;
}

void ConfigurationManager::undo()
{
    undoManager.undo();
    treeView->changePanel();
}

void ConfigurationManager::redo()
{
    undoManager.redo();
    treeView->changePanel();
}

bool ConfigurationManager::canPasteItem()
{
    if (treeView != nullptr)
        return treeView->canPasteItem();
    else
        return false;
}

ApplicationCommandTarget* ConfigurationManager::getNextCommandTarget()
{
    return nullptr;
}

void ConfigurationManager::childBoundsChanged(Component* child)
{
    if (child == treeView)
        resized();
}

void ConfigurationManager::saveTreeViewState()
{
    treeView->saveTreeViewState();
}

void ConfigurationManager::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    
    Rectangle<int> toolbar(localBounds.removeFromTop(40).reduced(8, 8));
    
    addButton.setBounds(toolbar.removeFromLeft(40));
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

void ConfigurationManager::paint(Graphics& g)
{
    g.fillAll (Colour (0xff434343));

    g.setColour(Colours::darkgrey);
    g.fillRect(0, 0, getWidth(), 40);
    g.fillRect(0, 0, getWidth(), getHeight() - 40);

    g.drawImageAt(ImageLoader::getInstance()->loadImage("divider", true, String::empty), 134, 8);
    g.drawImageAt(ImageLoader::getInstance()->loadImage("divider", true, String::empty), 228, 8);
}

void ConfigurationManager::paintOverChildren(Graphics& g)
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

void ConfigurationManager::timerCallback()
{
    undoManager.beginNewTransaction();
}

/* =========================================================================
 * ConfigurationManagerCallout
 */
ConfigurationManagerCallout::ConfigurationManagerCallout(
    ScopeSync& ss,
    int width,
    int height)
    : scopeSync(ss),
      undoManager(ss.getUndoManager())
{
    numActions = 0;
    
    setLookAndFeel(&lookAndFeel);

    commandManager = scopeSync.getCommandManager();
    commandManager->setFirstCommandTarget(nullptr);

    commandManager->registerAllCommandsForTarget(this);
    addKeyListener(scopeSync.getCommandManager()->getKeyMappings());
    
    setSize(width, height);
    
    startTimer(500);
}

ConfigurationManagerCallout::~ConfigurationManagerCallout()
{
    stopTimer();
    undoManager.beginNewTransaction();
    
    removeKeyListener(scopeSync.getCommandManager()->getKeyMappings());
}


void ConfigurationManagerCallout::timerCallback()
{
    numActions += undoManager.getNumActionsInCurrentTransaction();
    undoManager.beginNewTransaction();
}

void ConfigurationManagerCallout::changePanel(Component* newComponent)
{
    panel = newComponent;
    addAndMakeVisible(panel);
    resized();
}

void ConfigurationManagerCallout::getAllCommands (Array <CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::undo,
                              CommandIDs::redo
                            };

    commands.addArray (ids, numElementsInArray (ids));
}

void ConfigurationManagerCallout::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::undo:
        result.setInfo("Undo", "Undo latest change", CommandCategories::general, scopeSync.getUndoManager().canUndo() ? 0 : 1);
        result.defaultKeypresses.add(KeyPress ('z', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::redo:
        result.setInfo("Redo", "Redo latest change", CommandCategories::general, scopeSync.getUndoManager().canRedo() ? 0 : 1);
        result.defaultKeypresses.add(KeyPress ('y', ModifierKeys::commandModifier, 0));
        break;
    }
}

bool ConfigurationManagerCallout::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::undo:   undo(); break;
        case CommandIDs::redo:   redo(); break;
        default:                 return false;
    }

    return true;
}

void ConfigurationManagerCallout::undo()
{
    if (scopeSync.getUndoManager().undo())
        numActions += 1;
}

void ConfigurationManagerCallout::redo()
{
    if (scopeSync.getUndoManager().redo())
        numActions += 1;
}

ApplicationCommandTarget* ConfigurationManagerCallout::getNextCommandTarget()
{
    return nullptr;
}

void ConfigurationManagerCallout::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    
    if (panel != nullptr)
        panel->setBounds(localBounds);
}

void ConfigurationManagerCallout::paint(Graphics& g)
{
    g.fillAll (Colour (0xff434343));
}

void ConfigurationManagerCallout::changeListenerCallback(ChangeBroadcaster* /* source */)
{
    TabbedComponent* tabbedComponentPanel = dynamic_cast<TabbedComponent*>(panel.get());
    
	if (tabbedComponentPanel != nullptr)
	{
		tabbedComponentPanel->setCurrentTabIndex(0, false);

		ParameterPanel* parameterPanel = dynamic_cast<ParameterPanel*>(tabbedComponentPanel->getTabContentComponent(0));
		parameterPanel->rebuild();
	}
}

/* =========================================================================
 * ConfigurationMenuBarModel
 */
ConfigurationMenuBarModel::ConfigurationMenuBarModel(ConfigurationManagerWindow& owner) : configurationManagerWindow(owner)
{
    setApplicationCommandManagerToWatch(configurationManagerWindow.getCommandManager());
}

StringArray ConfigurationMenuBarModel::getMenuBarNames()
{
    return configurationManagerWindow.getMenuNames();
}

PopupMenu ConfigurationMenuBarModel::getMenuForIndex (int /*topLevelMenuIndex*/, const String& menuName)
{
    PopupMenu menu;
    configurationManagerWindow.createMenu(menu, menuName);
    return menu;
}

/* =========================================================================
 * ConfigurationManagerWindow
 */
ConfigurationManagerWindow::ConfigurationManagerWindow(ScopeSync& owner, int posX, int posY)
    : DocumentWindow("Configuration Manager",
                     Colour::greyLevel(0.6f),
                     DocumentWindow::allButtons,
                     true),
      scopeSync(owner)
{
    commandManager = owner.getCommandManager();
    setUsingNativeTitleBar (true);
    
    configurationManager = nullptr;
    refreshContent();
    
    menuModel = new ConfigurationMenuBarModel(*this);
    setMenuBar(menuModel);

    addKeyListener(commandManager->getKeyMappings());

    restoreWindowPosition(posX, posY);
    
    setVisible(true);
    setResizable(true, false);

    setWantsKeyboardFocus (false);

    setResizeLimits(600, 500, 32000, 32000);
}

ConfigurationManagerWindow::~ConfigurationManagerWindow()
{
    unload();
}

StringArray ConfigurationManagerWindow::getMenuNames()
{
    const char* const names[] = { "File", "Edit", nullptr };
    return StringArray (names);
}

void ConfigurationManagerWindow::createMenu(PopupMenu& menu, const String& menuName)
{
         if (menuName == "File") createFileMenu(menu);
    else if (menuName == "Edit") createEditMenu(menu);
    else                    jassertfalse; // names have changed?
}

void ConfigurationManagerWindow::createFileMenu(PopupMenu& menu)
{
    menu.addCommandItem(commandManager, CommandIDs::addConfig);
    menu.addCommandItem(commandManager, CommandIDs::saveConfig);
    menu.addCommandItem(commandManager, CommandIDs::saveConfigAs);
    menu.addSeparator();
    menu.addCommandItem(commandManager, CommandIDs::applyConfigChanges);   
    menu.addCommandItem(commandManager, CommandIDs::discardConfigChanges);   
    menu.addSeparator();
    menu.addCommandItem(commandManager, CommandIDs::closeConfig);   
}

void ConfigurationManagerWindow::createEditMenu(PopupMenu& menu)
{
    menu.addCommandItem(commandManager, CommandIDs::undo);
    menu.addCommandItem(commandManager, CommandIDs::redo);
}

void ConfigurationManagerWindow::closeButtonPressed()
{
    unload();
    scopeSync.applyConfiguration();
    scopeSync.hideConfigurationManager();
}

void ConfigurationManagerWindow::addConfig()
{
    if (configurationManager != nullptr)
        configurationManager->unload();

    scopeSync.addConfiguration(getParentMonitorArea());
}

void ConfigurationManagerWindow::save()
{
    scopeSync.saveConfiguration();
}

void ConfigurationManagerWindow::unload()
{
    scopeSync.getConfiguration().getConfigurationProperties().setValue("lastConfigMgrPos", getWindowStateAsString());

    if (configurationManager != nullptr)
        configurationManager->unload();
    
    removeKeyListener(commandManager->getKeyMappings());
    
    setMenuBar(nullptr);
}

void ConfigurationManagerWindow::saveAs()
{
    if (configurationManager != nullptr)
        configurationManager->unload();

    scopeSync.saveConfigurationAs();
}

void ConfigurationManagerWindow::reloadConfiguration()
{
    scopeSync.reloadSavedConfiguration();
    refreshContent();
}

void ConfigurationManagerWindow::refreshContent()
{
    clearContentComponent();
    setContentOwned(configurationManager = new ConfigurationManager(scopeSync, *this), true);
}

void ConfigurationManagerWindow::restoreWindowPosition(int posX, int posY)
{
    String windowState;

    windowState = scopeSync.getConfiguration().getConfigurationProperties().getValue("lastConfigMgrPos");

    if (windowState.isEmpty())
        setBounds(posX, posY, getWidth(), getHeight());
    else
        restoreWindowStateFromString(windowState);
}

void ConfigurationManagerWindow::restoreWindowPosition()
{
    String windowState;

    windowState = scopeSync.getConfiguration().getConfigurationProperties().getValue("lastConfigMgrPos");

    if (windowState.isNotEmpty())
        restoreWindowStateFromString(windowState);
}

/* =========================================================================
 * ConfigurationManagerCalloutWindow
 */
ConfigurationManagerCalloutWindow::ConfigurationManagerCalloutWindow(ScopeSync& owner, int width, int height)
    : scopeSync(owner), undoManager(owner.getUndoManager())
{
    commandManager = scopeSync.getCommandManager();
    
    undoManager.beginNewTransaction();
        
    configurationManagerCallout = new ConfigurationManagerCallout(owner, width, height);
    addAndMakeVisible(configurationManagerCallout);
    
    setOpaque(true);
    setVisible(true);
    
    setWantsKeyboardFocus (false);
    setSize(width, height);
}

ConfigurationManagerCalloutWindow::~ConfigurationManagerCalloutWindow()
{
    // Tell the parent component to reload, as we've made a change
    if (configurationManagerCallout->getNumActions() > 0)
        sendSynchronousChangeMessage();
}

void ConfigurationManagerCalloutWindow::setMappingPanel(ValueTree& mapping, const Identifier& componentType, const String& componentName)
{
    if (!(mapping.isValid()))
         scopeSync.getConfiguration().addNewMapping(componentType, componentName, String::empty, mapping, -1, &undoManager);

    MappingPanel* panelToShow;

    if (componentType == Ids::textButton)
        panelToShow = new TextButtonMappingPanel(mapping, undoManager, scopeSync, commandManager, true);
    else
        panelToShow = new MappingPanel(mapping, undoManager, scopeSync, commandManager, componentType, true);
    
        configurationManagerCallout->changePanel(panelToShow);
}

void ConfigurationManagerCalloutWindow::setParameterPanel(ValueTree& parameter, BCMParameter::ParameterType paramType)
{
    configurationManagerCallout->changePanel(ConfigurationManager::createParameterPanelComponent(parameter, paramType, scopeSync, undoManager, commandManager, configurationManagerCallout));
}

void ConfigurationManagerCalloutWindow::setStyleOverridePanel(ValueTree&  styleOverride, 
                                                        const Identifier& componentType, 
                                                        const String&     componentName, 
                                                        const String&     widgetTemplateId, 
                                                        const String&     fillColour,
                                                        const String&     lineColour,
                                                        const String&     fillColour2,
                                                        const String&     lineColour2)
{
    if (!(styleOverride.isValid()))
    {
        scopeSync.getConfiguration().addStyleOverride(componentType, componentName, widgetTemplateId, styleOverride, -1, &undoManager);
        styleOverride.setProperty(Ids::fillColour,  fillColour, &undoManager);
        styleOverride.setProperty(Ids::lineColour,  lineColour, &undoManager);
        styleOverride.setProperty(Ids::fillColour2, fillColour2, &undoManager);
        styleOverride.setProperty(Ids::lineColour2, lineColour2, &undoManager);
    }

    configurationManagerCallout->changePanel(new StyleOverridePanel(styleOverride, undoManager, scopeSync, commandManager, componentType, true));
}

void ConfigurationManagerCalloutWindow::paint(Graphics& g)
{
    g.fillAll(Colour(0xff434343));
}

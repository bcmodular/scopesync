/**
 * Main display component for the Configuration Manager. Contains
 * the TreeView and edit Panels
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

#include "ConfigurationManagerMain.h"

#include "../Core/Global.h"
#include "../Core/ScopeSyncApplication.h"
#include "../Utils/BCMMisc.h"
#include "../Core/ScopeSync.h"
#include "ConfigurationTree.h"
#include "../Resources/ImageLoader.h"

ConfigurationManagerMain::ConfigurationManagerMain(ConfigurationManager& owner,
                                                   ScopeSync& ss) : configurationManager(owner),
                                                                    scopeSync(ss),
                                                                    saveButton("Save"),
                                                                    saveAsButton("Save As..."),
                                                                    applyChangesButton("Apply Changes"),
                                                                    discardChangesButton("Discard All Unsaved Changes"),
                                                                    undoButton("Undo"),
                                                                    redoButton("Redo"),
                                                                    commandManager(scopeSync.getCommandManager())
{
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

    treeSizeConstrainer.setMinimumWidth (200);
    treeSizeConstrainer.setMaximumWidth (700);

    treeView = new ConfigurationTree(*this, scopeSync.getConfiguration(), undoManager);
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

void ConfigurationManagerMain::setButtonImages(ImageButton& button, const String& normalImage, const String& overImage, const String& downImage, const Colour& overlayColour)
{
    button.setImages(true, true, true,
                     ImageLoader::getInstance()->loadImage(normalImage, true, ""), 1.0f, overlayColour,
                     ImageLoader::getInstance()->loadImage(overImage,   true, ""), 1.0f, overlayColour,
                     ImageLoader::getInstance()->loadImage(downImage,   true, ""), 1.0f, overlayColour, 0);
}

ConfigurationManagerMain::~ConfigurationManagerMain() {}

void ConfigurationManagerMain::changePanel(Component* newComponent)
{
    panel = newComponent;
    addAndMakeVisible(panel);
    resized();
}

void ConfigurationManagerMain::unload()
{
    stopTimer();
    saveTreeViewState();
    scopeSync.getConfiguration().getConfigurationProperties().setValue("lastConfigMgrWidth", getWidth());
    scopeSync.getConfiguration().getConfigurationProperties().setValue("lastConfigMgrHeight", getHeight());
    scopeSync.getConfiguration().getConfigurationProperties().setValue("lastConfigTreeWidth", treeView->getWidth());
}

void ConfigurationManagerMain::updateConfigurationFileName()
{
    fileNameLabel.setText(scopeSync.getConfigurationFile().getFullPathName(), dontSendNotification);
    fileNameLabel.setTooltip(scopeSync.getConfigurationFile().getFullPathName());
}

void ConfigurationManagerMain::getAllCommands (Array <CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::saveConfig,
                              CommandIDs::saveConfigAs,
                              CommandIDs::applyConfigChanges,
                              CommandIDs::discardConfigChanges,
                              CommandIDs::closeConfig,
                              CommandIDs::undo,
                              CommandIDs::redo,
                              CommandIDs::deleteSelectedItems,
                              CommandIDs::focusOnPanel
                            };

    commands.addArray (ids, numElementsInArray (ids));
}

void ConfigurationManagerMain::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::undo:
        result.setInfo("Undo", "Undo latest change", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('z', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::redo:
        result.setInfo ("Redo", "Redo latest change", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('y', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::deleteSelectedItems:
        result.setInfo ("Delete", "Delete selected items", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add (KeyPress (KeyPress::deleteKey, 0, 0));
        result.defaultKeypresses.add (KeyPress (KeyPress::backspaceKey, 0, 0));
        break;
    case CommandIDs::saveConfig:
        result.setInfo ("Save Configuration", "Save Configuration", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('s', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::saveConfigAs:
        result.setInfo ("Save Configuration As...", "Save Configuration as a new file", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('s', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    case CommandIDs::applyConfigChanges:
        result.setInfo ("Apply Configuration Changes", "Applies changes made in the Configuration Manager to the relevant ScopeSync instance", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress (KeyPress::returnKey, ModifierKeys::altModifier, 0));
        break;
    case CommandIDs::discardConfigChanges:
        result.setInfo ("Discard Configuration Changes", "Discards all unsaved changes to the current Configuration", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('d', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::closeConfig:
        result.setInfo ("Close Configuration Manager", "Closes Configuration Manager window", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress ('q', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::focusOnPanel:
        result.setInfo ("Focus on panel", "Switches keyboard focus to edit panel", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress (KeyPress::F2Key, ModifierKeys::noModifiers, 0));
        break;
    }
}

bool ConfigurationManagerMain::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::undo:                 undoManager.undo(); break;
        case CommandIDs::redo:                 undoManager.redo(); break;
        case CommandIDs::deleteSelectedItems:  deleteSelectedTreeItems(); break;
        case CommandIDs::saveConfig:           configurationManager.save(); break;
        case CommandIDs::saveConfigAs:         configurationManager.saveAs(); break;
        case CommandIDs::applyConfigChanges:   scopeSync.applyConfiguration(); break;
        case CommandIDs::discardConfigChanges: configurationManager.discardChanges(); break;
        case CommandIDs::closeConfig:          scopeSync.hideConfigurationManager(); break;
        case CommandIDs::focusOnPanel:         switchFocusToPanel(); break;
        default:                               return false;
    }

    return true;
}

ApplicationCommandTarget* ConfigurationManagerMain::getNextCommandTarget()
{
    return nullptr;
}

void ConfigurationManagerMain::childBoundsChanged(Component* child)
{
    if (child == treeView)
        resized();
}

void ConfigurationManagerMain::saveTreeViewState()
{
    treeView->saveTreeViewState();
}

void ConfigurationManagerMain::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    
    Rectangle<int> toolbar(localBounds.removeFromTop(40).reduced(8, 8));
    
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

void ConfigurationManagerMain::paint(Graphics& g)
{
    g.fillAll (Colour (0xff434343));

    g.setColour(Colours::darkgrey);
    g.fillRect(0, 0, getWidth(), 40);
    g.fillRect(0, 0, getWidth(), getHeight() - 40);

    g.drawImageAt(ImageLoader::getInstance()->loadImage("divider", true, String::empty), 94, 8);
    g.drawImageAt(ImageLoader::getInstance()->loadImage("divider", true, String::empty), 188, 8);
}

void ConfigurationManagerMain::paintOverChildren(Graphics& g)
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

void ConfigurationManagerMain::timerCallback()
{
    undoManager.beginNewTransaction();
}

void ConfigurationManagerMain::deleteSelectedTreeItems()
{
    treeView->deleteSelectedItems();
}

void ConfigurationManagerMain::switchFocusToPanel()
{
    panel->grabKeyboardFocus();
}
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
#include "../Core/ScopeSync.h"
#include "ConfigurationManagerMain.h"

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
 * ConfigurationManager
 */
ConfigurationManager::ConfigurationManager(ScopeSync& owner)
    : scopeSync(owner)
{
    commandManager = scopeSync.getCommandManager();

    // Make sure the Application Command Manager uses the standard algorithm
    // to choose its target (overriden by callouts)
    commandManager->setFirstCommandTarget(nullptr);
}

ConfigurationManager::~ConfigurationManager() {}

void ConfigurationManager::save()
{
    scopeSync.saveConfiguration();
}

void ConfigurationManager::saveAs()
{
    File configurationFileDirectory = scopeSync.getConfigurationDirectory();
    
    FileChooser fileChooser("Save Configuration File As...",
                            configurationFileDirectory,
                            "*.configuration");
    
    if (fileChooser.browseForFileToSave(true))
    {
        scopeSync.saveConfigurationAs(fileChooser.getResult().getFullPathName());
    }
}

void ConfigurationManager::reloadConfiguration()
{
    scopeSync.reloadSavedConfiguration();
}

/* =========================================================================
 * ConfigurationManagerWindow
 */
ConfigurationManagerWindow::ConfigurationManagerWindow(ScopeSync& owner, int posX, int posY)
    : DocumentWindow("Configuration Manager",
                     Colour::greyLevel(0.6f),
                     DocumentWindow::allButtons,
                     true),
      configurationManager(owner)
{
    commandManager = owner.getCommandManager();
    setUsingNativeTitleBar (true);
    
    setContentOwned(configurationManagerMain = new ConfigurationManagerMain(configurationManager, owner), true);
    
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
    configurationManager.getScopeSync().applyConfiguration();
    configurationManager.getScopeSync().hideConfigurationManager();
}

void ConfigurationManagerWindow::save()
{
    configurationManager.save();
}

void ConfigurationManagerWindow::unload()
{
    configurationManager.getConfiguration().getConfigurationProperties().setValue("lastConfigMgrPos", getWindowStateAsString());

    if (configurationManagerMain != nullptr)
        configurationManagerMain->unload();
    
    removeKeyListener(commandManager->getKeyMappings());
    
    setMenuBar(nullptr);
}

void ConfigurationManagerWindow::saveAs()
{
    configurationManager.saveAs();
    configurationManagerMain->updateConfigurationFileName();
}

void ConfigurationManagerWindow::reloadConfiguration()
{
    configurationManager.reloadConfiguration();
    configurationManagerMain->unload();
    clearContentComponent();
    setContentOwned(configurationManagerMain = new ConfigurationManagerMain(configurationManager, configurationManager.getScopeSync()), true);
}

void ConfigurationManagerWindow::restoreWindowPosition(int posX, int posY)
{
    String windowState;

    windowState = configurationManager.getConfiguration().getConfigurationProperties().getValue("lastConfigMgrPos");

    if (windowState.isEmpty())
        setBounds(posX, posY, getWidth(), getHeight());
    else
        restoreWindowStateFromString(windowState);
}

/* =========================================================================
 * ConfigurationManagerCallout
 */
ConfigurationManagerCallout::ConfigurationManagerCallout(ScopeSync& owner, int width, int height)
    : configurationManager(owner), scopeSync(owner), undoManager(owner.getUndoManager())
{
    commandManager = owner.getCommandManager();
    
    undoManager.beginNewTransaction();
        
    configurationManagerCalloutMain = new ConfigurationManagerCalloutMain(configurationManager, owner, width, height);
    addAndMakeVisible(configurationManagerCalloutMain);
    
    setOpaque(true);
    setVisible(true);
    
    setWantsKeyboardFocus (false);
    setSize(width, height);
}

ConfigurationManagerCallout::~ConfigurationManagerCallout()
{
    // Tell the parent component to reload, as we've made a change
    if (configurationManagerCalloutMain->getNumActions() > 0)
        sendSynchronousChangeMessage();
}

void ConfigurationManagerCallout::setMappingPanel(ValueTree& mapping, const Identifier& componentType, const String& componentName)
{
    if (!(mapping.isValid()))
         configurationManager.getConfiguration().addNewMapping(componentType, componentName, String::empty, mapping, -1, &undoManager);

    MappingPanel* panelToShow;

    if (componentType == Ids::textButton)
        panelToShow = new TextButtonMappingPanel(mapping, undoManager, scopeSync, commandManager, true);
    else
        panelToShow = new MappingPanel(mapping, undoManager, scopeSync, commandManager, componentType, true);
    
        configurationManagerCalloutMain->changePanel(panelToShow);
}

void ConfigurationManagerCallout::setParameterPanel(ValueTree& parameter, BCMParameter::ParameterType paramType)
{
    configurationManagerCalloutMain->changePanel(new ParameterPanel(parameter, undoManager, paramType, scopeSync, commandManager, true));
}

void ConfigurationManagerCallout::setStyleOverridePanel(ValueTree& styleOverride, const Identifier& componentType, const String& componentName)
{
    if (!(styleOverride.isValid()))
         configurationManager.getConfiguration().addStyleOverride(componentType, componentName, styleOverride, -1, &undoManager);

    configurationManagerCalloutMain->changePanel(new StyleOverridePanel(styleOverride, undoManager, scopeSync, commandManager, componentType, true));
}

void ConfigurationManagerCallout::setRotaryStyleOverridePanel(ValueTree& styleOverride, const String& componentName, const String& fillColour)
{
    if (!(styleOverride.isValid()))
    {
        configurationManager.getConfiguration().addStyleOverride(Ids::slider, componentName, styleOverride, -1, &undoManager);
        styleOverride.setProperty(Ids::fillColour, fillColour, &undoManager);
    }

    configurationManagerCalloutMain->changePanel(new RotaryStyleOverridePanel(styleOverride, undoManager, scopeSync, commandManager, true));
}

void ConfigurationManagerCallout::paint(Graphics& g)
{
    g.fillAll(Colour(0xff434343));
}

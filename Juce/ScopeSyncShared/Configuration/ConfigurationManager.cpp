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
        //configurationManagerMain->updateConfigurationFileName();
    }
}

void ConfigurationManager::reloadConfiguration()
{
    scopeSync.reloadSavedConfiguration();
    //configurationManagerMain->unload();
    //clearContentComponent();
    //setContentOwned(configurationManagerMain = new ConfigurationManagerMain(*this, scopeSync), true);
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

    addKeyListener(configurationManager.getCommandManager()->getKeyMappings());

    restoreWindowPosition(posX, posY);
    
    setVisible(true);
    setResizable(true, false);

    setWantsKeyboardFocus (false);

    setResizeLimits(600, 500, 32000, 32000);
}

ConfigurationManagerWindow::~ConfigurationManagerWindow()
{
    setMenuBar(nullptr);
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
    if (configurationManagerMain != nullptr)
        configurationManagerMain->unload();

    configurationManager.getConfiguration().getConfigurationProperties().setValue("lastConfigMgrPos", getWindowStateAsString());
    
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
    : configurationManager(owner)
{
    commandManager = owner.getCommandManager();
    
    configurationManagerCalloutMain = new ConfigurationManagerCalloutMain(configurationManager, owner, width, height);
    addAndMakeVisible(configurationManagerCalloutMain);
    
    addKeyListener(commandManager->getKeyMappings());

    setVisible(true);
    
    setWantsKeyboardFocus (false);
    setSize(width, height);
}

ConfigurationManagerCallout::~ConfigurationManagerCallout()
{
    removeKeyListener(commandManager->getKeyMappings());
    configurationManager.getScopeSync().applyConfiguration();
}

void ConfigurationManagerCallout::setMappingPanel(ValueTree& mapping, const String& componentType, const String& componentName)
{
    if (!(mapping.isValid()))
         configurationManager.getConfiguration().addNewMapping(Ids::slider, componentName, String::empty, mapping, -1, &(configurationManagerCalloutMain->getUndoManager()));

    configurationManagerCalloutMain->changePanel(new MappingPanel(mapping, configurationManagerCalloutMain->getUndoManager(), configurationManager.getConfiguration(), commandManager, componentType, true));
}

void ConfigurationManagerCallout::setParameterPanel(ValueTree& parameter, BCMParameter::ParameterType paramType)
{
    configurationManagerCalloutMain->changePanel(new ParameterPanel(parameter, configurationManagerCalloutMain->getUndoManager(), paramType, configurationManager.getConfiguration(), commandManager, true));
}

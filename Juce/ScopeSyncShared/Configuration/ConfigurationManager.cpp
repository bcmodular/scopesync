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
#include "ConfigurationMenuBarModel.h"
#include "ConfigurationManagerMain.h"

ConfigurationManager::ConfigurationManager(ScopeSyncGUI& owner) : DocumentWindow("Configuration Manager", Colour::greyLevel(0.6f), DocumentWindow::allButtons, true),
                                                                  scopeSyncGUI(owner),
                                                                  commandManager(owner.getScopeSync().getCommandManager())
{
    setUsingNativeTitleBar (true);
    
    setContentOwned(configurationManagerMain = new ConfigurationManagerMain(*this, scopeSyncGUI), true);
    
    menuModel = new ConfigurationMenuBarModel(*this);
    setMenuBar(menuModel);

    addKeyListener (commandManager.getKeyMappings());

    centreWithSize(getWidth(), getHeight());
    setVisible(true);
    setResizable(true, false);

    setWantsKeyboardFocus (false);

    setResizeLimits(600, 500, 32000, 32000);
}

ConfigurationManager::~ConfigurationManager()
{
    removeKeyListener(commandManager.getKeyMappings());
    setMenuBar(nullptr);
    scopeSyncGUI.getScopeSync().applyConfiguration();
}

StringArray ConfigurationManager::getMenuNames()
{
    const char* const names[] = { "File", nullptr };
    return StringArray (names);
}

void ConfigurationManager::createMenu (PopupMenu& menu, const String& menuName)
{
    if (menuName == "File") createFileMenu(menu);
    else                    jassertfalse; // names have changed?
}

void ConfigurationManager::createFileMenu (PopupMenu& menu)
{
    menu.addCommandItem(&commandManager, CommandIDs::saveConfig);
    menu.addCommandItem(&commandManager, CommandIDs::saveAndCloseConfig);
    menu.addCommandItem(&commandManager, CommandIDs::saveConfigAs);
    menu.addCommandItem(&commandManager, CommandIDs::discardConfigChanges);   
    menu.addSeparator();
    menu.addCommandItem(&commandManager, CommandIDs::closeConfig);   
}

void ConfigurationManager::closeButtonPressed()
{
    scopeSyncGUI.hideConfigurationManager();
}


void ConfigurationManager::saveAndClose()
{
    save();
    scopeSyncGUI.hideConfigurationManager();
}

void ConfigurationManager::save()
{
    scopeSyncGUI.getScopeSync().saveConfiguration();
}

void ConfigurationManager::saveAs()
{
    File configurationFileDirectory = scopeSyncGUI.getScopeSync().getConfigurationDirectory();
    
    FileChooser fileChooser("Save Configuration File As...",
                            configurationFileDirectory,
                            "*.configuration");
    
    if (fileChooser.browseForFileToSave(true))
    {
        scopeSyncGUI.getScopeSync().saveConfigurationAs(fileChooser.getResult().getFullPathName());
        configurationManagerMain->updateConfigurationFileName();
    }
}

void ConfigurationManager::discardChanges()
{
    scopeSyncGUI.getScopeSync().reloadSavedConfiguration();
    scopeSyncGUI.hideConfigurationManager();
}

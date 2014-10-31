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
    
    configurationManagerMain = nullptr;
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
    if (configurationManagerMain != nullptr)
        configurationManagerMain->unload();

    scopeSync.removeAllChangeListeners();
    scopeSync.addChangeListener(this);
    scopeSync.addConfiguration(getParentMonitorArea());
}

void ConfigurationManagerWindow::changeListenerCallback(ChangeBroadcaster* source)
{ 
    if (source == &scopeSync)
    {
        // This is the callback for when a new configuration has been added
        ScopeSync::checkNewConfigIsInLocation(scopeSync.getConfiguration(), this, this);
        refreshContent();
    }
    else
    {
        // This is for when the File Locations window calls back on close
        UserSettings::getInstance()->hideFileLocationsWindow();
    }
}

void ConfigurationManagerWindow::save()
{
    scopeSync.saveConfiguration();
}

void ConfigurationManagerWindow::unload()
{
    scopeSync.getConfiguration().getConfigurationProperties().setValue("lastConfigMgrPos", getWindowStateAsString());

    if (configurationManagerMain != nullptr)
        configurationManagerMain->unload();
    
    removeKeyListener(commandManager->getKeyMappings());
    
    setMenuBar(nullptr);
}

void ConfigurationManagerWindow::saveAs()
{
    if (configurationManagerMain != nullptr)
        configurationManagerMain->unload();

    if (scopeSync.saveConfigurationAs())
        ScopeSync::checkNewConfigIsInLocation(scopeSync.getConfiguration(), this, this);

    refreshContent();
}

void ConfigurationManagerWindow::reloadConfiguration()
{
    scopeSync.reloadSavedConfiguration();
    refreshContent();
}

void ConfigurationManagerWindow::refreshContent()
{
    clearContentComponent();
    setContentOwned(configurationManagerMain = new ConfigurationManagerMain(scopeSync, *this), true);
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
 * ConfigurationManagerCallout
 */
ConfigurationManagerCallout::ConfigurationManagerCallout(ScopeSync& owner, int width, int height)
    : scopeSync(owner), undoManager(owner.getUndoManager())
{
    commandManager = owner.getCommandManager();
    
    undoManager.beginNewTransaction();
        
    configurationManagerCalloutMain = new ConfigurationManagerCalloutMain(owner, width, height);
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
         scopeSync.getConfiguration().addNewMapping(componentType, componentName, String::empty, mapping, -1, &undoManager);

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

void ConfigurationManagerCallout::setStyleOverridePanel(ValueTree&        styleOverride, 
                                                        const Identifier& componentType, 
                                                        const String&     componentName, 
                                                        const String&     fillColour,
                                                        const String&     lineColour,
                                                        const String&     fillColour2,
                                                        const String&     lineColour2)
{
    if (!(styleOverride.isValid()))
    {
        scopeSync.getConfiguration().addStyleOverride(componentType, componentName, styleOverride, -1, &undoManager);
        styleOverride.setProperty(Ids::fillColour,  fillColour, &undoManager);
        styleOverride.setProperty(Ids::lineColour,  lineColour, &undoManager);
        styleOverride.setProperty(Ids::fillColour2, fillColour2, &undoManager);
        styleOverride.setProperty(Ids::lineColour2, lineColour2, &undoManager);
    }

    configurationManagerCalloutMain->changePanel(new StyleOverridePanel(styleOverride, undoManager, scopeSync, commandManager, componentType, true));
}

void ConfigurationManagerCallout::paint(Graphics& g)
{
    g.fillAll(Colour(0xff434343));
}

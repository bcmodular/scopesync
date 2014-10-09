/**
 * UI Component for managing the current Configuration. The
 * ConfigurationManager class is also the container window that
 * acts as the parent for the sub-components
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

#ifndef CONFIGURATIONMANAGER_H_INCLUDED
#define CONFIGURATIONMANAGER_H_INCLUDED
#include <JuceHeader.h>
#include "../Core/ScopeSync.h"
#include "../Configuration/ConfigurationPanel.h"

class PropertyListBuilder;
class ConfigurationManagerMain;
class ConfigurationManagerWindow;

/* =========================================================================
 * ConfigurationMenuBarModel: Sets up the menu bar for the Config Mgr
 */
class ConfigurationMenuBarModel  : public MenuBarModel
{
public:
    ConfigurationMenuBarModel(ConfigurationManagerWindow& owner);

    StringArray getMenuBarNames();
    PopupMenu   getMenuForIndex(int topLevelMenuIndex, const String& menuName);
    void        menuItemSelected(int /* menuItemID */, int /* topLevelMenuIndex */) {};

    ConfigurationManagerWindow& configurationManagerWindow;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationMenuBarModel);
};

/* =========================================================================
 * ConfigurationManager: Controller for the Config Mgr
 */
class ConfigurationManager
{
public:
    ConfigurationManager(ScopeSync& owner);
    ~ConfigurationManager();

    ApplicationCommandManager* getCommandManager() { return commandManager; };
    Configuration&             getConfiguration() { return scopeSync.getConfiguration(); };
    ScopeSync&                 getScopeSync() { return scopeSync; };

    void save();
    void saveAs();
    void reloadConfiguration();

private:
    ScopeSync&                 scopeSync;
    ApplicationCommandManager* commandManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationManager);
};

/* =========================================================================
 * ConfigurationManagerWindow: Parent window for the Config Mgr
 */
class ConfigurationManagerWindow : public DocumentWindow
{
public:
    ConfigurationManagerWindow(ScopeSync& owner, int posX, int posY);
    ~ConfigurationManagerWindow();

    ApplicationCommandManager* getCommandManager() { return commandManager; };
    StringArray getMenuNames();
    void createMenu(PopupMenu& menu, const String& menuName);
    void createFileMenu(PopupMenu& menu);
    void createEditMenu(PopupMenu& menu);

    void save();
    void saveAndClose();
    void saveAs();
    void unload();
    void reloadConfiguration();

private:
    ApplicationCommandManager*               commandManager;
    ConfigurationManager                     configurationManager;
    ConfigurationManagerMain*                configurationManagerMain;
    ScopedPointer<ConfigurationMenuBarModel> menuModel;
    
    void closeButtonPressed() override;
    void restoreWindowPosition(int posX, int posY);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationManagerWindow);
};

/* =========================================================================
 * ConfigurationManagerCallout: Mini-version of the Config Mgr for a call-out box
 */
class ConfigurationManagerCalloutMain;

class ConfigurationManagerCallout : public Component,
                                    public ChangeBroadcaster
{
public:
    ConfigurationManagerCallout(ScopeSync& owner, int width, int height);
    ~ConfigurationManagerCallout();

    void setMappingPanel(ValueTree& mapping, const Identifier& componentType, const String& componentName);
    void setParameterPanel(ValueTree& parameter, BCMParameter::ParameterType paramType);
    void setStyleOverridePanel(ValueTree& styleOverride, const Identifier& componentType, const String& componentName);
    
private:
    ScopeSync&                 scopeSync;
    ApplicationCommandManager* commandManager;
    ConfigurationManager       configurationManager;
    ScopedPointer<ConfigurationManagerCalloutMain>  configurationManagerCalloutMain;

    void paint(Graphics& g) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationManagerCallout);
};

#endif  // CONFIGURATIONMANAGER_H_INCLUDED

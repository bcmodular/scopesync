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

#ifndef CONFIGURATIONMANAGER_H_INCLUDED
#define CONFIGURATIONMANAGER_H_INCLUDED
#include <JuceHeader.h>
#include "../Core/ScopeSync.h"
class ConfigurationMenuBarModel;
class PropertyListBuilder;
class ConfigurationManagerMain;

class ConfigurationManager : public DocumentWindow
{
public:
    ConfigurationManager(ScopeSync& owner, int posX, int posY);
    ~ConfigurationManager();

    ApplicationCommandManager& getCommandManager() { return commandManager; };
    StringArray getMenuNames();
    void createMenu(PopupMenu& menu, const String& menuName);
    void createFileMenu(PopupMenu& menu);
    void createEditMenu(PopupMenu& menu);

    void save();
    void saveAndClose();
    void saveAs();
    void discardChanges();

private:
    ScopeSync&                               scopeSync;
    ApplicationCommandManager&               commandManager;
    ScopedPointer<ConfigurationMenuBarModel> menuModel;
    ConfigurationManagerMain*                configurationManagerMain;
    
    void closeButtonPressed() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationManager);
};



#endif  // CONFIGURATIONMANAGER_H_INCLUDED

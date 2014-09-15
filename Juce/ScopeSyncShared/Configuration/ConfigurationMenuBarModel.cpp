/*
  ==============================================================================

    ConfigurationMenuBarModel.cpp
    Created: 14 Sep 2014 8:31:47am
    Author:  giles

  ==============================================================================
*/

#include "ConfigurationMenuBarModel.h"

ConfigurationMenuBarModel::ConfigurationMenuBarModel(ConfigurationManager& owner) : configurationManager(owner)
{
    setApplicationCommandManagerToWatch(&(configurationManager.getCommandManager()));
}

StringArray ConfigurationMenuBarModel::getMenuBarNames()
{
    return configurationManager.getMenuNames();
}

PopupMenu ConfigurationMenuBarModel::getMenuForIndex (int /*topLevelMenuIndex*/, const String& menuName)
{
    PopupMenu menu;
    configurationManager.createMenu(menu, menuName);
    return menu;
}

void ConfigurationMenuBarModel::menuItemSelected (int menuItemID, int /*topLevelMenuIndex*/)
{
}
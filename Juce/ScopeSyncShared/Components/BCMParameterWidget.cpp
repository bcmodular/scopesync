/**
 * Base class for Component classes that can map to a BCMParameter
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

#include "BCMParameterWidget.h"
#include "../Core/Global.h"
#include "../Core/ScopeSyncGUI.h"

BCMParameterWidget::BCMParameterWidget(ScopeSyncGUI& owner, Component* parent)
    : scopeSyncGUI(owner),
      commandManager(owner.getScopeSync().getCommandManager())
{
    parentComponent = parent;
    commandManager->registerAllCommandsForTarget(this);
}

BCMParameterWidget::~BCMParameterWidget()
{
    commandManager->setFirstCommandTarget(nullptr);
}

void BCMParameterWidget::getAllCommands(Array <CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::deleteItems,
                              CommandIDs::editItem,
                              CommandIDs::editMappedItem
                            };

    commands.addArray (ids, numElementsInArray (ids));
}

void BCMParameterWidget::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::deleteItems:
        result.setInfo("Delete", "Delete selected items", CommandCategories::configmgr, mapsToParameter ? 0 : 1);
        result.defaultKeypresses.add (KeyPress (KeyPress::deleteKey, 0, 0));
        result.defaultKeypresses.add (KeyPress (KeyPress::backspaceKey, 0, 0));
        break;
    case CommandIDs::editItem:
        result.setInfo("Edit Item", "Edit the most recently selected item", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('e', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::editMappedItem:
        result.setInfo("Edit Mapped Item", "Edit item mapped to the most recently selected item", CommandCategories::general, mapsToParameter ? 0 : 1);
        result.defaultKeypresses.add(KeyPress ('e', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    }
}

bool BCMParameterWidget::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::deleteItems:          deleteMapping(); break;
        case CommandIDs::editItem:             editMapping(); break;
        case CommandIDs::editMappedItem:       editMappedParameter(); break;
        default:                               return false;
    }

    return true;
}

ApplicationCommandTarget* BCMParameterWidget::getNextCommandTarget()
{
    return nullptr;
}

void BCMParameterWidget::showPopupMenu()
{
    commandManager->setFirstCommandTarget(this);

    PopupMenu m;
    m.addCommandItem(commandManager, CommandIDs::editItem, "Edit Parameter Mapping");
    m.addCommandItem(commandManager, CommandIDs::editMappedItem, "Edit Mapped Parameter");
    m.addSeparator();
    m.addCommandItem(commandManager, CommandIDs::deleteItems, "Delete Parameter Mapping");
    
    m.showMenuAsync(PopupMenu::Options(), nullptr);  
}

void BCMParameterWidget::setupMapping(const Identifier& componentType,     const String& componentName, 
                                      const Identifier& mappingParentType, const String& mappingParent)
{
    mapsToParameter      = false;
        
    // First try to use a specific mapping set up for this component. Even if it doesn't find a mapped parameter
    // we'll still stick to this mapping, so it can be fixed up later
    mappingComponentType = componentType;
    mappingComponentName = componentName;
    
    parameter = scopeSyncGUI.getUIMapping(Configuration::getMappingParentId(mappingComponentType), mappingComponentName, mapping);

    if (mapping.isValid())
    {
        DBG("BCMParameterWidget::setupMapping - Using direct mapping: " + String(componentType) + "/" + componentName);
        if (parameter != nullptr)
            mapsToParameter = true;

        return;
    }
    
    if (mappingParentType.isValid() && mappingParent.isNotEmpty())
    {
        DBG("BCMParameterWidget::setupMapping - Failing back to mappingParent: " + String(mappingComponentType) + "/" + mappingComponentName);
        // Otherwise fail back to a mappingParent (set in the layout XML)
        mappingComponentType = mappingParentType;
        mappingComponentName = mappingParent;

        parameter = scopeSyncGUI.getUIMapping(Configuration::getMappingParentId(mappingComponentType), mappingComponentName, mapping);

        if (parameter != nullptr)
        {
            mapsToParameter = true;
            return;
        }
    }

    DBG("BCMParameterWidget::setupMapping - No mapping or parent mapping found for component: " + String(componentType) + "/" + componentName);
}

void BCMParameterWidget::deleteMapping()
{
    scopeSyncGUI.getScopeSync().getConfiguration().deleteMapping(mappingComponentType, mapping, nullptr);
    scopeSyncGUI.getScopeSync().applyConfiguration();
}

void BCMParameterWidget::editMapping()
{
    ConfigurationManagerCallout* configurationManagerCallout = new ConfigurationManagerCallout(scopeSyncGUI.getScopeSync(), 400, 34);
    DBG("BCMParameterWidget::editMapping from component: " + parentComponent->getName() + " - " + String(mappingComponentType) + "/" + mappingComponentName);
    configurationManagerCallout->setMappingPanel(mapping, mappingComponentType, mappingComponentName);
    CallOutBox::launchAsynchronously(configurationManagerCallout, parentComponent->getScreenBounds(), nullptr);
}

void BCMParameterWidget::editMappedParameter()
{
    ConfigurationManagerCallout* configurationManagerCallout = new ConfigurationManagerCallout(scopeSyncGUI.getScopeSync(), 550, 700);
    configurationManagerCallout->setParameterPanel(parameter->getDefinition(), parameter->getParameterType());
    CallOutBox::launchAsynchronously(configurationManagerCallout, parentComponent->getScreenBounds(), nullptr);
}

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

BCMParameterWidget::BCMParameterWidget(ApplicationCommandManager* acm) : commandManager(acm)
{
    commandManager->registerAllCommandsForTarget(this);
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

void BCMParameterWidget::showPopup()
{
    PopupMenu m;
    m.addCommandItem(commandManager, CommandIDs::editItem, "Edit Parameter Mapping");
    m.addCommandItem(commandManager, CommandIDs::editMappedItem, "Edit Mapped Parameter");
    m.addSeparator();
    m.addCommandItem(commandManager, CommandIDs::deleteItems, "Delete Parameter Mapping");
    
    m.showMenuAsync(PopupMenu::Options(), nullptr);  
}

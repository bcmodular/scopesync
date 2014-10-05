/*
  ==============================================================================

    BCMParameterWidget.cpp
    Created: 5 Oct 2014 10:25:20am
    Author:  giles

  ==============================================================================
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

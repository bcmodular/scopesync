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

#include "BCMWidget.h"
#include "../Core/Global.h"
#include "../Core/BCMParameter.h"
#include "../Core/ScopeSyncGUI.h"
#include "../Core/ScopeSync.h"
#include "../Configuration/ConfigurationManager.h"
#include "../Properties/WidgetProperties.h"

BCMWidget::BCMWidget(ScopeSyncGUI& owner)
    : scopeSyncGUI(owner), scopeSync(owner.getScopeSync()),
      undoManager(owner.getScopeSync().getUndoManager()), 
      commandManager(owner.getScopeSync().getCommandManager())
{
    commandManager->registerAllCommandsForTarget(this);
}

BCMWidget::~BCMWidget()
{
    commandManager->setFirstCommandTarget(nullptr);
}

void BCMWidget::setParentWidget(Component* parent)
{
    parentWidget = parent;
}

void BCMWidget::applyWidgetProperties(WidgetProperties& properties)
{
    parentWidget->setComponentID(properties.id);

    properties.bounds.copyValues(componentBounds);
    applyBounds();

    bcmLookAndFeelId = properties.bcmLookAndFeelId;
    applyLookAndFeel(properties.noStyleOverride);
}

void BCMWidget::applyBounds()
{
    if (componentBounds.boundsType == BCMComponentBounds::relativeRectangle)
    {
        try
        {
            DBG("BCMWidget::applyBounds - component: " + parentWidget->getComponentID());
            parentWidget->setBounds(componentBounds.relativeRectangleString);
            DBG("BCMWidget::applyBounds - component: " + parentWidget->getComponentID() + ", set relativeRectangle bounds: " + componentBounds.relativeRectangleString);
        }
        catch (Expression::ParseError& error)
        {
            String errorText = "Failed to set RelativeRectangle bounds for component";
            String errorDetailsText = "Component: " + parentWidget->getName() + ", error: " + error.description;
            DBG("BCMWidget::applyBounds - " + errorText + ", " + errorDetailsText);
            scopeSyncGUI.getScopeSync().setSystemError(errorText, errorDetailsText);
            return;
        }
    }
    else if (componentBounds.boundsType == BCMComponentBounds::inset)
        parentWidget->setBoundsInset(componentBounds.borderSize);
    else
    {
        parentWidget->setBounds(
            componentBounds.x,
            componentBounds.y,
            componentBounds.width,
            componentBounds.height
        );
    }
}

void BCMWidget::applyLookAndFeel(bool noStyleOverride)
{
    if (!noStyleOverride)
    {
        styleOverride = scopeSyncGUI.getScopeSync().getConfiguration().getStyleOverride(getComponentType(), parentWidget->getName());
    
        if (styleOverride.isValid())
        {
            String overrideId = styleOverride.getProperty(Ids::lookAndFeelId);
            
            if (overrideId.isNotEmpty())
                bcmLookAndFeelId = overrideId;
        }
    }
    
    BCMLookAndFeel* bcmLookAndFeel = scopeSyncGUI.getScopeSync().getBCMLookAndFeelById(bcmLookAndFeelId);
    
    if (bcmLookAndFeel != nullptr)
        parentWidget->setLookAndFeel(bcmLookAndFeel);
    else
        bcmLookAndFeelId = String::empty;
}

void BCMWidget::getAllCommands(Array<CommandID>& commands)
{
    if (!scopeSync.configurationIsReadOnly())
    {
        const CommandID ids[] = { CommandIDs::overrideStyle,
                                  CommandIDs::clearStyleOverride
                                };

        commands.addArray (ids, numElementsInArray (ids));
    }
    
    String commandIDList = String(commands[0]);

    for (int i = 1; i < commands.size(); i++)
        commandIDList += ", " + String(commands[i]);

    //DBG("BCMWidget::getAllCommands - current array: " + commandIDList);
}    

void BCMWidget::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    //DBG("BCMWidget::getCommandInfo - commandID: " + String(commandID));
    
    switch (commandID)
    {
    case CommandIDs::overrideStyle:
        result.setInfo("Override Style", "Override the style of the most recently selected widget", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('l', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::clearStyleOverride:
        result.setInfo("Clear Style Override", "Clear a style override for the most recently selected widget", CommandCategories::general, !styleOverride.isValid());
        result.defaultKeypresses.add(KeyPress ('l', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    }
}

bool BCMWidget::perform(const InvocationInfo& info)
{
    //DBG("BCMWidget::perform - commandID: " + String(info.commandID));
   
    switch (info.commandID)
    {
        case CommandIDs::overrideStyle:        overrideStyle(); break;
        case CommandIDs::clearStyleOverride:   clearStyleOverride(); break;
        default:                               return false;
    }

    return true;
}

ApplicationCommandTarget* BCMWidget::getNextCommandTarget()
{
    return &scopeSyncGUI;
}

void BCMWidget::showPopupMenu()
{
    commandManager->setFirstCommandTarget(this);

    PopupMenu m;
    m.addSectionHeader(String(getComponentType()) + ": " + parentWidget->getName());
    m.addCommandItem(commandManager, CommandIDs::overrideStyle, "Override Style");
    m.addCommandItem(commandManager, CommandIDs::clearStyleOverride, "Clear Style Override");
    
    m.showMenuAsync(PopupMenu::Options(), nullptr);  
}

void BCMWidget::overrideStyle()
{
    ConfigurationManagerCallout* configurationManagerCallout = new ConfigurationManagerCallout(scopeSyncGUI.getScopeSync(), 550, 34);
    configurationManagerCallout->setStyleOverridePanel(styleOverride, getComponentType(), parentWidget->getName());
    configurationManagerCallout->addChangeListener(this);
    CallOutBox::launchAsynchronously(configurationManagerCallout, parentWidget->getScreenBounds(), nullptr);
}

void BCMWidget::clearStyleOverride()
{
    scopeSyncGUI.getScopeSync().getConfiguration().deleteStyleOverride(getComponentType(), styleOverride, &undoManager);
    scopeSyncGUI.getScopeSync().applyConfiguration();
}

void BCMWidget::changeListenerCallback(ChangeBroadcaster* /* source */)
{
    scopeSyncGUI.getScopeSync().applyConfiguration();
}

BCMParameterWidget::BCMParameterWidget(ScopeSyncGUI& owner) : BCMWidget(owner) 
{
    commandManager->registerAllCommandsForTarget(this);
}

void BCMParameterWidget::getAllCommands(Array<CommandID>& commands)
{
    if (!scopeSync.configurationIsReadOnly())
    {
        const CommandID ids[] = { CommandIDs::deleteItems,
                                  CommandIDs::editItem,
                                  CommandIDs::editMappedItem,
                                  CommandIDs::overrideStyle,
                                  CommandIDs::clearStyleOverride
                                };

        commands.addArray (ids, numElementsInArray (ids));
    }
    
    String commandIDList = String(commands[0]);

    for (int i = 1; i < commands.size(); i++)
        commandIDList += ", " + String(commands[i]);

    //DBG("BCMParameterWidget::getAllCommands - current array: " + commandIDList);
    
}

void BCMParameterWidget::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    //DBG("BCMParameterWidget::getCommandInfo - commandID: " + String(commandID));
    
    switch (commandID)
    {
    case CommandIDs::deleteItems:
        result.setInfo("Delete", "Delete selected items", CommandCategories::configmgr, !mapsToParameter);
        result.defaultKeypresses.add (KeyPress (KeyPress::deleteKey, 0, 0));
        result.defaultKeypresses.add (KeyPress (KeyPress::backspaceKey, 0, 0));
        break;
    case CommandIDs::editItem:
        result.setInfo("Edit Item", "Edit the most recently selected item", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('e', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::editMappedItem:
        result.setInfo("Edit Mapped Item", "Edit item mapped to the most recently selected item", CommandCategories::general, !mapsToParameter);
        result.defaultKeypresses.add(KeyPress ('e', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    default: return BCMWidget::getCommandInfo(commandID, result);
    }
}

bool BCMParameterWidget::perform(const InvocationInfo& info)
{
    //DBG("BCMParameterWidget::perform - commandID: " + String(info.commandID));
   
    switch (info.commandID)
    {
        case CommandIDs::deleteItems:    deleteMapping(); break;
        case CommandIDs::editItem:       editMapping(); break;
        case CommandIDs::editMappedItem: editMappedParameter(); break;
        default:                         BCMWidget::perform(info); break;
    }

    return true;
}

void BCMParameterWidget::showPopupMenu()
{
    commandManager->setFirstCommandTarget(this);

    PopupMenu m;
    m.addSectionHeader(String(getComponentType()) + ": " + parentWidget->getName());
    m.addCommandItem(commandManager, CommandIDs::editItem, "Edit Parameter Mapping");
    m.addCommandItem(commandManager, CommandIDs::editMappedItem, "Edit Mapped Parameter");
    m.addSeparator();
    m.addCommandItem(commandManager, CommandIDs::deleteItems, "Delete Parameter Mapping");
    m.addSeparator();
    m.addCommandItem(commandManager, CommandIDs::overrideStyle, "Override Style");
    m.addCommandItem(commandManager, CommandIDs::clearStyleOverride, "Clear Style Override");
    
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
        // DBG("BCMParameterWidget::setupMapping - Using direct mapping: " + String(componentType) + "/" + componentName);
        if (parameter != nullptr)
            mapsToParameter = true;

        return;
    }
    
    if (mappingParentType.isValid() && mappingParent.isNotEmpty())
    {
        // DBG("BCMParameterWidget::setupMapping - Failing back to mappingParent: " + String(mappingComponentType) + "/" + mappingComponentName);
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

    // DBG("BCMParameterWidget::setupMapping - No mapping or parent mapping found for component: " + String(componentType) + "/" + componentName);
}

void BCMParameterWidget::deleteMapping()
{
    scopeSyncGUI.getScopeSync().getConfiguration().deleteMapping(mappingComponentType, mapping, &undoManager);
    scopeSyncGUI.getScopeSync().applyConfiguration();
}

void BCMParameterWidget::editMapping()
{
    ConfigurationManagerCallout* configurationManagerCallout = new ConfigurationManagerCallout(scopeSyncGUI.getScopeSync(), 400, 34);
    DBG("BCMParameterWidget::editMapping from component: " + parentWidget->getName() + " - " + String(mappingComponentType) + "/" + mappingComponentName);
    configurationManagerCallout->setMappingPanel(mapping, mappingComponentType, mappingComponentName);
    configurationManagerCallout->addChangeListener(this);
    CallOutBox::launchAsynchronously(configurationManagerCallout, parentWidget->getScreenBounds(), nullptr);
}

void BCMParameterWidget::editMappedParameter()
{
    ConfigurationManagerCallout* configurationManagerCallout = new ConfigurationManagerCallout(scopeSyncGUI.getScopeSync(), 550, 700);
    configurationManagerCallout->setParameterPanel(parameter->getDefinition(), parameter->getParameterType());
    configurationManagerCallout->addChangeListener(this);
    CallOutBox::launchAsynchronously(configurationManagerCallout, parentWidget->getScreenBounds(), nullptr);
}

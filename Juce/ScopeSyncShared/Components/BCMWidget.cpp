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

BCMWidget::BCMWidget(ScopeSyncGUI& owner, Component* parent)
    : scopeSyncGUI(owner)
{
    parentBCMComponent = parent;
}

void BCMWidget::applyWidgetProperties(WidgetProperties& properties)
{
    parentBCMComponent->setComponentID(properties.id);

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
            parentBCMComponent->setBounds(componentBounds.relativeRectangleString);
        }
        catch (Expression::ParseError& error)
        {
            scopeSyncGUI.getScopeSync().setSystemError("Failed to set RelativeRectangle bounds for component", "Component: " + parentBCMComponent->getName() + ", error: " + error.description);
            return;
        }
    }
    else if (componentBounds.boundsType == BCMComponentBounds::inset)
        parentBCMComponent->setBoundsInset(componentBounds.borderSize);
    else
    {
        parentBCMComponent->setBounds(
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
        styleOverride = scopeSyncGUI.getScopeSync().getConfiguration().getStyleOverride(getComponentType(), parentBCMComponent->getName());
    
        if (styleOverride.isValid())
        {
            String overrideId = styleOverride.getProperty(Ids::lookAndFeelId);
            
            if (overrideId.isNotEmpty())
                bcmLookAndFeelId = overrideId;
        }
    }
    
    BCMLookAndFeel* bcmLookAndFeel = scopeSyncGUI.getScopeSync().getBCMLookAndFeelById(bcmLookAndFeelId);
    
    if (bcmLookAndFeel != nullptr)
        parentBCMComponent->setLookAndFeel(bcmLookAndFeel);
    else
        bcmLookAndFeelId = String::empty;
}

BCMParameterWidget::BCMParameterWidget(ScopeSyncGUI& owner, Component* parent)
    : BCMWidget(owner, parent),
      commandManager(owner.getScopeSync().getCommandManager())
{
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
                              CommandIDs::editMappedItem,
                              CommandIDs::overrideStyle,
                              CommandIDs::clearStyleOverride
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
    case CommandIDs::overrideStyle:
        result.setInfo("Override Style", "Override the style of the most recently selected widget", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('l', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::clearStyleOverride:
        result.setInfo("Clear Style Override", "Clear a style override for the most recently selected widget", CommandCategories::general, styleOverride.isValid() ? 0 : 1);
        result.defaultKeypresses.add(KeyPress ('l', ModifierKeys::commandModifier, 0));
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
        case CommandIDs::overrideStyle:        overrideStyle(); break;
        case CommandIDs::clearStyleOverride:   clearStyleOverride(); break;
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
    m.addSectionHeader(String(getComponentType()) + ": " + parentBCMComponent->getName());
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
    DBG("BCMParameterWidget::editMapping from component: " + parentBCMComponent->getName() + " - " + String(mappingComponentType) + "/" + mappingComponentName);
    configurationManagerCallout->setMappingPanel(mapping, mappingComponentType, mappingComponentName);
    configurationManagerCallout->addChangeListener(this);
    CallOutBox::launchAsynchronously(configurationManagerCallout, parentBCMComponent->getScreenBounds(), nullptr);
}

void BCMParameterWidget::editMappedParameter()
{
    ConfigurationManagerCallout* configurationManagerCallout = new ConfigurationManagerCallout(scopeSyncGUI.getScopeSync(), 550, 700);
    configurationManagerCallout->setParameterPanel(parameter->getDefinition(), parameter->getParameterType());
    configurationManagerCallout->addChangeListener(this);
    CallOutBox::launchAsynchronously(configurationManagerCallout, parentBCMComponent->getScreenBounds(), nullptr);
}

void BCMParameterWidget::overrideStyle()
{
    ConfigurationManagerCallout* configurationManagerCallout = new ConfigurationManagerCallout(scopeSyncGUI.getScopeSync(), 550, 34);
    configurationManagerCallout->setStyleOverridePanel(styleOverride, getComponentType(), parentBCMComponent->getName());
    configurationManagerCallout->addChangeListener(this);
    CallOutBox::launchAsynchronously(configurationManagerCallout, parentBCMComponent->getScreenBounds(), nullptr);
}

void BCMParameterWidget::clearStyleOverride()
{
    scopeSyncGUI.getScopeSync().getConfiguration().deleteStyleOverride(getComponentType(), styleOverride, nullptr);
    scopeSyncGUI.getScopeSync().applyConfiguration();
}

void BCMParameterWidget::changeListenerCallback(ChangeBroadcaster* /* source */)
{
    scopeSyncGUI.getScopeSync().applyConfiguration();
}

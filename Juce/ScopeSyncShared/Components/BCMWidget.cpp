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
#include "../Core/ScopeSyncApplication.h"

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
    
    widgetTemplateId = properties.widgetTemplateId;
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
                                  CommandIDs::clearStyleOverride,
                                  CommandIDs::copyStyleOverride,
                                  CommandIDs::copyStyleOverrideToAll,
                                  CommandIDs::pasteStyleOverride
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
    case CommandIDs::copyStyleOverride:
        result.setInfo ("Copy Style Override", "Copies a Style Override to the clipboard", CommandCategories::configmgr, !styleOverride.isValid());
        result.defaultKeypresses.add(KeyPress ('c', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    case CommandIDs::copyStyleOverrideToAll:
        result.setInfo ("Copy Style Override To All", "Copies a Style Override to all widgets of this type", CommandCategories::configmgr, !styleOverride.isValid());
        result.defaultKeypresses.add(KeyPress ('c', ModifierKeys::shiftModifier, 0));
        break;
    case CommandIDs::pasteStyleOverride:
        result.setInfo ("Paste Style Override", "Sets up a Style Override with values from the clipboard", CommandCategories::configmgr, !canPasteStyleOverride());
        result.defaultKeypresses.add(KeyPress ('v', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    }
}

bool BCMWidget::perform(const InvocationInfo& info)
{
    //DBG("BCMWidget::perform - commandID: " + String(info.commandID));
   
    switch (info.commandID)
    {
        case CommandIDs::overrideStyle:          overrideStyle(); break;
        case CommandIDs::clearStyleOverride:     clearStyleOverride(); break;
        case CommandIDs::copyStyleOverride:      copyStyleOverride(); break;
        case CommandIDs::copyStyleOverrideToAll: copyStyleOverrideToAll(); break;
        case CommandIDs::pasteStyleOverride:     pasteStyleOverride(); break;
        default:                                 return false;
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

    String headerText = getComponentType();

    if (widgetTemplateId.isNotEmpty())
        headerText += " (" + widgetTemplateId + ")";

    headerText += ": " + parentWidget->getName();

    String copyToText = String(getComponentType()) + "s";

    if (widgetTemplateId.isNotEmpty())
        copyToText = widgetTemplateId + " " + copyToText;

    PopupMenu m;
    m.addSectionHeader(headerText);
    m.addCommandItem(commandManager, CommandIDs::overrideStyle, "Override Style");
    m.addCommandItem(commandManager, CommandIDs::clearStyleOverride, "Clear Style Override");
    m.addSeparator();
    m.addCommandItem(commandManager, CommandIDs::copyStyleOverride, "Copy Style Override");
    m.addCommandItem(commandManager, CommandIDs::pasteStyleOverride, "Paste Style Override");
    m.addCommandItem(commandManager, CommandIDs::copyStyleOverrideToAll, "Copy Style Override to all " + copyToText);
    
    m.showMenuAsync(PopupMenu::Options(), nullptr);  
}

void BCMWidget::overrideStyle()
{
    ConfigurationManagerCallout* configurationManagerCallout = new ConfigurationManagerCallout(scopeSyncGUI.getScopeSync(), 550, 34);
    configurationManagerCallout->setStyleOverridePanel(styleOverride, getComponentType(), parentWidget->getName(), widgetTemplateId);
    configurationManagerCallout->addChangeListener(this);
    CallOutBox::launchAsynchronously(configurationManagerCallout, parentWidget->getScreenBounds(), nullptr);
}

void BCMWidget::clearStyleOverride()
{
    scopeSync.getConfiguration().deleteStyleOverride(getComponentType(), styleOverride, &undoManager);
    scopeSync.applyConfiguration();
}

void BCMWidget::copyStyleOverride()
{
    StyleOverrideClipboard::getInstance()->copy(styleOverride);

}

void BCMWidget::copyStyleOverrideToAll()
{
    copyStyleOverride();
    ValueTree styleOverrideCopy(styleOverride.createCopy());

    scopeSync.getConfiguration().addStyleOverrideToAll(getComponentType(), widgetTemplateId, styleOverrideCopy, &undoManager);
    scopeSyncGUI.getScopeSync().applyConfiguration();
}

void BCMWidget::pasteStyleOverride()
{
    if (!(styleOverride.isValid()))
        scopeSyncGUI.getScopeSync().getConfiguration().addStyleOverride(getComponentType(), 
                                                                        parentWidget->getName(), 
                                                                        widgetTemplateId, 
                                                                        styleOverride, 
                                                                        -1, 
                                                                        &undoManager);
    
    StyleOverrideClipboard::getInstance()->paste(styleOverride, &undoManager);
    scopeSyncGUI.getScopeSync().applyConfiguration();
}

bool BCMWidget::canPasteStyleOverride()
{
    return StyleOverrideClipboard::getInstance()->clipboardIsNotEmpty();
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
        const CommandID ids[] = { CommandIDs::addParameter,
                                  CommandIDs::addParameterFromClipboard,
                                  CommandIDs::deleteMapping,
                                  CommandIDs::editMapping,
                                  CommandIDs::copyParameter,
                                  CommandIDs::pasteParameter,
                                  CommandIDs::editParameter,
                                  CommandIDs::deleteParameter,
                                  CommandIDs::overrideStyle,
                                  CommandIDs::clearStyleOverride,
                                  CommandIDs::copyStyleOverride,
                                  CommandIDs::copyStyleOverrideToAll,
                                  CommandIDs::pasteStyleOverride
                                };

        commands.addArray (ids, numElementsInArray (ids));
    }
}

void BCMParameterWidget::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::addParameter:
        result.setInfo("Add parameter", "Adds a new parameter", CommandCategories::general, mapsToParameter);
        result.defaultKeypresses.add(KeyPress ('n', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::addParameterFromClipboard:
        result.setInfo("Add parameter from clipboard", "Adds a new parameter using the definition stored in the clipboard", CommandCategories::general, !(!mapsToParameter && canPasteParameter()));
        result.defaultKeypresses.add(KeyPress ('n', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    case CommandIDs::deleteMapping:
        result.setInfo("Delete", "Delete mapping", CommandCategories::general, !mapsToParameter);
        result.defaultKeypresses.add (KeyPress (KeyPress::deleteKey, 0, 0));
        result.defaultKeypresses.add (KeyPress (KeyPress::backspaceKey, 0, 0));
        break;
    case CommandIDs::editMapping:
        result.setInfo("Edit Mapping", "Edit mapping", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('e', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::editParameter:
        result.setInfo("Edit Parameter", "Edit parameter mapped to this widget", CommandCategories::general, !mapsToParameter);
        result.defaultKeypresses.add(KeyPress ('e', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    case CommandIDs::deleteParameter:
        result.setInfo("Delete Parameter", "Delete parameter mapped to this widget", CommandCategories::general, !mapsToParameter);
        result.defaultKeypresses.add (KeyPress (KeyPress::deleteKey, ModifierKeys::commandModifier, 0));
        result.defaultKeypresses.add (KeyPress (KeyPress::backspaceKey, ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::copyParameter:
        result.setInfo ("Copy parameter", "Copies a parameter to the clipboard", CommandCategories::general, !mapsToParameter);
        result.defaultKeypresses.add(KeyPress ('c', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::pasteParameter:
        result.setInfo ("Paste parameter", "Overwrites a parameter with values from the clipboard", CommandCategories::general, !canPasteParameter());
        result.defaultKeypresses.add(KeyPress ('v', ModifierKeys::commandModifier, 0));
        break;
    default: return BCMWidget::getCommandInfo(commandID, result);
    }
}

bool BCMParameterWidget::perform(const InvocationInfo& info)
{
    //DBG("BCMParameterWidget::perform - commandID: " + String(info.commandID));
   
    switch (info.commandID)
    {
        case CommandIDs::addParameter:              addParameter(false); break;
        case CommandIDs::addParameterFromClipboard: addParameter(true); break;
        case CommandIDs::deleteMapping:             deleteMapping(); break;
        case CommandIDs::editMapping:               editMapping(); break;
        case CommandIDs::editParameter:             editMappedParameter(); break;
        case CommandIDs::deleteParameter:           deleteMappedParameter(); break;
        case CommandIDs::copyParameter:             copyParameter(); break;
        case CommandIDs::pasteParameter:            pasteParameter(); break;
        default:                                    BCMWidget::perform(info); break;
    }

    return true;
}

void BCMParameterWidget::showPopupMenu()
{
    commandManager->setFirstCommandTarget(this);

    String headerText = getComponentType();

    if (widgetTemplateId.isNotEmpty())
        headerText += " (" + widgetTemplateId + ")";

    headerText += ": " + parentWidget->getName();

    String copyToText = String(getComponentType()) + "s";

    if (widgetTemplateId.isNotEmpty())
        copyToText = widgetTemplateId + " " + copyToText;

    PopupMenu m;
    m.addSectionHeader(headerText);
    m.addCommandItem(commandManager, CommandIDs::addParameter, "Add Parameter");
    m.addCommandItem(commandManager, CommandIDs::addParameterFromClipboard, "Add Parameter from Clipboard");
    m.addCommandItem(commandManager, CommandIDs::editParameter, "Edit Parameter");
    m.addCommandItem(commandManager, CommandIDs::deleteParameter, "Delete Parameter");
    m.addSeparator();
    m.addCommandItem(commandManager, CommandIDs::copyParameter, "Copy Parameter");
    m.addCommandItem(commandManager, CommandIDs::pasteParameter, "Paste Parameter");
    m.addSeparator();
    m.addCommandItem(commandManager, CommandIDs::editMapping, "Edit Mapping");
    m.addCommandItem(commandManager, CommandIDs::deleteMapping, "Delete Mapping");
    m.addSeparator();
    m.addCommandItem(commandManager, CommandIDs::overrideStyle, "Override Style");
    m.addCommandItem(commandManager, CommandIDs::clearStyleOverride, "Clear Style Override");
    m.addSeparator();
    m.addCommandItem(commandManager, CommandIDs::copyStyleOverride, "Copy Style Override");
    m.addCommandItem(commandManager, CommandIDs::pasteStyleOverride, "Paste Style Override");
    m.addCommandItem(commandManager, CommandIDs::copyStyleOverrideToAll, "Copy Style Override to all " + copyToText);
    
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
    scopeSync.getConfiguration().deleteMapping(mappingComponentType, mapping, &undoManager);
    scopeSync.applyConfiguration();
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

void BCMParameterWidget::deleteMappedParameter()
{
    parameter->getDefinition().getParent().removeChild(parameter->getDefinition(), &undoManager);
    deleteMapping();
}

void BCMParameterWidget::copyParameter()
{
    ParameterClipboard::getInstance()->copy(parameter->getDefinition());
}

void BCMParameterWidget::pasteParameter()
{
    ParameterClipboard::getInstance()->paste(parameter->getDefinition(), &undoManager);
    scopeSync.applyConfiguration();
}

bool BCMParameterWidget::canPasteParameter()
{
    return ParameterClipboard::getInstance()->clipboardIsNotEmpty();
}

void BCMParameterWidget::addParameter(bool fromClipboard)
{
    ValueTree newParameter;
    ValueTree definition;

    if (fromClipboard)
    {
        definition = ValueTree(Ids::parameter);
        ParameterClipboard::getInstance()->paste(definition, &undoManager);
    }        

    scopeSync.getConfiguration().addNewParameter(newParameter, definition, -1, Configuration::host, &undoManager);

    ValueTree newMapping;
    String parameterName = newParameter.getProperty(Ids::name);

    scopeSync.getConfiguration().addNewMapping(mappingComponentType, mappingComponentName, parameterName, newMapping, -1, &undoManager);
    scopeSync.applyConfiguration();
}

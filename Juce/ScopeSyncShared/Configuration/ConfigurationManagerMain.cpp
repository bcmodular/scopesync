/*
  ==============================================================================

    ConfigurationManagerMain.cpp
    Created: 14 Sep 2014 9:24:07am
    Author:  giles

  ==============================================================================
*/

#include "ConfigurationManagerMain.h"

#include "../Core/Global.h"
#include "../Core/ScopeSyncApplication.h"
#include "../Utils/BCMMisc.h"
#include "../Core/ScopeSync.h"

ConfigurationManagerMain::ConfigurationManagerMain(ConfigurationManager& owner,
                                                   ScopeSyncGUI& gui) :   configurationManager(owner),
                                                                          scopeSyncGUI(gui),
                                                                          saveButton("Save Configuration"),
                                                                          saveAndCloseButton("Save Configuration and Close"),
                                                                          saveAsButton("Save Configuration As..."),
                                                                          discardChangesButton("Discard All Unsaved Changes"),
                                                                          commandManager(gui.getScopeSync().getCommandManager())
{
    commandManager.registerAllCommandsForTarget(this);
    
    rebuildProperties();
    addAndMakeVisible(propertyPanel);

    fileNameLabel.setText("File path: " + scopeSyncGUI.getScopeSync().getConfigurationFile().getFullPathName(), dontSendNotification);
    fileNameLabel.setTooltip(scopeSyncGUI.getScopeSync().getConfigurationFile().getFullPathName());
    fileNameLabel.setColour(Label::textColourId, Colours::lightgrey);
    fileNameLabel.setMinimumHorizontalScale(1.0f);
    addAndMakeVisible(fileNameLabel);

    saveButton.setCommandToTrigger(&commandManager, CommandIDs::saveConfig, true);
    addAndMakeVisible(saveButton);

    saveAndCloseButton.setCommandToTrigger(&commandManager, CommandIDs::saveAndCloseConfig, true);
    addAndMakeVisible(saveAndCloseButton);

    saveAsButton.setCommandToTrigger(&commandManager, CommandIDs::saveConfigAs, true);
    addAndMakeVisible(saveAsButton);

    discardChangesButton.setCommandToTrigger(&commandManager, CommandIDs::discardConfigChanges, true);
    addAndMakeVisible(discardChangesButton);

    setSize (600, 500);
    
}

ConfigurationManagerMain::~ConfigurationManagerMain() {}

void ConfigurationManagerMain::updateConfigurationFileName()
{
    fileNameLabel.setText(scopeSyncGUI.getScopeSync().getConfigurationFile().getFullPathName(), dontSendNotification);
    fileNameLabel.setTooltip(scopeSyncGUI.getScopeSync().getConfigurationFile().getFullPathName());
}

void ConfigurationManagerMain::getAllCommands (Array <CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::saveConfig,
                              CommandIDs::saveAndCloseConfig,
                              CommandIDs::saveConfigAs,
                              CommandIDs::discardConfigChanges,
                              CommandIDs::closeConfig
                            };

    commands.addArray (ids, numElementsInArray (ids));
}

void ConfigurationManagerMain::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::saveConfig:
        result.setInfo ("Save Configuration", "Save Configuration", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add (KeyPress ('s', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::saveAndCloseConfig:
        result.setInfo ("Save and Close Configuration", "Save Configuration and closes the Configuration Manager popup", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add (KeyPress ('s', ModifierKeys::commandModifier | ModifierKeys::altModifier , 0));
        break;
    case CommandIDs::saveConfigAs:
        result.setInfo ("Save Configuration As...", "Save Configuration as a new file", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add (KeyPress ('s', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
        break;
    case CommandIDs::discardConfigChanges:
        result.setInfo ("Discard Configuration Changes", "Discards all unsaved changes to the current Configuration", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add (KeyPress ('d', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::closeConfig:
        result.setInfo ("Close Configuration Manager", "Closes Configuration Manager window", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add (KeyPress ('q', ModifierKeys::commandModifier, 0));
        break;
    }
}

bool ConfigurationManagerMain::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::saveConfig:           configurationManager.save(); break;
        case CommandIDs::saveAndCloseConfig:   configurationManager.saveAndClose(); break;
        case CommandIDs::saveConfigAs:         configurationManager.saveAs(); break;
        case CommandIDs::discardConfigChanges: configurationManager.discardChanges(); break;
        case CommandIDs::closeConfig:          scopeSyncGUI.hideConfigurationManager(); break;
        default:                        return false;
    }

    return true;
}

ApplicationCommandTarget* ConfigurationManagerMain::getNextCommandTarget()
{
    return nullptr;
}

void ConfigurationManagerMain::rebuildProperties()
{
    PropertyListBuilder props;
    createPropertyEditors(props);

    propertyPanel.addProperties(props.components);
}

void ConfigurationManagerMain::createPropertyEditors(PropertyListBuilder& props)
{
    ValueTree configurationRoot = scopeSyncGUI.getScopeSync().getConfigurationRoot();
    props.add(new TextPropertyComponent(configurationRoot.getPropertyAsValue(Ids::name, nullptr), "Name", 256, false));
}

void ConfigurationManagerMain::resized()
{
    Rectangle<int> parentBounds(getLocalBounds());
    Rectangle<int> toolbar(parentBounds.removeFromTop(30).reduced(1, 1));
    
    saveButton.setBounds(toolbar.removeFromLeft(toolbar.getWidth()/4).reduced(1, 1));
    saveAndCloseButton.setBounds(toolbar.removeFromLeft(toolbar.getWidth()/3).reduced(1, 1));
    saveAsButton.setBounds(toolbar.removeFromLeft(toolbar.getWidth()/2).reduced(1, 1));
    discardChangesButton.setBounds(toolbar.reduced(1, 3));
    
    fileNameLabel.setBounds(parentBounds.removeFromTop(30).reduced(4, 2));
    propertyPanel.setBounds(parentBounds.reduced(4, 2));
    
}

void ConfigurationManagerMain::paint(Graphics& g)
{
    g.fillAll (Colour (0xff434343));
}

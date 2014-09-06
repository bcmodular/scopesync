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

#include "ConfigurationManager.h"
#include "../Core/Global.h"
#include "../Utils/BCMMisc.h"

ConfigurationManager::ConfigurationManager(ScopeSyncGUI& owner) : scopeSyncGUI(owner), saveAndCloseButton("Save Configuration and Close"), saveAsButton("Save Configuration As..."), discardChangesButton("Discard All Unsaved Changes")
{
    rebuildProperties();
    addAndMakeVisible(propertyPanel);

    fileNameLabel.setText("File path: " + scopeSyncGUI.getScopeSync().getConfigurationFile().getFullPathName(), dontSendNotification);
    fileNameLabel.setTooltip(scopeSyncGUI.getScopeSync().getConfigurationFile().getFullPathName());
    fileNameLabel.setColour(Label::textColourId, Colours::lightgrey);
    fileNameLabel.setMinimumHorizontalScale(1.0f);
    addAndMakeVisible(fileNameLabel);

    saveAndCloseButton.addListener(this);
    addAndMakeVisible(saveAndCloseButton);

    saveAsButton.addListener(this);
    addAndMakeVisible(saveAsButton);

    discardChangesButton.addListener(this);
    addAndMakeVisible(discardChangesButton);

    setSize (600, 400);
}

ConfigurationManager::~ConfigurationManager()
{
    scopeSyncGUI.getScopeSync().applyConfiguration();
}

void ConfigurationManager::rebuildProperties()
{
    PropertyListBuilder props;
    createPropertyEditors(props);

    propertyPanel.addProperties(props.components);
}

void ConfigurationManager::createPropertyEditors(PropertyListBuilder& props)
{
    ValueTree configurationRoot = scopeSyncGUI.getScopeSync().getConfigurationRoot();
    props.add(new TextPropertyComponent(configurationRoot.getPropertyAsValue(Ids::name, nullptr), "Name", 256, false));
}

void ConfigurationManager::resized()
{
    Rectangle<int> r (getLocalBounds());
    fileNameLabel.setBounds(r.removeFromTop(30).reduced (4, 2));
    propertyPanel.setBounds(r.removeFromTop(getHeight() - 70).reduced (4, 2));
    saveAndCloseButton.setBounds(r.removeFromLeft(getWidth()/3).reduced(1, 3));
    saveAsButton.setBounds(r.removeFromLeft(getWidth()/3).removeFromRight(getWidth()/2).reduced (1, 3));
    discardChangesButton.setBounds(r.reduced (1, 3));
}

void ConfigurationManager::paint(Graphics& g)
{
    g.fillAll (Colour (0xff434343));
}

void ConfigurationManager::userTriedToCloseWindow()
{
    scopeSyncGUI.hideConfigurationManager();
}

void ConfigurationManager::buttonClicked(Button* buttonThatWasClicked)
{
    if (&saveAndCloseButton == buttonThatWasClicked)
    {
        scopeSyncGUI.getScopeSync().saveConfiguration();
        scopeSyncGUI.hideConfigurationManager();
    }
    else if (&saveAsButton == buttonThatWasClicked)
    {
        File configurationFileDirectory = scopeSyncGUI.getScopeSync().getConfigurationDirectory();
    
        FileChooser fileChooser("Save Configuration File As...",
                                configurationFileDirectory,
                                "*.configuration");
    
        if (fileChooser.browseForFileToSave(true))
        {
            scopeSyncGUI.getScopeSync().saveConfigurationAs(fileChooser.getResult().getFullPathName());
            fileNameLabel.setText(scopeSyncGUI.getScopeSync().getConfigurationFile().getFullPathName(), dontSendNotification);
            fileNameLabel.setTooltip(scopeSyncGUI.getScopeSync().getConfigurationFile().getFullPathName());
        }
    }
    else if (&discardChangesButton == buttonThatWasClicked)
    {
        scopeSyncGUI.getScopeSync().reloadSavedConfiguration();
        scopeSyncGUI.hideConfigurationManager();
    }
}

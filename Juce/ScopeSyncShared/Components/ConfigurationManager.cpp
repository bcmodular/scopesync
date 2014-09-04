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

ConfigurationManager::ConfigurationManager(ScopeSync& owner) : scopeSync(owner), saveButton ("Save")
{
    rebuildProperties();
    addAndMakeVisible(propertyPanel);

    saveButton.addListener(this);
    addAndMakeVisible(saveButton);

    setSize (450, 250);
}

ConfigurationManager::~ConfigurationManager()
{
    scopeSync.applyConfiguration();
}

void ConfigurationManager::rebuildProperties()
{
    PropertyListBuilder props;
    createPropertyEditors(props);

    propertyPanel.addProperties(props.components);
}

void ConfigurationManager::createPropertyEditors(PropertyListBuilder& props)
{
    ValueTree configurationRoot = scopeSync.getConfigurationRoot();
    props.add(new TextPropertyComponent(configurationRoot.getPropertyAsValue(Ids::name, nullptr), "Name", 256, false));
}

void ConfigurationManager::resized()
{
    Rectangle<int> r (getLocalBounds());
    propertyPanel.setBounds(r.removeFromTop (getHeight() - 28).reduced (4, 2));
    saveButton.setBounds(r.reduced (10, 3));
}

void ConfigurationManager::paint(Graphics& g)
{
    g.fillAll (Colour (0xff434343));
}

void ConfigurationManager::buttonClicked(Button* /*buttonThatWasClicked*/)
{
    scopeSync.saveConfiguration();
}



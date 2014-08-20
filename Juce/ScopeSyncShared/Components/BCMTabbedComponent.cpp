/**
 * The BCModular version of Juce's TabbedComponent, which adds the
 * ability to be created from an XML definition
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

#include "BCMTabbedComponent.h"
#include "../Utils/BCMMath.h"
#include "../Core/ScopeSync.h"
#include "../Core/ScopeSyncGUI.h"
#include "../Properties/TabbedComponentProperties.h"

BCMTabbedComponent::BCMTabbedComponent(TabbedButtonBar::Orientation orientation, ScopeSyncGUI& owner) : TabbedComponent(orientation), gui(owner) {}

BCMTabbedComponent::~BCMTabbedComponent() {}

void BCMTabbedComponent::applyProperties(TabbedComponentProperties& properties)
{
    setName(properties.name);
    setComponentID(properties.id);
    
    NamedValueSet& barProperties = getTabbedButtonBar().getProperties();

    setTabBarDepth(properties.tabBarDepth);
    
    mapsToParameter = false;
    
    ValueTree mapping;
    parameter = gui.getUIMapping(ScopeSyncGUI::mappingTabbedComponentId, getName(), mapping);

    if (parameter != nullptr)
    {
        mapsToParameter = true;      
        
        DBG("BCMTabbedComponent::applyProperties - " + getName() + " mapping to parameter: " + parameter->getName());
        parameter->mapToUIValue(parameterValue);
    }

    barProperties.set("showdropshadow", properties.showDropShadow);
    
    properties.bounds.copyValues(componentBounds);
    BCM_SET_BOUNDS
    BCM_SET_LOOK_AND_FEEL
}

void BCMTabbedComponent::valueChanged(Value& value)
{
    setCurrentTabIndex(value.getValue(), true);
}

void BCMTabbedComponent::currentTabChanged(int newCurrentTabIndex, const String& newCurrentTabName)
{
    (void)newCurrentTabName;

    if (parameter != nullptr)
        gui.getScopeSync().setParameterFromGUI(*parameter, (float)newCurrentTabIndex);
}
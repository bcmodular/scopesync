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
#include "../Core/Global.h"

BCMTabbedComponent::BCMTabbedComponent(TabbedButtonBar::Orientation orientation, ScopeSyncGUI& owner)
    : TabbedComponent(orientation), BCMParameterWidget(owner)
{
    setParentWidget(this);
    setWantsKeyboardFocus(true);
}

BCMTabbedComponent::~BCMTabbedComponent() {}

void BCMTabbedComponent::applyProperties(TabbedComponentProperties& properties)
{
    setName(properties.name);
    applyWidgetProperties(properties);
    
    NamedValueSet& barProperties = getTabbedButtonBar().getProperties();

    setTabBarDepth(properties.tabBarDepth);
    
	if (getName().equalsIgnoreCase("CP-Host Connection"))
	{
		parameterValue.addListener(this);
		scopeSync.referToDeviceType(parameterValue);
	}
    else
		setupMapping(Ids::tabbedComponent, getName(), properties.mappingParentType, properties.mappingParent);

    barProperties.set("showdropshadow", properties.showDropShadow);
}

const Identifier BCMTabbedComponent::getComponentType() const { return Ids::tabbedComponent; };

void BCMTabbedComponent::valueChanged(Value& value)
{
    if (getName().equalsIgnoreCase("CP-Host Connection"))
	{
		DBG("BCMTabbedComponent::valueChanged: new value: " + value.toString());

		if (value.getValue())
			setCurrentTabIndex(1, false);
		else
			setCurrentTabIndex(0, false);
	}
	else
	{
		DBG("BCMTabbedComponent::valueChanged: new value: " + String(roundDoubleToInt(value.getValue())));
		setCurrentTabIndex(roundDoubleToInt(value.getValue()), true);
	}	
}

void BCMTabbedComponent::currentTabChanged(int newCurrentTabIndex, const String& newCurrentTabName)
{
    DBG("BCMTabbedComponent::currentTabChanged: new tab: " + String(newCurrentTabIndex) + " - " + newCurrentTabName);
    
    (void)newCurrentTabName;

    if (parameter != nullptr)
        scopeSyncGUI.getScopeSync().setParameterFromGUI(*parameter, (float)newCurrentTabIndex);
}

void BCMTabbedComponent::attachToParameter()
{
	if (getName().equalsIgnoreCase("CP-Host Connection"))
	{
		valueChanged(parameterValue);
	}
	else
	{
		if (mapsToParameter && parameter->isDiscrete())
		{
			parameter->mapToUIValue(parameterValue);
			parameterValue.addListener(this);
		}

		DBG("BCMTabbedComponent::attachToParameter: initialise tab: " + String(roundDoubleToInt(parameterValue.getValue())));
		setCurrentTabIndex(roundDoubleToInt(parameterValue.getValue()), false);
	}
}

void BCMTabbedComponent::popupMenuClickOnTab(int /* tabIndex */, const String& /* tabName */)
{
    showPopupMenu();
}	

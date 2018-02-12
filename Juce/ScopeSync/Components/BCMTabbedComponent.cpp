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
 * the Free Software Foundation, either version 3 of the License, or
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
#include "../Core/ScopeSync.h"
#include "../Core/ScopeSyncGUI.h"
#include "../Properties/TabbedComponentProperties.h"
#include "../Core/Global.h"
#include "../Core/BCMParameterController.h"

BCMTabbedComponent::BCMTabbedComponent(TabbedButtonBar::Orientation orientation, ScopeSyncGUI& owner)
    : TabbedComponent(orientation), BCMParameterWidget(owner)
{
    setParentWidget(this);
    setWantsKeyboardFocus(true);
}

BCMTabbedComponent::~BCMTabbedComponent() {}

void BCMTabbedComponent::applyProperties(TabbedComponentProperties& props)
{
    setName(props.name);
    applyWidgetProperties(props);
    
    NamedValueSet& barProperties = getTabbedButtonBar().getProperties();

    setTabBarDepth(props.tabBarDepth);
    
    if (ScopeSync::fixedParameterNames.contains(getName()))
    {
		BCMParameter* bcmParameter(scopeSync.getParameterController()->getParameterByName(name));

		if (bcmParameter != nullptr)
		{
			setParameter(bcmParameter);
        
			parameterValue.addListener(this);
			getParameter()->mapToUIValue(parameterValue);
		}
	}
    else
		setupMapping(Ids::tabbedComponent, getName(), props.mappingParentType, props.mappingParent);

    barProperties.set("showdropshadow", props.showDropShadow);
}

const Identifier BCMTabbedComponent::getComponentType() const { return Ids::tabbedComponent; };

void BCMTabbedComponent::valueChanged(Value& value)
{
    if (getName().equalsIgnoreCase("Device Type"))
	{
		DBG("BCMTabbedComponent::valueChanged: new value: " + value.toString());

		setCurrentTabIndex(value.getValue(), false);
	}
	else
	{
		DBG("BCMTabbedComponent::valueChanged: new value: " + String(roundToInt(value.getValue())));
		setCurrentTabIndex(roundToInt(value.getValue()), true);
	}	
}

void BCMTabbedComponent::currentTabChanged(int newCurrentTabIndex, const String& newCurrentTabName)
{
    DBG("BCMTabbedComponent::currentTabChanged: new tab: " + String(newCurrentTabIndex) + " - " + newCurrentTabName);
    
    (void)newCurrentTabName;

    if (parameter != nullptr)
        scopeSync.getParameterController()->setParameterFromGUI(*parameter, static_cast<float>(newCurrentTabIndex));
}

void BCMTabbedComponent::attachToParameter()
{
	if (getName().equalsIgnoreCase("Device Type"))
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

		DBG("BCMTabbedComponent::attachToParameter: initialise tab: " + String(roundToInt(parameterValue.getValue())));
		setCurrentTabIndex(roundToInt(parameterValue.getValue()), false);
	}
}

void BCMTabbedComponent::popupMenuClickOnTab(int /* tabIndex */, const String& /* tabName */)
{
    showPopupMenu();
}	

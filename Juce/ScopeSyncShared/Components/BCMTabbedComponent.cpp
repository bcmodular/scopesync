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

BCMTabbedComponent::BCMTabbedComponent(TabbedButtonBar::Orientation orientation) : TabbedComponent(orientation) {}

BCMTabbedComponent::~BCMTabbedComponent() {}

void BCMTabbedComponent::applyProperties(TabbedComponentProperties& properties, ScopeSyncGUI& gui)
{
    setComponentID(properties.id);
    
    TabbedButtonBar& bar           = getTabbedButtonBar();
    NamedValueSet&   barProperties = bar.getProperties();

    setTabBarDepth(properties.tabBarDepth);
    
    setLookAndFeel(gui.getScopeSync().getBCMLookAndFeelById(properties.bcmLookAndFeelId));
    bar.setLookAndFeel(gui.getScopeSync().getBCMLookAndFeelById(properties.bcmLookAndFeelId));
    
    setName(properties.name);

    barProperties.set("showdropshadow", properties.showDropShadow);
    
    componentBounds = properties.bounds;
    BCM_SET_BOUNDS
    BCM_SET_LOOK_AND_FEEL
}

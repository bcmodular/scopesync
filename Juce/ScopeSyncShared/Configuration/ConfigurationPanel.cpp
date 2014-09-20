/**
 * Classes for displaying edit panels for the various Configuration
 * TreeViewItems
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

#include "ConfigurationPanel.h"
#include "../Core/Global.h"
#include "../Core/ScopeSyncApplication.h"
#include "../Core/ScopeSync.h"

/* =========================================================================
 * PropertyListBuilder
 */
void PropertyListBuilder::add(PropertyComponent* propertyComp)
{
    components.add (propertyComp);
}

void PropertyListBuilder::add(PropertyComponent* propertyComp, const String& tooltip)
{
    propertyComp->setTooltip (tooltip);
    add (propertyComp);
}

/* =========================================================================
 * ParameterPanel
 */
ParameterPanel::ParameterPanel(ValueTree& parameter) : valueTree(parameter)
{
    rebuildProperties();
    addAndMakeVisible(propertyPanel);
       
    setSize(getLocalBounds().getWidth(), getLocalBounds().getHeight());
}

ParameterPanel::~ParameterPanel() {}

void ParameterPanel::rebuildProperties()
{
    PropertyListBuilder props;
    createPropertyEditors(props);

    propertyPanel.addProperties(props.components);
}

void ParameterPanel::createPropertyEditors(PropertyListBuilder& props)
{
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::name, nullptr),             "Name", 256, false),              "Mapping name for parameter");
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::shortDescription, nullptr), "Short Description", 256, false), "Short Description of parameter");
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::fullDescription, nullptr),  "Full Description", 256, false),  "Full Description of parameter");

    int scopeMin = 0x80000000;
    int scopeMax = 0x7FFFFFFF;
    double scopeMinDbl = scopeMin;
    double scopeMaxDbl = scopeMax;

    //props.add(new SliderPropertyComponent(valueTree.getPropertyAsValue(Ids::scopeRangeMin, nullptr), "Min Scope Value", scopeMinDbl, scopeMaxDbl, 1.0));
    //props.add(new SliderPropertyComponent(valueTree.getPropertyAsValue(Ids::scopeRangeMax, nullptr), "Max Scope Value", scopeMinDbl, scopeMaxDbl, 1.0));
}

void ParameterPanel::resized()
{
    Rectangle<int> localBounds(getLocalBounds());

    propertyPanel.setBounds(getLocalBounds().reduced(4, 2));
}

void ParameterPanel::paint(Graphics& g)
{
    g.fillAll (Colour (0xff434343));
}
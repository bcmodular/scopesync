/**
 * The BCModular version of Juce's Label, which adds the ability
 * to be created from an XML definition, as well as being tied into
 * the ScopeSync parameter system
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

#include "BCMLabel.h"
#include "../Utils/BCMMath.h"
#include "../Components/BCMLookAndFeel.h"
#include "../Core/ScopeSync.h"
#include "../Core/ScopeSyncGUI.h"
#include "../Properties/LabelProperties.h"
#include "../Core/Global.h"
#include "../Configuration/ConfigurationManager.h"

BCMLabel::BCMLabel(String& name, String& text, ScopeSyncGUI& owner)
    : Label(name, text), BCMParameterWidget(owner)
{
    setParentWidget(this);
    setWantsKeyboardFocus(true);
}

BCMLabel::~BCMLabel() {}

void BCMLabel::applyProperties(LabelProperties& props) 
{
    applyWidgetProperties(props);
    
	maxTextLines = props.maxTextLines;

    String labelText = getText();
    String tooltip   = getText();
    
    mapsToParameter = false;
    
    if (getName().equalsIgnoreCase("configurationfilepath"))
        labelText = scopeSyncGUI.getScopeSync().getConfigurationFile().getFullPathName();
    else if (getName().equalsIgnoreCase("configurationname"))
        labelText = scopeSyncGUI.getScopeSync().getConfigurationName(true);
    else
    {
        setupMapping(Ids::label, getName(), props.mappingParentType, props.mappingParent);

        if (mapsToParameter)
        {
            String shortDescription;
            String fullDescription;
            
            parameter->getDescriptions(shortDescription, fullDescription);
            tooltip = fullDescription;
            
            if (props.parameterTextDisplay == LabelProperties::parameterName)
                labelText = parameter->getName();
            else if (props.parameterTextDisplay == LabelProperties::shortDescription)
                labelText = shortDescription;
            else if (props.parameterTextDisplay == LabelProperties::fullDescription)
                labelText = fullDescription;
            else if (props.parameterTextDisplay == LabelProperties::scopeCode)
                labelText = parameter->getScopeCode();
        }
    }

    // Only relevant if label is editable. Not currently supported
    setEditable(false, false, false);
      
    setText(labelText, dontSendNotification);
    setTooltip(tooltip);
    
    setFont(Font(props.fontHeight, props.fontStyleFlags));
    setJustificationType(Justification(props.justificationFlags));
}

const Identifier BCMLabel::getComponentType() const { return Ids::label; };

void BCMLabel::mouseDown(const MouseEvent& event)
{
    if (event.mods.isPopupMenu())
    {
        grabKeyboardFocus();
        showPopupMenu();
    }
    else
    {
        Label::mouseDown(event);
    }
}

void BCMLabel::applyLookAndFeel(bool noStyleOverride)
{
    //DBG("BCMLabel::applyLookAndFeel - " + getName());

    BCMWidget::applyLookAndFeel(noStyleOverride);

    bool useColourOverrides = styleOverride.getProperty(Ids::useColourOverrides);

    if (!noStyleOverride && useColourOverrides)
    {
        String fillColourString = styleOverride.getProperty(Ids::fillColour);
        String lineColourString = styleOverride.getProperty(Ids::lineColour);

        if (fillColourString.isNotEmpty())
        {
            Colour fillColour(Colour::fromString(fillColourString));
            Colour lineColour(Colour::fromString(lineColourString));

            setColour(Label::backgroundColourId, fillColour);
            setColour(Label::textColourId, lineColour);
        }
    }
}

void BCMLabel::overrideStyle()
{
    String fillColourString = findColour(Label::backgroundColourId).toString();
    String lineColourString = findColour(Label::textColourId).toString();
    
    ConfigurationManagerCalloutWindow* configurationManagerCalloutWindow = new ConfigurationManagerCalloutWindow(scopeSyncGUI.getScopeSync(), 550, 115);
    configurationManagerCalloutWindow->setStyleOverridePanel(styleOverride, Ids::label, getName(), widgetTemplateId, fillColourString, lineColourString);
    configurationManagerCalloutWindow->addChangeListener(this);
    CallOutBox::launchAsynchronously(configurationManagerCalloutWindow, getScreenBounds(), nullptr);
}

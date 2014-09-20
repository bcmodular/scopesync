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
    components.add(propertyComp);
}

void PropertyListBuilder::add(PropertyComponent* propertyComp, const String& tooltip)
{
    propertyComp->setTooltip (tooltip);
    add(propertyComp);
}

void PropertyListBuilder::clear()
{
    components.clear();
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
    createDescriptionProperties(props);
    propertyPanel.addProperties(props.components);

    createScopeProperties(props);
    propertyPanel.addSection("Scope Properties", props.components, true);
}

void ParameterPanel::createDescriptionProperties(PropertyListBuilder& props)
{
    props.clear();
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::name, nullptr),             "Name", 256, false),              "Mapping name for parameter");
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::shortDescription, nullptr), "Short Description", 256, false), "Short Description of parameter");
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::fullDescription, nullptr),  "Full Description", 256, false),  "Full Description of parameter");
}

void ParameterPanel::createScopeProperties(PropertyListBuilder& props)
{
    props.clear();
    props.add(new IntRangeProperty(valueTree.getPropertyAsValue(Ids::scopeRangeMin, nullptr), "Min Scope Value", 30), "Minimum Scope Integer Value");
    props.add(new IntRangeProperty(valueTree.getPropertyAsValue(Ids::scopeRangeMax, nullptr), "Max Scope Value", 30), "Maximum Scope Integer Value");
}

void ParameterPanel::resized()
{
    Rectangle<int> localBounds(getLocalBounds());

    propertyPanel.setBounds(getLocalBounds().reduced(4, 2));
}

void ParameterPanel::paint(Graphics& g)
{
    g.fillAll (Colours::lightgrey);
}

class IntRangeProperty::LabelComp  : public Label
{
public:
    LabelComp (IntRangeProperty& irp, const int charLimit)
        : Label (String::empty, String::empty),
          owner (irp),
          maxChars (charLimit)
    {
        setEditable(true, true, false);

        setColour(backgroundColourId, owner.findColour (IntRangeProperty::backgroundColourId));
        setColour(outlineColourId,    owner.findColour (IntRangeProperty::outlineColourId));
        setColour(textColourId,       owner.findColour (IntRangeProperty::textColourId));
    }

    TextEditor* createEditorComponent() override
    {
        TextEditor* const ed = Label::createEditorComponent();
        ed->setInputRestrictions (maxChars);

        return ed;
    }

    void textWasEdited() override
    {
        owner.textWasEdited();
    }

private:
    IntRangeProperty& owner;
    int maxChars;
};

IntRangeProperty::IntRangeProperty (const Value&  valueToControl,
                                    const String& propertyName,
                                    int           maxNumChars,
                                    const int     minInt,
                                    const int     maxInt)
    : PropertyComponent(propertyName), minValue(minInt), maxValue(maxInt)
{
    addAndMakeVisible(textEditor = new LabelComp(*this, maxNumChars));
    textEditor->getTextValue().referTo(valueToControl);
}

IntRangeProperty::~IntRangeProperty() {}

void IntRangeProperty::setText(const String& newText)
{
    const int newInt = jlimit<int>(minValue, maxValue, newText.getIntValue());

    textEditor->setText(String(newInt), sendNotificationSync);
}

String IntRangeProperty::getText() const
{
    return textEditor->getText();
}

void IntRangeProperty::refresh()
{
    textEditor->setText(getText(), dontSendNotification);
}

void IntRangeProperty::textWasEdited()
{
    setText(textEditor->getText());
}

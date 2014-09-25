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
#include "ConfigurationManagerMain.h"
#include "SettingsTable.h"
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
 * BasePanel
 */
BasePanel::BasePanel(ValueTree& parameter, UndoManager& um, ConfigurationManagerMain& cmm)
    : valueTree(parameter), undoManager(um), configurationManagerMain(cmm)
{
    addAndMakeVisible(propertyPanel);
   
    setSize(getLocalBounds().getWidth(), getLocalBounds().getHeight());
}

BasePanel::~BasePanel() {}

void BasePanel::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    
    propertyPanel.setBounds(localBounds.reduced(4, 2));
}

void BasePanel::paint(Graphics& g)
{
    g.fillAll (Colours::lightgrey);
}

/* =========================================================================
 * ConfigurationPanel
 */
ConfigurationPanel::ConfigurationPanel(ValueTree& parameter, UndoManager& um, ConfigurationManagerMain& cmm)
    : BasePanel(parameter, um, cmm)
{
    rebuildProperties();
}

ConfigurationPanel::~ConfigurationPanel() {}

void ConfigurationPanel::rebuildProperties()
{
    PropertyListBuilder props;

    props.clear();
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::name, &undoManager),           "Name",        256, false), "Name of Configuration");
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::layoutFilename, &undoManager), "Layout File", 256, false), "Relative path to Layout File");

    propertyPanel.addProperties(props.components);
}

/* =========================================================================
 * ParameterPanel
 */
ParameterPanel::ParameterPanel(ValueTree& parameter, UndoManager& um, 
                               ParameterType paramType, ConfigurationManagerMain& cmm)
    : BasePanel(parameter, um, cmm), parameterType(paramType)
{
    rebuildProperties();

    valueType = valueTree.getPropertyAsValue(Ids::valueType, &undoManager);
    valueType.addListener(this);

    if (int(valueType.getValue()) == 1)
    {
        ValueTree settings = valueTree.getOrCreateChildWithName(Ids::settings, &undoManager);
        addAndMakeVisible(settingsTable = new SettingsTable(settings, undoManager, configurationManagerMain, valueTree));
    }
}

ParameterPanel::~ParameterPanel() {}

void ParameterPanel::rebuildProperties()
{
    PropertyListBuilder props;
    createDescriptionProperties(props);
    propertyPanel.addProperties(props.components);

    createScopeProperties(props);
    propertyPanel.addSection("Scope Properties", props.components, true);
    
    createUIProperties(props);
    propertyPanel.addSection("UI Properties", props.components, true);
}

void ParameterPanel::createDescriptionProperties(PropertyListBuilder& props)
{
    props.clear();
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::name, &undoManager),             "Name", 256, false),              "Mapping name for parameter");
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::shortDescription, &undoManager), "Short Description", 256, false), "Short Description of parameter");
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::fullDescription, &undoManager),  "Full Description", 256, false),  "Full Description of parameter");

    if (parameterType == hostParameter)
    {
        StringArray scopeSyncCodes = ScopeSync::getScopeSyncCodes();
        Array<var>  scopeSyncValues;

        for (int i = 0; i < scopeSyncCodes.size(); i++)
            scopeSyncValues.add(i);

        props.add(new ChoicePropertyComponent(valueTree.getPropertyAsValue(Ids::scopeSync, &undoManager), "ScopeSync Code", scopeSyncCodes, scopeSyncValues), "ScopeSync Code");
    }
    else if (parameterType == scopeLocal)
    {
        StringArray scopeLocalCodes = ScopeSync::getScopeLocalCodes();
        Array<var>  scopeLocalValues;

        for (int i = 0; i < scopeLocalCodes.size(); i++)
            scopeLocalValues.add(i);

        props.add(new ChoicePropertyComponent(valueTree.getPropertyAsValue(Ids::scopeLocal, &undoManager), "ScopeLocal Code", scopeLocalCodes, scopeLocalValues), "ScopeLocal Code");
    }
}

void ParameterPanel::createScopeProperties(PropertyListBuilder& props)
{
    props.clear();
    props.add(new IntRangeProperty        (valueTree.getPropertyAsValue(Ids::scopeRangeMin, &undoManager), "Min Scope Value"),             "Minimum Scope Value (Integer)");
    props.add(new IntRangeProperty        (valueTree.getPropertyAsValue(Ids::scopeRangeMax, &undoManager), "Max Scope Value"),             "Maximum Scope Value (Integer)");
    props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::scopeDBRef,    &undoManager), "Scope dB Reference"),          "Scope dB Reference Value (only set for dB-based parameters)");
    props.add(new BooleanPropertyComponent(valueTree.getPropertyAsValue(Ids::skewUIOnly,    &undoManager), "Skew UI Only", String::empty), "Only apply the Skew factor to the UI elements, not Scope values");
}

void ParameterPanel::createUIProperties(PropertyListBuilder& props)
{
    props.clear();
    props.add(new TextPropertyComponent   (valueTree.getPropertyAsValue(Ids::uiSuffix,        &undoManager), "UI Suffix", 32, false),  "Text to display after value in the User Interface, e.g. Hz, %");
    props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::uiRangeMin,      &undoManager), "Min UI Value"),          "Minimum User Interface Value (Float)");
    props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::uiRangeMax,      &undoManager), "Max UI Value"),          "Maximum User Interface Value (Float)");
    props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::uiRangeInterval, &undoManager), "Value Interval"),        "Step between consecutive User Interface values");
    props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::uiResetValue,    &undoManager), "Reset Value"),           "Value to reset parameter to on double-click or initialisation");
    props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::uiSkewMidpoint,  &undoManager), "Mid-point Value", true), "Value that should be at the mid-point of the UI range (leave blank if no skew required)");
    props.add(new BooleanPropertyComponent(valueTree.getPropertyAsValue(Ids::valueType,       &undoManager), "Use discrete values", String::empty), "Use a set of discrete parameter values relating to specific control settings");
}

void ParameterPanel::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    
    if (settingsTable != nullptr)
        settingsTable->setBounds(localBounds.removeFromBottom(localBounds.getHeight() / 3).reduced(4, 2));

    propertyPanel.setBounds(localBounds.reduced(4, 2));
}

void ParameterPanel::valueChanged(Value& valueThatChanged)
{
    if (int(valueThatChanged.getValue()) == 0)
    {
        settingsTable = nullptr;
    }
    else
    {
        ValueTree settings = valueTree.getOrCreateChildWithName(Ids::settings, &undoManager);
        addAndMakeVisible(settingsTable = new SettingsTable(settings, undoManager, configurationManagerMain, valueTree));
        resized();
    }
}

/* =========================================================================
 * NumericProperty
 */
class NumericProperty::LabelComp  : public Label
{
public:
    LabelComp (NumericProperty& np, const String& validInputString)
        : Label (String::empty, String::empty),
          owner(np), validInput(validInputString)
    {
        setEditable(true, true, false);

        setColour(backgroundColourId, owner.findColour(NumericProperty::backgroundColourId));
        setColour(outlineColourId,    owner.findColour(NumericProperty::outlineColourId));
        setColour(textColourId,       owner.findColour(NumericProperty::textColourId));
    }

    TextEditor* createEditorComponent() override
    {
        TextEditor* const ed = Label::createEditorComponent();
        ed->setInputRestrictions(32, validInput);

        return ed;
    }

    void setError(const String& errorText)
    {
        setColour(backgroundColourId, Colours::lightpink);
        setTooltip(errorText);
    }

    void clearError()
    {
        setColour(backgroundColourId, owner.findColour(NumericProperty::backgroundColourId));
        setTooltip(String::empty);
    }

private:
    NumericProperty& owner;
    int maxChars;
    String validInput;
};

NumericProperty::NumericProperty(const Value&  valueToControl,
                                 const String& propertyName,
                                 const String& validInputString)
    : PropertyComponent(propertyName)
{
    addAndMakeVisible(textEditor = new LabelComp(*this, validInputString));
    textEditor->getTextValue().referTo(valueToControl);
    textEditor->addListener(this);
}

NumericProperty::~NumericProperty()
{
    textEditor->removeListener(this);
}

void NumericProperty::setText(const String& newText)
{
    textEditor->setText(newText, sendNotificationSync);
}

String NumericProperty::getText() const
{
    return textEditor->getText();
}

void NumericProperty::refresh()
{
    textEditor->setText(getText(), dontSendNotification);
}

void NumericProperty::labelTextChanged(Label* /* labelThatHasChanged */)
{
    setText(textEditor->getText());
}

/* =========================================================================
 * IntRangeProperty
 */
IntRangeProperty::IntRangeProperty (const Value&  valueToControl,
                                    const String& propertyName,
                                    const int     minInt,
                                    const int     maxInt)
    : NumericProperty(valueToControl, propertyName, "-0123456789"),
      minValue(minInt), 
      maxValue(maxInt)
{
}

IntRangeProperty::~IntRangeProperty() {}

void IntRangeProperty::labelTextChanged(Label* labelThatHasChanged)
{
    String errorText;
    String newText = String(labelThatHasChanged->getText().getIntValue());

    const int newInt = newText.getIntValue();
    
    if (newText.isEmpty())
        errorText = "Must provide a valid integer";
    else if (newInt < minValue)
        errorText = "Value less than minimum";
    else if (newInt > maxValue)
        errorText = "Value greater than maximum";

    if (errorText.isNotEmpty())
        textEditor->setError(errorText);
    else
        textEditor->clearError();

    NumericProperty::setText(newText);
}

/* =========================================================================
 * FltRangeProperty
 */
FltProperty::FltProperty (const Value&  valueToControl,
                          const String& propertyName,
                          const bool    allowBlank)
    : NumericProperty(valueToControl, propertyName, "-0123456789."),
      allowedToBeBlank(allowBlank)
{
}

FltProperty::~FltProperty() {}

void FltProperty::labelTextChanged(Label* labelThatHasChanged)
{
    String newText = labelThatHasChanged->getText();
    String errorText;
    
    if (newText.isNotEmpty())
        newText = String(newText.getDoubleValue());
    else if (!allowedToBeBlank)
        errorText = "Must provide a valid floating point value";
    
    if (errorText.isNotEmpty())
        textEditor->setError(errorText);
    else
        textEditor->clearError();

    NumericProperty::setText(newText);
}

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
 * SettingsTable
 */
class SettingsTable::LabelComp : public Component
{
public:
    LabelComp(Value& valueToEdit)
        : label(String::empty, String::empty),
          value(valueToEdit)
    {
        addAndMakeVisible(label);
        label.getTextValue().referTo(value);
        label.setEditable(true, true, false);  
    }

    void resized() override
    {
        label.setBoundsInset(BorderSize<int> (2));
    }

    void setLabelValue(Value& valueToReferTo)
    {
        label.getTextValue().referTo(valueToReferTo);
    }

private:
    Label label;
    Value value;
    int maxChars;
};

SettingsTable::SettingsTable(const ValueTree& valueTree, UndoManager& um)
    : tree(valueTree), undoManager(um), font(14.0f)
{
    addAndMakeVisible (table);
    
    table.setModel(this);
    
    table.setColour(ListBox::outlineColourId, Colours::grey);
    table.setOutlineThickness (1);
    
    table.getHeader().addColumn(String::empty,     1, 30,  30, 30,  TableHeaderComponent::notResizableOrSortable);
    table.getHeader().addColumn("Name",            2, 150, 50, 400, TableHeaderComponent::defaultFlags);
    table.getHeader().addColumn("Scope Int Value", 3, 150, 50, 400, TableHeaderComponent::defaultFlags);
    
    table.getHeader().setStretchToFitActive(true);
    table.setMultipleSelectionEnabled (true);
    
    //table.updateContent();

    tree.addListener(this);
}

SettingsTable::~SettingsTable()
{
    tree.removeListener(this);
}

void SettingsTable::resized()
{
    table.setBoundsInset(BorderSize<int> (8));
}
    
int SettingsTable::getNumRows()
{
    return tree.getNumChildren();
}

Component* SettingsTable::refreshComponentForCell(int rowNumber, int columnId, bool /* isRowSelected */, Component* existingComponentToUpdate)
{
    Identifier propertyId;

    if (columnId == 2)
    {
        propertyId = Ids::name;
    }
    else if (columnId == 3)
    {
        propertyId = Ids::intValue;
    }
    else
    {
        return nullptr;
    }
    
    Value valueToEdit(tree.getChild(rowNumber).getPropertyAsValue(propertyId, &undoManager));
    
    LabelComp* labelComp = (LabelComp*)existingComponentToUpdate;

    if (labelComp == nullptr)
        labelComp = new LabelComp(valueToEdit);
    else
        labelComp->setLabelValue(valueToEdit);
    
    return labelComp;
}

void SettingsTable::sortOrderChanged(int newSortColumnId, bool isForwards)
{
    (void)newSortColumnId;
    (void)isForwards;
}

void SettingsTable::selectedRowsChanged(int lastRowSelected)
{
    (void)lastRowSelected;
}

void SettingsTable::paintRowBackground(Graphics& g, int /* rowNumber */, int /* width */, int /* height */, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll (findColour (TextEditor::highlightColourId));
}

void SettingsTable::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool /* rowIsSelected */)
{
    g.setColour(Colours::black);
    g.setFont(font);

    if (columnId == 1)
    {
        const String text(rowNumber + 1);

        g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);
    }

    g.setColour(Colours::black.withAlpha(0.2f));
    g.fillRect(width - 1, 0, 1, height);
}

void SettingsTable::backgroundClicked(const MouseEvent&)
{
    table.deselectAllRows();
}

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
ParameterPanel::ParameterPanel(ValueTree& parameter, UndoManager& um, ParameterType paramType)
    : valueTree(parameter), undoManager(um), parameterType(paramType)
{
    rebuildProperties();
    addAndMakeVisible(propertyPanel);

    ValueTree settings = valueTree.getChildWithName(Ids::settings);
    addAndMakeVisible(settingsTable = new SettingsTable(settings, undoManager));
       
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
    props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::scopeDBRef,    &undoManager), "Scope dB Reference"),          "Scope dB Reference Value (only set for dB-based parameters");
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
    props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::uiSkewMidpoint,  &undoManager), "Mid-point Value", true), "Value that should be at the mid-point of the UI range (leave blank if no skew required");
}

void ParameterPanel::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    
    settingsTable->setBounds(localBounds.removeFromBottom(localBounds.getHeight() / 3).reduced(4, 2));

    propertyPanel.setBounds(localBounds.reduced(4, 2));
}

void ParameterPanel::paint(Graphics& g)
{
    g.fillAll (Colours::lightgrey);
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
    String newText = String(labelThatHasChanged->getText().getDoubleValue());
    
    String errorText;
    
    if ((newText.isEmpty() && !allowedToBeBlank))
        errorText = "Must provide a valid floating point value";
    
    if (errorText.isNotEmpty())
        textEditor->setError(errorText);
    else
        textEditor->clearError();

    NumericProperty::setText(newText);
}

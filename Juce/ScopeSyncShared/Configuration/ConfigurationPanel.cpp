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
    setWantsKeyboardFocus(true);
    propertyPanel.setWantsKeyboardFocus(true);
    setSize(getLocalBounds().getWidth(), getLocalBounds().getHeight());
}

BasePanel::~BasePanel() {}

void BasePanel::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    
    localBounds.removeFromLeft(4);
    localBounds.removeFromRight(8);
    localBounds.removeFromTop(8);

    propertyPanel.setBounds(localBounds);
}

void BasePanel::paint(Graphics& g)
{
    g.fillAll (Colour(0xff434343));

    g.setColour(Colours::lightgrey);

    Rectangle<int> localBounds(getLocalBounds());
    
    localBounds.removeFromLeft(4);
    localBounds.removeFromRight(8);
    localBounds.removeFromTop(8);
    g.fillRect(localBounds);
}

void BasePanel::focusGained(FocusChangeType /* cause */)
{
    propertyPanel.grabKeyboardFocus();
}

/* =========================================================================
 * EmptyPanel
 */
EmptyPanel::EmptyPanel(ValueTree& parameter, UndoManager& um, ConfigurationManagerMain& cmm)
    : BasePanel(parameter, um, cmm)
{
    propertyPanel.setVisible(false);
}

EmptyPanel::~EmptyPanel() {}

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
        createSettingsTable();
    }
}

ParameterPanel::~ParameterPanel()
{
    valueType.removeListener(this);
}

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

void ParameterPanel::childBoundsChanged(Component* child)
{
    if (child == settingsTable)
        resized();
}

void ParameterPanel::paintOverChildren(Graphics& g)
{
    if (settingsTable != nullptr)
    {
        const int shadowSize = 15;
    
        const int resizerY = resizerBar->getY();

        ColourGradient resizerCG (Colours::black.withAlpha (0.25f), 0, (float) resizerY, 
                                  Colours::transparentBlack,        0, (float) (resizerY + shadowSize), false);
        resizerCG.addColour (0.4, Colours::black.withAlpha (0.07f));
        resizerCG.addColour (0.6, Colours::black.withAlpha (0.02f));

        g.setGradientFill(resizerCG);
        g.fillRect(resizerBar->getX(), resizerY, resizerBar->getWidth(), shadowSize);
    }
}


void ParameterPanel::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    
    localBounds.removeFromLeft(4);
    localBounds.removeFromRight(8);
    localBounds.removeFromTop(8);

    if (settingsTable != nullptr)
    {
        settingsTable->setBounds(localBounds.removeFromBottom(settingsTable->getHeight()));
        resizerBar->setBounds(localBounds.removeFromBottom(4));
    }

    propertyPanel.setBounds(localBounds);
}

void ParameterPanel::valueChanged(Value& valueThatChanged)
{
    if (int(valueThatChanged.getValue()) == 0)
    {
        settingsTable = nullptr;
        resized();
    }
    else
    {
        createSettingsTable();
        resized();
    }
}

void ParameterPanel::createSettingsTable()
{
    settingsTableConstrainer.setMinimumHeight(200);
    settingsTableConstrainer.setMaximumHeight(700);

    ValueTree settings = valueTree.getOrCreateChildWithName(Ids::settings, &undoManager);
    settingsTable = new SettingsTable(settings, undoManager, configurationManagerMain, valueTree);

    int lastSettingsTableHeight = configurationManagerMain.getConfiguration().getConfigurationProperties().getIntValue("lastSettingsTableHeight", 250);
    settingsTable->setBounds(0, 0, getWidth(), lastSettingsTableHeight);
    addAndMakeVisible(settingsTable);

    addAndMakeVisible(resizerBar = new ResizableEdgeComponent(settingsTable, &settingsTableConstrainer,
                                                                ResizableEdgeComponent::topEdge));
    resizerBar->setAlwaysOnTop (true);
}

/* =========================================================================
 * MappingPanel
 */
MappingPanel::MappingPanel(ValueTree& parameter, UndoManager& um, ConfigurationManagerMain& cmm, const String& compType)
    : BasePanel(parameter, um, cmm), componentType(compType)
{
    rebuildProperties();
}

MappingPanel::~MappingPanel() {}

void MappingPanel::rebuildProperties()
{
    PropertyListBuilder props;

    props.clear();

    // Set up Component Names
    StringArray componentNames = configurationManagerMain.getConfiguration().getComponentNames(componentType);
    Array<var>  componentValues;

    for (int i = 0; i < componentNames.size(); i++) componentValues.add(componentNames[i]);

    props.add(new ChoicePropertyComponent(valueTree.getPropertyAsValue(Ids::name, &undoManager), componentType + " Name", componentNames, componentValues), "Choose the "+ componentType + " to map from");

    // Set up Parameter Names
    StringArray parameterNames;
    Array<var>  parameterValues;

    bool discreteOnly(componentType == "TextButton" ? true : false);

    configurationManagerMain.getConfiguration().setupParameterLists(parameterNames, parameterValues, discreteOnly);
    props.add(new ChoicePropertyComponent(valueTree.getPropertyAsValue(Ids::mapTo, &undoManager), "Parameter", parameterNames, parameterValues), "Choose the Parameter to map to");

    propertyPanel.addProperties(props.components);
}


/* =========================================================================
 * TextButtonMappingPanel
 */
TextButtonMappingPanel::TextButtonMappingPanel(ValueTree& parameter, UndoManager& um, ConfigurationManagerMain& cmm)
    : MappingPanel(parameter, um, cmm, "TextButton"),
      parameterName(valueTree.getPropertyAsValue(Ids::mapTo, &undoManager)),
      mappingType(valueTree.getPropertyAsValue(Ids::type, &undoManager))
{
    parameterName.addListener(this);
    mappingType.addListener(this);

    rebuildProperties();
}

TextButtonMappingPanel::~TextButtonMappingPanel()
{
    parameterName.removeListener(this);
    mappingType.removeListener(this);
}

void TextButtonMappingPanel::rebuildProperties()
{
    PropertyListBuilder props;

    props.clear();

    // Mapping Type
    const StringArray mappingTypes  = StringArray::fromTokens("No Toggle,Toggle,Increment,Decrement,Increment (Wrap-around),Decrement (Wrap-around)",",","");
    Array<var>        mappingValues;

    for (int i = 0; i < mappingTypes.size(); i++) mappingValues.add(i);
    props.add(new ChoicePropertyComponent(valueTree.getPropertyAsValue(Ids::type, &undoManager), "Mapping Type", mappingTypes, mappingValues), "Mapping type to apply to this TextButton mapping");
    
    // Set up Setting Names
    StringArray settingNames;
    Array<var>  settingValues;
    
    if (parameterName.toString().isNotEmpty())
    {
        configurationManagerMain.getConfiguration().setupSettingLists(parameterName.toString(), settingNames, settingValues);
    }
    
    props.add(new ChoicePropertyComponent(valueTree.getPropertyAsValue(Ids::settingDown, &undoManager), "Down Setting", settingNames, settingValues), "Parameter Setting to be sent when button is moved into the down position");
    
    if (int(mappingType.getValue()) < 2)
        props.add(new ChoicePropertyComponent(valueTree.getPropertyAsValue(Ids::settingUp, &undoManager), "Up Setting", settingNames, settingValues), "Parameter Setting to be sent when button is moved into the up position");

    // Radio Group
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::radioGroup, &undoManager), "Radio Group", 32, false), "Text identifier to match TextButtons within a radio group. Leave blank if no radio group needed.");

    propertyPanel.addProperties(props.components);
}

void TextButtonMappingPanel::valueChanged(Value& /* valueThatChanged */)
{
    propertyPanel.clear();
    MappingPanel::rebuildProperties();
    rebuildProperties();
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

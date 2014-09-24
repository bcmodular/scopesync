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

SettingsTable::SettingsTable(const ValueTree& valueTree, UndoManager& um, ConfigurationManagerMain& cmm,
                             ValueTree& param)
    : tree(valueTree), undoManager(um), font(14.0f), configurationManagerMain(cmm),
      numSettingsTextLabel(String::empty, "Num to add:"),
      addSettingsButton("Add"),
      removeSettingsButton("Remove"),
      autoFillValuesButton("Auto-fill"),
      moveUpButton("Move Up"),
      moveDownButton("Move Down"),
      parameter(param)
{
    configurationManagerMain.getCommandManager()->registerAllCommandsForTarget(this);

    addAndMakeVisible(table);
    
    table.setModel(this);
    
    table.setColour(ListBox::outlineColourId, Colours::grey);
    table.setOutlineThickness (1);
    
    table.getHeader().addColumn(String::empty,     1, 30,  30, 30,  TableHeaderComponent::notResizableOrSortable);
    table.getHeader().addColumn("Name",            2, 150, 50, 400, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    table.getHeader().addColumn("Scope Int Value", 3, 150, 50, 400, TableHeaderComponent::defaultFlags & ~TableHeaderComponent::draggable);
    
    table.getHeader().setStretchToFitActive(true);
    table.setMultipleSelectionEnabled (true);
    
    tree.addListener(this);

    addAndMakeVisible(numSettingsTextLabel);

    addAndMakeVisible(numSettingsToAddLabel);
    numSettingsToAddLabel.setEditable(true);
    numSettingsToAddLabel.getTextValue().referTo(numSettingsToAdd);
    
    numSettingsToAdd = 1;

    addAndMakeVisible(addSettingsButton);
    addSettingsButton.setCommandToTrigger(configurationManagerMain.getCommandManager(), CommandIDs::addSettings, true);
    
    addAndMakeVisible(removeSettingsButton);
    removeSettingsButton.setCommandToTrigger(configurationManagerMain.getCommandManager(), CommandIDs::removeSettings, true);
    
    addAndMakeVisible(autoFillValuesButton);
    autoFillValuesButton.setCommandToTrigger(configurationManagerMain.getCommandManager(), CommandIDs::autoFill, true);

    addAndMakeVisible(moveUpButton);
    moveUpButton.setCommandToTrigger(configurationManagerMain.getCommandManager(), CommandIDs::moveUp, true);

    addAndMakeVisible(moveDownButton);
    moveDownButton.setCommandToTrigger(configurationManagerMain.getCommandManager(), CommandIDs::moveDown, true);
}

SettingsTable::~SettingsTable()
{
    tree.removeListener(this);
}

void SettingsTable::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    Rectangle<int> buttonBar(localBounds.removeFromBottom(30).reduced(4, 4));

    numSettingsTextLabel.setBounds(buttonBar.removeFromLeft(100));
    numSettingsToAddLabel.setBounds(buttonBar.removeFromLeft(35));
    addSettingsButton.setBounds(buttonBar.removeFromLeft(70));
    removeSettingsButton.setBounds(buttonBar.removeFromLeft(70));
    buttonBar.removeFromLeft(10);
    autoFillValuesButton.setBounds(buttonBar.removeFromLeft(70));
    buttonBar.removeFromLeft(10);
    moveUpButton.setBounds(buttonBar.removeFromLeft(70));
    moveDownButton.setBounds(buttonBar.removeFromLeft(70));

    table.setBounds(localBounds.reduced(4, 4));
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
    SettingsSorter sorter(newSortColumnId, isForwards);
    tree.sort(sorter, &undoManager, true);
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

void SettingsTable::deleteKeyPressed(int)
{
    removeSettings();
}

void SettingsTable::getAllCommands(Array <CommandID>& commands)
{
    const CommandID ids[] = { CommandIDs::addSettings,
                              CommandIDs::removeSettings,
                              CommandIDs::autoFill,
                              CommandIDs::moveUp,
                              CommandIDs::moveDown
                            };

    commands.addArray(ids, numElementsInArray (ids));
}

void SettingsTable::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::addSettings:
        result.setInfo("Add", "Add new Settings", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress('n', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::removeSettings:
        result.setInfo ("Remove", "Remove selected Settings", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress(KeyPress::deleteKey));
        break;
    case CommandIDs::autoFill:
        result.setInfo ("Auto-fill", "Automatically fill all Setting values (evenly spread from min to max)", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress('f', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::moveUp:
        result.setInfo ("Move Up", "Move all selected Settings up one position", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress(KeyPress::upKey, ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::moveDown:
        result.setInfo ("Move Down", "Move all selected Settings down one position", CommandCategories::configmgr, 0);
        result.defaultKeypresses.add(KeyPress(KeyPress::downKey, ModifierKeys::commandModifier, 0));
        break;
    }
}

bool SettingsTable::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::addSettings:          addSettings(); break;
        case CommandIDs::removeSettings:       removeSettings(); break;
        case CommandIDs::autoFill:             autoFill(); break;
        case CommandIDs::moveUp:               moveSettings(true); break;
        case CommandIDs::moveDown:             moveSettings(false); break;
        default:                               return false;
    }

    return true;
}

void SettingsTable::addSettings()
{
    for (int i = 0; i < int(numSettingsToAdd.getValue()); i++)
    {
        ValueTree newSetting(Ids::setting);
        String newSettingName;

        int settingNum = 1;

        for(;;)
        {
            newSettingName = "New Setting " + String(settingNum);

            if (tree.getChildWithProperty(Ids::name, newSettingName).isValid())
            {
                settingNum++;
                continue;
            }
            else
                break;
        }

        newSetting.setProperty(Ids::name, newSettingName, &undoManager);
        newSetting.setProperty(Ids::intValue, 0, &undoManager);

        tree.addChild(newSetting, -1, &undoManager);
    }
}

void SettingsTable::removeSettings()
{
    SparseSet<int> selectedRows(table.getSelectedRows());
    Array<int> settingsToRemove;
    
    for (int i = 0; i < selectedRows.size(); i++)
    {
        settingsToRemove.add(selectedRows[i]);
    }

    DefaultElementComparator<int> sorter;
    settingsToRemove.sort(sorter);
    
    for (int i = settingsToRemove.size() - 1; i > -1; i--)
    {
        tree.removeChild(settingsToRemove[i], &undoManager);
    }
}
void SettingsTable::autoFill()
{
    int    numSettings = tree.getNumChildren();
    double scopeRangeMin = parameter.getProperty(Ids::scopeRangeMin);
    double scopeRangeMax = parameter.getProperty(Ids::scopeRangeMax);

    for (int i = 0; i < numSettings; i++)
    {
        int newValue = roundDoubleToInt((i * ((scopeRangeMax - scopeRangeMin) / (numSettings - 1))) + scopeRangeMin);

        tree.getChild(i).setProperty(Ids::intValue, newValue, &undoManager);
    }
}

void SettingsTable::moveSettings(bool moveUp)
{
    SparseSet<int> selectedRows(table.getSelectedRows());
    Array<int> settingsToMove;
    
    for (int i = 0; i < selectedRows.size(); i++)
    {
        settingsToMove.add(selectedRows[i]);
    }

    DefaultElementComparator<int> sorter;
    settingsToMove.sort(sorter);
    
    if (moveUp)
    {
        for (int i = 0; i < settingsToMove.size(); i++)
        {
            int currentIndex = settingsToMove[i];
            int newIndex     = jmax(settingsToMove[i] - 1, 0);

            table.deselectRow(currentIndex);

            tree.moveChild(currentIndex, newIndex, &undoManager);
            table.selectRow(newIndex, false, false);
        }
    }
    else
    {
        for (int i = settingsToMove.size() - 1; i > -1; i--)
        {
            int currentIndex = settingsToMove[i];
            int newIndex     = jmin(settingsToMove[i] + 1, tree.getNumChildren());
            
            table.deselectRow(currentIndex);

            tree.moveChild(currentIndex, newIndex, &undoManager);
            table.selectRow(newIndex, false, false);
        }
    }  
}

ApplicationCommandTarget* SettingsTable::getNextCommandTarget()
{
    return &configurationManagerMain;
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
ParameterPanel::ParameterPanel(ValueTree& parameter, UndoManager& um, 
                               ParameterType paramType, ConfigurationManagerMain& cmm)
    : valueTree(parameter), undoManager(um), parameterType(paramType), configurationManagerMain(cmm)
{
    rebuildProperties();
    addAndMakeVisible(propertyPanel);

    valueType = valueTree.getPropertyAsValue(Ids::valueType, &undoManager);
    valueType.addListener(this);

    if (int(valueType.getValue()) == 1)
    {
        ValueTree settings = valueTree.getOrCreateChildWithName(Ids::settings, &undoManager);
        addAndMakeVisible(settingsTable = new SettingsTable(settings, undoManager, configurationManagerMain, valueTree));
    }
       
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

void ParameterPanel::paint(Graphics& g)
{
    g.fillAll (Colours::lightgrey);
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

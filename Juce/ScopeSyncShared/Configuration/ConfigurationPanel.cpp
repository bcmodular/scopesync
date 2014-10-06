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
#include "Configuration.h"
#include "SettingsTable.h"
#include "../Core/Global.h"
#include "../Core/ScopeSyncApplication.h"
#include "../Core/ScopeSync.h"
#include "../Utils/BCMMisc.h"

/* =========================================================================
 * BasePanel
 */
BasePanel::BasePanel(ValueTree& node, UndoManager& um, Configuration& config, ApplicationCommandManager* acm)
    : valueTree(node), undoManager(um), configuration(config), commandManager(acm)
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
EmptyPanel::EmptyPanel(ValueTree& node, UndoManager& um, Configuration& config, ApplicationCommandManager* acm)
    : BasePanel(node, um, config, acm)
{
    propertyPanel.setVisible(false);
}

EmptyPanel::~EmptyPanel() {}

/* =========================================================================
 * ConfigurationPanel
 */
ConfigurationPanel::ConfigurationPanel(ValueTree& node, UndoManager& um, Configuration& config, ApplicationCommandManager* acm)
    : BasePanel(node, um, config, acm)
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
                               BCMParameter::ParameterType paramType, Configuration& config,
                               ApplicationCommandManager* acm, bool showCalloutView)
    : BasePanel(parameter, um, config, acm), parameterType(paramType), calloutView(showCalloutView)
{
    // Listen for changes to the valueType, to decide whether or not to show
    // the SettingsTable
    valueType = valueTree.getPropertyAsValue(Ids::valueType, &undoManager);
    valueType.addListener(this);

    rebuildProperties();

    if (int(valueType.getValue()) == 1)
        createSettingsTable();
}

ParameterPanel::~ParameterPanel()
{
    valueType.removeListener(this);
}

void ParameterPanel::rebuildProperties()
{
    propertyPanel.clear();

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

    if (parameterType == BCMParameter::hostParameter)
    {
        StringArray scopeSyncCodes = ScopeSync::getScopeSyncCodes();
        Array<var>  scopeSyncValues;

        for (int i = 0; i < scopeSyncCodes.size(); i++)
            scopeSyncValues.add(i);

        props.add(new ChoicePropertyComponent(valueTree.getPropertyAsValue(Ids::scopeSync, &undoManager), "ScopeSync Code", scopeSyncCodes, scopeSyncValues), "ScopeSync Code");
    }
    else if (parameterType == BCMParameter::scopeLocal)
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
    
    if (int(valueType.getValue()) == 0)
    {
        props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::scopeDBRef,    &undoManager), "Scope dB Reference"),          "Scope dB Reference Value (only set for dB-based parameters)");
        props.add(new BooleanPropertyComponent(valueTree.getPropertyAsValue(Ids::skewUIOnly,    &undoManager), "Skew UI Only", String::empty), "Only apply the Skew factor to the UI elements, not Scope values");
    }
}

void ParameterPanel::createUIProperties(PropertyListBuilder& props)
{
    props.clear();
    
    if (int(valueType.getValue()) == 0)
    {
        props.add(new TextPropertyComponent   (valueTree.getPropertyAsValue(Ids::uiSuffix,        &undoManager), "UI Suffix", 32, false),  "Text to display after value in the User Interface, e.g. Hz, %");
        props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::uiRangeMin,      &undoManager), "Min UI Value"),          "Minimum User Interface Value (Float)");
        props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::uiRangeMax,      &undoManager), "Max UI Value"),          "Maximum User Interface Value (Float)");
        props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::uiResetValue,    &undoManager), "Reset Value"),           "Value to reset parameter to on double-click or initialisation");
        props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::uiSkewMidpoint,  &undoManager), "Mid-point Value", true), "Value that should be at the mid-point of the UI range (leave blank if no skew required)");
    }
    
    String valIntTooltip;
    if (int(valueType.getValue()) == 0)
        valIntTooltip = "Step between consecutive User Interface values";
    else
        valIntTooltip = "Step between consecutive User Interface values (use 1 to snap to discrete values, or a smaller number to allow smooth scrolling)";

    props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::uiRangeInterval, &undoManager), "Value Interval"),                     valIntTooltip);
    props.add(new BooleanPropertyComponent(valueTree.getPropertyAsValue(Ids::valueType,       &undoManager), "Use discrete values", String::empty), "Use a set of discrete parameter values relating to specific control settings");
}

void ParameterPanel::childBoundsChanged(Component* child)
{
    if (child == settingsTable)
    {
        if (calloutView)
            configuration.getConfigurationProperties().setValue("lastCalloutSettingsTableHeight", settingsTable->getHeight());
        else
            configuration.getConfigurationProperties().setValue("lastSettingsTableHeight", settingsTable->getHeight());
        
        resized();
    }      
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
        resizerBar = nullptr;
        settingsTable = nullptr;
        setParameterUIRanges(0, 100, 0);
        rebuildProperties();
        resized();
    }
    else
    {
        rebuildProperties();
        createSettingsTable();
        resized();
    }
}

void ParameterPanel::setParameterUIRanges(double min, double max, double reset)
{
    valueTree.setProperty(Ids::uiRangeMin,      min,      &undoManager);
    valueTree.setProperty(Ids::uiRangeMax,      max,      &undoManager);
    valueTree.setProperty(Ids::uiResetValue,    reset,    &undoManager);
}

void ParameterPanel::createSettingsTable()
{
    int minHeight;
    int maxHeight;
    int lastSettingsTableHeight;

    if (calloutView)
    {
        minHeight = 100;
        maxHeight = 450;
        lastSettingsTableHeight = configuration.getConfigurationProperties().getIntValue("lastCalloutSettingsTableHeight", 250);
    }
    else
    {
        minHeight = 200;
        maxHeight = 700;
        lastSettingsTableHeight = configuration.getConfigurationProperties().getIntValue("lastSettingsTableHeight", 250);
    }

    settingsTableConstrainer.setMinimumHeight(minHeight);
    settingsTableConstrainer.setMaximumHeight(maxHeight);

    ValueTree settings = valueTree.getOrCreateChildWithName(Ids::settings, &undoManager);
    
    int maxValue = jmax(settings.getNumChildren() - 1, 1);
    setParameterUIRanges(0, maxValue, 0);

    // Reset Skew values to default to save confusion
    valueTree.setProperty(Ids::uiSkewMidpoint, String::empty, &undoManager);
    valueTree.setProperty(Ids::scopeDBRef, 0.0f, &undoManager);

    settingsTable = new SettingsTable(settings, undoManager, configuration, commandManager, valueTree);
    settingsTable->setBounds(0, 0, getWidth(), lastSettingsTableHeight);
    addAndMakeVisible(settingsTable);

    addAndMakeVisible(resizerBar = new ResizableEdgeComponent(settingsTable, &settingsTableConstrainer,
                                                                ResizableEdgeComponent::topEdge));
    resizerBar->setAlwaysOnTop (true);
}

/* =========================================================================
 * MappingPanel
 */
MappingPanel::MappingPanel(ValueTree& mapping, UndoManager& um, 
                           Configuration& config, ApplicationCommandManager* acm, 
                           const Identifier& compType, bool calloutView)
    : BasePanel(mapping, um, config, acm), componentType(compType), showComponent(!calloutView)
{
    rebuildProperties();
}

MappingPanel::~MappingPanel() {}

void MappingPanel::rebuildProperties()
{
    PropertyListBuilder props;

    props.clear();

    if (showComponent)
    {
        // Set up Component Names
        StringArray componentNames = configuration.getComponentNames(componentType);
        Array<var>  componentValues;

        for (int i = 0; i < componentNames.size(); i++) componentValues.add(componentNames[i]);

        componentNames.insert(0, "- No Component -");
        componentValues.insert(0, String::empty);

        String componentTypeName;

             if (componentType == Ids::slider)          componentTypeName = "Slider";
        else if (componentType == Ids::label)           componentTypeName = "Label";
        else if (componentType == Ids::comboBox)        componentTypeName = "Combo Box";
        else if (componentType == Ids::tabbedComponent) componentTypeName = "Tabbed Component";
        else if (componentType == Ids::textButton)      componentTypeName = "Text Button";
        
        props.add(new ChoicePropertyComponent(valueTree.getPropertyAsValue(Ids::name, &undoManager), componentTypeName + " Name", componentNames, componentValues), "Choose the "+ componentTypeName + " to map from");
    }

    // Set up Parameter Names
    StringArray parameterNames;
    Array<var>  parameterValues;

    bool discreteOnly(componentType == Ids::textButton ? true : false);

    configuration.setupParameterLists(parameterNames, parameterValues, discreteOnly);

    parameterNames.insert(0, "- No Mapping -");
    parameterValues.insert(0, String::empty);

    props.add(new ChoicePropertyComponent(valueTree.getPropertyAsValue(Ids::mapTo, &undoManager), "Parameter", parameterNames, parameterValues), "Choose the Parameter to map to");

    propertyPanel.addProperties(props.components);
}


/* =========================================================================
 * TextButtonMappingPanel
 */
TextButtonMappingPanel::TextButtonMappingPanel(ValueTree& mapping, UndoManager& um, Configuration& config, ApplicationCommandManager* acm, bool hideComponentName)
    : MappingPanel(mapping, um, config, acm, Ids::textButton, hideComponentName),
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
        configuration.setupSettingLists(parameterName.toString(), settingNames, settingValues);
    }

    settingNames.insert(0, "- No Setting -");
    settingValues.insert(0, String::empty);

    if (int(mappingType.getValue()) < 2)
    {
        props.add(new ChoicePropertyComponent(valueTree.getPropertyAsValue(Ids::settingDown, &undoManager), "Down Setting", settingNames, settingValues), "Parameter Setting to be sent when button is moved into the down position");
        props.add(new ChoicePropertyComponent(valueTree.getPropertyAsValue(Ids::settingUp, &undoManager), "Up Setting", settingNames, settingValues), "Parameter Setting to be sent when button is moved into the up position");
    }
        
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

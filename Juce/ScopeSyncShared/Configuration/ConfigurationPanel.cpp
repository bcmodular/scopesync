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
#include "../Windows/LayoutChooser.h"
#include "../Windows/UserSettings.h"

//==============================================================================
class ComponentBackgroundColourProperty : public  ColourPropertyComponent,
                                          private Value::Listener
{
public:
    ComponentBackgroundColourProperty(const Value& valueToControl, const String& colourName)
        : ColourPropertyComponent(colourName, false),
          colourValue(valueToControl)
    {
        colourValue.referTo(valueToControl);
        colourValue.addListener(this);
    }

    ~ComponentBackgroundColourProperty() {}

    void valueChanged(Value& /* valueThatChanged */) override
    {
        refresh();
    }

    void   setColour(Colour newColour) override { colourValue = newColour.toString(); }
    Colour getColour() const override           { return Colour::fromString(colourValue.toString()); }

    void resetToDefault() override
    {
        jassertfalse; // option shouldn't be visible
    }

protected:
    Value colourValue;
};

/* =========================================================================
 * BasePanel
 */
BasePanel::BasePanel(ValueTree& node, UndoManager& um, ApplicationCommandManager* acm)
    : valueTree(node), undoManager(um), commandManager(acm)
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
EmptyPanel::EmptyPanel(ValueTree& node, UndoManager& um, ApplicationCommandManager* acm)
    : BasePanel(node, um, acm)
{
    propertyPanel.setVisible(false);
}

EmptyPanel::~EmptyPanel() {}

class LayoutChooserButton : public ButtonPropertyComponent
{
public:
    LayoutChooserButton(ConfigurationPanel& cp)
        : ButtonPropertyComponent("Layout Chooser", false),
          configurationPanel(cp) {}

    void buttonClicked() override
    {
        configurationPanel.chooseLayout();
    }

    String getButtonText() const override { return "Choose Layout ..."; }

private:
    ConfigurationPanel& configurationPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LayoutChooserButton)
};

/* =========================================================================
 * ConfigurationPanel
 */
ConfigurationPanel::ConfigurationPanel(ValueTree& node, UndoManager& um, ScopeSync& ss, ApplicationCommandManager* acm, bool newConfiguration)
    : BasePanel(node, um, acm), isNewConfiguration(newConfiguration), configuration(ss.getConfiguration())
{
    rebuildProperties();
}

ConfigurationPanel::~ConfigurationPanel() {}

void ConfigurationPanel::rebuildProperties()
{
    layoutName       = valueTree.getPropertyAsValue(Ids::layoutName,       &undoManager);
    layoutLibrarySet = valueTree.getPropertyAsValue(Ids::layoutLibrarySet, &undoManager);

    propertyPanel.clear();

    PropertyListBuilder props;

    props.clear();
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::name,       &undoManager), "Name",        256,  false), "Name of Configuration");
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::librarySet, &undoManager), "Library Set", 256,  false), "Library Set for Configuration");
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::author,     &undoManager), "Author",      256,  false), "Name of Configuration's creator");
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::blurb,      &undoManager), "Blurb",       1024, true),  "Textual description of Configuration (shown in Chooser)");

    if (isNewConfiguration)
    {
        valueTree.setProperty(Ids::includeScopeLocal, true, &undoManager);
        props.add(new BooleanPropertyComponent(valueTree.getPropertyAsValue(Ids::includeScopeLocal, &undoManager), "Standard Scope Parameters", "Include"), "Tick to automatically include the standard Scope parameters and their mappings, to support features like Open/Close Preset window etc.");
    }

    propertyPanel.addSection("Configuration Settings", props.components);

    props.clear();
    props.add(new LayoutChooserButton(*this),                                                                                     "Launch the Layout Chooser");
    props.add(new TextPropertyComponent(layoutName,                                            "Layout Name",     256, false),    "Name of Layout");
    props.add(new TextPropertyComponent(layoutLibrarySet,                                      "Layout Library Set", 256, false), "Library Set containing Layout");
    propertyPanel.addSection("Layout Settings", props.components);
    

}

void ConfigurationPanel::hideLayoutChooser()
{
    layoutChooserWindow = nullptr;
}

void ConfigurationPanel::updateLayout()
{ 
    rebuildProperties();
    String newLayoutFilePath = UserSettings::getInstance()->getLayoutFilename(valueTree.getProperty(Ids::layoutName),
                                                                              valueTree.getProperty(Ids::layoutLibrarySet));
    UserSettings::getInstance()->setLastTimeLayoutLoaded(newLayoutFilePath);
}

void ConfigurationPanel::chooseLayout()
{
    if (layoutChooserWindow == nullptr)
        layoutChooserWindow = new LayoutChooserWindow
                                      (
                                      getParentMonitorArea().getCentreX(), 
                                      getParentMonitorArea().getCentreY(), 
                                      layoutName,
                                      layoutLibrarySet,
                                      *this,
                                      commandManager
                                      );
    
    layoutChooserWindow->setVisible(true);
    
    if (ScopeSyncApplication::inScopeFXContext())
        layoutChooserWindow->setAlwaysOnTop(true);
    
    layoutChooserWindow->toFront(true);
}

/* =========================================================================
 * ParameterPanel
 */
ParameterPanel::ParameterPanel(ValueTree& parameter, UndoManager& um, 
                               BCMParameter::ParameterType paramType, ScopeSync& ss,
                               ApplicationCommandManager* acm, bool showCalloutView)
    : BasePanel(parameter, um, acm), parameterType(paramType), calloutView(showCalloutView), configuration(ss.getConfiguration())
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
    createDescriptionProperties(props, undoManager, valueTree, parameterType);
    propertyPanel.addSection("Main Properties", props.components, true);

    createScopeProperties(props, undoManager, valueTree, valueType.getValue());
    propertyPanel.addSection("Scope Properties", props.components, true);
    
    createUIProperties(props, undoManager, valueTree, valueType.getValue());
    propertyPanel.addSection("UI Properties", props.components, true);
}

void ParameterPanel::createDescriptionProperties(PropertyListBuilder& props, UndoManager& undoManager, ValueTree& valueTree, BCMParameter::ParameterType parameterType)
{
    props.clear();

    String nameTooltip("Mapping name for parameter");

    if (parameterType == BCMParameter::preset)
        nameTooltip = "Name of Preset";

    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::name, &undoManager), "Name", 256, false), nameTooltip);
    
    if (parameterType != BCMParameter::preset)
    {
        props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::shortDescription, &undoManager), "Short Description", 256, false), "Short Description of parameter");
        props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::fullDescription, &undoManager),  "Full Description", 256, false),  "Full Description of parameter");
    }
    else
    {
        props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::blurb, &undoManager), "Blurb", 1024, true), "Textual description of Preset (shown in Chooser)");
    }

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

void ParameterPanel::createScopeProperties(PropertyListBuilder& props, UndoManager& undoManager, ValueTree& valueTree, int valueType)
{
    props.clear();
    props.add(new IntRangeProperty        (valueTree.getPropertyAsValue(Ids::scopeRangeMin, &undoManager), "Min Scope Value"),             "Minimum Scope Value (Integer)");
    props.add(new IntRangeProperty        (valueTree.getPropertyAsValue(Ids::scopeRangeMax, &undoManager), "Max Scope Value"),             "Maximum Scope Value (Integer)");
    
    if (valueType == 0)
    {
        props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::scopeDBRef,    &undoManager), "Scope dB Reference"),          "Scope dB Reference Value (only set for dB-based parameters)");
        props.add(new BooleanPropertyComponent(valueTree.getPropertyAsValue(Ids::skewUIOnly,    &undoManager), "Skew UI Only", String::empty), "Only apply the Skew factor to the UI elements, not Scope values");
    }
}

void ParameterPanel::createUIProperties(PropertyListBuilder& props, UndoManager& undoManager, ValueTree& valueTree, int valueType)
{
    props.clear();
    
    if (valueType == 0)
    {
        props.add(new TextPropertyComponent   (valueTree.getPropertyAsValue(Ids::uiSuffix,        &undoManager), "UI Suffix", 32, false),  "Text to display after value in the User Interface, e.g. Hz, %");
        props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::uiRangeMin,      &undoManager), "Min UI Value"),          "Minimum User Interface Value (Float)");
        props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::uiRangeMax,      &undoManager), "Max UI Value"),          "Maximum User Interface Value (Float)");
        props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::uiResetValue,    &undoManager), "Reset Value"),           "Value to reset parameter to on double-click or initialisation");
        props.add(new FltProperty             (valueTree.getPropertyAsValue(Ids::uiSkewMidpoint,  &undoManager), "Mid-point Value", true), "Value that should be at the mid-point of the UI range (leave blank if no skew required)");
    }
    
    String valIntTooltip;
    if (valueType == 0)
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
        setParameterUIRanges(0, 100, 0, undoManager, valueTree);
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

void ParameterPanel::setParameterUIRanges(double min, double max, double reset, UndoManager& undoManager, ValueTree& valueTree)
{
    valueTree.setProperty(Ids::uiRangeMin,   min,   &undoManager);
    valueTree.setProperty(Ids::uiRangeMax,   max,   &undoManager);
    valueTree.setProperty(Ids::uiResetValue, reset, &undoManager);
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
    setParameterUIRanges(0, maxValue, 0, undoManager, valueTree);

    // Reset Skew values to default to save confusion
    valueTree.setProperty(Ids::uiSkewMidpoint, String::empty, &undoManager);
    valueTree.setProperty(Ids::scopeDBRef, 0.0f, &undoManager);

    settingsTable = new SettingsTable(settings, undoManager, commandManager, valueTree);
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
                           ScopeSync& ss, ApplicationCommandManager* acm, 
                           const Identifier& compType, bool calloutView)
    : BasePanel(mapping, um, acm), componentType(compType), 
      showComponent(!calloutView), configuration(ss.getConfiguration())
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

        String componentTypeName = Configuration::getComponentTypeName(componentType);
        
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
TextButtonMappingPanel::TextButtonMappingPanel(ValueTree& mapping, UndoManager& um, ScopeSync& ss, ApplicationCommandManager* acm, bool hideComponentName)
    : MappingPanel(mapping, um, ss, acm, Ids::textButton, hideComponentName),
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

#include "../Components/BCMWidget.h"

/* =========================================================================
 * StyleOverridePanel
 */
StyleOverridePanel::StyleOverridePanel(ValueTree& mapping, UndoManager& um, 
                                       ScopeSync& ss, ApplicationCommandManager* acm, 
                                       const Identifier& compType,
                                       bool calloutView)
    : BasePanel(mapping, um, acm), 
      componentType(compType), 
      showComponent(!calloutView),
      useColourOverrides(valueTree.getPropertyAsValue(Ids::useColourOverrides, &undoManager)),
      scopeSync(ss), configuration(ss.getConfiguration())
{
    useColourOverrides.addListener(this);
    rebuildProperties();
}

StyleOverridePanel::~StyleOverridePanel()
{
    useColourOverrides.removeListener(this);
}

void StyleOverridePanel::rebuildProperties()
{
    String componentTypeName = Configuration::getComponentTypeName(componentType);
    PropertyListBuilder props;

    props.clear();

    if (showComponent)
    {
        // Set up Component Names
        StringArray componentNames = configuration.getComponentNames(componentType, true);
        Array<var>  componentValues;

        for (int i = 0; i < componentNames.size(); i++) componentValues.add(componentNames[i]);

        componentNames.insert(0, "- No Component -");
        componentValues.insert(0, String::empty);
        
        props.add(new ChoicePropertyComponent(valueTree.getPropertyAsValue(Ids::name, &undoManager), componentTypeName + " Name", componentNames, componentValues), "Choose the "+ componentTypeName + " to override style for");
    }

    // Set up LookAndFeels
    StringArray lookAndFeelIds = scopeSync.getBCMLookAndFeelIds(componentType);
    Array<var>  lookAndFeelValues;

    for (int i = 0; i < lookAndFeelIds.size(); i++) lookAndFeelValues.add(lookAndFeelIds[i]);
    
    lookAndFeelIds.insert(0, String::empty);
    lookAndFeelValues.insert(0, String::empty);
    
    lookAndFeelIds.insert(0, "- No Override -");
    lookAndFeelValues.insert(0, String::empty);

    props.add(new ChoicePropertyComponent(valueTree.getPropertyAsValue(Ids::lookAndFeelId, &undoManager), "LookAndFeel", lookAndFeelIds, lookAndFeelValues), "Choose the LookAndFeel to use");
    
    String buttonOffText;
    String buttonOffTooltipText;

    if (componentType == Ids::textButton)
    {
        buttonOffText = " (Off)";
        buttonOffTooltipText = " when it's off";
    }

    if (componentType == Ids::slider || componentType == Ids::label || componentType == Ids::component || componentType == Ids::textButton)
    {
        props.add(new BooleanPropertyComponent(valueTree.getPropertyAsValue(Ids::useColourOverrides, &undoManager), "Use Colour Overrides", String::empty), "Check the box to enable individual colour overrides");
    }

    if (bool(valueTree.getProperty(Ids::useColourOverrides, true)))
    {
        if (componentType == Ids::slider || componentType == Ids::label || componentType == Ids::component || componentType == Ids::textButton)
        {
            props.add(new ComponentBackgroundColourProperty(valueTree.getPropertyAsValue(Ids::fillColour, &undoManager), "Fill Colour" + buttonOffText), "Choose the Colour to fill this " + componentTypeName + " with" + buttonOffTooltipText);
        }
    
        if (componentType == Ids::slider || componentType == Ids::label || componentType == Ids::textButton)
        {
            String colourText;

            if (componentType == Ids::label || componentType == Ids::textButton)
                colourText = "Text Colour";
            else
                colourText = "Line Colour";

            props.add(new ComponentBackgroundColourProperty(valueTree.getPropertyAsValue(Ids::lineColour, &undoManager), colourText), "Choose the " + colourText + " for this " + componentTypeName + buttonOffTooltipText);
        }

        if (componentType == Ids::textButton)
        {
            props.add(new ComponentBackgroundColourProperty(valueTree.getPropertyAsValue(Ids::fillColour2, &undoManager), "Fill Colour (On)"), "Choose the Fill Colour for this " + componentTypeName + " when it's on");
            props.add(new ComponentBackgroundColourProperty(valueTree.getPropertyAsValue(Ids::lineColour2, &undoManager), "Text Colour (On)"), "Choose the Text Colour for this " + componentTypeName + " when it's on");
        }
    
        if (componentType == Ids::slider)
        {
            props.add(new ComponentBackgroundColourProperty(valueTree.getPropertyAsValue(Ids::fillColour2, &undoManager), "Text Box Fill Colour"), "Choose the Fill Colour for this " + componentTypeName + "'s Text Box");
            props.add(new ComponentBackgroundColourProperty(valueTree.getPropertyAsValue(Ids::lineColour2, &undoManager), "Text Box Text Colour"), "Choose the Text Colour for this " + componentTypeName + "'s Text Box");
        }
    }
    
    propertyPanel.addProperties(props.components);
}

void StyleOverridePanel::valueChanged(Value& /* valueThatChanged */)
{
    propertyPanel.clear();
    rebuildProperties();
}

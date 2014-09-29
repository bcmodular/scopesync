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

#ifndef PARAMETERPANEL_H_INCLUDED
#define PARAMETERPANEL_H_INCLUDED

#include <JuceHeader.h>
#include "../Core/Global.h"
class SettingsTable;
class ConfigurationManagerMain;

/* =========================================================================
 * PropertyListBuilder: Utility class to help set up Property Lists
 */
class PropertyListBuilder
{
public:
    PropertyListBuilder() {}

    void add(PropertyComponent* propertyComp);
    void add(PropertyComponent* propertyComp, const String& tooltip);
    void clear();

    Array<PropertyComponent*> components;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertyListBuilder)
};

/* =========================================================================
 * BasePanel: Base Edit Panel
 */
class BasePanel : public Component
{
public:
    BasePanel(ValueTree& parameter, UndoManager& um, ConfigurationManagerMain& cmm);
    ~BasePanel();

protected:
    ValueTree     valueTree;
    PropertyPanel propertyPanel;
    UndoManager&  undoManager;
    ConfigurationManagerMain& configurationManagerMain;
    virtual void rebuildProperties() {};
    
private:
    
    virtual void resized() override;
    virtual void paint(Graphics& g) override;

    void focusGained(FocusChangeType cause) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasePanel)
};

/* =========================================================================
 * EmptyPanel: Panel for situations where there's nothing to edit
 */
class EmptyPanel : public BasePanel
{
public:
    EmptyPanel(ValueTree& parameter, UndoManager& um, ConfigurationManagerMain& cmm);
    ~EmptyPanel();

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EmptyPanel)
};

/* =========================================================================
 * ConfigurationPanel: Edit Panel for Configuration
 */
class ConfigurationPanel : public BasePanel
{
public:
    ConfigurationPanel(ValueTree& parameter, UndoManager& um, ConfigurationManagerMain& cmm);
    ~ConfigurationPanel();

protected:
    void rebuildProperties() override;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationPanel)
};

/* =========================================================================
 * ParameterPanel: Edit Panel for Parameters
 */
class ParameterPanel : public BasePanel,
                       public Value::Listener
{
public:
    enum ParameterType {hostParameter, scopeLocal};

    ParameterPanel(ValueTree& parameter, UndoManager& um,
                   ParameterType paramType, ConfigurationManagerMain& cmm);
    ~ParameterPanel();

    void paintOverChildren(Graphics& g);
    void childBoundsChanged(Component* child) override;
    void setParameterUIRanges(double min, double max, double reset);

private:
    ScopedPointer<SettingsTable> settingsTable;
    Value         valueType;

    ScopedPointer<ResizableEdgeComponent> resizerBar;
    ComponentBoundsConstrainer settingsTableConstrainer;

    ParameterType parameterType;

    void rebuildProperties() override;
    void createDescriptionProperties(PropertyListBuilder& propertyPanel);
    void createScopeProperties(PropertyListBuilder& propertyPanel);
    void createUIProperties(PropertyListBuilder& propertyPanel);
    
    void createSettingsTable();

    void resized() override;
    
    void valueChanged(Value& valueThatChanged) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterPanel)
};

/* =========================================================================
 * MappingPanel: Edit Panel for Mappings
 */
class MappingPanel : public BasePanel
{
public:
    MappingPanel(ValueTree& parameter, UndoManager& um, ConfigurationManagerMain& cmm, const String& compType);
    ~MappingPanel();

protected:
    void rebuildProperties() override;

private:
    String componentType;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MappingPanel)
};

/* =========================================================================
 * TextButtonMappingPanel: Edit Panel for TextButton Mappings
 */
class TextButtonMappingPanel : public MappingPanel,
                               public Value::Listener
{
public:
    TextButtonMappingPanel(ValueTree& parameter, UndoManager& um, ConfigurationManagerMain& cmm);
    ~TextButtonMappingPanel();

private:
    Value parameterName;
    Value mappingType;

    void rebuildProperties() override;

    void valueChanged(Value& valueThatChanged) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextButtonMappingPanel)
};

/* =========================================================================
 * NumericProperty: TextPropertyComponent for numeric values
 */
class NumericProperty : public PropertyComponent,
                        public Label::Listener
{
public:
    NumericProperty(const Value&  valueToControl,
                    const String& propertyName,
                    const String& validInputString);
    ~NumericProperty();

    virtual void setText (const String& newText);
    String       getText() const;

    enum ColourIds
    {
        backgroundColourId = 0x100e401,
        textColourId       = 0x100e402,
        outlineColourId    = 0x100e403,
    };

    void refresh();
    void labelTextChanged(Label* labelThatHasChanged) override;

protected:
    class LabelComp;
    friend class LabelComp;

    ScopedPointer<LabelComp> textEditor;

private:
    void textWasEdited();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NumericProperty)
};

/* =========================================================================
 * IntRangeProperty: TextPropertyComponent for Integer values (with range)
 */
class IntRangeProperty : public NumericProperty
{
public:
    IntRangeProperty(const Value&  valueToControl,
                     const String& propertyName,
                     const int     minInt = INT_MIN,
                     const int     maxInt = INT_MAX);
    ~IntRangeProperty();

    void labelTextChanged(Label* labelThatHasChanged) override;
    
private:
    int minValue, maxValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IntRangeProperty)
};

/* =========================================================================
 * FltProperty: TextPropertyComponent for Float values
 */
class FltProperty : public NumericProperty
{
public:
    FltProperty(const Value&  valueToControl,
                const String& propertyName,
                const bool    allowBlank = false);
    ~FltProperty();

    void labelTextChanged(Label* labelThatHasChanged) override;
    
private:
    bool allowedToBeBlank;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FltProperty)
};

#endif  // PARAMETERPANEL_H_INCLUDED

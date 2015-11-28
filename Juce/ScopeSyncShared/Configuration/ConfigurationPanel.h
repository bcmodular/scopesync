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
#include "../Core/BCMParameter.h"
class SettingsTable;
class Configuration;
class PropertyListBuilder;
class ScopeSync;

/* =========================================================================
 * BasePanel: Base Edit Panel
 */
class BasePanel : public Component
{
public:
    BasePanel(ValueTree& node, UndoManager& um, ApplicationCommandManager* acm);
    ~BasePanel();

    void focusGained(FocusChangeType cause) override;

protected:
    ValueTree      valueTree;
    PropertyPanel  propertyPanel;
    UndoManager&   undoManager;
    ApplicationCommandManager* commandManager;
    virtual void rebuildProperties() {};
    
private:
    
    virtual void resized() override;
    virtual void paint(Graphics& g) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasePanel)
};

/* =========================================================================
 * EmptyPanel: Panel for situations where there's nothing to edit
 */
class EmptyPanel : public BasePanel
{
public:
    EmptyPanel(ValueTree& node, UndoManager& um, ApplicationCommandManager* acm);
    ~EmptyPanel();

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EmptyPanel)
};

class LayoutChooserWindow;

/* =========================================================================
 * ConfigurationPanel: Edit Panel for Configuration
 */
class ConfigurationPanel : public BasePanel
{
public:
    ConfigurationPanel(ValueTree& node, UndoManager& um, ScopeSync& ss, ApplicationCommandManager* acm, bool newConfiguration);
    ~ConfigurationPanel();
    void chooseLayout();
    
    void updateLayout();
    void hideLayoutChooser();
    
protected:
    void rebuildProperties() override;

private:
    Configuration& configuration;
    Value          layoutName;
    Value          layoutLibrarySet;
    bool           isNewConfiguration;

    
    ScopedPointer<LayoutChooserWindow> layoutChooserWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationPanel)
};

/* =========================================================================
 * ParameterPanel: Edit Panel for Parameters
 */
class ParameterPanel : public BasePanel,
                       public Value::Listener
{
public:
    ParameterPanel(ValueTree& parameter, UndoManager& um,
                   ScopeSync& ss,
                   ApplicationCommandManager* acm, bool showCalloutView = false);
    ~ParameterPanel();

    void paintOverChildren(Graphics& g);
    void childBoundsChanged(Component* child) override;
    
    static void setParameterUIRanges(double min, double max, double reset, UndoManager& undoManager, ValueTree& valueTree);
    static void createDescriptionProperties(PropertyListBuilder& propertyPanel, UndoManager& undoManager, ValueTree& valueTree, BCMParameter::ParameterType parameterType);
    static void createScopeProperties(PropertyListBuilder& propertyPanel, UndoManager& undoManager, ValueTree& valueTree, int valueType);
    static void createUIProperties(PropertyListBuilder& propertyPanel, UndoManager& undoManager, ValueTree& valueTree, int valueType);

	void rebuild();

private:
    Configuration& configuration;
    ScopedPointer<SettingsTable> settingsTable;
    Value valueType;
    bool  calloutView;

    ScopedPointer<ResizableEdgeComponent> resizerBar;
    ComponentBoundsConstrainer settingsTableConstrainer;

    BCMParameter::ParameterType parameterType;

    void rebuildProperties() override;
    
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
    MappingPanel(ValueTree& mapping, UndoManager& um, ScopeSync& ss, ApplicationCommandManager* acm, const Identifier& compType, bool calloutView = false);
    ~MappingPanel();

protected:
    Configuration& configuration;
    
    void rebuildProperties() override;

private:
    Identifier     componentType;
    bool           showComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MappingPanel)
};

/* =========================================================================
 * TextButtonMappingPanel: Edit Panel for TextButton Mappings
 */
class TextButtonMappingPanel : public MappingPanel,
                               public Value::Listener
{
public:
    TextButtonMappingPanel(ValueTree& mapping, UndoManager& um, ScopeSync& ss, ApplicationCommandManager* acm, bool hideComponentName = false);
    ~TextButtonMappingPanel();

private:
    Value parameterName;
    Value mappingType;

    void rebuildProperties() override;

    void valueChanged(Value& valueThatChanged) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextButtonMappingPanel)
};

/* =========================================================================
 * StyleOverridePanel: Edit Panel for Style Overrides
 */
class StyleOverridePanel : public BasePanel,
                           public Value::Listener
{
public:
    StyleOverridePanel(ValueTree& styleOverride, UndoManager& um, ScopeSync& ss, ApplicationCommandManager* acm, const Identifier& compType, bool calloutView = false);
    ~StyleOverridePanel();

protected:
    virtual void rebuildProperties() override;

private:
    ScopeSync&     scopeSync;
    Configuration& configuration;
    
    Identifier componentType;
    bool       showComponent;
    Value      useColourOverrides;
    
    void valueChanged(Value& valueThatChanged) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StyleOverridePanel)
};

#endif  // PARAMETERPANEL_H_INCLUDED

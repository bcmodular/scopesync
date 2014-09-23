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

/* =========================================================================
 * SettingsTable: TableListBox for managing Parameter Settings
 */
class SettingsTable : public  Component,
                      private TableListBoxModel,
                      private ValueTree::Listener
{
public:
    SettingsTable(const ValueTree& vt, UndoManager& um);
    ~SettingsTable();

    void       resized() override;
    
    int        getNumRows();
    void       paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void       paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;
    void       sortOrderChanged(int newSortColumnId, bool isForwards) override;
    void       selectedRowsChanged(int lastRowSelected) override;
    void       backgroundClicked(const MouseEvent&) override;

    // Overridden methods for ValueTree::Listener
    void valueTreePropertyChanged(ValueTree& /* treeWhosePropertyHasChanged */, const Identifier& /* property */) override { table.updateContent(); };
    void valueTreeChildAdded(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenAdded */) override { table.updateContent(); };
    void valueTreeChildRemoved(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenRemoved */) override { table.updateContent(); };
    void valueTreeChildOrderChanged(ValueTree& /* parentTreeWhoseChildrenHaveMoved */) override { table.updateContent(); };
    void valueTreeParentChanged(ValueTree& /* treeWhoseParentHasChanged */) override { table.updateContent(); };

private:
    TableListBox table;
    Font         font;
    ValueTree    tree;
    UndoManager& undoManager;
    
    class LabelComp;
    friend class LabelComp;

    void textWasEdited();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsTable)
};

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
 * ParameterPanel: Edit Panel for Parameters
 */
class ParameterPanel : public Component
{
public:
    enum ParameterType {hostParameter, scopeLocal};

    ParameterPanel(ValueTree& parameter, UndoManager& um, ParameterType paramType);
    ~ParameterPanel();

private:
    ValueTree     valueTree;
    PropertyPanel propertyPanel;
    UndoManager&  undoManager;
    ScopedPointer<SettingsTable> settingsTable;

    ParameterType parameterType;

    void rebuildProperties();
    void createDescriptionProperties(PropertyListBuilder& propertyPanel);
    void createScopeProperties(PropertyListBuilder& propertyPanel);
    void createUIProperties(PropertyListBuilder& propertyPanel);
    void resized();
    void paint(Graphics& g);
};

/* =========================================================================
 * NumericProperty: TextPropertyComponent for numeric values
 */
class NumericProperty : public PropertyComponent
{
public:
    NumericProperty(const Value&  valueToControl,
                    const String& propertyName);
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

private:
    ScopedPointer<Label> textEditor;
    
    class LabelComp;
    friend class LabelComp;

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

    void setText (const String& newText);
    
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

    void setText (const String& newText);
    
private:
    bool allowedToBeBlank;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FltProperty)
};


#endif  // PARAMETERPANEL_H_INCLUDED

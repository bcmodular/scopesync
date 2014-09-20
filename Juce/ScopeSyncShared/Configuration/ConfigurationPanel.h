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
    ParameterPanel(ValueTree& parameter);
    ~ParameterPanel();

private:
    ValueTree     valueTree;
    PropertyPanel propertyPanel;

    void rebuildProperties();
    void createDescriptionProperties(PropertyListBuilder& propertyPanel);
    void createScopeProperties(PropertyListBuilder& propertyPanel);
    void resized();
    void paint(Graphics& g);
};

/* =========================================================================
 * IntRangeProperty: TextPropertyComponent for Integer values
 */
class IntRangeProperty : public PropertyComponent
{
public:
    IntRangeProperty(const Value&  valueToControl,
                     const String& propertyName,
                     int           maxNumChars,
                     const int     minInt = INT_MIN,
                     const int     maxInt = INT_MAX);
    ~IntRangeProperty();

    void   setText (const String& newText);
    String getText() const;

    enum ColourIds
    {
        backgroundColourId = 0x100e401,
        textColourId       = 0x100e402,
        outlineColourId    = 0x100e403,
    };

    void refresh();

private:
    ScopedPointer<Label> textEditor;
    int minValue, maxValue;

    class LabelComp;
    friend class LabelComp;

    void textWasEdited();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IntRangeProperty)
};

#endif  // PARAMETERPANEL_H_INCLUDED

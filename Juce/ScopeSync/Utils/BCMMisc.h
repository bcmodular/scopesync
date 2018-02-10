/**
 * Miscellaneous utility classes & methods
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

#ifndef BCMMISC_H_INCLUDED
#define BCMMISC_H_INCLUDED

#include <JuceHeader.h>
#include "../Windows/UserSettings.h"

String createAlphaNumericUID();

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

    void refresh() override;
    void labelTextChanged(Label* labelThatHasChanged) override;

protected:
    class LabelComp;
    friend class LabelComp;

    ScopedPointer<LabelComp> textEditor;

private:
    
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

/* =========================================================================
 * ColourPropertyComponent: PropertyComponent for choosing Colour values
 *                          (appropriated from the Introjucer)
 */
class ColourPropertyComponent : public PropertyComponent
{
public:
    ColourPropertyComponent(const String& name,
                            const bool canReset);

    virtual void   setColour(Colour newColour) = 0;
    virtual Colour getColour() const = 0;
    virtual void   resetToDefault() = 0;

    void refresh() override;

    class ColourEditorComponent : public Component,
                                  public ChangeListener
    {
    public:
        ColourEditorComponent(const bool canReset);

        void paint(Graphics& g) override;

        virtual void setColour (Colour newColour) = 0;
        virtual void resetToDefault() = 0;
        virtual Colour getColour() const = 0;

        void refresh();

        void mouseDown(const MouseEvent&) override;

        void changeListenerCallback(ChangeBroadcaster* source) override;

        class ColourSelectorComp : public Component,
								   public Button::Listener
        {
        public:
            ColourSelectorComp(ColourEditorComponent* owner_,
                               bool canReset);

            void resized() override;

            void buttonClicked(Button*) override;

        private:
            class ColourSelectorWithSwatches : public ColourSelector
            {
            public:
                ColourSelectorWithSwatches();

                int getNumSwatches() const override;

                Colour getSwatchColour(int index) const override;

                void setSwatchColour(int index, const Colour& newColour) override;
            
            private:
				SharedResourcePointer<UserSettings> userSettings;
            };

            ColourEditorComponent* owner;
            ColourSelectorWithSwatches selector;
            TextButton defaultButton;
        };

    private:
        Colour colour;
        bool canResetToDefault;
    };

    class ColourPropEditorComponent : public ColourEditorComponent
    {
        ColourPropertyComponent* const owner;

    public:
        ColourPropEditorComponent(ColourPropertyComponent* const owner_,
                                  const bool canReset);

        void setColour(Colour newColour) override;

        Colour getColour() const override;

        void resetToDefault() override;
    };

    ScopedPointer<ColourPropEditorComponent> colourPropEditor;
};

#endif  // BCMMISC_H_INCLUDED

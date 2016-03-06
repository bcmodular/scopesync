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

/* =========================================================================
 * ColourPropertyComponent: PropertyComponent for choosing Colour values
 *                          (appropriated from the Introjucer)
 */
class ColourPropertyComponent : public PropertyComponent
{
public:
    ColourPropertyComponent(const String& name,
                            const bool canReset)
        : PropertyComponent (name)
    {
        colourPropEditor = new ColourPropEditorComponent(this, canReset);
        addAndMakeVisible(colourPropEditor);
    }

    virtual void   setColour(Colour newColour) = 0;
    virtual Colour getColour() const = 0;
    virtual void   resetToDefault() = 0;

    void refresh()
    {
        ((ColourPropEditorComponent*)getChildComponent(0))->refresh();
    }

    class ColourEditorComponent : public Component,
                                  public ChangeListener
    {
    public:
        ColourEditorComponent (const bool canReset)
            : canResetToDefault (canReset)
        {}

        void paint (Graphics& g)
        {
            g.fillAll (Colours::grey);

            g.fillCheckerBoard (getLocalBounds().reduced (2, 2),
                                10, 10,
                                Colour (0xffdddddd).overlaidWith (colour),
                                Colour (0xffffffff).overlaidWith (colour));

            g.setColour (Colours::white.overlaidWith (colour).contrasting());
            g.setFont (Font (getHeight() * 0.6f, Font::bold));
            g.drawFittedText (colour.toDisplayString (true),
                              2, 1, getWidth() - 4, getHeight() - 1,
                              Justification::centred, 1);
        }

        virtual void setColour (Colour newColour) = 0;
        virtual void resetToDefault() = 0;
        virtual Colour getColour() const = 0;

        void refresh()
        {
            const Colour col (getColour());

            if (col != colour)
            {
                colour = col;
                repaint();
            }
        }

        void mouseDown (const MouseEvent&)
        {
            CallOutBox::launchAsynchronously (new ColourSelectorComp (this, canResetToDefault),
                                              getScreenBounds(), nullptr);
        }

        void changeListenerCallback (ChangeBroadcaster* source)
        {
            const ColourSelector* const cs = (const ColourSelector*) source;

            if (cs->getCurrentColour() != getColour())
                setColour(cs->getCurrentColour());
        }

        class ColourSelectorComp : public Component,
                                   public ButtonListener
        {
        public:
            ColourSelectorComp (ColourEditorComponent* owner_,
                                const bool canReset)
                : owner (owner_),
                  defaultButton ("Reset to Default")
            {
                addAndMakeVisible (selector);
                selector.setName ("Colour");
                selector.setCurrentColour (owner->getColour());
                selector.addChangeListener (owner);

                if (canReset)
                {
                    addAndMakeVisible (defaultButton);
                    defaultButton.addListener (this);
                }

                setSize (300, 400);
            }

            void resized()
            {
                if (defaultButton.isVisible())
                {
                    selector.setBounds (0, 0, getWidth(), getHeight() - 30);
                    defaultButton.changeWidthToFitText (22);
                    defaultButton.setTopLeftPosition (10, getHeight() - 26);
                }
                else
                {
                    selector.setBounds (getLocalBounds());
                }
            }

            void buttonClicked (Button*)
            {
                owner->resetToDefault();
                owner->refresh();
                selector.setCurrentColour (owner->getColour());
            }

        private:
            class ColourSelectorWithSwatches : public ColourSelector
            {
            public:
                ColourSelectorWithSwatches()
                {
                }

                int getNumSwatches() const override
                {
                    return UserSettings::getInstance()->swatchColours.size();
                }

                Colour getSwatchColour (int index) const override
                {
                    return UserSettings::getInstance()->swatchColours [index];
                }

                void setSwatchColour (int index, const Colour& newColour) const override
                {
                    UserSettings::getInstance()->swatchColours.set(index, newColour);
                }
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
                                  const bool canReset)
            : ColourEditorComponent(canReset),
              owner (owner_)
        {}

        void setColour (Colour newColour) override
        {
            owner->setColour(newColour);
        }

        Colour getColour() const override
        {
            return owner->getColour();
        }

        void resetToDefault()
        {
            owner->resetToDefault();
        }
    };

    ScopedPointer<ColourPropEditorComponent> colourPropEditor;
};

#endif  // BCMMISC_H_INCLUDED

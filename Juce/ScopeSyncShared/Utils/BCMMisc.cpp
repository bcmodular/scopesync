/*
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

#include "BCMMisc.h"
#include "../Resources/Icons.h"
#include "../Core/Global.h"

String createAlphaNumericUID()
{
    String uid;
    const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    Random r;

    uid << chars [r.nextInt (52)]; // make sure the first character is always a letter

    for (int i = 5; --i >= 0;)
    {
        r.setSeedRandomly();
        uid << chars [r.nextInt (62)];
    }

    return uid;
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabelComp);
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

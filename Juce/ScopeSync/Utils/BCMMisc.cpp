/*
 * Miscellaneous utility classes & methods
 *
 *  (C) Copyright 2014 bcmodular (http://www.bcmodular.co.uk/)
 *
 * This file is part of ScopeSync.
 *
 * ScopeSync is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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
#include "BCMMath.h"

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

#ifdef __DLL_EFFECT__
#ifndef NOGDI
	#define NOGDI
#endif
#include <WinSock2.h>
#include <WS2tcpip.h>

String ipAddressFromHostName(StringRef hostName, StringRef port)
{
	String ipAddress("127.0.0.1");

	// Look up IP address from address
    struct addrinfo hints;
    zerostruct (hints);

    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags    = AI_NUMERICSERV;

    struct addrinfo* info = nullptr;

    if (getaddrinfo(String(hostName).toRawUTF8(), String(port).toRawUTF8(), &hints, &info) == 0)
    {
	    if (info != nullptr)
		{
			for (auto* i = info; i != nullptr; i = i->ai_next)
			{
				if (i->ai_family == AF_INET)
				{
					auto sockaddr_ipv4 = reinterpret_cast<struct sockaddr_in*>(i->ai_addr);
					ipAddress = String(inet_ntoa(sockaddr_ipv4->sin_addr));

					DBG("ipAddressFromHostName: IP Address - " + ipAddress);
				}
			}

			freeaddrinfo(info);
		}
	}

	return ipAddress;
}
#endif // __DLL_EFFECT__

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
    {
        double newDouble    = newText.getDoubleValue();
        double roundedValue = roundDouble(newDouble);
        
        if (abs(newDouble - roundedValue) < FLT_EPSILON)
            newText = String(roundToInt(newDouble));
        else
            newText = String(newDouble);
    }
    else if (!allowedToBeBlank)
        errorText = "Must provide a valid floating point value";
    
    if (errorText.isNotEmpty())
        textEditor->setError(errorText);
    else
        textEditor->clearError();

    NumericProperty::setText(newText);
}

ColourPropertyComponent::ColourPropertyComponent(const String& name, const bool canReset): PropertyComponent (name)
{
    colourPropEditor = new ColourPropEditorComponent(this, canReset);
    addAndMakeVisible(colourPropEditor);
}

void ColourPropertyComponent::refresh()
{
    static_cast<ColourPropEditorComponent*>(getChildComponent(0))->refresh();
}

ColourPropertyComponent::ColourEditorComponent::ColourEditorComponent(const bool canReset): canResetToDefault (canReset) {}

void ColourPropertyComponent::ColourEditorComponent::paint(Graphics& g)
{
    g.fillAll (Colours::grey);

	g.fillCheckerBoard (getLocalBounds().reduced (2, 2).toFloat(),
						10.0f, 10.0f,
						Colour (0xffdddddd).overlaidWith (colour),
						Colour (0xffffffff).overlaidWith (colour));

    g.setColour (Colours::white.overlaidWith (colour).contrasting());
    g.setFont (Font (getHeight() * 0.6f, Font::bold));
    g.drawFittedText (colour.toDisplayString (true),
                      2, 1, getWidth() - 4, getHeight() - 1,
                      Justification::centred, 1);
}

void ColourPropertyComponent::ColourEditorComponent::refresh()
{
    const Colour col (getColour());

    if (col != colour)
    {
        colour = col;
        repaint();
    }
}

void ColourPropertyComponent::ColourEditorComponent::mouseDown(const MouseEvent&)
{
    CallOutBox::launchAsynchronously (new ColourSelectorComp (this, canResetToDefault),
                                      getScreenBounds(), nullptr);
}

void ColourPropertyComponent::ColourEditorComponent::changeListenerCallback(ChangeBroadcaster* source)
{
    const ColourSelector* const cs = static_cast<const ColourSelector*>(source);

    if (cs->getCurrentColour() != getColour())
        setColour(cs->getCurrentColour());
}

ColourPropertyComponent::ColourEditorComponent::ColourSelectorComp::ColourSelectorComp(ColourEditorComponent* owner_, const bool canReset): owner (owner_),
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

void ColourPropertyComponent::ColourEditorComponent::ColourSelectorComp::resized()
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

void ColourPropertyComponent::ColourEditorComponent::ColourSelectorComp::buttonClicked(Button*)
{
    owner->resetToDefault();
    owner->refresh();
    selector.setCurrentColour (owner->getColour());
}

ColourPropertyComponent::ColourEditorComponent::ColourSelectorComp::ColourSelectorWithSwatches::ColourSelectorWithSwatches() = default;

int ColourPropertyComponent::ColourEditorComponent::ColourSelectorComp::ColourSelectorWithSwatches::getNumSwatches() const
{
    return userSettings->swatchColours.size();
}

Colour ColourPropertyComponent::ColourEditorComponent::ColourSelectorComp::ColourSelectorWithSwatches::getSwatchColour(int index) const
{
    return userSettings->swatchColours [index];
}

void ColourPropertyComponent::ColourEditorComponent::ColourSelectorComp::ColourSelectorWithSwatches::setSwatchColour(int index, const Colour& newColour)
{
    userSettings->swatchColours.set(index, newColour);
}

ColourPropertyComponent::ColourPropEditorComponent::ColourPropEditorComponent(ColourPropertyComponent* const owner_, const bool canReset): ColourEditorComponent(canReset), owner (owner_) {}

void ColourPropertyComponent::ColourPropEditorComponent::setColour(Colour newColour)
{
    owner->setColour(newColour);
}

Colour ColourPropertyComponent::ColourPropEditorComponent::getColour() const
{
    return owner->getColour();
}

void ColourPropertyComponent::ColourPropEditorComponent::resetToDefault()
{
    owner->resetToDefault();
}

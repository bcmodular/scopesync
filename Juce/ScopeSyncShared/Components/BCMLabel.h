/**
 * The BCModular version of Juce's Label, which adds the ability
 * to be created from an XML definition, as well as being tied into
 * the ScopeSync parameter system
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

#ifndef BCMLABEL_H_INCLUDED
#define BCMLABEL_H_INCLUDED

class ScopeSyncGUI;
class LabelProperties;

#include <JuceHeader.h>
#include "../Core/BCMParameter.h"
#include "../Components/BCMComponentBounds.h"
#include "BCMWidget.h"

class BCMLabel : public Label,
                 public BCMParameterWidget
{
public:
    BCMLabel(String& name, String& text, ScopeSyncGUI& owner);
    ~BCMLabel();

    void applyProperties(LabelProperties& properties);
    const Identifier getComponentType() const override;

    void handleValueChanged(Value& valueThatChanged);

private:
    
    class BCMLabelValueListener : public Value::Listener
    {
    public:
        BCMLabelValueListener(BCMLabel& label) : parent(label) {};
        ~BCMLabelValueListener() {};

        BCMLabel& parent;
        void valueChanged (Value& v)  { parent.handleValueChanged(v); }
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BCMLabelValueListener)
    };

    ScopedPointer<BCMLabelValueListener> valueListener;
    
    void mouseDown(const MouseEvent& event) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BCMLabel);
};

#endif  // BCMLABEL_H_INCLUDED

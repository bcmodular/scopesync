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

//==============================================================================
class PropertyListBuilder
{
public:
    PropertyListBuilder() {}

    void add (PropertyComponent* propertyComp)
    {
        components.add (propertyComp);
    }

    void add (PropertyComponent* propertyComp, const String& tooltip)
    {
        propertyComp->setTooltip (tooltip);
        add (propertyComp);
    }

    void addSearchPathProperty (const Value& value, const String& name, const String& mainHelpText)
    {
        add (new TextPropertyComponent (value, name, 16384, true),
             mainHelpText + " Use semi-colons or new-lines to separate multiple paths.");
    }

    void setPreferredHeight (int height)
    {
        for (int j = components.size(); --j >= 0;)
            components.getUnchecked(j)->setPreferredHeight (height);
    }

    Array <PropertyComponent*> components;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertyListBuilder)
};

#endif  // BCMMISC_H_INCLUDED

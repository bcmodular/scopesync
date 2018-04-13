/**
 * Intermediary class for extracting BCMComponent definitions from
 * XML layout files
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

#ifndef COMPONENTPROPERTIES_H_INCLUDED
#define COMPONENTPROPERTIES_H_INCLUDED

#include <JuceHeader.h>
#include "WidgetProperties.h"

class ComponentProperties : public WidgetProperties
{
public:
    // Fall-back constructor in case no default XML supplied
    ComponentProperties();

    // Constructor for use when setting up defaults
    ComponentProperties(XmlElement& componentXML);
    
    // Constructor for use when overriding defaults for a specific Component
    ComponentProperties(XmlElement& componentXML, ComponentProperties& parentComponentProperties);
    ~ComponentProperties();

    String             backgroundColour;
    String             backgroundImageFileName;
    RectanglePlacement backgroundImagePlacement;
    
private:
    void initialise();
    void copyProperties(ComponentProperties& parentComponentProperties);
    void setValuesFromXML(XmlElement& componentXML);
    static void getRectanglePlacementFromString(String string, RectanglePlacement& placement);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComponentProperties);
};

#endif  // COMPONENTPROPERTIES_H_INCLUDED

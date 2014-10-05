/**
 * Intermediary class for extracting BCMTabbedComponent definitions from
 * XML layout files
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

#ifndef TABBEDCOMPONENTPROPERTIES_H_INCLUDED
#define TABBEDCOMPONENTPROPERTIES_H_INCLUDED

#include <JuceHeader.h>
#include "../Components/BCMComponentBounds.h"

class TabbedComponentProperties
{
public:
    // Fall-back constructor in case no default XML supplied
    TabbedComponentProperties();
    
    // Constructor for use when setting up defaults
    TabbedComponentProperties(XmlElement& tabbedComponentXML);

    // Constructor for use when overriding defaults for a specific BCMTabbedComponent
    TabbedComponentProperties(XmlElement& tabbedComponentXML, TabbedComponentProperties& parentTabbedComponentXML);
    ~TabbedComponentProperties();

    String                       name;
    String                       id;
    BCMComponentBounds           bounds;
    int                          tabBarDepth;
    String                       bcmLookAndFeelId;
    TabbedButtonBar::Orientation tabBarOrientation;
    bool                         showDropShadow;
    Identifier                   mappingParentType;
    String                       mappingParent;
private:
    void initialise();
    void copyProperties(TabbedComponentProperties& parentTabbedComponentXML);
    void setValuesFromXML(XmlElement& tabbedComponentXML);

    void getOrientationFromXml(const XmlElement& xml, TabbedButtonBar::Orientation& orientation);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabbedComponentProperties);
};

#endif  // TABBEDCOMPONENTPROPERTIES_H_INCLUDED

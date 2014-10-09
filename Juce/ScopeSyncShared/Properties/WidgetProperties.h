/**
 * Base class for extracting BCMWidget definitions from
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

#ifndef WIDGETPROPERTIES_H_INCLUDED
#define WIDGETPROPERTIES_H_INCLUDED

#include <JuceHeader.h>
#include "../Components/BCMComponentBounds.h"

class WidgetProperties
{
public:
    // Fall-back constructor in case no default XML supplied
    WidgetProperties();

    // Constructor for use when setting up defaults
    WidgetProperties(XmlElement& widgetXML);

    // Constructor for use when overriding defaults
    WidgetProperties(XmlElement& widgetXML, WidgetProperties& parentWidgetProperties);

    ~WidgetProperties();

    String             name;
    String             id;
    BCMComponentBounds bounds;
    String             bcmLookAndFeelId;
    Identifier         mappingParentType;
    String             mappingParent;  
    bool               noStyleOverride;

private:
    void initialiseWidget();
    void copyWidgetProperties(const WidgetProperties& parentWidgetProperties);
    void setWidgetValuesFromXML(const XmlElement& widgetXML);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WidgetProperties);
};

#endif  // WIDGETPROPERTIES_H_INCLUDED

/**
 * Intermediary class for extracting Tab definitions from
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

#ifndef TABPROPERTIES_H_INCLUDED
#define TABPROPERTIES_H_INCLUDED

#include <JuceHeader.h>

class TabProperties
{
public:
    // Fall-back constructor in case no default XML supplied
    TabProperties();

    // Constructor for use when setting up defaults
    TabProperties(XmlElement& tabXML);

    // Constructor for use when overriding defaults for a specific Tab
    TabProperties(XmlElement& tabXML, TabProperties& parentTabXML);

    ~TabProperties();

    String name;
    String backgroundColour;
    int    idx;
    String bcmLookAndFeelId;

private:
    void initialise();
    void copyProperties(TabProperties& parentTabXML);
    void setValuesFromXML(XmlElement& tabXML);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabProperties);
};




#endif  // TABPROPERTIES_H_INCLUDED

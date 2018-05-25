/**
 * A simple Image class, for drawing onto a Component
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

#ifndef BCMIMAGE_H_INCLUDED
#define BCMIMAGE_H_INCLUDED
#include <JuceHeader.h>
#include "BCMGraphic.h"

class BCMImage : public BCMGraphic
{
public:
    BCMImage(XmlElement& xml);
    ~BCMImage();
    
    enum StretchMode{stretchedToFit, maintainAspect, maintainAspectOnlyReduce};

    String      fileName;
    StretchMode stretchMode;
    float       opacity;

private:
	static StretchMode getStretchModeFromXml(XmlElement& xml);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BCMImage);
};



#endif  // BCMIMAGE_H_INCLUDED

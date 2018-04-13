/**
 * A simple Rectangle class, for drawing onto a Component
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

#ifndef BCMRECTANGLE_H_INCLUDED
#define BCMRECTANGLE_H_INCLUDED
#include <JuceHeader.h>
#include "BCMGraphic.h"

class BCMRectangle : public BCMGraphic
{
public:
    BCMRectangle(XmlElement& xml);
    ~BCMRectangle();

    float  cornerSize;
    String fillColour;
    float  outlineThickness;
    String outlineColour;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BCMRectangle);
};



#endif  // BCMRECTANGLE_H_INCLUDED

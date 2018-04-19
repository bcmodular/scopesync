/**
 * Data structure for storing information about position and size
 * of a component
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

#ifndef BCMCOMPONENTBOUNDS_H_INCLUDED
#define BCMCOMPONENTBOUNDS_H_INCLUDED
#include <JuceHeader.h>

class BCMComponentBounds
{
public:
    BCMComponentBounds()  {};
    ~BCMComponentBounds() {};

    void copyValues(BCMComponentBounds& targetBounds) const
    {
        targetBounds.width                   = width;
        targetBounds.height                  = height;
        targetBounds.x                       = x;
        targetBounds.y                       = y;
        targetBounds.relativeRectangleString = relativeRectangleString;
        targetBounds.borderSize              = borderSize;
        targetBounds.justificationFlags      = justificationFlags;
        targetBounds.onlyReduceInSize        = onlyReduceInSize;
        targetBounds.boundsType              = boundsType;
    }

    int                  width;
    int                  height;
    int                  x;
    int                  y;
    String               relativeRectangleString;
    BorderSize<int>      borderSize; 
    Justification::Flags justificationFlags;
    bool                 onlyReduceInSize;

    enum BoundsType {standard, relativeRectangle, inset, fillParent};

    BoundsType boundsType;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BCMComponentBounds);
};

#endif  // BCMCOMPONENTBOUNDS_H_INCLUDED

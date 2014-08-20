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

#ifndef BCMCOMPONENTBOUNDS_H_INCLUDED
#define BCMCOMPONENTBOUNDS_H_INCLUDED
#include <JuceHeader.h>

class BCMComponentBounds
{
public:
    int                  width;
    int                  height;
    int                  x;
    int                  y;
    String               relativeRectangleString;
    BorderSize<int>      borderSize; 
    Justification::Flags justificationFlags;
    bool                 onlyReduceInSize;

    enum BoundsType {standard, relativeRectangle, inset};

    BoundsType boundsType;
};

#endif  // BCMCOMPONENTBOUNDS_H_INCLUDED

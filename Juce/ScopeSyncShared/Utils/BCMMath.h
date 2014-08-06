/**
 * BCModular Maths utilities
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

#ifndef __BCMMATH_HEADER__
#define __BCMMATH_HEADER__

#include <JuceHeader.h>

namespace BCMMath
{
    inline double scaleDouble(double sourceMin, double sourceMax, double targetMin, double targetMax, double value)
    {
        if (value <= sourceMin)
        {
            return targetMin;
        }
        else if (value >= sourceMax)
        {
            return targetMax;
        }
        else
        {
            double sourceRange  = sourceMax - sourceMin;
            double sourceOffset = value - sourceMin;
            double targetRange  = targetMax - targetMin;
            double targetOffset = (targetRange * sourceOffset) / sourceRange;

            return targetOffset + targetMin;
        }


    }

	inline double roundDouble(float num) {
		return (num > 0.0) ? floor(num + 0.5) : ceil(num - 0.5);
	}
};

#ifndef __BCMMATH_HEADER_NO_NAMESPACE__
using namespace BCMMath;
#endif

#endif

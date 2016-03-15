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

    inline double scopeIntToDouble(int scopeInt)
    {
        if (scopeInt > 0)
            return scopeInt / static_cast<double>(0x7FFFFFFF);
        else if (scopeInt < 0)
            return scopeInt / static_cast<double>(0x80000000);
        else
            return 0.0;
    }

    inline int doubleToScopeInt(double doubleToConvert)
    {
        if (doubleToConvert > 0.0)
            return roundDoubleToInt(doubleToConvert * 0x7FFFFFFF);
        else if (doubleToConvert < 0.0)
            return roundDoubleToInt(doubleToConvert * 0x80000000);
        else
            return 0;
    }

	inline double roundDouble(double num)
    {
		return (num > 0.0) ? floor(num + 0.5) : ceil(num - 0.5);
	}

    inline void skewValue(double& valueToSkew, double skewFactor, double minValue, double maxValue, bool invert)
    {
        DBG("BCMMath::skewValue - valueToSkew: " + String(valueToSkew) + ", skewFactor: " + String(skewFactor) + ", minValue: " + String(minValue) + ", maxValue: " + String(maxValue) + ", invert: " + String(invert));
        
        if (skewFactor != 1.0)
        {
            if (invert)
            {
                const double normalisedValue = (valueToSkew - minValue) / (maxValue - minValue);
                DBG("BCMMath::skewValue - normalisedValue: " + String(normalisedValue));
                valueToSkew = (pow(normalisedValue, skewFactor) * (maxValue - minValue)) + minValue;
                DBG("BCMMath::skewValue - new valueToSkew: " + String(valueToSkew));
            }
            else
            {
                const double normalisedValue = (valueToSkew - minValue) / (maxValue - minValue);
                DBG("BCMMath::skewValue - normalisedValue: " + String(normalisedValue));
                valueToSkew = (exp(log(normalisedValue) / skewFactor) * (maxValue - minValue)) + minValue;
                DBG("BCMMath::skewValue - new valueToUnSkew: " + String(valueToSkew));
            }
        }
    }
};

#ifndef __BCMMATH_HEADER_NO_NAMESPACE__
using namespace BCMMath;
#endif

#endif

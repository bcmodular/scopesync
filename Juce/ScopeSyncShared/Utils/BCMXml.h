/**
 * BCModular XML utilities
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

#ifndef BCMXML_H_INCLUDED
#define BCMXML_H_INCLUDED

#include <JuceHeader.h>

namespace BCMXml
{
    inline String floatArrayToString(const Array<float>& floatData, int numFloat)
    {
        //Return String of multiple float values separated by commas
        String result = "";

        if (numFloat < 1)
            return result;

        for (int i = 0; i < (numFloat - 1); i++)
            result << String(floatData[i]) << ",";

        result << String(floatData[numFloat - 1]);
    
        return result;
    }

    inline int stringToFloatArray(const String& floatCSV, Array<float>& floatData, int maxNumFloat)
    {
        //Return is number of floats copied to the fData array
        //-1 if there were more in the string than maxNumFloat
        StringArray tokenizer;

        int tokenCount  = tokenizer.addTokens(floatCSV, ",", "");
        int resultCount = (maxNumFloat <= tokenCount) ? maxNumFloat : tokenCount;

        for (int i = 0; i < resultCount; i++)               //only go as far as resultCount for valid data
            floatData.set(i, tokenizer[i].getFloatValue()); //fill data using String class float conversion

        return ((tokenCount <= maxNumFloat) ? resultCount : -1);
    }
}

#ifndef __BCMXML_HEADER_NO_NAMESPACE__
using namespace BCMXml;
#endif

#endif  // BCMXML_H_INCLUDED

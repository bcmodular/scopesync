/**
 * Class representing an individual ScopeSync Control being managed
 * through the 32-bit Audio stream
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

#include "ScopeSyncControl.h"
#include "../Utils/BCMMath.h"

const float ScopeSyncControl::destValueScaler  = 256.0f;
const int   ScopeSyncControl::maxDeadTimeCount = 10;

ScopeSyncControl::ScopeSyncControl(int index)
{
    initialiseValues(index);
}

ScopeSyncControl::~ScopeSyncControl() {}

void ScopeSyncControl::initialiseValues(int index)
{
    destVal        = (index + 1 + (2 * (index / 16))) / destValueScaler;
    dataVal        = 0.0f;     
    dataValLast    = 0.0f;
    deadTimeCount  = 0;
    snapshot       = false;
}

void ScopeSyncControl::setValue(float newValue, bool setDeadTime, bool dontSend)
{
    dataVal = newValue;
    
    if (setDeadTime)
        deadTimeCount = maxDeadTimeCount;
    else
        deadTimeCount = 0;

    if (dontSend)
        dataValLast = newValue;
    
    //DBG("ScopeSyncControl::setValue: ctrlCode=" + destCode + ", ctrlValue=" + String(newValue));
}

int ScopeSyncControl::getControlIdxFromDestValue(const float destValue)
{
    float dest = destValue * destValueScaler;
    int index  = -1;

    float destRounded = (float)roundDouble(dest);

    if (abs(dest - destRounded) < FLT_EPSILON)
    {
        int intDest = (int)destRounded;
        index = intDest - 1 - (2 * ((intDest / 18)));
    }
    
    return index;
}

bool ScopeSyncControl::dataChanged()
{
    return (dataVal != dataValLast) || snapshot;
}

void ScopeSyncControl::markDataSent()
{
    dataValLast = dataVal;
    snapshot    = false;
}

void ScopeSyncControl::decDeadTimeCount()
{
    if (deadTimeCount > 0)
        deadTimeCount--;
}

void ScopeSyncControl::setSnapshot()
{
    snapshot = true;
}
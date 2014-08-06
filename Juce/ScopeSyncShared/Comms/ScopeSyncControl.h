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

#ifndef SCOPESYNCCONTROL_H_INCLUDED
#define SCOPESYNCCONTROL_H_INCLUDED

#include <JuceHeader.h>

class ScopeSyncControl {

public:

    ScopeSyncControl(int index);
    ~ScopeSyncControl();
    
    // Identify which Control is being modified by interpreting
    // the floating point value passed in the Audio stream
    static int getControlIdxFromDestValue(const float destValue);

    // Set current value for a Control
    void setValue(float newValue, bool setDeadTime, bool dontSend);

    // Indicates whether the Control's value has changed since the
    // last time it was marked as sent
    bool dataChanged();

    // Mark the Control's data value as having been sent via ScopeSync
    void markDataSent();

    // Decrement the "dead-time" counter. This is used to prevent data
    // feedback loops
    void decDeadTimeCount();

    // Mark this control as being part of the next snapshot
    void setSnapshot();

    float  destVal;          // Value to be put into the audio stream to identify the Control
    float  dataVal;          // The current value of the Control
    float  dataValLast;      // The data value previously sent in the audio stream
    int    deadTimeCount;    // A counter that's used to prevent control change feedback loops
    bool   snapshot;         // Indicates whether a snapshot has been requested. Used to force re-send of all Control values

private:
    // Scale factor to be used when multiplying/dividing the floating point value found in the
    // Audio stream. This is used as very small floating point values don't accurately convert
    // into integers, causing issues with identifying the Control destination
    static const float destValueScaler;

    // The number of signal vectors that ScopeSyncAudio will ignore updates coming from Scope
    // for a given control after it has processed the last one from the plugin
    static const int   maxDeadTimeCount;
    
    // Initialise the member variables
    void initialiseValues(int index);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeSyncControl);
};

#endif  // SCOPESYNCCONTROL_H_INCLUDED

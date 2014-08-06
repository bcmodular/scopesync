/**
 * ScopeSync Class for transmitting MIDI and control data to Sonic|Core's
 * Scope system via 32-bit audio.
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

#ifndef SCOPESYNCAUDIO_H_INCLUDED
#define SCOPESYNCAUDIO_H_INCLUDED

class ScopeSyncControl;

#include <JuceHeader.h>

class ScopeSyncAudio {

public:
    
    ScopeSyncAudio();
    ~ScopeSyncAudio();
    
    // Process a block of signal data, typically provided by the plugin host
    void processBlock(AudioSampleBuffer& buffer);

    // Update the data value for a Control
    void setControlValue(int index, float ctrlValue);
    
    // Mark all control values as needing to be sent in the next update(s)
    void snapshot();

    CriticalSection controlUpdateLock;
    
    // Updates received from the ScopeSync input
    Array<std::pair<int,float>, CriticalSection> controlUpdates; 

private:
    // The ScopeSync controls being monitored
    OwnedArray<ScopeSyncControl> controls;

    static const int numControls; // Number of Controls being tracked
    int lastSigvControlIdx;	      // Array position of the last Control whose change was sent 

    // Create Control array
    void setupControls();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeSyncAudio);
};

#endif  // SCOPESYNCAUDIO_H_INCLUDED
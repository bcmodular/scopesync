/**
 * Main ScopeSync Class for transmitting MIDI and control data to Sonic|Core's
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

#include "ScopeSyncAudio.h"
#include "ScopeSyncControl.h"

const int ScopeSyncAudio::numControls = 128;

ScopeSyncAudio::ScopeSyncAudio()
{
    setupControls();
}

ScopeSyncAudio::~ScopeSyncAudio() {}

void ScopeSyncAudio::setupControls()
{
    lastSigvControlIdx = numControls - 1;
    
    for (int i = 0; i < numControls; i++)
        controls.add(new ScopeSyncControl(i));
}

void ScopeSyncAudio::snapshot()
{
    DBG("ScopeSyncAudio::snapshot");

    for (int i = 0; i < numControls; i++)
    {
        controls[i]->setSnapshot();
    }
}

void ScopeSyncAudio::processBlock(AudioSampleBuffer& buffer)
{
    int numSamples = buffer.getNumSamples();
    
    float* dataValue = buffer.getWritePointer(0); // Buffer for the Control Data
    float* destValue = buffer.getWritePointer(1); // Buffer for the Control Destination
    
    int ctrlIdx = -1;

    for (int i = 0; i < numSamples; i++)
    {
        // Read current sample values to see if there are any control updates
        // to add to the queue
        if (destValue[i] > 0.0f)
        {
            int index = ScopeSyncControl::getControlIdxFromDestValue(destValue[i]);

            if (index != -1)
            {
                ScopeSyncControl* control = controls[index];

                if (control->deadTimeCount == 0)
                {
                    control->setValue(dataValue[i], false, true);
                    
                    // Add it to the queue for sending to host and GUI
                    const ScopedLock cuLock(controlUpdateLock);
                    std::pair<int,float> controlUpdate = std::make_pair(index, dataValue[i]);
                    controlUpdates.add(controlUpdate);
                }
            }
        }
    }

    buffer.clear();
    
    for (int i = 0; i < numSamples; i++)
    {
        // Loop through the Controls, starting at the one after the 
        // last one we looked at
        while (ctrlIdx != lastSigvControlIdx)
        {
            if (ctrlIdx == -1)
            {
                // We've entered for the first time, so start with the next one
                // after the last one reached in the previous signal vector
                ctrlIdx = (lastSigvControlIdx + 1) % numControls;
            }
            else
            {
                // Increment, but need to loop around the Controls
                ctrlIdx = (ctrlIdx + 1) % numControls;
            }

            ScopeSyncControl* control = controls[ctrlIdx];

            if (control->dataChanged())
            {
                dataValue[i] = control->dataVal;
                destValue[i] = control->destVal;

                control->markDataSent();
                break;
            }
        }

        if (ctrlIdx == lastSigvControlIdx)
        {
            break;
        }
	}

    lastSigvControlIdx = ctrlIdx;

    for (int i = 0; i < numControls; i++)
    {
        controls[i]->decDeadTimeCount();
    }
}

void ScopeSyncAudio::setControlValue(int index, float ctrlValue)
{
    if (index != -1 && index < numControls)
        controls[index]->setValue(ctrlValue, true, false);
    else
        DBG("ScopeSyncAudio::setControlValue - value sent for invalid index: " + String(index));
}

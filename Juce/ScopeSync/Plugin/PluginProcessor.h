/**
 * The main application wrapper in the plugin AppContext. This is
 * the class of object that is returned to a plugin host as the
 * plugin Processor. All of the "real" work is done by the
 * ScopeSync object it hosts.
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

#ifndef __DLL_EFFECT__

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED
//#include <vld.h>
class PluginGUI;
#include <JuceHeader.h>
#include "../Core/ScopeSync.h"

class PluginProcessor  : public AudioProcessor
{
public:
    PluginProcessor();
    ~PluginProcessor();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages) override;

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const String getName() const override;

    bool   acceptsMidi() const override;
    bool   producesMidi() const override;
    double getTailLengthSeconds() const override;

    int    getNumPrograms() override;
    int    getCurrentProgram() override;
    void   setCurrentProgram (int index) override;
    const  String getProgramName (int index) override;
    void   changeProgramName (int index, const String& newName) override;

    void   getStateInformation (MemoryBlock& destData) override;
    void   setStateInformation (const void* data, int sizeInBytes) override;
    
    void updateListeners(int index, float newHostValue);
    
    ScopeSync& getScopeSync() const { return *scopeSync; };
    void setGUIEnabled(bool shouldBeEnabled);

private:
    ScopedPointer<ScopeSync> scopeSync;
    WeakReference<PluginGUI> pluginGUI;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};

#endif  // PLUGINPROCESSOR_H_INCLUDED
#endif  // __DLL_EFFECT__

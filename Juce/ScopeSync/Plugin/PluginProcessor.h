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

    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

    AudioProcessorEditor* createEditor();
    bool hasEditor() const;

    const String getName() const;

    int getNumParameters();

    float getParameter (int index);
    void setParameter (int index, float newValue);

    const String getParameterName (int index);
    const String getParameterText (int index);

    bool   acceptsMidi() const;
    bool   producesMidi() const;
    double getTailLengthSeconds() const;

    int    getNumPrograms();
    int    getCurrentProgram();
    void   setCurrentProgram (int index);
    const  String getProgramName (int index);
    void   changeProgramName (int index, const String& newName);

    void   getStateInformation (MemoryBlock& destData);
    void   setStateInformation (const void* data, int sizeInBytes);
    
    String floatArrayToString(const Array<float>& floatData, int numFloat);
    int    stringToFloatArray(const String& sFloatCSV, Array<float>& floatData, int maxNumFloat);

    void updateListeners(int index, float newHostValue);
    
    ScopeSync& getScopeSync() const { return *scopeSync; };
    void setGUIEnabled(bool shouldBeEnabled);

private:
    ScopedPointer<ScopeSync> scopeSync;
    WeakReference<PluginGUI> pluginGUI;

    void readStateInformation(XmlElement& data);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};

#endif  // PLUGINPROCESSOR_H_INCLUDED
#endif  // __DLL_EFFECT__

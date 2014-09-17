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

#include "PluginProcessor.h"

#include "../../ScopeSyncShared/Utils/BCMMath.h"
#include "../../ScopeSyncShared/Utils/BCMXml.h"
#include "PluginGUI.h"
#include "../../ScopeSyncShared/Resources/ImageLoader.h"
#include "../../ScopeSyncShared/Resources/Icons.h"
#include "../../ScopeSyncShared/Components/UserSettings.h"
#include "../../ScopeSyncShared/Core/ScopeSyncApplication.h"

const int PluginProcessor::timerInterval   = 20;

PluginProcessor::PluginProcessor()
{
    scopeSync = new ScopeSync(this);
    startTimer(timerInterval);
}

PluginProcessor::~PluginProcessor()
{
    stopTimer();
    scopeSync->unload();
    scopeSync = nullptr;

    if (ScopeSync::getNumScopeSyncInstances() == 0)
    {
        Icons::deleteInstance();
        ImageLoader::deleteInstance();
        UserSettings::deleteInstance();
    }
}

const String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

int PluginProcessor::getNumParameters()
{
    int numParameters = scopeSync->getNumParametersForHost();
    
    //DBG("PluginProcessor::getNumParameters - " + String(numParameters));
    return numParameters;
}

float PluginProcessor::getParameter (int index)
{
    float parameterValue = scopeSync->getParameterHostValue(index);
    
    DBG("PluginProcessor::getParameter - " + String(parameterValue));
    return parameterValue;
}

void PluginProcessor::setParameter(int index, float newValue)
{
    DBG("PluginProcessor::setParameter - index: " + String(index) + ", newValue: " + String(newValue));
    scopeSync->setParameterFromHost(index, newValue);
}

const String PluginProcessor::getParameterName(int index)
{
    String parameterName = String::empty;
    scopeSync->getParameterNameForHost(index, parameterName);
    
    DBG("PluginProcessor::getParameterName - " + parameterName);
    return parameterName;
}

const String PluginProcessor::getParameterText (int index)
{
    String parameterText = String::empty;
    scopeSync->getParameterText(index, parameterText);

    DBG("PluginProcessor::getParameterText - " + parameterText);
    return parameterText;
}

const String PluginProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String PluginProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool PluginProcessor::isInputChannelStereoPair (int index) const
{
    (void)index;
    return true;
}

bool PluginProcessor::isOutputChannelStereoPair (int index) const
{
    (void)index;
    return true;
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;
}

int PluginProcessor::getCurrentProgram()
{
    return 1;
}

void PluginProcessor::setCurrentProgram (int index)
{
    (void)index;
}

const String PluginProcessor::getProgramName (int index)
{
    (void)index;
    return String::empty;
}

void PluginProcessor::changeProgramName (int index, const String& newName)
{
    (void)index;
    (void)newName;
}

void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    (void)sampleRate;
    (void)samplesPerBlock;
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void PluginProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    scopeSync->processBlock(buffer, midiMessages);

    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
}

bool PluginProcessor::hasEditor() const { return true; }

AudioProcessorEditor* PluginProcessor::createEditor()
{
    pluginGUI = new PluginGUI(this);
    pluginGUI->setVisible(true);
    return pluginGUI;
}

void PluginProcessor::getStateInformation (MemoryBlock& destData)
{
    // First put current parameter values into storage
    scopeSync->storeParameterValues();
    
    XmlElement root("root");

    XmlElement* parameterValues = new XmlElement(scopeSync->getParameterValueStore());
    root.addChildElement(parameterValues);
    
    XmlElement* configurationFilePathXml = root.createNewChildElement("configurationfilepath");
    configurationFilePathXml->addTextElement(scopeSync->getConfigurationFile().getFullPathName());

    copyXmlToBinary(root, destData);

    DBG("PluginProcessor::getStateInformation - Storing XML: " + root.createDocument(""));
}

void PluginProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore parameters from memory block whose contents will have been 
    // created by the getStateInformation() call.
    ScopedPointer<XmlElement> root = getXmlFromBinary(data, sizeInBytes);

    if (root)
    {
        // Squirrel this data away, in case we need to reload it, e.g. after
        // recreating the device parameters
        if (root->getChildByName("parametervalues"))
        {
            scopeSync->storeParameterValues(*(root->getChildByName("parametervalues")));
            scopeSync->restoreParameterValues();
        }
        
        if (root->getChildByName("configurationfilepath"))
        {
            String configurationFilePath = root->getChildByName("configurationfilepath")->getAllSubText();

            scopeSync->changeConfiguration(configurationFilePath, false);
        }
    }
    else
    {
        DBG("PluginProcessor::setStateInformation - Could not restore XML");
    }
}

void PluginProcessor::timerCallback()
{
    scopeSync->receiveUpdates();
}

void PluginProcessor::updateListeners(int index, float newValue)
{
    sendParamChangeMessageToListeners(index, newValue);
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    PluginProcessor* pp = new PluginProcessor();
    
    return pp;
}

#endif  // __DLLEFFECT__

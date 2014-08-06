/**
 * The GUI parent for the "plugin" AppContext. Objects of this class
 * are what are returned as the Editor for a plugin. All of the
 * "real" work is done by the ScopeSyncGUI class.
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

#ifndef PLUGINGUI_H_INCLUDED
#define PLUGINGUI_H_INCLUDED

class ScopeSyncGUI;

#include <JuceHeader.h>
#include "PluginProcessor.h"

class PluginGUI : public AudioProcessorEditor, public Timer
{
public:
    PluginGUI (PluginProcessor* owner);
    ~PluginGUI();

    PluginProcessor* getProcessor() const { return static_cast <PluginProcessor*>(getAudioProcessor());	}

    // Callback for timer. Used to check whether this component needs resizing to accommodate a
    // ScopeSyncGUI with a different size
    void timerCallback();

private:
    ScopedPointer<ScopeSyncGUI> scopeSyncGUI;

    int width;
    int height;

    static const int timerInterval;
    void paint(Graphics& /* g */) {};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginGUI)
};

#endif   // PLUGINGUI_H_INCLUDED
#endif   // __DLL_EFFECT__

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

#include "PluginGUI.h"
#include "../../ScopeSyncShared/Core/ScopeSyncGUI.h"

const int PluginGUI::timerInterval = 200;

PluginGUI::PluginGUI (PluginProcessor* owner)
    : AudioProcessorEditor(owner)
{
    scopeSyncGUI = new ScopeSyncGUI(*(owner->getScopeSync()));
    
    width  = jmin(scopeSyncGUI->getWidth(), 10);
    height = jmin(scopeSyncGUI->getHeight(), 10);
    
    DBG("PluginGUI::PluginGUI - Creating scopeSyncGUI, width=" + String(width) + ", height=" + String(height));
    
    setSize(width, height);
    addAndMakeVisible(scopeSyncGUI);

    startTimer(timerInterval);
}

PluginGUI::~PluginGUI()
{
    stopTimer();
}

void PluginGUI::timerCallback()
{
    // In case the mainComponent of the scopeSyncGUI has been changed
    // such that the size has changed, then change our plugin size
    bool sizeChanged = false;

    if (width != scopeSyncGUI->getWidth())
    {
        width = scopeSyncGUI->getWidth();
        sizeChanged = true;
    }

    if (height != scopeSyncGUI->getHeight())
    {
        height = scopeSyncGUI->getHeight();
        sizeChanged = true;
    }

    if (sizeChanged)
    {
        DBG("PluginGUI::timerCallback - GUI size changed");
        setSize(width, height);
    }
}

#endif  // __DLLEFFECT__

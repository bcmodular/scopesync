/**
 * Wrapper class that provides the ScopeSync GUI to the Scope application.
 * Handles things like window title bar etc.
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

#ifdef __DLL_EFFECT__

#include "ScopeFXGUI.h"
#include "ScopeFX.h"

ScopeFXGUI::ScopeFXGUI(ScopeFX* owner)
{
    scopeFX = owner;
    scopeSyncGUI = new ScopeSyncGUI(scopeFX->getScopeSync());
    
    int width  = jmax(scopeSyncGUI->getWidth(), 100);
    int height = jmax(scopeSyncGUI->getHeight(), 100);

    firstTimeShow = true;
    
    DBG("ScopeFXGUI::ScopeFXGUI - Creating scopeSyncGUI, width=" + String(width) + ", height=" + String(height));
    
    setSize(width, height);
    addAndMakeVisible(scopeSyncGUI);
}

ScopeFXGUI::~ScopeFXGUI()
{
    scopeFX = nullptr;
}

void ScopeFXGUI::refreshWindow()
{
    // In case the mainComponent of the scopeSyncGUI has been changed
    // such that the size has changed, then change our plugin size
    bool sizeChanged = false;
    int newWidth  = jmax(scopeSyncGUI->getWidth(), 100);
    int newHeight = jmax(scopeSyncGUI->getHeight(), 100);

    if (getWidth() != newWidth)
    {
        sizeChanged = true;
    }

    if (getHeight() != newHeight)
    {
        sizeChanged = true;
    }

    if (sizeChanged || firstTimeShow)
    {
        DBG("ScopeFXGUI::timerCallback - GUI size changed: My size: " + String(getWidth()) + ", " + String(getHeight()) + " ScopeSyncGUI size: " + String(newWidth) + ", " + String(newHeight));
        setSize(newWidth, newHeight);
        grabKeyboardFocus();
        firstTimeShow = false;
    }

    String newWindowName = scopeFX->getScopeSync().getConfigurationName(true);

    if (windowName != newWindowName)
    {
        windowName = newWindowName;
        setName(windowName);
    }
}

void ScopeFXGUI::userTriedToCloseWindow()
{
    scopeFX->hideWindow();
}

void ScopeFXGUI::moved()
{
    scopeFX->positionChanged(getScreenPosition().getX(), getScreenPosition().getY());
}

void ScopeFXGUI::paint(Graphics& g)
{
    g.fillAll(Colour::fromString("FF2D3035"));
}

#endif  // __DLLEFFECT__

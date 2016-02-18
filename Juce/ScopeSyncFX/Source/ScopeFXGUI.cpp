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
#include "../../ScopeSyncShared/Core/ScopeSync.h"
#include "../../ScopeSyncShared/Core/BCMParameterController.h"

ScopeFXGUI::ScopeFXGUI(ScopeFX* owner, HWND scopeWindow) :
    scopeFX(owner)
{
    setOpaque(true);
    setVisible(true);
    setName("ScopeSync");

    scopeFX->getScopeSync().getParameterController()->getParameterByScopeCode("X")->mapToUIValue(xPos);
    scopeFX->getScopeSync().getParameterController()->getParameterByScopeCode("Y")->mapToUIValue(yPos);

    xPos.addListener(this);
    yPos.addListener(this);

    setTopLeftPosition(xPos.getValue(), yPos.getValue());
    addToDesktop(ComponentPeer::windowHasTitleBar | ComponentPeer::windowHasCloseButton | ComponentPeer::windowHasDropShadow, scopeWindow);

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
    xPos.removeListener(this);
    yPos.removeListener(this);
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

void ScopeFXGUI::valueChanged(Value & valueThatChanged)
{
    if (valueThatChanged.refersToSameSourceAs(xPos) || valueThatChanged.refersToSameSourceAs(yPos))
    {
        DBG("ScopeFXGUI::valueChanged - new position: X = " + xPos.getValue().toString() + " Y = " + yPos.getValue().toString());
        setTopLeftPosition(xPos.getValue(), yPos.getValue());
    }
}

void ScopeFXGUI::userTriedToCloseWindow()
{
    scopeFX->hideWindow();
}

void ScopeFXGUI::moved()
{
    DBG("ScopeFXGUI::moved - new position: X = " + String(getScreenPosition().getX()) + " Y = " + String(getScreenPosition().getY()));
    scopeFX->getScopeSync().getParameterController()->getParameterByScopeCode("X")->setUIValue((float)getScreenPosition().getX());
    scopeFX->getScopeSync().getParameterController()->getParameterByScopeCode("Y")->setUIValue((float)getScreenPosition().getY());
}

void ScopeFXGUI::paint(Graphics& g)
{
    g.fillAll(Colour::fromString("FF2D3035"));
}

#endif  // __DLLEFFECT__

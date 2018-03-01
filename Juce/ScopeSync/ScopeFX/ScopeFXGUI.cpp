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

#ifdef __DLL_EFFECT__

#include "ScopeFXGUI.h"
#include "ScopeFX.h"
#include "../Core/ScopeSync.h"

ScopeFXGUI::ScopeFXGUI(ScopeFX* owner) :
    scopeFX(owner),
	parameterController(owner->getScopeSync().getParameterController())
{

}

ScopeFXGUI::~ScopeFXGUI()
{
	stopTimer();
	
	scopeSyncGUI->removeComponentListener(this);
	configurationName.removeListener(this);
    xPos.removeListener(this);
    yPos.removeListener(this);
}

void ScopeFXGUI::open(HWND scopeWindow)
{
    setOpaque(true);
    setVisible(true);
    setName("ScopeSync");

    parameterController->getParameterByName("X")->mapToUIValue(xPos);
    parameterController->getParameterByName("Y")->mapToUIValue(yPos);

	configurationName.addListener(this);
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
	
	scopeSyncGUI->addComponentListener(this);
	scopeFX->getScopeSync().listenForConfigurationNameChanges(configurationName);

	grabKeyboardFocus();
}

void ScopeFXGUI::valueChanged(Value & valueThatChanged)
{
	// This will be called when Scope sets the values, typically on Project/Preset/Screenset load
    if (!ignoreXYFromScope && (valueThatChanged.refersToSameSourceAs(xPos) || valueThatChanged.refersToSameSourceAs(yPos)))
    {
        DBG("ScopeFXGUI::valueChanged - new position: X = " + xPos.getValue().toString() + " Y = " + yPos.getValue().toString());
        setTopLeftPosition(xPos.getValue(), yPos.getValue());
    }
	else if (valueThatChanged.refersToSameSourceAs(configurationName))
    {
		setName(scopeFX->getScopeSync().getConfigurationName(true));
    }
}

void ScopeFXGUI::componentMovedOrResized(Component& component, bool wasMoved, bool wasResized)
{
	(void)wasMoved;

    if (wasResized || firstTimeShow)
    {
        setSize(jmax(component.getWidth(), 100), jmax(component.getHeight(), 100));
        grabKeyboardFocus();
    	firstTimeShow = false;
    }
}

void ScopeFXGUI::userTriedToCloseWindow()
{
   parameterController->getParameterByName("Show")->setUIValue(0);
}

void ScopeFXGUI::moved()
{
	// This is called when someone is physically moving the window
    DBG("ScopeFXGUI::moved - new position: X = " + String(getScreenPosition().getX()) + " Y = " + String(getScreenPosition().getY()));
    parameterController->getParameterByName("X")->setUIValue(float(getScreenPosition().getX()));
    parameterController->getParameterByName("Y")->setUIValue(float(getScreenPosition().getY()));

	// We don't want to be affected by values coming back from Scope, so ignore for half a second
	ignoreXYFromScope = true;
	startTimer(500);
}

void ScopeFXGUI::paint(Graphics& g)
{
    g.fillAll(Colour::fromString("FF2D3035"));
}

void ScopeFXGUI::timerCallback()
{
	stopTimer();
	ignoreXYFromScope = false;
}

#endif  // __DLL_EFFECT__

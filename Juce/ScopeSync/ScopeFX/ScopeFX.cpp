/**
 * Wrapper class that hosts the ScopeSync object and its GUI
 * on behalf of the Scope (or Scope SDK) applications, using 
 * the ScopeFX SDK. This requires it to be derived from the Effect
 * class.
 *
 * Also operates as a Timer source for the ScopeSync and ScopeFXGUI
 * objects.
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
#include "ScopeFX.h"
#include "ScopeFXGUI.h"
#include "../Resources/ImageLoader.h"
#include "../Resources/Icons.h"
#include "../Core/ScopeSyncApplication.h"
#include "../Windows/UserSettings.h"
#include "../Core/BCMParameterController.h"
#include "../Core/BCMParameter.h"

#ifdef _WIN32
#include <Windows.h>
#endif
#include <float.h>

#ifdef _WIN32
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)  
HWND scopeWindow;
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM /* lParam */)
{
    HINSTANCE hinst = (HINSTANCE)GetModuleHandle(NULL);

    if((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE) == hinst && IsWindowVisible(hwnd))
    {
        scopeWindow = hwnd;
        return FALSE;
    }
    else
        return TRUE;
}
#endif

using namespace ScopeFXParameterDefinitions;

ScopeFX::ScopeFX() : Effect(&effectDescription)
{
    initValues();
    
    if (ScopeSync::getNumScopeSyncInstances() == 0)
    {
#ifdef _WIN32
        Process::setCurrentModuleInstanceHandle(HINST_THISCOMPONENT);
#endif
        initialiseJuce_GUI();
    }

    scopeSync = new ScopeSync(this);

    DBG("ScopeFX::ScopeFX - Number of module instances: " + String(ScopeSync::getNumScopeSyncInstances()));

	scopeSync->getParameterController()->getParameterByScopeCode("show")->mapToUIValue(shouldShowWindow);
    shouldShowWindow.addListener(this);
}

ScopeFX::~ScopeFX()
{
	shouldShowWindow.removeListener(this);

    scopeFXGUI = nullptr;
    scopeSync->unload();
    scopeSync = nullptr;

    DBG("ScopeFX::~ScopeFX - Number of module instances: " + String(ScopeSync::getNumScopeSyncInstances()));
    
    ScopeSync::shutDownIfLastInstance();
}
   
void ScopeFX::initValues()
{
    shouldShowWindow = false;

	for (int i = 0; i < numParameters; i++)
		currentValues[i] = 0;
}

void ScopeFX::toggleWindow(bool show)
{
	if (show && scopeFXGUI == nullptr)
	{
#ifdef _WIN32
		if (scopeWindow == nullptr)
			EnumWindows(EnumWindowsProc, 0);
#else
		// If Scope ever ends up on non-Windows, we'll
		// probably want to implement something here
		void* scopeWindow = nullptr;
#endif

		scopeFXGUI = new ScopeFXGUI(this, scopeWindow);
	}
	else if (!show)
		scopeFXGUI = nullptr;
}

void ScopeFX::setGUIEnabled(bool shouldBeEnabled)
{
    if (scopeFXGUI != nullptr)
        scopeFXGUI->setEnabled(shouldBeEnabled);
}

void ScopeFX::valueChanged(Value& valueThatChanged)
{
	if (valueThatChanged.refersToSameSourceAs(shouldShowWindow))
		toggleWindow(int(shouldShowWindow.getValue()) > 0);
}


int ScopeFX::async(PadData** asyncIn,  PadData* /*syncIn*/,
                   PadData*  asyncOut, PadData* /*syncOut*/)
{
	int asyncValues[numParameters];

	int i = 0;
	int j = 0;

    int* parameterArray = (int*)asyncIn[INPAD_PARAMS]->itg;
    
    // Grab ScopeSync values from input
	while (i < numScopeParameters)
	{
		if (parameterArray != nullptr)
			asyncValues[i] = parameterArray[j];
		else
			asyncValues[i] = 0;
		
		//DBG("INPUT: i = " + String(i) + ", j = " + String(j) + ", value = " + String(asyncValues[i]));
		
		i++;
		j++;
	}

    int* localArray = (int*)asyncIn[INPAD_LOCALS]->itg;

    j = 0;
	// Grab ScopeLocal values from input
	while (j < numLocalParameters)
	{
		if (localArray != nullptr)
			asyncValues[i] = localArray[j];
		else
			asyncValues[i] = 0;

		//DBG("INPUT: i = " + String(i) + ", j = " + String(j) + ", value = " + String(asyncValues[i]));
		
		i++;
		j++;
	}

	int* feedbackArray = (int*)asyncIn[INPAD_FEEDBACK]->itg;
	
	j = 0;
    // Grab Feedback values from input
    while (j < numFeedbackParameters)
    {
        if (feedbackArray != nullptr)
			asyncValues[i] = feedbackArray[j];
		else
			asyncValues[i] = 0;
        
		//DBG("INPUT: i = " + String(i) + ", j = " + String(j) + ", value = " + String(asyncValues[i]));
		
		i++;
        j++;
    }

    // Grab fixed parameters values from input
    j = INPAD_X;
    while (j < INPAD_X + numFixedBiDirParameters + numFixedInputOnlyParameters)
    {
        asyncValues[i] = asyncIn[j]->itg;
        
		//DBG("INPUT: i = " + String(i) + ", j = " + String(j) + ", value = " + String(asyncValues[i]));
		
		i++;
        j++;
    }
	
	int enableScopeInputs = asyncValues[i];

	if (enableScopeInputs != currentValues[i])
		ScopeSyncAsync::setScopeInputEnablement(enableScopeInputs > 0);
	
    // Get ScopeSync to process the inputs and pass on changes from the SS system
    if (scopeSync != nullptr)
        scopeSync->getScopeSyncAsync().handleUpdate(asyncValues, currentValues);
    
	i = 0;

	// Write to the async outputs for all output parameters
    for (int k = 0; k < numParameters; k++)
    {
		if (   ScopeSync::getScopeCodeType(k) != BCMParameter::feedback
			&& ScopeSync::getScopeCodeType(k) != BCMParameter::fixedInputOnly)
		{
			//DBG("OUTPUT: i = " + String(i) + ", k = " + String(k) + ", value = " + String(asyncValues[k]));
			asyncOut[i].itg  = asyncValues[k];
			i++;
		}
    }
	
	// Tell Scope when the DLL has been loaded
	asyncOut[OUTPAD_LOADED].itg = (scopeSync != nullptr && scopeSync->isInitialised()) ? FRAC_MAX : 0;

	return 0;
}

int ScopeFX::syncBlock(PadData** /*asyncIn*/, PadData* /*syncIn*/,
                       PadData* /*asyncOut*/, PadData* /*syncOut*/, 
                       int /*off*/,
                       int /*cnt*/)
{
	return -1;
}

Effect *newEffect()
{
	return new ScopeFX();
};

void deleteEffect (Effect *eff)
{
   delete eff;
}

EffectDescription *descEffect ()
{
    return &effectDescription;
}

extern "C"
::uint32 ioctlEffect (
					  ::uint32  /* dwService */, ::uint32  /* dwDDB */, ::uint32  /* hDevice */,
					  void * /* lpvInBuffer */, ::uint32 /* cbInBuffer */, 
					  void * /* lpvOutBuffer */, ::uint32 /* cbOutBuffer */,
					  ::uint32 * lpcbBytesReturned )
{
   *lpcbBytesReturned = 0;
   return ERROR_NOT_SUPPORTED;
}

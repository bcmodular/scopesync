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
#include "../../ScopeSyncShared/Resources/ImageLoader.h"
#include "../../ScopeSyncShared/Resources/Icons.h"
#include "../../ScopeSyncShared/Core/ScopeSyncApplication.h"
#include "../../ScopeSyncShared/Windows/UserSettings.h"
#include "../../ScopeSyncShared/Core/BCMParameterController.h"

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

    startTimer(100);
}

ScopeFX::~ScopeFX()
{
    stopTimer();

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

void ScopeFX::showWindow()
{
	if (scopeFXGUI == nullptr)
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
}

void ScopeFX::setGUIEnabled(bool shouldBeEnabled)
{
    if (scopeFXGUI != nullptr)
        scopeFXGUI->setEnabled(shouldBeEnabled);
}

void ScopeFX::timerCallback()
{
    if (shouldShowWindow)
        showWindow();
    else
        hideWindow();

    if (scopeFXGUI)
        scopeFXGUI->refreshWindow();
}

void ScopeFX::hideWindow()
{
    scopeSync->getScopeSyncAsync().setValue("show", 0);
    shouldShowWindow = false;
    scopeFXGUI = nullptr;
}

int ScopeFX::async(PadData** asyncIn,  PadData* /*syncIn*/,
                   PadData*  asyncOut, PadData* /*syncOut*/)
{
	int asyncValues[numParameters];

	int i = 0;
	int j = 0;

    int* parameterArray = (int*)asyncIn[INPAD_PARAMS]->itg;

    if (parameterArray == nullptr)
        return 0;
    
    // Grab ScopeSync values from input
	while (i < numScopeParameters)
	{
		asyncValues[i] = parameterArray[j];
		i++;
		j++;
	}

    int* localArray = (int*)asyncIn[INPAD_LOCALS]->itg;

    j = 0;
	// Grab ScopeLocal values from input
	while (j < numLocalParameters)
	{
		asyncValues[i] = localArray[j];
		i++;
		j++;
	}

    // Grab Bi-direction fixed parameter values from input
    j = INPAD_X;
    while (j < INPAD_X + numFixedBiDirParameters)
    {
        asyncValues[i] = asyncIn[j]->itg;
        i++;
        j++;
    }

    int* feedbackArray = (int*)asyncIn[INPAD_FEEDBACK]->itg;

    if (feedbackArray != nullptr)
    {
        j = 0;
        // Grab Feedback values from input
        while (j < numFeedbackParameters)
        {
            asyncValues[i] = feedbackArray[j];
            i++;
            j++;
        }
    }
    else
    {
        j = 0;
        
        while (j < numFeedbackParameters)
        {
            asyncValues[i] = 0;
            i++;
            j++;
        }
    }

    // Grab input only fixed parameter values from input
    j = INPAD_DEVICE_TYPE;
	while (j < INPAD_X + numFixedInputOnlyParameters)
	{
		asyncValues[i] = asyncIn[j]->itg;
		i++;
		j++;
	}
	
    // Get ScopeSync to process the inputs and pass on changes from the SS system
    if (scopeSync != nullptr)
    {
        bool perfMode = ScopeSync::getPerformanceModeGlobalDisable() ? false : (scopeSync->getPerformanceMode() > 0);
        scopeSync->getScopeSyncAsync().handleUpdate(asyncValues, currentValues, perfMode);
    }

	// Write to the async outputs for all output parameters
    for (int k = 0; k < numOutputParameters; k++)
    {
        asyncOut[k].itg  = asyncValues[k];
    }
	
	// Tell Scope when the DLL has been loaded
	asyncOut[OUTPAD_LOADED].itg = (scopeSync != nullptr && scopeSync->isInitialised()) ? FRAC_MAX : 0;

    // Handle window
    if (asyncOut[OUTPAD_SHOW].itg > 0)
        shouldShowWindow = true;
    else
        shouldShowWindow = false;

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

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
#include "../../ScopeSyncShared/Components/ImageLoader.h"

const int ScopeFX::initPositionX           = 100;
const int ScopeFX::initPositionY           = 100;
const int ScopeFX::windowHandlerDelayMax   = 6;
const int ScopeFX::timerFrequency          = 20;
const int ScopeFX::numParameters           = 128;
const int ScopeFX::numLocalValues          = 16;

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

Array<ScopeFX*> ScopeFX::moduleInstances;

ScopeFX::ScopeFX() : Effect(&effectDescription)
{
    initValues();

    scopeSync = new ScopeSync(this);

    if (moduleInstances.size() == 0)
    {
#ifdef _WIN32
        Process::setCurrentModuleInstanceHandle(HINST_THISCOMPONENT);
#endif
        initialiseJuce_GUI();
    }

    moduleInstances.add(this);
    DBG("ScopeFX::ScopeFX - Number of module instances: " + String(moduleInstances.size()));

    startTimer(timerFrequency);
}

ScopeFX::~ScopeFX()
{
    stopTimer();
    
    scopeFXGUI = nullptr;
    scopeSync->unload();

    moduleInstances.removeAllInstancesOf(this);
    DBG("ScopeFX::~ScopeFX - Number of module instances: " + String(moduleInstances.size()));
    
    if (moduleInstances.size() == 0)
    {
        ImageLoader::deleteInstance();
        UserSettings::deleteInstance();
        shutdownJuce_GUI();
    }
}
   
void ScopeFX::initValues()
{
	positionX          = initPositionX;
	positionY          = initPositionY;
    requestWindowShow  = false;
    windowShown        = false;
    windowHandlerDelay = 0;
}

void ScopeFX::timerCallback()
{
    if (scopeFXGUI)
        scopeFXGUI->refreshWindow();

    if (windowHandlerDelay == 0)
    {   
		if ((requestWindowShow || scopeSync->getSystemError().toString().isNotEmpty()) && !windowShown)
        {
            DBG("ScopeFX::timerCallback - Request to show window");
            showWindow();
        }
        else if (!requestWindowShow && windowShown)
        {
            DBG("ScopeFX::timerCallback - Request to hide window");
            hideWindow();
        }

        //DBG("ScopeFX::timerCallback: " + String(positionX) + ", " + String(positionY) + ", " + String(scopeFXGUI->getScreenPosition().getX()) + ", " + String(scopeFXGUI->getScreenPosition().getY()));
        if (scopeFXGUI != nullptr
            && (positionX != scopeFXGUI->getScreenPosition().getX()
            ||  positionY != scopeFXGUI->getScreenPosition().getY()))
        {
            scopeFXGUI->setTopLeftPosition(positionX, positionY);
        }
    }
    else
    {
        DBG("ScopeFX::timerCallback - Ignoring values: " + String(windowHandlerDelay))
        windowHandlerDelay--;
    }
    
    if (!(scopeSync->processConfigurationChange()))
    {
        scopeSync->receiveUpdates();
    }
}

void ScopeFX::showWindow()
{
    DBG("ScopeFX::showWindow");
    scopeFXGUI = new ScopeFXGUI(this);

#ifdef _WIN32
    if (scopeWindow == nullptr)
    {
        EnumWindows(EnumWindowsProc, 0);
    }
#else
    // If Scope ever ends up on non-Windows, we'll
    // probably want to implement something here
    void* scopeWindow = nullptr;
#endif
    scopeFXGUI->setOpaque(true);
    scopeFXGUI->setVisible(true);
    scopeFXGUI->setName("ScopeSync");

    scopeFXGUI->setTopLeftPosition(positionX, positionY);
    scopeFXGUI->addToDesktop(ComponentPeer::windowHasTitleBar | ComponentPeer::windowHasCloseButton | ComponentPeer::windowHasDropShadow, scopeWindow);
    windowShown = true;
    windowHandlerDelay = windowHandlerDelayMax;
}

void ScopeFX::positionChanged(int newPosX, int newPosY)
{
    DBG("ScopeFX::positionChanged - moving window to: " + String(newPosX) + "," + String(newPosY));
    positionX = newPosX;
    positionY = newPosY;
    windowHandlerDelay = windowHandlerDelayMax;
}

void ScopeFX::hideWindow()
{
    scopeFXGUI = nullptr;
    windowShown = false;
    windowHandlerDelay = windowHandlerDelayMax;
}

void ScopeFX::reloadAllGUIs()
{
    for (int i = 0; i < moduleInstances.size(); i++)
    {
        moduleInstances[i]->getScopeSync().setGUIReload(true);
    }
}

int ScopeFX::async(PadData **asyncIn, PadData * /*syncIn*/,
                   PadData *asyncOut, PadData * /*syncOut*/)
{
    int* parameterArray = (int*)asyncIn[INPAD_PARAMS]->itg;
    int* localArray     = (int*)asyncIn[INPAD_LOCALS]->itg;
    
    Array<int> asyncValues;
    
    if (parameterArray != nullptr)
        for (int i = 0; i < numParameters; i++)
            asyncValues.add(parameterArray[i]);
    else
        for (int i = 0; i < numParameters; i++)
            asyncValues.add(0);
    
    if (localArray != nullptr)
        for (int i = 0; i < numLocalValues; i++)
            asyncValues.add(localArray[i]);
    else        
        for (int i = numParameters; i < numParameters + numLocalValues; i++)
            asyncValues.add(0);

    scopeSync->handleScopeSyncAsyncUpdate(asyncValues);

    for (int i = 0; i < numParameters + numLocalValues; i++)
        asyncOut[i].itg = asyncValues[i];

    if (asyncIn[INPAD_SHOW]->itg == 0)
        requestWindowShow = false;
    else
        requestWindowShow = true;

    String newConfigurationFileName = asyncIn[INPAD_CONFIGFILE]->str;

    if (!(newConfigurationFileName.equalsIgnoreCase(configurationFileName)))
    {
        scopeSync->changeConfiguration(newConfigurationFileName, true);
        configurationFileName = newConfigurationFileName;
    }

    positionX = asyncIn[INPAD_X]->itg;
    positionY = asyncIn[INPAD_Y]->itg;
    //DBG("ScopeFX::async " + String(positionX) + ", " + String(positionY));

    asyncOut[OUTPAD_SHOW].itg     = windowShown ? 1 : 0;
    asyncOut[OUTPAD_X].itg        = (scopeFXGUI != nullptr) ? scopeFXGUI->getScreenPosition().getX() : positionX;
    asyncOut[OUTPAD_Y].itg        = (scopeFXGUI != nullptr) ? scopeFXGUI->getScreenPosition().getY() : positionY;
   
    return 0;
}

int ScopeFX::syncBlock(PadData ** /*asyncIn*/, PadData * /*syncIn*/,
                       PadData * /*asyncOut*/, PadData * syncOut, 
                       int off,
                       int cnt)
{
    PadData *outData = (PadData*)syncOut[OUTPAD_DATA].buf + off;
    PadData *outDest = (PadData*)syncOut[OUTPAD_DEST].buf + off;
    
    if (outData && outDest)
    {
        Array<std::pair<int,int>> snapshotSubset;
        
        scopeSync->getSnapshot(snapshotSubset, cnt);
    
        for (int i = 0; i < cnt; i++, outData++, outDest++)
        {
            if (i < snapshotSubset.size())
            {
                outData->itg = snapshotSubset[i].first;
                outDest->itg = snapshotSubset[i].second;
    
                // DBG("ScopeFX::syncBlock - Read from snapshot: data: " + String(outData->itg) + ", dest: " + String(outDest->itg));
            }
            else
            {
                // Clear output buffer
                outData->itg = 0;
                outDest->itg = 0;
            }
        }
        return 0;
    }
    else
    {
        return -1;
    }
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

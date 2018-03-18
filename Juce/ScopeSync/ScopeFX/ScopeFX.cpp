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
#include "ScopeFX.h"
#include "ScopeFXGUI.h"
#include "../Parameters/BCMParameterController.h"
#include "../Parameters/BCMParameter.h"

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

	scopeSync->getParameterController()->getParameterByName("Show")->mapToUIValue(shouldShowWindow);
    shouldShowWindow.addListener(this);

	scopeSync->getUserSettings()->referToScopeFXOSCSettings(pluginHost, pluginPort, scopeSyncPort);
	pluginHost.addListener(this);
	pluginPort.addListener(this);
	scopeSyncPort.addListener(this);
}

ScopeFX::~ScopeFX()
{
	shouldShowWindow.removeListener(this);

    scopeFXGUI = nullptr;
    scopeSync->unload();
    scopeSync = nullptr;

	if (ScopeSync::getNumScopeSyncInstances() == 0)
        shutdownJuce_GUI();
}
   
void ScopeFX::initValues()
{
    shouldShowWindow = false;
}

void ScopeFX::toggleWindow(bool show)
{
	if (show && scopeFXGUI == nullptr)
	{
		if (scopeWindow == nullptr)
			EnumWindows(EnumWindowsProc, 0);

		scopeFXGUI = new ScopeFXGUI(this);
		scopeFXGUI->open(scopeWindow);
	}
	else if (!show)
		scopeFXGUI = nullptr;
}

void ScopeFX::setGUIEnabled(bool shouldBeEnabled)
{
    if (scopeFXGUI != nullptr)
        scopeFXGUI->setEnabled(shouldBeEnabled);
}

void ScopeFX::snapshot()
{
	snapshotValue.fetch_add(1, std::memory_order_relaxed);
}

void ScopeFX::setPluginHostIP(StringRef address)
{
	StringArray const octets(address, ".", String::empty);
	setPluginHostIP(octets[0].getIntValue(), octets[1].getIntValue(), octets[2].getIntValue(), octets[3].getIntValue());
}

void ScopeFX::setPluginHostIP(int oct1, int oct2, int oct3, int oct4)
{
	pluginHostOctet1.store(oct1, std::memory_order_relaxed);
	pluginHostOctet2.store(oct2, std::memory_order_relaxed);
	pluginHostOctet3.store(oct3, std::memory_order_relaxed);
	pluginHostOctet4.store(oct4, std::memory_order_relaxed);
}

void ScopeFX::setPluginListenerPort(int port)
{
	pluginListenerPort.store(port, std::memory_order_relaxed);
}

void ScopeFX::setScopeSyncListenerPort(int port)
{
	scopeSyncListenerPort.store(port, std::memory_order_relaxed);
}

void ScopeFX::valueChanged(Value& valueThatChanged)
{
	if (valueThatChanged.refersToSameSourceAs(shouldShowWindow))
	{
		toggleWindow(int(shouldShowWindow.getValue()) > 0);
		return;
	}
	
	if (valueThatChanged.refersToSameSourceAs(pluginHost))
	{
		setPluginHostIP(pluginHost.toString());
		return;
	}

	if (valueThatChanged.refersToSameSourceAs(pluginPort))
	{
		setPluginListenerPort(pluginPort.getValue());
		return;
	}

	if (valueThatChanged.refersToSameSourceAs(scopeSyncPort))
	{
		setScopeSyncListenerPort(scopeSyncPort.getValue());
		return;
	}
}


int ScopeFX::async(PadData** asyncIn,  PadData* /*syncIn*/,
                   PadData*  asyncOut, PadData* /*syncOut*/)
{
	int newOSCUID = asyncIn[INPAD_OSCUID]->itg;

	if (newOSCUID != oscUID)
	{
		oscUID = newOSCUID;
		scopeSync->setOSCUID(oscUID);
	}
	else
		oscUID = scopeSync->getOSCUID();
	
	asyncOut[OUTPAD_OSCUID].itg   = oscUID;
	asyncOut[OUTPAD_SNAPSHOT].itg = snapshotValue.load(std::memory_order_relaxed);

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

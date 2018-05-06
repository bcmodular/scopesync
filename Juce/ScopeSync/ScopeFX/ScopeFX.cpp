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
#include "../Utils/BCMMisc.h"

#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
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

ScopeFX::ScopeFX() 
	: Effect(&effectDescription), 
	  snapshotValue(0), syncScopeValue(0), 
	  pluginHostOctet1(127), pluginHostOctet2(0), pluginHostOctet3(0), pluginHostOctet4(1),
	  pluginListenerPort(8002), scopeSyncListenerPort(8001),
	  configUID(0), scopeConfigUID(0)
{
	shouldShowWindow = false;

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

	pluginHost.addListener(this);
	pluginPort.addListener(this);
	scopeSyncPort.addListener(this);
	scopeSync->getUserSettings()->referToScopeFXOSCSettings(pluginHost, pluginPort, scopeSyncPort);

	configurationUID.addListener(this);
	scopeSync->referToConfigurationUID(configurationUID);
}

ScopeFX::~ScopeFX()
{
	stopTimer();
	
	shouldShowWindow.removeListener(this);

    scopeFXGUI = nullptr;
    scopeSync->unload();
    scopeSync = nullptr;

	if (ScopeSync::getNumScopeSyncInstances() == 0)
        shutdownJuce_GUI();
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

void ScopeFX::syncScope()
{
	DBG("ScopeFX::syncScope - starting timer");
	// Race conditions on loading module mean we need to delay this
	// for a little bit
	startTimer(1000);
}

void ScopeFX::timerCallback()
{
	DBG("ScopeFX::timerCallback - setting sync Scope value");
	stopTimer();
	syncScopeValue.fetch_add(1, std::memory_order_relaxed);
}

void ScopeFX::setPluginHostIP(StringRef address)
{
	StringArray const octets(StringArray::fromTokens(ipAddressFromHostName(address, pluginPort.toString()), ".", String::empty));
	DBG("ScopeFX::setPluginHostIP - address passed in: " + address);
	DBG("ScopeFX::setPluginHostIP - num octets: " + String(octets.size()) + ", " + octets[0] + ";" + octets[1] + ";" + octets[2] + ";" + octets[3]);
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

void ScopeFX::setConfigUID(int newConfigUID)
{
	configUID.store(newConfigUID, std::memory_order_relaxed);
}

void ScopeFX::valueChanged(Value& valueThatChanged)
{
	if (valueThatChanged.refersToSameSourceAs(shouldShowWindow))
	{
		toggleWindow(int(valueThatChanged.getValue()) > 0);
		return;
	}
	
	if (valueThatChanged.refersToSameSourceAs(pluginHost))
	{
		setPluginHostIP(valueThatChanged.toString());
		return;
	}

	if (valueThatChanged.refersToSameSourceAs(pluginPort))
	{
		setPluginListenerPort(valueThatChanged.getValue());
		return;
	}

	if (valueThatChanged.refersToSameSourceAs(scopeSyncPort))
	{
		setScopeSyncListenerPort(valueThatChanged.getValue());
		return;
	}

	if (valueThatChanged.refersToSameSourceAs(configurationUID))
	{
		setConfigUID(valueThatChanged.getValue());
		return;
	}
}


int ScopeFX::async(PadData** asyncIn,  PadData* /*syncIn*/,
                   PadData*  asyncOut, PadData* /*syncOut*/)
{
	// TODO: this needs to be tweaked to be thread-safe
	int newDeviceInstance = asyncIn[INPAD_DEVICE_INSTANCE]->itg;

	if (newDeviceInstance != deviceInstance)
	{
		deviceInstance = newDeviceInstance;
		scopeSync->setDeviceInstance(deviceInstance);
	}
	else
		deviceInstance = scopeSync->getDeviceInstance();
	
	int oldScopeConfigUID = scopeConfigUID.load(std::memory_order_relaxed);
	int newScopeConfigUID = asyncIn[INPAD_CONFIGUID]->itg;

	if (!scopeConfigUID.compare_exchange_weak(oldScopeConfigUID, newScopeConfigUID))
	{
		// Scope value has changed since the last async call, so let's tell ScopeSync about it
		scopeSync->setConfigurationUID(scopeConfigUID.load(std::memory_order_relaxed));
	}
	else
	{
		// Scope value hasn't changed, so let's swap in the one from ScopeSync (could have been 
		// updated in the valueChanged() method)
		scopeConfigUID.store(configUID.load(std::memory_order_relaxed), std::memory_order_relaxed);
	}

	asyncOut[OUTPAD_DEVICE_INSTANCE].itg         = deviceInstance;
	asyncOut[OUTPAD_SNAPSHOT].itg                = snapshotValue.load(std::memory_order_relaxed);
	asyncOut[OUTPAD_PLUGIN_HOST_OCT1].itg        = pluginHostOctet1.load(std::memory_order_relaxed);
	asyncOut[OUTPAD_PLUGIN_HOST_OCT2].itg        = pluginHostOctet2.load(std::memory_order_relaxed);
	asyncOut[OUTPAD_PLUGIN_HOST_OCT3].itg        = pluginHostOctet3.load(std::memory_order_relaxed);
	asyncOut[OUTPAD_PLUGIN_HOST_OCT4].itg        = pluginHostOctet4.load(std::memory_order_relaxed);
	asyncOut[OUTPAD_PLUGIN_LISTENER_PORT].itg    = pluginListenerPort.load(std::memory_order_relaxed);
	asyncOut[OUTPAD_SCOPESYNC_LISTENER_PORT].itg = scopeSyncListenerPort.load(std::memory_order_relaxed);
	asyncOut[OUTPAD_CONFIGUID].itg               = scopeConfigUID.load(std::memory_order_relaxed);
	asyncOut[OUTPAD_SYNC_SCOPE].itg              = syncScopeValue.load(std::memory_order_relaxed);
	
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

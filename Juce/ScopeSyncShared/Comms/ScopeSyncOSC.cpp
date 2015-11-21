/**
 * O(pen) S(ound) C(ontrol) message handler
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

#include "ScopeSyncOSC.h"
#include "..\Core\ScopeSync.h"
#include "..\Windows\UserSettings.h"

juce_ImplementSingleton(ScopeSyncOSCServer)

ScopeSyncOSCServer::ScopeSyncOSCServer()
{
	setup();
}

void ScopeSyncOSCServer::setup()
{
	setRemoteHostname(UserSettings::getInstance()->getPropertyValue("oscremotehost"));
    setRemotePortNumber(UserSettings::getInstance()->getPropertyIntValue("oscremoteportnum"));		
	setLocalPortNumber(UserSettings::getInstance()->getPropertyIntValue("osclocalportnum"));
}

void ScopeSyncOSCServer::setLocalPortNumber(int portNumber)
{
	if (connect(portNumber))
	{
		DBG("ScopeSyncOSCServer::setLocalPortNumber - set local port number to: " + String(portNumber));
		receivePortNumber = portNumber;
	}
	else
	{
        AlertWindow::showMessageBoxAsync (
            AlertWindow::WarningIcon,
            "Connection error",
            "OSC Error: could not connect to UDP port: " + String(receivePortNumber),
            "OK");		
	}
}

void ScopeSyncOSCServer::registerOSCListener(ListenerWithOSCAddress<RealtimeCallback>* newListener, OSCAddress address)
{
	removeListener(newListener);
	addListener(newListener, address);
}

void ScopeSyncOSCServer::unregisterOSCListener(ListenerWithOSCAddress<RealtimeCallback>* listenerToRemove)
{
	removeListener(listenerToRemove);
}

void ScopeSyncOSCServer::setRemoteHostname(String hostname)
{
	DBG("ScopeSyncOSCServer::setRemoteHostname - changed remote hostname to: " + hostname);
	remoteHostname = hostname;
	remoteChanged  = true;
}

String ScopeSyncOSCServer::getRemoteHostname()
{
	return remoteHostname;
}

void ScopeSyncOSCServer::setRemotePortNumber(int portNumber)
{
	DBG("ScopeSyncOSCServer::setRemotePortNumber - changed remote port number to: " + String(portNumber));
	remotePortNumber = portNumber;
	remoteChanged    = true;
}

int ScopeSyncOSCServer::getRemotePortNumber()
{
	return remotePortNumber;
}

bool ScopeSyncOSCServer::sendMessage(const OSCAddressPattern pattern, float valueToSend)
{

	if (remoteChanged) {
        if (sender.connect(remoteHostname, remotePortNumber))
			remoteChanged = false;
		else
		{
			AlertWindow::showMessageBoxAsync (
				AlertWindow::WarningIcon,
				"Connection error",
				"OSC Error: could not connect to remote UDP port: " + String(remotePortNumber) + ", on host: " + remoteHostname,
				"OK");

			return false;
		}
    }

	OSCMessage message(pattern);
	message.addArgument(valueToSend);

	sender.send(message);
	return true;
}

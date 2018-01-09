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

#include "OSCServer.h"
#include "..\Windows\UserSettings.h"

OSCServer::OSCServer()
{
	setup();
}

OSCServer::~OSCServer()
{
	oscLocalPortNum.removeListener(this);
	oscRemoteHost.removeListener(this);
	oscRemotePortNum.removeListener(this);
}

void OSCServer::setup()
{
	oscLocalPortNum.addListener(this);
	oscRemoteHost.addListener(this);
	oscRemotePortNum.addListener(this);

	UserSettings::getInstance()->referToOSCSettings(oscLocalPortNum, oscRemoteHost, oscRemotePortNum);
}

void OSCServer::setLocalPortNumber(int portNumber)
{
	if (connect(portNumber))
	{
		DBG("OSCServer::setLocalPortNumber - set local port number to: " + String(portNumber));
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

void OSCServer::registerOSCListener(ListenerWithOSCAddress<MessageLoopCallback>* newListener, OSCAddress address)
{
	removeListener(newListener);
	addListener(newListener, address);
}

void OSCServer::unregisterOSCListener(ListenerWithOSCAddress<MessageLoopCallback>* listenerToRemove)
{
	removeListener(listenerToRemove);
}

void OSCServer::setRemoteHostname(String hostname)
{
	DBG("ScopeSyncOSCServer::setRemoteHostname - changed remote hostname to: " + hostname);
	remoteHostname = hostname;
	remoteChanged  = true;
}

String OSCServer::getRemoteHostname() const
{
	return remoteHostname;
}

void OSCServer::setRemotePortNumber(int portNumber)
{
	DBG("ScopeSyncOSCServer::setRemotePortNumber - changed remote port number to: " + String(portNumber));
	remotePortNumber = portNumber;
	remoteChanged    = true;
}

int OSCServer::getRemotePortNumber() const
{
	return remotePortNumber;
}

bool OSCServer::sendFloatMessage(const OSCAddressPattern pattern, float valueToSend)
{
	if (connectToListener())
	{
		OSCMessage message(pattern);
		message.addFloat32(valueToSend);

		sender.send(message);
		return true;
	}
	else
		return false;
}

bool OSCServer::sendIntMessage(const OSCAddressPattern pattern, int valueToSend)
{
	if (connectToListener())
	{
		OSCMessage message(pattern);
		message.addInt32(valueToSend);

		sender.send(message);
		return true;
	}
	else
		return false;
}

bool OSCServer::connectToListener()
{
	if (remoteChanged) {
        if (sender.connect(remoteHostname, remotePortNumber))
		{	remoteChanged = false;
			return true;
		}
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
	else
		return true;
}

void OSCServer::valueChanged(Value& valueThatChanged)
{
	if (valueThatChanged.refersToSameSourceAs(oscLocalPortNum))
		setLocalPortNumber(valueThatChanged.getValue());
	else if (valueThatChanged.refersToSameSourceAs(oscRemoteHost))
		setRemoteHostname(valueThatChanged.getValue());
	else if (valueThatChanged.refersToSameSourceAs(oscRemotePortNum))
		setRemotePortNumber(valueThatChanged.getValue());
}

ScopeOSCServer::ScopeOSCServer()
{
	setup();
}

void ScopeOSCServer::setup()
{
	oscLocalPortNum.addListener(this);
	oscRemoteHost.addListener(this);
	oscRemotePortNum.addListener(this);

	UserSettings::getInstance()->referToScopeOSCSettings(oscLocalPortNum, oscRemoteHost, oscRemotePortNum);
}

ScopeOSCServer::~ScopeOSCServer()
{
	oscLocalPortNum.removeListener(this);
	oscRemoteHost.removeListener(this);
	oscRemotePortNum.removeListener(this);
}
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
    : Thread("OscServer")
{
	setup();
}

ScopeSyncOSCServer::~ScopeSyncOSCServer()
{
	signalThreadShouldExit();
	
	receiveDatagramSocket = nullptr;
	remoteDatagramSocket  = nullptr;

	waitForThreadToExit (-1);
}

void ScopeSyncOSCServer::setup()
{
	setRemoteHostname(UserSettings::getInstance()->getPropertyValue("oscremotehost"));
    setRemotePortNumber(UserSettings::getInstance()->getPropertyIntValue("oscremoteportnum"));		
	setLocalPortNumber(UserSettings::getInstance()->getPropertyIntValue("osclocalportnum"));
}

void ScopeSyncOSCServer::setLocalPortNumber(int portNumber)
{
	if (!isThreadRunning() || portNumber != receivePortNumber)
	{
		DBG("ScopeSyncOSCServer::setLocalPortNumber - set local port number to: " + String(portNumber));
		receivePortNumber = portNumber;
		listen();
	}
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

void ScopeSyncOSCServer::listen()
{
	stopListening();
	startThread(3);
}

void ScopeSyncOSCServer::stopListening() 
{
	if (isThreadRunning())
	{
		signalThreadShouldExit();
	
		stopThread(500);
		receiveDatagramSocket = nullptr;
	}
}

void ScopeSyncOSCServer::run()
{
	DBG("ScopeSyncOSCServer::run - Initialise ScopeSync OSC Server");
	receiveDatagramSocket = new DatagramSocket(false);

	if (!receiveDatagramSocket->bindToPort(receivePortNumber))
	{
		DBG("ScopeSyncOSCServer::run - OSC server failed to bind to receive port: " + String(receivePortNumber));
		return;
	}

	char buffer[1024];
	int  ret;

	while (!threadShouldExit())
	{
        if (receiveDatagramSocket->getBoundPort() == -1)
		{
            if (!receiveDatagramSocket->bindToPort(receivePortNumber))
			{
                DBG("ScopeSyncOSCServer::run - error port " + String(receivePortNumber) + "is already bound");
                return;
            }
        }

		ret = receiveDatagramSocket->waitUntilReady(true, 100);
		
		if (ret == 1)
		{
			String ip;
			int port;

			int size = receiveDatagramSocket->read(buffer, 1024, false, ip, port);
		
			if (threadShouldExit())
			{
				DBG("ScopeSyncOSCServer::run - OSC server shutdown");
				return;
			}

			try
			{
				osc::ReceivedPacket packet(buffer, size);

				if (!packet.IsMessage())
				{
					DBG("ScopeSyncOSCServer::run - packet isn't an OSC Message");
					return;
				}

				osc::ReceivedMessage oscMessage(packet);

				String addressPattern(oscMessage.AddressPattern());

				if (addressPattern.startsWith("/"))
				{
					osc::ReceivedMessageArgumentStream args = oscMessage.ArgumentStream();
					float value;
					args >> value >> osc::EndMessage;

					DBG("ScopeSyncOSCServer::run - received OSC message with pattern: " + addressPattern + " and value: " + String(value)); 

					ScopeSync::addToOSCControlUpdateBuffers(addressPattern, value);
				}
				else
				{
					DBG("ScopeSync::handleOSCMessage - received other OSC message");                              
				}
			} 
			catch (osc::Exception& e)
			{
				(void)e;
				DBG("ScopeSyncOSCServer::run - error while parsing packet: " + String(e.what()));
			}
		}
		else if (ret == -1)
		{
			DBG("ScopeSyncOSCServer::run - Error from waitUntilReady: " + String(ret));
		}
	}

	DBG("ScopeSyncOSCServer::run - OSC server shutdown");
}

bool ScopeSyncOSCServer::sendMessage(osc::OutboundPacketStream stream)
{
	if (!stream.IsReady())
	{
		DBG("ScopeSyncOSCServer::sendMessage - error OSC packet is not ready");
		return false;
	}

	if (!remoteDatagramSocket || remoteChanged) {
        remoteChanged = false;
        remoteDatagramSocket = new DatagramSocket(false);
    }

	if (remoteDatagramSocket->waitUntilReady(false, 100))
	{
		if (remoteDatagramSocket->write(remoteHostname, remotePortNumber, stream.Data(), stream.Size()) > 0)
			return true;
	}

	return false;
}

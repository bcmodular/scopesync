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
}

ScopeSyncOSCServer::~ScopeSyncOSCServer()
{
	signalThreadShouldExit();

	if (receiveDatagramSocket)
		receiveDatagramSocket->close();
	
	if (remoteDatagramSocket)
		remoteDatagramSocket->close();
	
	stopThread(500);
	
	receiveDatagramSocket = nullptr;
	remoteDatagramSocket  = nullptr;
}

void ScopeSyncOSCServer::setup(bool startListening)
{
	ScopeSyncOSCServer* oscServer = ScopeSyncOSCServer::getInstance();
    
	oscServer->setLocalPortNumber(UserSettings::getInstance()->getPropertyIntValue("osclocalportnum"));
    oscServer->setRemoteHostname(UserSettings::getInstance()->getPropertyValue("oscremotehost"));
    oscServer->setRemotePortNumber(UserSettings::getInstance()->getPropertyIntValue("oscremoteportnum"));

	if (startListening)
		oscServer->listen();
}

void ScopeSyncOSCServer::setLocalPortNumber(int portNumber)
{
	DBG("ScopeSyncOSCServer::setLocalPortNumber - changed local port number to: " + String(portNumber));
	receivePortNumber = portNumber;
}

int ScopeSyncOSCServer::getLocalPortNumber()
{
	return receivePortNumber;
}

const String& ScopeSyncOSCServer::getLocalHostname() 
{
	if (receiveDatagramSocket)
		return receiveDatagramSocket->getHostName();
  
	return String::empty;
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
	if (isThreadRunning())
	{
		signalThreadShouldExit();

		if (receiveDatagramSocket)
			receiveDatagramSocket->close();

		stopThread(500);
		receiveDatagramSocket = nullptr;
	}

	startThread(1);
}

void ScopeSyncOSCServer::stopListening() 
{
	if (isThreadRunning())
	{
		signalThreadShouldExit();
	
		if (receiveDatagramSocket)
			receiveDatagramSocket->close();
	
		stopThread(500);
		receiveDatagramSocket = nullptr;
	}
}

void ScopeSyncOSCServer::run()
{
	DBG("ScopeSyncOSCServer::run - Initialise ScopeSync OSC Server");

	receiveDatagramSocket = new DatagramSocket(receivePortNumber);

	MemoryBlock buffer(bufferSize, true);

	while (!threadShouldExit())
	{
		if (receiveDatagramSocket->getPort())
		{
			if (!receiveDatagramSocket->bindToPort(receivePortNumber))
			{
				DBG("ScopeSyncOSCServer::run - Error while binding to port");
				return;
			}
		}
	
		if (receiveDatagramSocket->waitUntilReady(true, 100))
		{
			int size = receiveDatagramSocket->read(buffer.getData(), buffer.getSize(), false);
		
			if (threadShouldExit())
			{
				DBG("ScopeSyncOSCServer::run - OSC server shutdown");
				return;
			}

			try
			{
				osc::ReceivedPacket packet((const char*)buffer.getData(), size);

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
				DBG("ScopeSyncOSCServer::run - error while parsing packet: " + String(e.what()));
			}
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

	if (!remoteDatagramSocket || remoteChanged)
	{
		remoteChanged        = false;
		remoteDatagramSocket = new DatagramSocket(0);
		remoteDatagramSocket->connect(remoteHostname, remotePortNumber);
	}

	if (remoteDatagramSocket->waitUntilReady(false, 100))
	{
		if (remoteDatagramSocket->write(stream.Data(), stream.Size()) > 0)
			return true;
	}

	return false;
}

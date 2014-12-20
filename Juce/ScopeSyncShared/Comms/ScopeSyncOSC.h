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

#ifndef SCOPESYNCOSC_H_INCLUDED
#define SCOPESYNCOSC_H_INCLUDED

#include <JuceHeader.h>

class ScopeSyncOSCMessage : public Message
{
public:
	ScopeSyncOSCMessage(osc::ReceivedPacket packet, bool isOutgoing = false)
		: packet(packet), 
		  isOutgoing(isOutgoing), 
		  isIncoming(isOutgoing) {}
	~ScopeSyncOSCMessage() {}
 
	osc::ReceivedPacket packet;
	bool                isOutgoing;
	bool                isIncoming;
};

class ScopeSyncOSCMessageListener : public MessageListener
{
	virtual void handleOSCMessage(osc::ReceivedPacket packet) = 0;

	void handleMessage(const Message& message) override
	{
		const ScopeSyncOSCMessage& receivedOscMessage = dynamic_cast<const ScopeSyncOSCMessage&>(message);
		handleOSCMessage(receivedOscMessage.packet);
	}
};

class ScopeSyncOSCMessageLogger : public MessageListener
{
	virtual void logOSCMessage(String message) = 0;

	void handleMessage(const Message& message) override
	{
		const ScopeSyncOSCMessage& oscMessage = dynamic_cast<const ScopeSyncOSCMessage&>(message);
    
		if (oscMessage.isIncoming)
			logOSCMessage("<-" + String(oscMessage.packet.Contents()));
		else 
			logOSCMessage("-> " + String(oscMessage.packet.Contents()));
	}
};

class ScopeSyncOSCServer : public Thread
{
	static const int bufferSize = 4098;

public:
	ScopeSyncOSCServer(ScopeSyncOSCMessageListener *listener);
	~ScopeSyncOSCServer();

	// UDP Setup
	void          setLocalPortNumber(int portNumber);
	int           getLocalPortNumber();
	const String& getLocalHostname();

	void   setRemoteHostname(String hostname);
	String getRemoteHostname();
	void   setRemotePortNumber(int portNumber);
	int    getRemotePortNumber();

	// UDP Server
	void listen();
	void stopListening();

	// Server Thread
	void run();

	// UDP Sender
	bool sendMessage(osc::OutboundPacketStream stream);

	// Logger
	void setLogger(ScopeSyncOSCMessageLogger* newLogger) { logger = newLogger; }
	void removeLogger() { logger = nullptr; }

private:
	ScopeSyncOSCMessageListener* listener;
	ScopeSyncOSCMessageLogger*   logger;

	int							  receivePortNumber;
	ScopedPointer<DatagramSocket> receiveDatagramSocket;

	String						  remoteHostname;
	int							  remotePortNumber;
	ScopedPointer<DatagramSocket> remoteDatagramSocket;
	bool					      remoteChanged;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeSyncOSCServer)
};

#endif  // SCOPESYNCOSC_H_INCLUDED

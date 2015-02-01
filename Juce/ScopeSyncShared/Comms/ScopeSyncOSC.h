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
class ScopeSync;

class ScopeSyncOSCServer : public Thread
{
public:
	ScopeSyncOSCServer();
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

	// Passes on the contents of the queue of updates received from the OSC Server
    void getOSCUpdatesArray(ScopeSync* scopeSync, HashMap<String, float, DefaultHashFunctions, CriticalSection>& oscUpdateArray);

	void registerListener(ScopeSync* scopeSync);
	void unregisterListener(ScopeSync* scopeSync);
	
	juce_DeclareSingleton (ScopeSyncOSCServer, false)

private:
	static const int bufferSize = 4098;

	int							  receivePortNumber;
	ScopedPointer<DatagramSocket> receiveDatagramSocket;

	String						  remoteHostname;
	int							  remotePortNumber;
	ScopedPointer<DatagramSocket> remoteDatagramSocket;
	bool					      remoteChanged;

	OwnedArray<HashMap<String, float, DefaultHashFunctions, CriticalSection>>           oscUpdatesArray; // Updates received from the OSC Server
	HashMap<ScopeSync*, HashMap<String, float, DefaultHashFunctions, CriticalSection>*> oscUpdateLookup;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeSyncOSCServer)
};

#endif  // SCOPESYNCOSC_H_INCLUDED

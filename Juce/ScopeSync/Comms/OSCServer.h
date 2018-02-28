/**
 * O(pen) S(ound) C(ontrol) message handler
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

#ifndef OSCSERVER_H_INCLUDED
#define OSCSERVER_H_INCLUDED

#include <JuceHeader.h>
#include "../Windows/UserSettings.h"

class ScopeSync;

class OSCServer : private OSCReceiver, protected Value::Listener, OSCReceiver::Listener<OSCReceiver::MessageLoopCallback>
{
public:
	OSCServer();
	~OSCServer();

	virtual void setup();

	// UDP Setup
	void   setLocalPortNumber(int portNumber);
	
	void   setRemoteHostname(String hostname);
	String getRemoteHostname() const;
	void   setRemotePortNumber(int portNumber);
	int    getRemotePortNumber() const;

	bool   sendFloatMessage(const OSCAddressPattern pattern, float valueToSend);
	bool   sendIntMessage(const OSCAddressPattern pattern, int valueToSend);

	void   registerOSCListener(ListenerWithOSCAddress<MessageLoopCallback>* newListener, OSCAddress address);
	void   unregisterOSCListener(ListenerWithOSCAddress<MessageLoopCallback>* listenerToRemove);

protected:
	Value oscLocalPortNum;
	Value oscRemoteHost;
	Value oscRemotePortNum;
	
	SharedResourcePointer<UserSettings> userSettings;

	bool connectToListener();
	void oscMessageReceived (const OSCMessage& message) override;

private:

	static const int bufferSize = 4098;

	int	   receivePortNumber;
		   
	String remoteHostname;
	int	   remotePortNumber;
	bool   remoteChanged;

	OSCSender sender;
	
	void valueChanged(Value& valueThatChanged) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCServer)
};

class ScopeOSCServer : public OSCServer
{
public:
	ScopeOSCServer();
	~ScopeOSCServer();

	void setup() override;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeOSCServer)
};

#endif  // OSCSERVER_H_INCLUDED

/**
 * Models a Scope OSC parameter
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

#ifndef SCOPEOSCPARAMETER_H_INCLUDED
#define SCOPEOSCPARAMETER_H_INCLUDED

#include <JuceHeader.h>
#include "../Comms/OSCServer.h"
#include "../Core/ScopeIDs.h"

class BCMParameter;

class ScopeOSCParameter : OSCReceiver::ListenerWithOSCAddress<OSCReceiver::MessageLoopCallback>
{
public:
	ScopeOSCParameter(ScopeOSCParamID oscParamID, BCMParameter* owner, ValueTree parameterDefinition);

	int  getValue() const {return intValue;}

	String getScopeCode();
	String getScopeParamText();

	void setOSCUID(int newUID);
	void updateValue(double linearNormalisedValue, double uiValue, double uiMinValue, double uiMaxValue);

	void startListening() {isListening = true;}
	void stopListening() {isListening = false;}

	void startSending() {isSending = true;}
	void stopSending() {isSending = false;}

private:
	SharedResourcePointer<ScopeOSCServer> oscServer;
	BCMParameter* parameter;
	
	ScopeOSCParamID paramID;
	String          scopeCode;

	int oscUID{};
    
	int intValue{};
	
	int    minValue;
	int    maxValue;
	double dBRef;

	bool isListening{};
	bool isSending{};

	int   findNearestParameterSetting(int value) const;
	double dbSkew(double valueToSkew, double ref, double uiMinValue, double uiMaxValue, bool invert) const;

	String getOSCPath() const;
	
	void oscMessageReceived(const OSCMessage& message) override;
};

#endif  // SCOPEOSCPARAMETER_H_INCLUDED

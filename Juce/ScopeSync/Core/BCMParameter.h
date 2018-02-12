/**
 * Models a Host or Scope parameter, including the handling
 * of its ValueTree and various values
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

#ifndef BCMPARAMETER_H_INCLUDED
#define BCMPARAMETER_H_INCLUDED

#include <JuceHeader.h>
#include "../Comms/OSCServer.h"
#include "../Core/ScopeIDs.h"
#include "../Core/ScopeOSCParameter.h"

class BCMParameterController;

class BCMParameter : public Value::Listener,
					 public Timer,
					 OSCReceiver::ListenerWithOSCAddress<OSCReceiver::MessageLoopCallback>
{
public:
    /* ============================ Enumerations ============================== */
    enum ParameterValueType    {continuous, discrete}; // Possible types of Parameter Value
	enum ParameterUpdateSource {internalUpdate, hostUpdate, guiUpdate, oscUpdate, midiUpdate, scopeOSCUpdate};

	BCMParameter(ValueTree parameterDefinition, BCMParameterController& pc, bool oscAble,
			     ScopeOSCParamID scopeOSCParamID);
    ~BCMParameter();

    void beginParameterChangeGesture();
    void endParameterChangeGesture();
    
    void          mapToUIValue(Value& valueToMapTo) const;
    String        getName() const;
	void          setHostIdx(int newIndex) { hostIdx = newIndex; }
    int           getHostIdx() const { return hostIdx; }
	ValueTree&    getDefinition() { return definition; }
    void          getSettings(ValueTree& settings) const;
    void          getDescriptions(String& shortDesc, String& fullDesc) const;
    void          getUIRanges(double& rangeMin, double& rangeMax, double& rangeInt, String& suffix) const;
	double        getUIRangeMin() const {return uiRangeMin;}
	double        getUIRangeMax() const {return uiRangeMax;}

    double        getUIResetValue() const;
    double        getUISkewFactor() const;

    void          getUITextValue(String& textValue) const;
    int           getUIValue() const { return uiValue.getValue(); };

    float         getHostValue() const;
    
	ScopeOSCParameter& getScopeOSCParameter() {return scopeOSCParameter;}

    bool isDiscrete() const;
	bool isReadOnly() const;
    
    void setHostValue(float newValue);
    void setUIValue(float newValue, bool updateHost = true);
	void setOSCValue(float newValue);

	void setParameterValues(ParameterUpdateSource updateSource, double newLinearNormalisedValue, double newUIValue, bool updateHost = true);

	double        convertLinearNormalisedToUIValue(double lnValue) const;
	double        convertUIToLinearNormalisedValue(double newValue) const;

	String        getOSCPath() const;
	void          sendOSCParameterUpdate() const;

private:
    WeakReference<BCMParameter>::Master masterReference;
    friend class WeakReference<BCMParameter>;

	SharedResourcePointer<OSCServer> oscServer;
	ScopeOSCParameter scopeOSCParameter;

	void oscMessageReceived (const OSCMessage& message) override;

    /* ====================== Private Parameter Methods ======================= */
    // Either on initialisation, or after ranges have changed, this method will
    // ensure the UI and LinearNormalised values are within the ranges
    void   putValuesInRange(bool initialise);
    void   setNumDecimalPlaces();
    float  skewHostValue(float hostValue, bool invert) const;

	void  valueChanged(Value& valueThatChanged) override;
	void  timerCallback() override;

	void  decDeadTimes();
    
    /* ===================== Private member variables ========================= */
	BCMParameterController& parameterController;

    bool      oscEnabled;
	bool	  readOnly;
    ValueTree definition;
    Value     uiValue;
    Value     linearNormalisedValue;
	Value     oscUID;
    int       hostIdx;
	int       numDecimalPlaces;

	double    uiRangeMin;
	double    uiRangeMax;
	double    uiRangeInterval;
	double    uiResetValue;
	String    uiSuffix;
	
	int oscDeadTimeCounter;

	static const int deadTimeTimerInterval;
	static const int maxOSCDeadTime;
};

#endif  // BCMPARAMETER_H_INCLUDED

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

class BCMParameter : public Value::Listener
{
public:
    /* ============================ Enumerations ============================== */
    enum ParameterValueType    {continuous, discrete}; // Possible types of Parameter Value
	enum ParameterUpdateSource {internalUpdate, hostUpdate, guiUpdate, oscUpdate, midiUpdate, scopeOSCUpdate};

	BCMParameter(ValueTree parameterDefinition, BCMParameterController& pc, ScopeOSCParamID scopeOSCParamID);
    ~BCMParameter();

    void beginParameterChangeGesture();
    void endParameterChangeGesture();
    
    void          mapToUIValue(Value& valueToMapTo) const;
    String        getName() const { return name; }
    ValueTree&    getDefinition() { return definition; }
	void          setHostIdx(int newIndex) { hostIdx = newIndex; }
    int           getHostIdx() const { return hostIdx; }
    void          getSettings(ValueTree& paramSettings) const { paramSettings = settings; }
    void          getDescriptions(String& shortDesc, String& fullDesc) const;
    void          getUIRanges(double& rangeMin, double& rangeMax, double& rangeInt, String& suffix) const;
	double        getUIRangeMin() const {return uiRangeMin;}
	double        getUIRangeMax() const {return uiRangeMax;}

    double        getUIResetValue() const { return uiResetValue; }
    double        getUISkewFactor() const { return uiSkewFactor; }

    void          getUITextValue(String& textValue) const;
    int           getUIValue() const { return uiValue.getValue(); };

    float         getHostValue() const;
    
	ScopeOSCParameter& getScopeOSCParameter() { return scopeOSCParameter; }

    static bool checkDiscrete(ValueTree& definition);
    bool isDiscrete() const { return paramDiscrete; }
    bool isReadOnly() const { return readOnly; }
    
    void setHostValue(float newValue);
    void setUIValue(float newValue, bool updateHost = true);
	
	void setParameterValues(ParameterUpdateSource updateSource, double newLinearNormalisedValue, double newUIValue, bool updateHost = true);

	double        convertLinearNormalisedToUIValue(double lnValue) const;
	double        convertUIToLinearNormalisedValue(double newValue) const;

private:
	JUCE_DECLARE_WEAK_REFERENCEABLE(BCMParameter)

	ScopeOSCParameter scopeOSCParameter;

    /* ====================== Private Parameter Methods ======================= */
    // Either on initialisation, or after ranges have changed, this method will
    // ensure the UI and LinearNormalised values are within the ranges
    void   putValuesInRange();
    void   setNumDecimalPlaces();
    float  skewHostValue(float hostValue, bool invert) const;

	void  valueChanged(Value& valueThatChanged) override;
	
    /* ===================== Private member variables ========================= */
	BCMParameterController& parameterController;

    String    name;
    String    shortDescription;
    String    fullDescription;
    bool      oscEnabled;
	bool	  readOnly;
    ValueTree definition;
    ValueTree settings;
    Value     uiValue;
    Value     linearNormalisedValue;
	Value     oscUID;
    int       hostIdx;
	int       numDecimalPlaces;
    bool      paramDiscrete;
    
	double    uiRangeMin;
	double    uiRangeMax;
	double    uiRangeInterval;
	double    uiResetValue;
    double    uiSkewFactor;
	String    uiSuffix;
    bool      skewUIOnly;
};

#endif  // BCMPARAMETER_H_INCLUDED

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

#ifndef BCMPARAMETER_H_INCLUDED
#define BCMPARAMETER_H_INCLUDED

#include <JuceHeader.h>
class ScopeSync;

class BCMParameter : public Value::Listener,
					 public Timer
{
public:
    /* ============================ Enumerations ============================== */
    enum ParameterType         {hostParameter, scopeLocal, preset};
    enum ParameterValueType    {continuous, discrete}; // Possible types of Parameter Value
	enum ParameterUpdateSource {internalUpdate, hostUpdate, guiUpdate, oscUpdate, midiUpdate, asyncUpdate, scopeAudioUpdate};
    
    BCMParameter(int index, ValueTree parameterDefinition, ParameterType parameterType, ScopeSync& ss);
    ~BCMParameter();

    void          mapToUIValue(Value& valueToMapTo);
    void          setAffectedByUI(bool isAffected);
    bool          isAffectedByUI();
    String        getName();
    int           getHostIdx() { return hostIdx; };
    int           getScopeCode();
    String        getScopeCodeText();
    ParameterType getParameterType() { return type; };
    ValueTree&    getDefinition() { return definition; };
    void          getSettings(ValueTree& settings);
    void          getDescriptions(String& shortDesc, String& fullDesc);
    void          getUIRanges(double& rangeMin, double& rangeMax, double& rangeInt, String& uiSuffix);
    void          getScopeRanges(int& min, int& max);
    double        getUIResetValue();
    double        getUISkewFactor();

    void  getUITextValue(String& textValue);
    float getHostValue();
    float getScopeFltValue();
    int   getScopeIntValue();

    bool isDiscrete();
    
    void setHostValue(float newValue);
    void setScopeFltValue(float newValue);
    void setScopeIntValue(int newValue);
    void setUIValue(float newValue);
	void setOSCValue(float newValue);

private:
    WeakReference<BCMParameter>::Master masterReference;
    friend class WeakReference<BCMParameter>;

    /* ====================== Private Parameter Methods ======================= */
    // Either on initialisation, or after ranges have changed, this method will
    // ensure the UI and LinearNormalised values are within the ranges
    void   putValuesInRange(bool initialise);
    void   setNumDecimalPlaces();
    float  skewHostValue(float hostValue, bool invert);
    double dbSkew(double valueToSkew, double ref, double uiMinValue, double uiMaxValue, bool invert);
    double convertLinearNormalisedToUIValue(double linearNormalisedValue);
	double convertUIToLinearNormalisedValue(double newValue);

    void  setParameterValues(ParameterUpdateSource updateSource, double newLinearNormalisedValue, double newUIValue);
    int   findNearestParameterSetting(int value);

	void  valueChanged(Value& valueThatChanged) override;
	void  timerCallback() override;

	void  decDeadTimes();
    
    /* ===================== Private member variables ========================= */
	ScopeSync&       scopeSync;
    ParameterType    type;
    ValueTree        definition;
    Value            uiValue;
    Value            linearNormalisedValue;
    bool             affectedByUI;
    int              hostIdx;
    int              numDecimalPlaces;
	
	int oscDeadTimeCounter;
	int asyncDeadTimeCounter;

	static const int deadTimeTimerInterval;
	static const int maxOSCDeadTime;
	static const int maxAsyncDeadTime;

};

#endif  // BCMPARAMETER_H_INCLUDED

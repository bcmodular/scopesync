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

#include "BCMParameter.h"
#include "../Utils/BCMMath.h"
#include "Global.h"
#include "ScopeSync.h"
#include "BCMParameterController.h"

#ifndef __DLL_EFFECT__
    #include "../Plugin/PluginProcessor.h"
#endif // __DLL_EFFECT__

const int BCMParameter::deadTimeTimerInterval = 100;
const int BCMParameter::maxOSCDeadTime   = 3;

ScopeOSCParameter::ScopeOSCParameter(ScopeOSCParamID oscParamID, BCMParameter* owner, int min, int max, bool discrete, double scopedBRef)
	: parameter(owner), paramID(oscParamID), minValue(min), maxValue(max), isDiscrete(discrete), dbRef(scopedBRef)
{
}

void ScopeOSCParameter::setOSCUID(int newUID)
{
	oscUID = newUID;
	oscServer->registerOSCListener(this, getOSCPath());
}

void ScopeOSCParameter::updateValue(double linearNormalisedValue, double uiValue, double uiMinValue, double uiMaxValue)
{
	int oldIntValue = intValue;

    if (isDiscrete)
    {
        int newValue = 0;
        
		ValueTree paramSettings;
		parameter->getSettings(paramSettings);

        if (paramSettings.isValid())
        {
            const int settingIdx = roundToInt(uiValue);
        
            if (settingIdx < paramSettings.getNumChildren())
                newValue = paramSettings.getChild(settingIdx).getProperty(Ids::intValue);
        }
        
        intValue = newValue;
    }
    else
    {
        double valueToScale = linearNormalisedValue;

        if (dbRef != 0.0f)
            valueToScale = dbSkew(valueToScale, dbRef, uiMinValue, uiMaxValue, true);

        intValue = roundToInt(scaleDouble(0.0f, 1.0f, minValue, maxValue, valueToScale));
    }

	if (oldIntValue != intValue)
		oscServer->sendIntMessage(getOSCPath(), intValue);
}

double ScopeOSCParameter::dbSkew(double valueToSkew, double ref, double uiMinValue, double uiMaxValue, bool invert) const
{
    if (!invert)
    {
        const double minNormalised = ref * pow(10, (uiMinValue/20));

        if (valueToSkew > minNormalised)
            return scaleDouble(uiMinValue, uiMaxValue, 0.0f, 1.0f, (20 * log10(valueToSkew / ref)));
        else
            return minNormalised;
    }
    else
    {
        valueToSkew = scaleDouble(0.0f, 1.0f, uiMinValue, uiMaxValue, valueToSkew);

        if (valueToSkew > uiMinValue && valueToSkew < uiMaxValue)
            return ref * pow(10, (valueToSkew / 20));
        else if (valueToSkew >= 0)
            return 1;
        else
            return 0;
    }
}

String ScopeOSCParameter::getOSCPath() const
{
	const String oscUIDStr(oscUID);
	String address = "/" + oscUIDStr + "/0/" + String(paramID.paramGroup) + "/" + String(paramID.paramId);
	DBG("ScopeSyncFX::ScopeOSCParameter::getScopeOSCPath = " + address);

	return address;
}

void ScopeOSCParameter::oscMessageReceived (const OSCMessage& message)
{
	String address = message.getAddressPattern().toString();

	DBG("ScopeOSCParameter::oscMessageReceived - " + address);

	if (message.size() == 1)
	{
		if (message[0].isInt32() && address == getOSCPath())
		{
			intValue = message[0].getInt32();
			DBG("ScopeOSCParameter::oscMessageReceived - new Scope OSC Value: " + String(intValue));
			
			parameter->setScopeIntValue(intValue);
		}
		else
		{
			DBG("BCMParameter::handleOSCMessage - OSC message not processed");
		}
	}
	else
		DBG("BCMParameter::handleOSCMessage - empty OSC message");
}

BCMParameter::BCMParameter(ValueTree parameterDefinition, BCMParameterController& pc, bool oscAble,
	ScopeOSCParamID scopeOSCParamID, int scopeMin, int scopeMax, bool isDiscrete, double dbRef)
    : scopeOSCParameter(scopeOSCParamID, this, scopeMin, scopeMax, isDiscrete, dbRef),
      parameterController(pc),
	  oscEnabled(oscAble),
	  definition(parameterDefinition),
      affectedByUI(false)
{
	initialise();
}

void BCMParameter::initialise()
{
	hostIdx = -1;
	
	// TODO: Read this from config file - 
	readOnly = false;

	oscDeadTimeCounter = 0;

	startTimer(deadTimeTimerInterval);
	
	putValuesInRange(true);
    setNumDecimalPlaces();
	uiValue.addListener(this);

    if (oscEnabled)
    {
        parameterController.getScopeSync()->referToOSCUID(oscUID);
        oscUID.addListener(this);
    }
}

BCMParameter::~BCMParameter()
{
    if (oscEnabled)
        oscServer->unregisterOSCListener(this);

	DBG("BCMParameter::~BCMParameter - deleting parameter: " + getName());
	masterReference.clear();
	stopTimer();
};

void BCMParameter::setNumDecimalPlaces()
{
    float uiInterval = definition.getProperty(Ids::uiRangeInterval);
    
    // figure out the number of DPs needed to display all values at this
    // interval setting.
    numDecimalPlaces = 7;

    if (uiInterval != 0)
    {
        int v = std::abs (roundToInt(uiInterval * 10000000));

        while ((v % 10) == 0)
        {
            --numDecimalPlaces;
            v /= 10;
        }
    }
}

void BCMParameter::setParameterValues(ParameterUpdateSource updateSource, double newLinearNormalisedValue, double newUIValue, bool updateHost)
{
	linearNormalisedValue = newLinearNormalisedValue;
	uiValue               = newUIValue;

	if (updateSource == oscUpdate)
		oscDeadTimeCounter = maxOSCDeadTime;

	if (updateSource != scopeOSCUpdate)
		scopeOSCParameter.updateValue(linearNormalisedValue.getValue(), uiValue.getValue(), definition.getProperty(Ids::uiRangeMin), definition.getProperty(Ids::uiRangeMax));

	#ifndef __DLL_EFFECT__
	if (updateHost && getHostIdx() >= 0)
		parameterController.updateHost(getHostIdx(), getHostValue());
	#endif // __DLL_EFFECT__
}

void BCMParameter::putValuesInRange(bool initialise)
{
    DBG("BCMParameter::putValuesInRange - " + String(getName()));
    float uiMinValue = definition.getProperty(Ids::uiRangeMin);
    float uiMaxValue = definition.getProperty(Ids::uiRangeMax);
    
    DBG("BCMParameter::putValuesInRange - uiMinValue: " + String(uiMinValue) + ", uiMaxValue: " + String(uiMaxValue));
    
	double newUIValue = 0.0f;

    if (initialise)
    {
        DBG("BCMParameter::putValuesInRange - Initialise to: " + definition.getProperty(Ids::uiResetValue).toString());
        newUIValue = definition.getProperty(Ids::uiResetValue);
    }
    else
    {
        if (float(uiValue.getValue()) < uiMinValue)
        {
            DBG("BCMParameter::putValuesInRange - Bumping up to: " + String(uiMinValue));
            newUIValue = uiMinValue;
        }
        else if (float(uiValue.getValue()) > uiMaxValue)
        {
            DBG("BCMParameter::putValuesInRange - Dropping down to: " + String(uiMaxValue));
            newUIValue = uiMaxValue;
        }
    }

	double newLinearNormalisedValue = scaleDouble(uiMinValue, uiMaxValue, 0.0f, 1.0f, newUIValue);

	setParameterValues(internalUpdate, newLinearNormalisedValue, newUIValue, false);
}

void BCMParameter::mapToUIValue(Value& valueToMapTo) const
{
    // DBG("BCMParameter::mapToUIValue - current uiValue: " + uiValue.getValue().toString());
    valueToMapTo.referTo(uiValue);
}

void BCMParameter::setAffectedByUI(bool isAffected)
{
    affectedByUI = isAffected;
}

bool BCMParameter::isAffectedByUI() const
{
    return affectedByUI;
}

String BCMParameter::getName() const
{
    return definition.getProperty(Ids::name).toString();
}

void BCMParameter::getSettings(ValueTree& settings) const
{
    settings = definition.getChildWithName(Ids::settings);
}

void BCMParameter::getDescriptions(String& shortDesc, String& fullDesc) const
{
    shortDesc = definition.getProperty(Ids::shortDescription).toString();
    fullDesc  = definition.getProperty(Ids::fullDescription).toString();
}

void BCMParameter::getUIRanges(double& rangeMin, double& rangeMax, double& rangeInt, String& uiSuffix) const
{
    rangeMin = definition.getProperty(Ids::uiRangeMin);
    rangeMax = definition.getProperty(Ids::uiRangeMax);

    if (rangeMin == rangeMax)
        rangeMax = rangeMin + 1.0f;
    rangeInt = definition.getProperty(Ids::uiRangeInterval);
    uiSuffix = definition.getProperty(Ids::uiSuffix);
}

double BCMParameter::getUIResetValue() const
{
    return definition.getProperty(Ids::uiResetValue);
}

double BCMParameter::getUISkewFactor() const
{
    return definition.getProperty(Ids::uiSkewFactor);
}

void BCMParameter::getUITextValue(String& textValue) const
{
    int parameterValueType = definition.getProperty(Ids::valueType);

    if (parameterValueType == discrete)
    {
        ValueTree paramSettings = definition.getChildWithName(Ids::settings);

        if (paramSettings.isValid())
        {
            int settingIdx = roundToInt(uiValue.getValue());
            String settingName = paramSettings.getChild(settingIdx).getProperty(Ids::name);
                    
            if (settingName.isNotEmpty())
                textValue = settingName;
        }
    }
    else
    {
        textValue = String(float(uiValue.getValue()), numDecimalPlaces) + definition.getProperty(Ids::uiSuffix).toString();
    }
    //DBG("BCMParameter::getUITextValue - " + definition.getProperty(paramNameId).toString() + ": " + textValue);
}

float BCMParameter::getHostValue() const
{
	float hostValue = linearNormalisedValue.getValue();

	if (!definition.getProperty(Ids::skewUIOnly))
		hostValue = skewHostValue(hostValue, true);

    // DBG("BCMParameter::getHostValue - " + definition.getProperty(Ids::name).toString() + ": " + String(hostValue));
    return hostValue;
}

bool BCMParameter::isDiscrete() const
{
    int parameterValueType = definition.getProperty(Ids::valueType);

    if (parameterValueType == discrete)
    {
        ValueTree parameterSettings = definition.getChildWithName(Ids::settings);

        if (parameterSettings.isValid() && parameterSettings.getNumChildren() > 0)
            return true;
    }

    return false;
}

bool BCMParameter::isReadOnly() const
{
	return readOnly;
}

double BCMParameter::convertLinearNormalisedToUIValue(double lnValue) const
{
    double minUIValue;
    double maxUIValue;
    double uiInterval;
    String uiSuffix;
    
    getUIRanges(minUIValue, maxUIValue, uiInterval, uiSuffix);
    
    return scaleDouble(0.0f, 1.0f, minUIValue, maxUIValue, lnValue);
}

double BCMParameter::convertUIToLinearNormalisedValue(double newValue) const
{
    double minUIValue;
    double maxUIValue;
    double uiInterval;
    String uiSuffix;
    
    getUIRanges(minUIValue, maxUIValue, uiInterval, uiSuffix);
    
    return scaleDouble(minUIValue, maxUIValue, 0.0f, 1.0f, newValue);
}

void BCMParameter::setHostValue(float newValue)
{
	if (!definition.getProperty(Ids::skewUIOnly))
		newValue = skewHostValue(newValue, false);

	double newUIValue               = convertLinearNormalisedToUIValue(newValue);
    
	setParameterValues(hostUpdate, newValue, newUIValue, false);
    
	DBG("BCMParameter::setHostValue - " + definition.getProperty(Ids::name).toString() + " linearNormalisedValue: " + linearNormalisedValue.toString() + ", uiValue: " + uiValue.toString());
}

void BCMParameter::setScopeIntValue(int newValue)
{
	if (!affectedByUI)
    {
		double newUIValue;
		double newLinearNormalisedValue;

        int parameterValueType = definition.getProperty(Ids::valueType);

        if (parameterValueType == discrete)
        {
            int newSetting = findNearestParameterSetting(newValue);
            
			newUIValue = newSetting;
			newLinearNormalisedValue = convertUIToLinearNormalisedValue(newUIValue);
        }
        else
        {
            int minScopeValue;
            int maxScopeValue;

            scopeOSCParameter.getRanges(minScopeValue, maxScopeValue);
            newLinearNormalisedValue = scaleDouble(minScopeValue, maxScopeValue, 0.0f, 1.0f, newValue);

            double ref        = definition.getProperty(Ids::scopeDBRef);
        
            if (ref != 0.0f)
            {
                double uiMinValue = definition.getProperty(Ids::uiRangeMin);
                double uiMaxValue = definition.getProperty(Ids::uiRangeMax);
            
                newLinearNormalisedValue = dbSkew(newLinearNormalisedValue, ref, uiMinValue, uiMaxValue, false);
            }
			//else
			//{
			//	if (definition.getProperty(Ids::skewUIOnly))
			//		newLinearNormalisedValue = skewHostValue(newLinearNormalisedValue, false);
			//}

            newUIValue = convertLinearNormalisedToUIValue(newLinearNormalisedValue);
        }
        
		setParameterValues(scopeOSCUpdate, newLinearNormalisedValue, newUIValue);

        DBG("BCMParameter::setScopeIntValue - " + definition.getProperty(Ids::name).toString() + " linearNormalisedValue: " + linearNormalisedValue.toString() + ", uiValue: " + uiValue.toString() + ", scopeValue: " + String(scopeOSCParameter.getValue()));
    }
    else
    {
        DBG("ScopeSync::receiveUpdatesFromScopeAsync: Parameter affected by UI since last update, or still in async dead time: " + definition.getProperty(Ids::name).toString());
        // TODO: Look into what's going on here
		//sendToScopeSyncAsync();
    }
}

void BCMParameter::setUIValue(float newValue, bool updateHost)
{
	double newUIValue = newValue;
    double newLinearNormalisedValue = convertUIToLinearNormalisedValue(newUIValue);

	//if (ScopeSyncApplication::inScopeFXContext() && definition.getProperty(Ids::skewUIOnly))
	//	newLinearNormalisedValue = skewHostValue(newLinearNormalisedValue, true);

	setParameterValues(guiUpdate, newLinearNormalisedValue, newUIValue, updateHost);
    DBG("BCMParameter::setUIValue - " + definition.getProperty(Ids::name).toString() + " linearNormalisedValue: " + linearNormalisedValue.toString() + ", uiValue: " + uiValue.toString() + ", scopeValue: " + String(scopeOSCParameter.getValue()));
}

void BCMParameter::setOSCValue(float newValue)
{
	double newUIValue = newValue;
    double newLinearNormalisedValue = convertUIToLinearNormalisedValue(newUIValue);

	setParameterValues(oscUpdate, newLinearNormalisedValue, newUIValue);
}

void BCMParameter::timerCallback()
{
	decDeadTimes();
}

void BCMParameter::decDeadTimes()
{
	if (oscDeadTimeCounter > 0)
        oscDeadTimeCounter--;
}

float BCMParameter::skewHostValue(float hostValue, bool invert) const
{
    double skewedValue = hostValue;
    double skewFactor = definition.getProperty(Ids::uiSkewFactor);

    if ((skewFactor != 1.0f))
    {
        skewValue(skewedValue, skewFactor, 0.0f, 1.0f, invert);
    }
    
    return static_cast<float>(skewedValue);
}

int BCMParameter::findNearestParameterSetting(int value) const
{
    ValueTree settings = definition.getChildWithName(Ids::settings);

    int nearestItem = 0;
    int smallestGap = INT_MAX;

    for (int i = 0; i < settings.getNumChildren(); i++)
    {
        int settingValue = settings.getChild(i).getProperty(Ids::intValue);
        int gap = abs(value - settingValue);

        if (gap == 0)
        {
            DBG("BCMParameter::findNearestParameterSetting - Found 'exact' match for setting: " + settings.getChild(i).getProperty(Ids::name).toString());

            nearestItem = i;
            break;
        }
        else if (gap < smallestGap)
        {
            smallestGap = gap;
            nearestItem = i;
        }
    }

    return nearestItem;
}

void BCMParameter::valueChanged(Value& valueThatChanged)
{
	if (valueThatChanged.refersToSameSourceAs(uiValue))
	{
		if (oscEnabled && oscDeadTimeCounter == 0)
			sendOSCParameterUpdate();
	}
	else if (valueThatChanged.refersToSameSourceAs(oscUID))
	{
		oscServer->registerOSCListener(this, getOSCPath());
	}
}

String BCMParameter::getOSCPath() const
{
	String oscUIDStr = oscUID.getValue();
	String address = "/" + oscUIDStr + "/" + String(hostIdx);

	return address;
}
  
void BCMParameter::sendOSCParameterUpdate() const
{
    oscServer->sendFloatMessage(getOSCPath(), uiValue.getValue());
}

void BCMParameter::oscMessageReceived (const OSCMessage& message)
{
	String address = message.getAddressPattern().toString();

	DBG("BCMParameter::oscMessageReceived - " + address);

	if (message.size() == 1)
	{
		if (message[0].isFloat32() && address == getOSCPath())
		{
			float newValue = message[0].getFloat32();
			DBG("BCMParameter::oscMessageReceived - new OSC Value: " + String(newValue));
			
			setOSCValue(newValue);
		}
		else
			DBG("BCMParameter::handleOSCMessage - OSC message not processed");
	}
	else
		DBG("BCMParameter::handleOSCMessage - empty OSC message");
}

void BCMParameter::beginParameterChangeGesture()
{
#ifndef __DLL_EFFECT__
    parameterController.beginParameterChangeGesture(getHostIdx());
#else
     setAffectedByUI(true);
#endif // __DLL_EFFECT__
}

void BCMParameter::endParameterChangeGesture()
{
#ifndef __DLL_EFFECT__
    parameterController.endParameterChangeGesture(getHostIdx()); 
#else
    setAffectedByUI(false);
#endif // __DLL_EFFECT__
}

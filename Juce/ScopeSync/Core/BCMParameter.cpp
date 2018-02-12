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

BCMParameter::BCMParameter(ValueTree parameterDefinition, BCMParameterController& pc, bool oscAble,
	ScopeOSCParamID scopeOSCParamID)
    : scopeOSCParameter(scopeOSCParamID, this, parameterDefinition),
      parameterController(pc),
	  oscEnabled(oscAble),
	  definition(parameterDefinition),
      uiRangeMin(definition.getProperty(Ids::uiRangeMin)),
	  uiRangeMax(definition.getProperty(Ids::uiRangeMax)),
	  uiRangeInterval(definition.getProperty(Ids::uiRangeInterval)),
	  uiResetValue(definition.getProperty(Ids::uiResetValue)),
	  uiSuffix(definition.getProperty(Ids::uiSuffix).toString()),
	  hostIdx(-1),
	  readOnly(false),
	  oscDeadTimeCounter(0),
	  numDecimalPlaces(7)
{
    if (uiRangeMin == uiRangeMax)
        uiRangeMax = uiRangeMin + 1.0f;

	startTimer(deadTimeTimerInterval);
	
	putValuesInRange(true);
    setNumDecimalPlaces();
	uiValue.addListener(this);

    if (oscEnabled)
    {
        parameterController.getScopeSync()->referToOSCUID(oscUID);
        oscUID.addListener(this);
		oscServer->registerOSCListener(this, getOSCPath());
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
		scopeOSCParameter.updateValue(linearNormalisedValue.getValue(), uiValue.getValue(), uiRangeMin, uiRangeMax);

	#ifndef __DLL_EFFECT__
	if (updateHost && getHostIdx() >= 0)
		parameterController.updateHost(getHostIdx(), getHostValue());
	#else
	(void)updateHost;
	#endif // __DLL_EFFECT__
}

void BCMParameter::putValuesInRange(bool initialise)
{
    DBG("BCMParameter::putValuesInRange - " + String(getName()));
    DBG("BCMParameter::putValuesInRange - uiMinValue: " + String(uiRangeMin) + ", uiMaxValue: " + String(uiRangeMax));
    
	double newUIValue = 0.0f;

    if (initialise)
    {
        DBG("BCMParameter::putValuesInRange - Initialise to: " + String(uiResetValue));
        newUIValue = uiResetValue;
    }
    else
    {
        if (float(uiValue.getValue()) < uiRangeMin)
        {
            DBG("BCMParameter::putValuesInRange - Bumping up to: " + String(uiRangeMin));
            newUIValue = uiRangeMin;
        }
        else if (float(uiValue.getValue()) > uiRangeMax)
        {
            DBG("BCMParameter::putValuesInRange - Dropping down to: " + String(uiRangeMax));
            newUIValue = uiRangeMax;
        }
    }

	double newLinearNormalisedValue = scaleDouble(uiRangeMin, uiRangeMax, 0.0f, 1.0f, newUIValue);

	setParameterValues(internalUpdate, newLinearNormalisedValue, newUIValue, false);
}

void BCMParameter::mapToUIValue(Value& valueToMapTo) const
{
    // DBG("BCMParameter::mapToUIValue - current uiValue: " + uiValue.getValue().toString());
    valueToMapTo.referTo(uiValue);
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

void BCMParameter::getUIRanges(double& rangeMin, double& rangeMax, double& rangeInt, String& suffix) const
{
    rangeMin = uiRangeMin;
    rangeMax = uiRangeMax;
	rangeInt = uiRangeInterval;
    suffix   = uiSuffix;
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
     scopeOSCParameter.stopListening();
#endif // __DLL_EFFECT__
}

void BCMParameter::endParameterChangeGesture()
{
#ifndef __DLL_EFFECT__
    parameterController.endParameterChangeGesture(getHostIdx()); 
#else
    scopeOSCParameter.startListening();
#endif // __DLL_EFFECT__
}
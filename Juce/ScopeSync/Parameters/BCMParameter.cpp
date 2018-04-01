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
#include "../Core/Global.h"
#include "../Core/ScopeSync.h"
#include "BCMParameterController.h"

#ifndef __DLL_EFFECT__
    #include "../Plugin/PluginProcessor.h"
	#include "../Parameters/HostParameter.h"
#endif // __DLL_EFFECT__

BCMParameter::BCMParameter(ValueTree parameterDefinition, BCMParameterController& pc,
	ScopeOSCParamID scopeOSCParamID)
    : scopeOSCParameter(scopeOSCParamID, this, parameterDefinition),
	  updateSourceBlock(none),
	  parameterController(pc),
      name(parameterDefinition.getProperty(Ids::name).toString()),
      shortDescription(parameterDefinition.getProperty(Ids::shortDescription).toString()),
      fullDescription(parameterDefinition.getProperty(Ids::fullDescription).toString()),
      readOnly(false),
      definition(parameterDefinition),
      settings(parameterDefinition.getChildWithName(Ids::settings)),
	  hostIdx(-1),
	  numDecimalPlaces(7),
      paramDiscrete(checkDiscrete(parameterDefinition)),
	  uiRangeMin(parameterDefinition.getProperty(Ids::uiRangeMin)),
	  uiRangeMax(parameterDefinition.getProperty(Ids::uiRangeMax)),
	  uiRangeInterval(parameterDefinition.getProperty(Ids::uiRangeInterval)),
	  uiResetValue(parameterDefinition.getProperty(Ids::uiResetValue)),
      uiSkewFactor(parameterDefinition.getProperty(Ids::uiSkewFactor)),
	  uiSuffix(parameterDefinition.getProperty(Ids::uiSuffix).toString()),
      skewUIOnly(parameterDefinition.getProperty(Ids::skewUIOnly))
{
    if (uiRangeMin == uiRangeMax)
        uiRangeMax = uiRangeMin + 1.0f;

	putValuesInRange();
    setNumDecimalPlaces();
	uiValue.addListener(this);

    parameterController.getScopeSync()->referToDeviceInstance(deviceInstance);
    deviceInstance.addListener(this);
	
	parameterController.getScopeSync()->referToConfigurationUID(configurationUID);
    configurationUID.addListener(this);

	scopeOSCParameter.setDeviceInstance(deviceInstance.getValue());
	scopeOSCParameter.startListening();
}

BCMParameter::~BCMParameter()
{
	stopTimer();
	DBG("BCMParameter::~BCMParameter - deleting parameter: " + getName());
	masterReference.clear();
};

void BCMParameter::setNumDecimalPlaces()
{
    // figure out the number of DPs needed to display all values at this
    // interval setting.
    if (uiRangeInterval != 0)
    {
        int v = std::abs (roundToInt(uiRangeInterval * 10000000));

        while ((v % 10) == 0)
        {
            --numDecimalPlaces;
            v /= 10;
        }
    }
}

void BCMParameter::setParameterValues(ParameterUpdateSource updateSource, double newLinearNormalisedValue, double newUIValue, bool forceUpdate)
{
	// Enforce update block if one is in place
	if (!forceUpdate && updateSourceBlock != none && updateSourceBlock != updateSource)
		return;
		
	// Set update block
	updateSourceBlock = updateSource;
	startTimer(200);

	DBG("BCMParameter::setParameterValues - updateSource: " + String(updateSource));
	linearNormalisedValue = newLinearNormalisedValue;
	uiValue               = newUIValue;

	if (updateSource != scopeOSCUpdate)
		scopeOSCParameter.updateValue(linearNormalisedValue.getValue(), uiValue.getValue(), uiRangeMin, uiRangeMax);

	#ifndef __DLL_EFFECT__
	if (updateSource != hostUpdate && updateSource != internalUpdate && hostParameter != nullptr)
		hostParameter->setValueNotifyingHost(getHostValue());
	#endif // __DLL_EFFECT__
}

void BCMParameter::putValuesInRange()
{
    DBG("BCMParameter::putValuesInRange - Initialise to: " + String(uiResetValue));
    double newLinearNormalisedValue = scaleDouble(uiRangeMin, uiRangeMax, 0.0f, 1.0f, uiResetValue);

	setParameterValues(internalUpdate, newLinearNormalisedValue, uiResetValue);
}

void BCMParameter::mapToUIValue(Value& valueToMapTo) const
{
    // DBG("BCMParameter::mapToUIValue - current uiValue: " + uiValue.getValue().toString());
    valueToMapTo.referTo(uiValue);
}

String BCMParameter::getName() const 
{
	return name; 
}

#ifndef __DLL_EFFECT__
void BCMParameter::setHostParameter(HostParameter * hostParam)
{
	hostParameter = hostParam;
}

HostParameter * BCMParameter::getHostParameter() const
{ 
	return hostParameter;
}
#endif // __DLL_EFFECT__

String BCMParameter::getFullDescription(bool includeScopeParam) const
{
	String scopeParameterText(scopeOSCParameter.getScopeParamText());

	if (includeScopeParam && !scopeParameterText.equalsIgnoreCase("-1:-1"))
		return fullDescription + " (" + scopeParameterText + ")";

	return fullDescription;
}

void BCMParameter::getUIRanges(double& rangeMin, double& rangeMax, double& rangeInt, String& suffix) const
{
    rangeMin = uiRangeMin;
    rangeMax = uiRangeMax;
	rangeInt = uiRangeInterval;
    suffix   = uiSuffix;
}

// TODO: Replace this with a version that takes in a value...
void BCMParameter::getUITextValue(String& textValue) const
{
    if (paramDiscrete)
    {
        if (settings.isValid())
        {
            int settingIdx = roundToInt(uiValue.getValue());
            String settingName = settings.getChild(settingIdx).getProperty(Ids::name);
                    
            if (settingName.isNotEmpty())
                textValue = settingName;
        }
    }
    else
    {
        textValue = String(float(uiValue.getValue()), numDecimalPlaces) + uiSuffix;
    }
    //DBG("BCMParameter::getUITextValue - " + definition.getProperty(paramNameId).toString() + ": " + textValue);
}

float BCMParameter::getHostValue() const
{
	float hostValue = linearNormalisedValue.getValue();

	if (!skewUIOnly)
		hostValue = skewHostValue(hostValue, true);

    // DBG("BCMParameter::getHostValue - " + definition.getProperty(Ids::name).toString() + ": " + String(hostValue));
    return hostValue;
}

float BCMParameter::getDefaultHostValue() const
{
	float hostValue = float(convertUIToLinearNormalisedValue(uiResetValue));

	if (!skewUIOnly)
		hostValue = skewHostValue(hostValue, true);

	return hostValue;
}

int BCMParameter::getConfigurationUID() const
{
	return parameterController.getScopeSync()->getConfigurationUID();
}

bool BCMParameter::checkDiscrete(ValueTree& definition)
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

double BCMParameter::convertLinearNormalisedToUIValue(double lnValue) const
{
   return scaleDouble(0.0f, 1.0f, uiRangeMin, uiRangeMax, lnValue);
}

double BCMParameter::convertUIToLinearNormalisedValue(double newValue) const
{
   return scaleDouble(uiRangeMin, uiRangeMax, 0.0f, 1.0f, newValue);
}

double BCMParameter::convertHostToUIValue(double newValue) const
{
	if (!skewUIOnly)
		newValue = double(skewHostValue(float(newValue), false));

	return convertLinearNormalisedToUIValue(newValue);
}

void BCMParameter::setHostValue(float newValue, bool forceUpdate)
{
	double newUIValue = convertHostToUIValue(newValue);
    
	setParameterValues(hostUpdate, newValue, newUIValue, forceUpdate);
    
	DBG("BCMParameter::setHostValue - " + name + " linearNormalisedValue: " + linearNormalisedValue.toString() + ", uiValue: " + uiValue.toString());
}

void BCMParameter::setUIValue(float newValue)
{
	double newUIValue = newValue;
    double newLinearNormalisedValue = convertUIToLinearNormalisedValue(newUIValue);

	//if (ScopeSyncApplication::inScopeFXContext() && definition.getProperty(Ids::skewUIOnly))
	//	newLinearNormalisedValue = skewHostValue(newLinearNormalisedValue, true);

	setParameterValues(guiUpdate, newLinearNormalisedValue, newUIValue);
    DBG("BCMParameter::setUIValue - " + name + " linearNormalisedValue: " + linearNormalisedValue.toString() + ", uiValue: " + uiValue.toString() + ", scopeValue: " + String(scopeOSCParameter.getValue()));
}

float BCMParameter::skewHostValue(float hostValue, bool invert) const
{
    double skewedValue = hostValue;
    
    if ((uiSkewFactor != 1.0f))
        skewValue(skewedValue, uiSkewFactor, 0.0f, 1.0f, invert);
    
    return float(skewedValue);
}

void BCMParameter::valueChanged(Value& valueThatChanged)
{
	if (valueThatChanged.refersToSameSourceAs(uiValue))
	{
		DBG("BCMParameter::valueChanged (uiValue)");
		setUIValue(float(valueThatChanged.getValue()));
	}
	else if (valueThatChanged.refersToSameSourceAs(deviceInstance))
		scopeOSCParameter.setDeviceInstance(deviceInstance.getValue());
	else if (valueThatChanged.refersToSameSourceAs(configurationUID))
		scopeOSCParameter.setConfigurationUID(configurationUID.getValue());
}

void BCMParameter::timerCallback()
{
	updateSourceBlock = none;
	stopTimer();
}

void BCMParameter::beginChangeGesture()
{
#ifndef __DLL_EFFECT__
	if (hostParameter != nullptr)
		hostParameter->beginChangeGesture();
#endif // __DLL_EFFECT__
}

void BCMParameter::endChangeGesture()
{
#ifndef __DLL_EFFECT__
	if (hostParameter != nullptr)
		hostParameter->endChangeGesture();
#endif // __DLL_EFFECT__
}

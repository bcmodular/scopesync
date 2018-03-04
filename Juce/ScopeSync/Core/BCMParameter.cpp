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

BCMParameter::BCMParameter(ValueTree parameterDefinition, BCMParameterController& pc,
	ScopeOSCParamID scopeOSCParamID)
    : scopeOSCParameter(scopeOSCParamID, this, parameterDefinition),
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

    parameterController.getScopeSync()->referToOSCUID(oscUID);
    oscUID.addListener(this);
	
	scopeOSCParameter.setOSCUID(oscUID.getValue());
	scopeOSCParameter.startListening();
}

BCMParameter::~BCMParameter()
{
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

void BCMParameter::setParameterValues(ParameterUpdateSource updateSource, double newLinearNormalisedValue, double newUIValue, bool updateHost)
{
	linearNormalisedValue = newLinearNormalisedValue;
	uiValue               = newUIValue;

	if (updateSource != scopeOSCUpdate)
		scopeOSCParameter.updateValue(linearNormalisedValue.getValue(), uiValue.getValue(), uiRangeMin, uiRangeMax);

	#ifndef __DLL_EFFECT__
	if (updateHost && getHostIdx() >= 0)
		parameterController.updateHost(getHostIdx(), getHostValue());
	#else
	(void)updateHost;
	#endif // __DLL_EFFECT__
}

void BCMParameter::putValuesInRange()
{
    DBG("BCMParameter::putValuesInRange - Initialise to: " + String(uiResetValue));
    double newLinearNormalisedValue = scaleDouble(uiRangeMin, uiRangeMax, 0.0f, 1.0f, uiResetValue);

	setParameterValues(internalUpdate, newLinearNormalisedValue, uiResetValue, false);
}

void BCMParameter::mapToUIValue(Value& valueToMapTo) const
{
    // DBG("BCMParameter::mapToUIValue - current uiValue: " + uiValue.getValue().toString());
    valueToMapTo.referTo(uiValue);
}

void BCMParameter::getDescriptions(String& shortDesc, String& fullDesc) const
{
    shortDesc = shortDescription;
    fullDesc  = fullDescription;
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
	float hostValue = convertUIToLinearNormalisedValue(uiResetValue);

	if (!skewUIOnly)
		hostValue = skewHostValue(hostValue, true);

	return hostValue;
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
		newValue = skewHostValue(newValue, false);

	return convertLinearNormalisedToUIValue(newValue);
}

void BCMParameter::setHostValue(float newValue)
{
	double newUIValue = convertHostToUIValue(newValue);
    
	setParameterValues(hostUpdate, newValue, newUIValue, false);
    
	DBG("BCMParameter::setHostValue - " + name + " linearNormalisedValue: " + linearNormalisedValue.toString() + ", uiValue: " + uiValue.toString());
}

void BCMParameter::setUIValue(float newValue, bool updateHost)
{
	double newUIValue = newValue;
    double newLinearNormalisedValue = convertUIToLinearNormalisedValue(newUIValue);

	//if (ScopeSyncApplication::inScopeFXContext() && definition.getProperty(Ids::skewUIOnly))
	//	newLinearNormalisedValue = skewHostValue(newLinearNormalisedValue, true);

	setParameterValues(guiUpdate, newLinearNormalisedValue, newUIValue, updateHost);
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
		// TODO: Probably this is where buttons need to be handled (i.e. send to scopeInt)
	}
	else if (valueThatChanged.refersToSameSourceAs(oscUID))
	{
		scopeOSCParameter.setOSCUID(oscUID.getValue());
	}
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

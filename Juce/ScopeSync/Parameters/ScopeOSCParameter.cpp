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

#include "BCMParameter.h"
#include "../Utils/BCMMath.h"
#include "../Core/Global.h"

ScopeOSCParameter::ScopeOSCParameter(ScopeOSCParamID oscParamID, BCMParameter* owner, ValueTree parameterDefinition)
	: parameter(owner),
      paramID(oscParamID), 
	  oscUID(0),
	  minValue(parameterDefinition.getProperty(Ids::scopeRangeMin)),
	  maxValue(parameterDefinition.getProperty(Ids::scopeRangeMax)),
	  dBRef(parameterDefinition.getProperty(Ids::scopeDBRef)),
	  isListening(false),
	  isSending(false)
{
	DBG("ScopeOSCParameter::ScopeOSCParameter - creating new parameter with paramID: " + String(paramID.paramGroup) + ":" + String(paramID.paramId));
}

ScopeOSCParameter::~ScopeOSCParameter()
{
	stopTimer();
	oscServer->unregisterOSCListener(this);
}

String ScopeOSCParameter::getScopeParamText() const
{
	return String(paramID.paramGroup) + ":" + String(paramID.paramId);
}

void ScopeOSCParameter::setOSCUID(int newUID)
{
	DBG("ScopeOSCParameter::setOSCUID - " + String(newUID));
	oscUID = newUID;
	DBG("ScopeOSCParameter::setOSCUID - Registering as listener to " + getOSCPath());
	oscServer->registerOSCListener(this, getOSCPath());
}

void ScopeOSCParameter::sendCurrentValue()
{
	DBG("ScopeOSCParameter::sendCurrentValue - " + getOSCPath() + " " + String(intValue));
	oscServer->sendIntMessage(getOSCPath(), intValue);
	stopListening();
	startTimer(500);
}

void ScopeOSCParameter::updateValue(int newValue)
{
	intValue = newValue;
	sendCurrentValue();
}

void ScopeOSCParameter::updateValue(double linearNormalisedValue, double uiValue, double uiMinValue, double uiMaxValue)
{
	int oldIntValue = intValue;

    if (parameter->isDiscrete())
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

        if (dBRef != 0.0f)
            valueToScale = dbSkew(valueToScale, dBRef, uiMinValue, uiMaxValue, true);

        intValue = roundToInt(scaleDouble(0.0f, 1.0f, minValue, maxValue, valueToScale));
    }

	if (oldIntValue != intValue)
		sendCurrentValue();
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
	//DBG("ScopeOSCParameter::getScopeOSCPath = " + address);

	return address;
}

void ScopeOSCParameter::oscMessageReceived (const OSCMessage& message)
{
	if (!isListening)
		return;

	String address = message.getAddressPattern().toString();

	//DBG("ScopeOSCParameter::oscMessageReceived - " + address);

	if (message.size() == 1)
	{
		if (message[0].isInt32() && address == getOSCPath())
		{
			intValue = message[0].getInt32();
			//DBG("ScopeOSCParameter::oscMessageReceived - new Scope OSC Value: " + String(intValue));
			
			double newUIValue;
			double newLinearNormalisedValue;

			if (parameter->isDiscrete())
			{
				int newSetting = findNearestParameterSetting(intValue);
            
				newUIValue = newSetting;
				newLinearNormalisedValue = parameter->convertUIToLinearNormalisedValue(newUIValue);
			}
			else
			{
				newLinearNormalisedValue = scaleDouble(minValue, maxValue, 0.0f, 1.0f, intValue);

				if (dBRef != 0.0f)
					newLinearNormalisedValue = dbSkew(newLinearNormalisedValue, dBRef, parameter->getUIRangeMin(), parameter->getUIRangeMax(), false);
					
				newUIValue = parameter->convertLinearNormalisedToUIValue(newLinearNormalisedValue);
			}
        
			parameter->setParameterValues(BCMParameter::scopeOSCUpdate, newLinearNormalisedValue, newUIValue);

			DBG("ScopeOSCParameter::oscMessageReceived - " + parameter->getName() + " linearNormalisedValue: " + String(newLinearNormalisedValue) + ", uiValue: " + String(newUIValue) + ", scopeValue: " + String(intValue));
		}
		else
		{
			DBG("ScopeOSCParameter::oscMessageReceived - OSC message not processed");
		}
	}
	else
		DBG("ScopeOSCParameter::oscMessageReceived - empty OSC message");
}

void ScopeOSCParameter::timerCallback()
{
	startListening();
	stopTimer();
}

int ScopeOSCParameter::findNearestParameterSetting(int value) const
{
	ValueTree paramSettings;
	parameter->getSettings(paramSettings);

    int nearestItem = 0;
    int smallestGap = INT_MAX;

    for (int i = 0; i < paramSettings.getNumChildren(); i++)
    {
        int settingValue = paramSettings.getChild(i).getProperty(Ids::intValue);
        int gap = abs(value - settingValue);

        if (gap == 0)
        {
            DBG("BCMParameter::findNearestParameterSetting - Found 'exact' match for setting: " + paramSettings.getChild(i).getProperty(Ids::name).toString());

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

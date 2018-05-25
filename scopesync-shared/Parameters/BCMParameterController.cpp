/**
 * Sets up all parameters and then provides access to them for
 * other objects as necessary
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

#include "BCMParameterController.h"
#include "../Core/ScopeSyncApplication.h"
#include "../Utils/BCMXml.h"
#include "../Core/Global.h"

#ifndef __DLL_EFFECT__
    #include "../Plugin/PluginProcessor.h"
#endif // __DLL_EFFECT__

BCMParameterController::BCMParameterController(ScopeSync* owner) :
parameterValueStore("parametervalues"), scopeSync(owner)
{
}

void BCMParameterController::initialise()
{
#ifndef __DLL_EFFECT__
	for (int i = 0; i < 512; i++)
		scopeSync->getPluginProcessor()->addParameter(new HostParameter());
#else
	const StringArray scopeFixedParameters = StringArray::fromTokens("DUMMY,X,Y,Show,Show Preset Window,Show Patch Window,Mono Effect,BypassEffect,Show Shell Preset Window,Voice Count,MIDI Channel,Device Type,MIDI Activity",",", "");
    
    for (int i = 1; i < scopeFixedParameters.size(); i++)
        addFixedScopeParameter(scopeFixedParameters[i], i);
#endif //__DLL_EFFECT__
}

void BCMParameterController::addFixedScopeParameter(const String& name, const int scopeParamId)
{
	DBG("BCMParameterController::addFixedScopeParameter - adding: " + name + ", scopeParamId: " + String(scopeParamId));

    ValueTree tmpParameter = Configuration::getDefaultFixedParameter();
    tmpParameter.setProperty(Ids::name, name, nullptr);
    tmpParameter.setProperty(Ids::shortDescription, name, nullptr);
    tmpParameter.setProperty(Ids::fullDescription, name, nullptr);
    tmpParameter.setProperty(Ids::scopeParamId, scopeParamId, nullptr);
	tmpParameter.setProperty(Ids::scopeParamGroup, 0, nullptr);

	if (name == "MIDI Channel")
	{
		// MIDI Channel is a Discrete parameter, with settings (1-16 + Omni)
		tmpParameter.setProperty(Ids::valueType, 1, nullptr);

		ValueTree tmpSettings(Ids::settings);
		
		for (int i = 0; i < 17; i++)
		{
			ValueTree tmpSetting(Ids::setting);

			tmpSetting.setProperty(Ids::intValue, i, nullptr);

			if (i < 16)
				tmpSetting.setProperty(Ids::name, String(i + 1), nullptr);
			else
				tmpSetting.setProperty(Ids::name, "Omni", nullptr);

			tmpSettings.addChild(tmpSetting, -1, nullptr);
		}

		tmpParameter.addChild(tmpSettings, -1, nullptr);
	}

    addParameter(tmpParameter, true);
}

void BCMParameterController::addParameter(ValueTree parameterDefinition, bool fixedParameter)
{
	ScopeOSCParamID scopeOSCParamID(int(parameterDefinition.getProperty(Ids::scopeParamGroup, -1)), 
									int(parameterDefinition.getProperty(Ids::scopeParamId, -1)));

	String name               = parameterDefinition.getProperty(Ids::name);
	
	BCMParameter* parameter = new BCMParameter(parameterDefinition, *this, scopeOSCParamID);

    if (fixedParameter)
	{
		DBG("BCMParameterController::addParameter - added Fixed Parameter: " + name);
    	fixedParameters.add(parameter);
	}
    else
	{
		DBG("BCMParameterController::addParameter - added Dynamic Parameter: " + name);
        dynamicParameters.add(parameter);
	}

    parameters.add(parameter);
	parametersByName.set(name, parameter);
}

void BCMParameterController::setupHostParameters()
{
#ifndef __DLL_EFFECT__
	int i = 0;
    
	const OwnedArray<AudioProcessorParameter>& pluginParameters(scopeSync->getPluginProcessor()->getParameters());

	// Loop through all the dynamic parameters and attach them to a host/plugin parameter until we run out of host parameters
    for (auto dynamicParameter : dynamicParameters)
    {
		HostParameter* hostParameter;
		int numPluginParameters = pluginParameters.size();

		// Try to bind to existing parameters first
		if (i < 512)
		{
			DBG("BCMParameterController::setupHostParameters: numPluginParameters = " + String(numPluginParameters) + ", binding to existing param: " + String(i));
			hostParameter = static_cast<HostParameter*>(pluginParameters[i]);
			hostParameter->setBCMParameter(dynamicParameter);
			dynamicParameter->setHostParameter(hostParameter);

			++i;
		}
		else
		{
			break;
		}
    }
#endif // __DLL_EFFECT__
}

void BCMParameterController::reset()
{
	DBG("BCMParameterController::reset - clearing parameters array");
    parameters.clear();
	parametersByName.clear();
    dynamicParameters.clear();

#ifdef __DLL_EFFECT__
	for (auto fixedParameter : fixedParameters)
	{
		DBG("BCMParameterController::reset - Added parameter: " + fixedParameter->getName());
		parameters.add(fixedParameter);
		parametersByName.set(fixedParameter->getName(), fixedParameter);
	}
#endif // __DLL_EFFECT__
}

void BCMParameterController::snapshot()
{
#ifdef __DLL_EFFECT__
	sendAllCurrentValues();
	scopeSync->snapshotFX();
#else
	// To perform a snapshot from the Plugin, we need to force the update
	// by sending all values as minimum, then maximum, then current, with a 
	// small delay between each step. This gets around some weird behaviour in
	// Scope
	currentStage = sendMin;
	startTimer(100);
#endif // __DLL_EFFECT__
}

void BCMParameterController::timerCallback()
{
	if (currentStage == sendMin)
	{
		sendAllMinValues();
		currentStage = sendMax;
	}
	else if (currentStage == sendMax)
	{
		sendAllMaxValues();
		currentStage = sendCurrent;
	}
	else
	{
		sendAllCurrentValues();
		stopTimer();
	}
}

void BCMParameterController::sendAllCurrentValues()
{
	for (auto parameter : parameters)
		parameter->getScopeOSCParameter().sendCurrentValue();
}

void BCMParameterController::sendAllMinValues()
{
	for (auto parameter : parameters)
		parameter->getScopeOSCParameter().sendMinValue();
}

void BCMParameterController::sendAllMaxValues()
{
	for (auto parameter : parameters)
		parameter->getScopeOSCParameter().sendMaxValue();
}

BCMParameter* BCMParameterController::getParameterByName(StringRef name) const
{
	if (parametersByName.contains(name))
		return parametersByName[name];
	
	return nullptr;
}

BCMParameter* BCMParameterController::getFixedParameterByName(StringRef name) const
{
	BCMParameter* parameter = getParameterByName(name);

	if (!fixedParameters.contains(parameter))
		parameter = nullptr;

	return parameter;
}

void BCMParameterController::setParameterFromGUI(BCMParameter& parameter, float newValue)
{
    parameter.setUIValue(newValue);
}

void BCMParameterController::storeParameterValues()
{
#ifndef __DLL_EFFECT__
	Array<float> currentParameterValues;

	for (auto dynamicParameter : dynamicParameters)
        currentParameterValues.add(dynamicParameter->getHostValue());

	parameterValueStore = XmlElement("parametervalues");
	parameterValueStore.addTextElement(String(floatArrayToString(currentParameterValues, currentParameterValues.size())));
	DBG("ScopeSync::storeParameterValues - Storing XML: " + parameterValueStore.createDocument(""));

#endif // __DLL_EFFECT__
}

void BCMParameterController::storeParameterValues(XmlElement& parameterValues)
{
    parameterValueStore = XmlElement(parameterValues);
    
    //DBG("ScopeSync::storeParameterValues - Storing XML: " + parameterValueStore.createDocument(""));
}

void BCMParameterController::restoreParameterValues()
{
#ifndef __DLL_EFFECT__
	Array<float> parameterValues;

	String floatCSV = parameterValueStore.getAllSubText();
	stringToFloatArray(floatCSV, parameterValues, dynamicParameters.size());

	for (int i = 0; i < dynamicParameters.size(); i++)
		dynamicParameters[i]->setHostValue(parameterValues[i], true);

	// Make sure Scope really does have the current values by performing a snapshot
	snapshot();
    
    DBG("ScopeSync::restoreParameterValues - Restoring XML: " + parameterValueStore.createDocument(""));
#endif // __DLL_EFFECT__
}



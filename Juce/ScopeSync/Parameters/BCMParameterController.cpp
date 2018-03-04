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
	const StringArray scopeFixedParameters = StringArray::fromTokens("DUMMY,X,Y,Show,Config ID,Show Preset Window,Show Patch Window,Mono Effect,BypassEffect,Show Shell Preset Window,Voice Count,MIDI Channel,Device Type,MIDI Activity",",", "");
    
    for (int i = 1; i < scopeFixedParameters.size(); i++)
        addFixedScopeParameter(scopeFixedParameters[i], i);
    
	scopeSync->initOSCUID();
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
	// We assume that the hostParameters array is empty at this point
	jassert(hostParameters.isEmpty());

	int i = 0;
    
	const OwnedArray<AudioProcessorParameter>& pluginParameters(scopeSync->getPluginProcessor()->getParameters());

	// Loop through all the non-fixed parameters and attach them to a host/plugin parameter
    for (auto dynamicParameter : dynamicParameters)
    {
		HostParameter* hostParameter;

		// Try to bind to existing parameters first
		if (i < pluginParameters.size())
			hostParameter = static_cast<HostParameter*>(pluginParameters[i]);
		else
			scopeSync->getPluginProcessor()->addParameter(hostParameter = new HostParameter());
		
		// TODO: Do we even need this array now?
		hostParameters.add(hostParameter);

		hostParameter->setBCMParameter(dynamicParameter);
		dynamicParameter->setHostParameter(hostParameter);
		
		++i;
    }
#endif // __DLL_EFFECT__
}

void BCMParameterController::reset()
{
	DBG("BCMParameterController::reset - clearing parameters array");
    parameters.clear();
	parametersByName.clear();
#ifndef __DLL_EFFECT__
    hostParameters.clear();
#endif // __DLL_EFFECT__
    dynamicParameters.clear();

	for (auto fixedParameter : fixedParameters)
	{
		DBG("BCMParameterController::reset - Added parameter: " + fixedParameter->getName());
		parameters.add(fixedParameter);
		parametersByName.set(fixedParameter->getName(), fixedParameter);
	}
}

void BCMParameterController::snapshot() const
{
	for (auto parameter : parameters)
		parameter->getScopeOSCParameter().sendCurrentValue();
}

BCMParameter* BCMParameterController::getParameterByName(StringRef name) const
{
	if (parametersByName.contains(name))
		return parametersByName[name];
	
	return nullptr;
}

void BCMParameterController::setParameterFromGUI(BCMParameter& parameter, float newValue)
{
    parameter.setUIValue(newValue);
}

void BCMParameterController::storeParameterValues()
{
#ifndef __DLL_EFFECT__
	Array<float> currentParameterValues;

	for (auto hostParameter : hostParameters)
        currentParameterValues.add(hostParameter->getValue());

	parameterValueStore = XmlElement("parametervalues");
	parameterValueStore.addTextElement(String(floatArrayToString(currentParameterValues, currentParameterValues.size())));
	//DBG("ScopeSync::storeParameterValues - Storing XML: " + parameterValueStore.createDocument(""));

#endif // __DLL_EFFECT__
}

void BCMParameterController::storeParameterValues(XmlElement& parameterValues)
{
    parameterValueStore = XmlElement(parameterValues);
    
    //DBG("ScopeSync::storeParameterValues - Storing XML: " + parameterValueStore.createDocument(""));
}

void BCMParameterController::restoreParameterValues() const
{
#ifndef __DLL_EFFECT__
	Array<float> parameterValues;

	String floatCSV = parameterValueStore.getAllSubText();
	stringToFloatArray(floatCSV, parameterValues, hostParameters.size());

	for (int i = 0; i < hostParameters.size(); i++)
		hostParameters[i]->setValue(parameterValues[i]);
    
    //DBG("ScopeSync::restoreParameterValues - Restoring XML: " + parameterValueStore.createDocument(""));
#endif // __DLL_EFFECT__
} 


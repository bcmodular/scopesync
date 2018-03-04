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

const int BCMParameterController::hostParameterCount = 128;

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
	int hostIdx = 0;
    
    for (auto parameter : dynamicParameters)
    {
        parameter->setHostIdx(hostIdx);
        hostParameters.add(parameter);

        hostIdx++;
    }
}

void BCMParameterController::reset()
{
	DBG("BCMParameterController::reset - clearing parameters array");
    parameters.clear();
	parametersByName.clear();
    hostParameters.clear();
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

int BCMParameterController::getNumParametersForHost()
{
    return hostParameterCount;
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

void BCMParameterController::endAllParameterChangeGestures()
{
    for (int i = 0; i < hostParameters.size(); i++)
    {
        BCMParameter* parameter = hostParameters[i];
#ifndef __DLL_EFFECT__
        int hostIdx = parameter->getHostIdx();

        if (changingParams[hostIdx])
        {
            scopeSync->getPluginProcessor()->endParameterChangeGesture(hostIdx); 
            changingParams.clearBit(hostIdx);
        }
#else
        parameter->getScopeOSCParameter().startListening();
#endif // __DLL_EFFECT__
    }
}

void BCMParameterController::updateHost(int hostIdx, float newValue) const
{
#ifndef __DLL_EFFECT__
	scopeSync->getPluginProcessor()->updateListeners(hostIdx, newValue);
#else
	(void)hostIdx;
	(void)newValue;
#endif
}

void BCMParameterController::beginParameterChangeGesture(int hostIdx)
{
#ifndef __DLL_EFFECT__
    if (hostIdx >= 0 && !changingParams[hostIdx])
    {
        scopeSync->getPluginProcessor()->beginParameterChangeGesture(hostIdx);
        changingParams.setBit(hostIdx);
    }
#else
    (void)hostIdx;
#endif
}

void BCMParameterController::endParameterChangeGesture(int hostIdx)
{
#ifndef __DLL_EFFECT__
    if (hostIdx >= 0 && changingParams[hostIdx])
    {
        scopeSync->getPluginProcessor()->endParameterChangeGesture(hostIdx); 
        changingParams.clearBit(hostIdx);
    }
#else
    (void)hostIdx;
#endif
}

void BCMParameterController::storeParameterValues()
{
    int numHostParameters = hostParameters.size();
    
    if (numHostParameters > 0)
    {
        Array<float> currentParameterValues;
    
        for (int i = 0; i < numHostParameters; i++)
            currentParameterValues.set(i, getParameterHostValue(i));

        parameterValueStore = XmlElement("parametervalues");
        parameterValueStore.addTextElement(String(floatArrayToString(currentParameterValues, numHostParameters)));

        //DBG("ScopeSync::storeParameterValues - Storing XML: " + parameterValueStore.createDocument(""));
    }
    else
    {
        //DBG("ScopeSync::storeParameterValues - leaving storage alone, as we don't have any host parameters");
    }
}

void BCMParameterController::storeParameterValues(XmlElement& parameterValues)
{
    parameterValueStore = XmlElement(parameterValues);
    
    //DBG("ScopeSync::storeParameterValues - Storing XML: " + parameterValueStore.createDocument(""));
}

void BCMParameterController::restoreParameterValues() const
{
    Array<float> parameterValues;
    int numHostParameters = getNumParametersForHost();

    String floatCSV = parameterValueStore.getAllSubText();
    int numParametersToRead = jmin(numHostParameters, stringToFloatArray(floatCSV, parameterValues, numHostParameters));

    for (int i = 0; i < numParametersToRead; i++)
    {
        setParameterFromHost(i, parameterValues[i]);
    }

    //DBG("ScopeSync::restoreParameterValues - Restoring XML: " + parameterValueStore.createDocument(""));
} 


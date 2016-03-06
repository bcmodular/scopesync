/*
  ==============================================================================

    BCMParameterController.cpp
    Created: 29 Nov 2015 8:33:20pm
    Author:  giles

  ==============================================================================
*/

#include "BCMParameterController.h"
#include "ScopeSyncApplication.h"
#include "../Utils/BCMXml.h"

#ifndef __DLL_EFFECT__
    #include "../Plugin/PluginProcessor.h"
#endif // __DLL_EFFECT__

const int BCMParameterController::minHostParameters = 128;
const int BCMParameterController::maxHostParameters = 128;
const int BCMParameterController::timerFrequency    = 100;

BCMParameterController::BCMParameterController(ScopeSync* owner) :
scopeSync(owner), parameterValueStore("parametervalues")
{
	shouldReceiveAsyncUpdates = false;

    resetScopeCodeIndexes();

    const StringArray scopeCodes = StringArray::fromTokens("osc,X,Y,show,cfg,spr,spa,mono,byp,sspr,vc,midc,type,mida",",", "");
    
    for (int i = 0; i < scopeCodes.size(); i++)
        addFixedScopeParameter(scopeCodes[i]);
    
	initOSCUID();

    startTimer(timerFrequency);
}

void BCMParameterController::addFixedScopeParameter(const String& scopeCode)
{
    ValueTree tmpParameter = Configuration::getDefaultFixedParameter();
    tmpParameter.setProperty(Ids::name, scopeCode, nullptr);
    tmpParameter.setProperty(Ids::shortDescription, scopeCode, nullptr);
    tmpParameter.setProperty(Ids::fullDescription, scopeCode, nullptr);
    tmpParameter.setProperty(Ids::scopeCode, scopeCode, nullptr);

    addParameter(tmpParameter, true, (scopeCode != "osc"));
}

BCMParameterController::~BCMParameterController()
{
    stopTimer();
}

void BCMParameterController::addParameter(ValueTree parameterDefinition, bool fixedParameter, bool oscAble)
{
	BCMParameter* parameter;

	BCMParameter::ParameterType type = ScopeSync::getScopeCodeType(parameterDefinition.getProperty(Ids::scopeCode).toString());

#ifdef __DLL_EFFECT__
    parameter = new BCMParameter(parameterDefinition, type, *this, scopeSync->getScopeSyncAsync(), oscAble);
#else									   
	parameter = new BCMParameter(parameterDefinition, type, *this, oscAble);
#endif __DLL_EFFECT__

    if (fixedParameter)
        fixedParameters.add(parameter);
    else
        dynamicParameters.add(parameter);

    parameters.add(parameter);

    addToParametersByScopeCodeId(parameter, parameter->getScopeCodeId());
}

void BCMParameterController::setupHostParameters()
{
	int hostIdx = 0;
    int paramCounter = 0;

    BCMParameter* parameter;

    // Add the dynamic parameters
    while (paramCounter < dynamicParameters.size())
    {
        parameter = dynamicParameters[paramCounter];
		
		parameter->setHostIdx(hostIdx);
		parameter->registerOSCListener();

        hostParameters.add(parameter);

        paramCounter++;
        hostIdx++;
    }

	paramCounter = 0;

	// Add the fixed parameters (excluding a few that don't make much sense)
    while (paramCounter < fixedParameters.size())
    {
        parameter = fixedParameters[paramCounter];
		
		String scopeCode = parameter->getScopeCode();
		StringArray scopeCodes = StringArray::fromTokens("X,Y,show,cfg,osc,mida",",","");

		if (scopeCodes.indexOf(scopeCode) == -1)
		{
			parameter->setHostIdx(hostIdx);
			parameter->registerOSCListener();

	        hostParameters.add(parameter);
			hostIdx++;
		}

        paramCounter++;
    }
}

void BCMParameterController::addToParametersByScopeCodeId(BCMParameter* parameter, int scopeCodeId)
{
    //DBG("BCMParameterController::addToParametersByScopeCodeId - Added parameter: " + parameter->getName() + ", ScopeCodeId: " + String(scopeCodeId));
        
    if (scopeCodeId > -1 && scopeCodeId < ScopeSync::getScopeCodes().size())
        parametersByScopeCodeId.set(scopeCodeId, parameter);
}

void BCMParameterController::reset()
{
	//DBG("BCMParameterController::reset - clearing parameters array");
    parameters.clear();

    for (int i = 0; i < fixedParameters.size(); i++)
	{
		//DBG("BCMParameterController::reset - Added parameter: " + fixedParameters[i]->getName());
		parameters.add(fixedParameters[i]);
	}

    hostParameters.clear();
    dynamicParameters.clear();
    resetScopeCodeIndexes();
}

void BCMParameterController::resetScopeCodeIndexes()
{
	//DBG("BCMParameterController::resetScopeCodeIndexes - clearing parametersByScopeCodeId array");
    
	parametersByScopeCodeId.clear();

    for (int i = 0; i < fixedParameters.size(); i++)
	{
		//DBG("BCMParameterController::resetScopeCodeIndexes - adding fixed parameter back in: " + fixedParameters[i]->getName() + "(" + String(fixedParameters[i]->getScopeCodeId()) + ")");
        addToParametersByScopeCodeId(fixedParameters[i], fixedParameters[i]->getScopeCodeId());
	}
}

void BCMParameterController::snapshot()
{
	for (int i = 0; i < parameters.size(); i++)
		parameters[i]->sendOSCParameterUpdate();
}

int BCMParameterController::getNumParametersForHost()
{
    return minHostParameters;
}

BCMParameter* BCMParameterController::getParameterByName(const String& name)
{
	for (int i = 0; i < parameters.size(); i++)
    {
        if (parameters[i]->getName().equalsIgnoreCase(name))
			return parameters[i];
    }

    return nullptr;
}

BCMParameter* BCMParameterController::getParameterByScopeCode(const String& scopeCode)
{
    BCMParameter* parameter = parametersByScopeCodeId[ScopeSync::getScopeCodeId(scopeCode)];

	//DBG("BCMParameterController::getParameterByScopeCode: " + parameter->getName());

    return parameter;
}

float BCMParameterController::getParameterHostValue(int hostIdx)
{
    if (isPositiveAndBelow(hostIdx, hostParameters.size()))
        return hostParameters[hostIdx]->getHostValue();
    else
        return 0.0f;
}

void BCMParameterController::getParameterNameForHost(int hostIdx, String& parameterName)
{
    if (isPositiveAndBelow(hostIdx, hostParameters.size()))
    {
        String shortDesc;
        hostParameters[hostIdx]->getDescriptions(shortDesc, parameterName);
    }
    else
    {
        parameterName = "Dummy Param";
    }
}

void BCMParameterController::getParameterText(int hostIdx, String& parameterText)
{
    if (isPositiveAndBelow(hostIdx, hostParameters.size()))
        hostParameters[hostIdx]->getUITextValue(parameterText);
}

void BCMParameterController::setParameterFromHost(int hostIdx, float newHostValue)
{
    if (isPositiveAndBelow(hostIdx, hostParameters.size()))
        hostParameters[hostIdx]->setHostValue(newHostValue);
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
        parameter->setAffectedByUI(false);
#endif // __DLL_EFFECT__
    }
}

void BCMParameterController::initOSCUID()
{
	int initialOSCUID = 0;

	while (initialOSCUID < INT_MAX && scopeSync->oscUIDInUse(initialOSCUID, scopeSync))
		initialOSCUID++;
	
    getParameterByScopeCode("osc")->setUIValue((float)initialOSCUID, false);
}

void BCMParameterController::referToOSCUID(Value & valueToLink)
{
    BCMParameter* param = getParameterByScopeCode("osc");

    param->mapToUIValue(valueToLink);
}

int BCMParameterController::getOSCUID()
{
    BCMParameter* param = getParameterByScopeCode("osc");

    return roundDoubleToInt(param->getUIValue());
}

void BCMParameterController::updateHost(int hostIdx, float newValue)
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

void BCMParameterController::restoreParameterValues()
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

void BCMParameterController::timerCallback()
{
    if (ScopeSyncApplication::inScopeFXContext() && !scopeSync->processConfigurationChange())
        receiveUpdatesFromScopeAsync();
}

void BCMParameterController::receiveUpdatesFromScopeAsync()
{
#ifdef __DLL_EFFECT__
	// DBG("BCMParameterController::receiveUpdatesFromScopeAsync");

	if (shouldReceiveAsyncUpdates)
	{
		scopeSync->getScopeSyncAsync().getAsyncUpdates(asyncControlUpdates);

		for (HashMap<int, int, DefaultHashFunctions, CriticalSection>::Iterator i(asyncControlUpdates); i.next();)
		{
    		int scopeCodeId   = i.getKey();
			int newScopeValue = i.getValue();
			
			if (scopeCodeId < ScopeSync::getScopeCodes().size())
			{
				BCMParameter* parameter = parametersByScopeCodeId[scopeCodeId];
                
				if (parameter != nullptr)
                {
                    DBG("BCMParameterController::receiveUpdatesFromScopeAsync - Processing async update for param with ScopeCode: " + ScopeSync::getScopeCodes()[scopeCodeId] + "(" + String(scopeCodeId) + ")" + ", value: " + String(newScopeValue));
                    parameter->setScopeIntValue(newScopeValue);
                }
			}
		}
		asyncControlUpdates.clear();
	}
#endif // __DLL_EFFECT__
} 


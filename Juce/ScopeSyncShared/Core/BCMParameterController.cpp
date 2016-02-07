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
    #include "../../ScopeSyncPlugin/Source/PluginProcessor.h"
#endif // __DLL_EFFECT__

const int BCMParameterController::minHostParameters = 128;
const int BCMParameterController::maxHostParameters = 128;
const int BCMParameterController::timerFrequency = 20;

BCMParameterController::BCMParameterController(ScopeSync* owner) :
scopeSync(owner), parameterValueStore("parametervalues")
{
    shouldReceiveAsyncUpdates = false;

    initOSCUID();
    resetScopeCodeIndexes();

    // Create X Parameter
    ValueTree tmpParameter = Configuration::getDefaultParameter();
    tmpParameter.setProperty(Ids::name,             "X",           nullptr);
    tmpParameter.setProperty(Ids::shortDescription, "X",           nullptr);
    tmpParameter.setProperty(Ids::fullDescription,  "X",           nullptr);
    tmpParameter.setProperty(Ids::scopeCodeId,      144,           nullptr);
    addParameter(-1, tmpParameter, BCMParameter::regular, true);

    // Create Y Parameter
    tmpParameter = Configuration::getDefaultParameter();
    tmpParameter.setProperty(Ids::name,             "Y",           nullptr);
    tmpParameter.setProperty(Ids::shortDescription, "Y",           nullptr);
    tmpParameter.setProperty(Ids::fullDescription,  "Y",           nullptr);
    tmpParameter.setProperty(Ids::scopeCodeId,      145,           nullptr);
    addParameter(-1, tmpParameter, BCMParameter::regular, true);

    startTimer(timerFrequency);
}

BCMParameterController::~BCMParameterController()
{
    stopTimer();
}

void BCMParameterController::addParameter(int index, ValueTree parameterDefinition, BCMParameter::ParameterType parameterType, bool fixedParameter)
{
    BCMParameter* parameter;
#ifdef __DLL_EFFECT__
    parameter = new BCMParameter(index, parameterDefinition, parameterType, *this, scopeSync->getScopeSyncAsync());
#else									   
	parameter = new BCMParameter(index, parameterDefinition, parameterType, *this);
#endif __DLL_EFFECT__

    if (fixedParameter)
        fixedParameters.add(parameter);
    else
        dynamicParameters.add(parameter);

    parameters.add(parameter);
}

void BCMParameterController::setupHostParameters()
{
    int hostIdx = 0;
    int paramCounter = 0;

	BCMParameter* parameter;

	// Firstly add the dynamic parameters
    while (hostIdx < maxHostParameters && paramCounter < dynamicParameters.size())
    {
		parameter = dynamicParameters[paramCounter];
        hostParameters.add(parameter);

        addToParamIdxByScopeCodeId(parameter, hostIdx);
        
        paramCounter++;
        hostIdx++;
    }

    // If there is room add the fixed parameters
    paramCounter = 0;
    
    while (hostIdx < maxHostParameters && paramCounter < fixedParameters.size())
    {
        parameter = fixedParameters[paramCounter];
        hostParameters.add(parameter);

        addToParamIdxByScopeCodeId(parameter, hostIdx);
        
        paramCounter++;
        hostIdx++;
    }
}

void BCMParameterController::addToParamIdxByScopeCodeId(BCMParameter* parameter, int index)
{
    int scopeCodeId = parameter->getScopeCodeId();
    DBG("BCMParameterController::addParameter - Added parameter: " + parameter->getName() + ", ScopeCode: " + String(scopeCodeId));
        
    if (index > -1 && scopeCodeId > -1 && scopeCodeId < scopeSync->getScopeCodes().size())
        paramIdxByScopeCodeId.set(scopeCodeId, index);
}

void BCMParameterController::reset()
{
    hostParameters.clear();
    dynamicParameters.clear();
    resetScopeCodeIndexes();
}

void BCMParameterController::resetScopeCodeIndexes()
{
    paramIdxByScopeCodeId.clear();

    for (int i = 0; i < ScopeSyncApplication::numScopeParameters; i++)
        paramIdxByScopeCodeId.add(-1);    
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

float BCMParameterController::getParameterHostValue(int hostIdx)
{
    if (isPositiveAndBelow(hostIdx, parameters.size()))
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
    for (int i = 0; i < parameters.size(); i++)
    {
        BCMParameter* parameter = parameters[i];
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
	
	setOSCUID(initialOSCUID);
}

int BCMParameterController::getOSCUID() { return oscUID.getValue(); }

void BCMParameterController::setOSCUID(int uid) { oscUID = uid; }

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
    if (!changingParams[hostIdx])
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
    if (changingParams[hostIdx])
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
    receiveUpdatesFromScopeAsync();
}

void BCMParameterController::receiveUpdatesFromScopeAsync()
{
#ifdef __DLL_EFFECT__
	// DBG("ScopeSync::receiveUpdatesFromScopeAsync");

	if (shouldReceiveAsyncUpdates)
	{
		scopeSync->getScopeSyncAsync().getAsyncUpdates(asyncControlUpdates);

		for (HashMap<int, int, DefaultHashFunctions, CriticalSection>::Iterator i(asyncControlUpdates); i.next();)
		{
    		int scopeCode     = i.getKey();
			int paramIdx      = -1;
			int newScopeValue = i.getValue();
			
			if (scopeCode < ScopeSyncApplication::numScopeParameters)
			{
				paramIdx = paramIdxByScopeCodeId[scopeCode];
                
				if (paramIdx >= 0)
                {
                    DBG("ScopeSync::receiveUpdatesFromScopeAsync - Processing async update for param " + String(paramIdx) + ", value: " + String(newScopeValue));
				    hostParameters[paramIdx]->setScopeIntValue(newScopeValue);
                }
			}
		}
		asyncControlUpdates.clear();
	}
#endif // __DLL_EFFECT__
} 



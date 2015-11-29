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

BCMParameterController::BCMParameterController(ScopeSync* owner) :
scopeSync(owner), parameterValueStore("parametervalues")
{
    shouldReceiveAsyncUpdates = false;

    initOSCUID();
    resetScopeCodeIndexes();
}

void BCMParameterController::addParameter(int index, ValueTree parameterDefinition, BCMParameter::ParameterType parameterType)
{
#ifdef __DLL_EFFECT__
	parameters.add(new BCMParameter(index, parameterDefinition, parameterType, *this, scopeSync->getScopeSyncAsync()));
#else									   
	parameters.add(new BCMParameter(index, parameterDefinition, parameterType, *this));
#endif __DLL_EFFECT__

    int scopeCodeId = parameters[index]->getScopeCodeId();
    DBG("BCMParameterController::addParameter - Added parameter: " + parameters[index]->getName() + ", ScopeCode: " + String(scopeCodeId));
        
    if (scopeCodeId > -1 && scopeCodeId < scopeSync->getScopeCodes().size())
        paramIdxByScopeCodeId.set(scopeCodeId, index);
}

void BCMParameterController::reset()
{
    parameters.clear();
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
        return parameters[hostIdx]->getHostValue();
    else
        return 0.0f;
}

void BCMParameterController::getParameterNameForHost(int hostIdx, String& parameterName)
{
    if (isPositiveAndBelow(hostIdx, parameters.size()))
    {
        String shortDesc;
        parameters[hostIdx]->getDescriptions(shortDesc, parameterName);
    }
    else
    {
        parameterName = "Dummy Param";
    }
}

void BCMParameterController::getParameterText(int hostIdx, String& parameterText)
{
    if (isPositiveAndBelow(hostIdx, parameters.size()))
        parameters[hostIdx]->getUITextValue(parameterText);
}

void BCMParameterController::setParameterFromHost(int hostIdx, float newHostValue)
{
    if (isPositiveAndBelow(hostIdx, parameters.size()))
        parameters[hostIdx]->setHostValue(newHostValue);
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
    int numHostParameters = parameters.size();
    
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
			
			BCMParameter* parameter = nullptr;

			if (scopeCode < ScopeSyncApplication::numScopeParameters)
			{
				paramIdx = paramIdxByScopeCodeId[scopeCode];
                
				if (paramIdx >= 0)
					parameter = parameters[paramIdx];
			}
			
			if (parameter != nullptr)
			{
				DBG("ScopeSync::receiveUpdatesFromScopeAsync - Processing async update for param " + String(paramIdx) + ", value: " + String(newScopeValue));
				parameter->setScopeIntValue(newScopeValue);
			}
			else
			{
				// DBG("ScopeSync::receiveUpdatesFromScopeAsync - Failed to process async update for scopeCode " + String(scopeCode) + ", value: " + String(newScopeValue) + " - No parameter found");
			}
		}
		asyncControlUpdates.clear();
	}
#endif // __DLL_EFFECT__
} 

void BCMParameterController::handleScopeSyncAsyncUpdate(int* asyncValues)
{
#ifdef __DLL_EFFECT__
	bool perfMode = ScopeSync::getPerformanceModeGlobalDisable() ? false : (scopeSync->getPerformanceMode() > 0);
	
	scopeSync->getScopeSyncAsync().handleUpdate(asyncValues, initialiseScopeParameters, perfMode);
    initialiseScopeParameters = false;
#else
    (void)asyncValues;
#endif // __DLL_EFFECT__
}




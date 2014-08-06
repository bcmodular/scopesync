/**
 * The Core of BCModular's ScopeSync system. Handles configuration,
 * parameter management and pulling together the various aspects
 * of communication between different parts of the system.
 *
 *  (C) Copyright 2014 bcmodular (http://www.bcmodular.co.uk/)
 *
 * This file is part of ScopeSync.
 *
 * ScopeSync is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#include "ScopeSync.h"

#include "../Utils/BCMMath.h"
#include "../Utils/BCMXml.h"
#include "../Components/ImageLoader.h"
#include <utility>

#ifndef __DLL_EFFECT__
    #include "../../ScopeSyncPlugin/Source/PluginProcessor.h"
const ScopeSync::AppContext ScopeSync::appContext = plugin;
#else
    #include "../../ScopeSyncFX/Source/ScopeFX.h"
const ScopeSync::AppContext ScopeSync::appContext = scopefx;
#endif // __DLL_EFFECT__

const int ScopeSync::minHostParameters = 128;
const int ScopeSync::MIN_SCOPE_INTEGER = INT_MIN;
const int ScopeSync::MAX_SCOPE_INTEGER = INT_MAX;

const int ScopeSync::numAsyncParameters  = 128;
const int ScopeSync::numAsyncLocalValues = 16;

const StringArray ScopeSync::scopeSyncCodes = StringArray::fromTokens(
"A1,A2,A3,A4,A5,A6,A7,A8,\
B1,B2,B3,B4,B5,B6,B7,B8,\
C1,C2,C3,C4,C5,C6,C7,C8,\
D1,D2,D3,D4,D5,D6,D7,D8,\
E1,E2,E3,E4,E5,E6,E7,E8,\
F1,F2,F3,F4,F5,F6,F7,F8,\
G1,G2,G3,G4,G5,G6,G7,G8,\
H1,H2,H3,H4,H5,H6,H7,H8,\
I1,I2,I3,I4,I5,I6,I7,I8,\
J1,J2,J3,J4,J5,J6,J7,J8,\
K1,K2,K3,K4,K5,K6,K7,K8,\
L1,L2,L3,L4,L5,L6,L7,L8,\
M1,M2,M3,M4,M5,M6,M7,M8,\
N1,N2,N3,N4,N5,N6,N7,N8,\
O1,O2,O3,O4,O5,O6,O7,O8,\
P1,P2,P3,P4,P5,P6,P7,P8",
",",""
);

const StringArray ScopeSync::scopeLocalCodes = StringArray::fromTokens(
"Y1,Y2,Y3,Y4,Y5,Y6,Y7,Y8,\
Z1,Z2,Z3,Z4,Z5,Z6,Z7,Z8",
",",""
);

const Identifier ScopeSync::paramTypesId                = "parametertypes";
const Identifier ScopeSync::paramTypeId                 = "parametertype";
const Identifier ScopeSync::paramTypeNameId             = "name";
const Identifier ScopeSync::paramTypeValueTypeId        = "valuetype";
const Identifier ScopeSync::paramTypeScopeRangeMinId    = "scoperangemin";
const Identifier ScopeSync::paramTypeScopeRangeMaxId    = "scoperangemax";
const Identifier ScopeSync::paramTypeScopeRangeMinFltId = "scoperangeminflt";
const Identifier ScopeSync::paramTypeScopeRangeMaxFltId = "scoperangemaxflt";
const Identifier ScopeSync::paramTypeUISuffixId         = "uisuffix";
const Identifier ScopeSync::paramTypeUIRangeMinId       = "uirangemin";
const Identifier ScopeSync::paramTypeUIRangeMaxId       = "uirangemax";
const Identifier ScopeSync::paramTypeUIRangeIntervalId  = "uirangeinterval";
const Identifier ScopeSync::paramTypeUIResetValueId     = "uiresetvalue";
const Identifier ScopeSync::paramTypeUISkewFactorId     = "uiskewfactor";
const Identifier ScopeSync::paramTypeUISkewFactorTypeId = "uiskewfactortype";
const Identifier ScopeSync::paramTypeSettingsId         = "settings";
const Identifier ScopeSync::paramTypeSettingId          = "setting";
const Identifier ScopeSync::paramTypeSettingNameId      = "name";
const Identifier ScopeSync::paramTypeSettingValueId     = "value";

const Identifier ScopeSync::deviceId                    = "device";
const Identifier ScopeSync::paramId                     = "parameter";
const Identifier ScopeSync::paramNameId                 = "name";
const Identifier ScopeSync::paramShortDescId            = "shortdescription";
const Identifier ScopeSync::paramFullDescId             = "fulldescription";
const Identifier ScopeSync::paramScopeSyncId            = "scopesync";
const Identifier ScopeSync::paramScopeLocalId           = "scopelocal";
const Identifier ScopeSync::paramHostValueId            = "hostvalue";
const Identifier ScopeSync::paramUIValueId              = "uivalue";
const Identifier ScopeSync::affectedByUIId              = "affectedbyui";

ScopeSync::ScopeSync() : parameterValueStore("parametervalues"), configurationXml("configuration")
{
    initialise();
}

#ifndef __DLL_EFFECT__
ScopeSync::ScopeSync(PluginProcessor* owner) : parameterValueStore("parametervalues"), configurationXml("configuration")
{
    configurationLoading = true;
    pluginProcessor = owner;
    initialise();
}
#else
ScopeSync::ScopeSync(ScopeFX* owner) : parameterValueStore("parametervalues"), configurationXml("configuration")
{
    configurationLoading = true;
    scopeFX = owner;
    initialise();
}
#endif // __DLL_EFFECT__

ScopeSync::~ScopeSync()
{
    ImageLoader::deleteInstance();
}

void ScopeSync::initialise()
{
    loadConfiguration(true, false);
    configurationFilePath.addListener(this);
}

void ScopeSync::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    (void)midiMessages;
#ifndef __DLL_EFFECT__
    scopeSyncAudio.processBlock(buffer);
#else
    (void)buffer;
#endif // __DLL_EFFECT__
}

void ScopeSync::snapshot()
{
#ifndef __DLL_EFFECT__
    scopeSyncAudio.snapshot();
#else
    scopeSyncAsync.createSnapshot();
#endif // __DLL_EFFECT__
}

void ScopeSync::beginParameterChangeGesture(int paramIdx)
{
#ifndef __DLL_EFFECT__
    if (pluginProcessor)
        pluginProcessor->beginParameterChangeGesture(paramIdx); 
#else
    setAffectedByUI(paramIdx, true);
#endif // __DLL_EFFECT__
}

void ScopeSync::endParameterChangeGesture(int paramIdx)
{
#ifndef __DLL_EFFECT__
    pluginProcessor->endParameterChangeGesture(paramIdx); 
#else
    setAffectedByUI(paramIdx, false);
#endif // __DLL_EFFECT__
}

void ScopeSync::receiveUpdatesFromScopeAudio()
{
    if (scopeSyncAudio.controlUpdates.size() > 0)
    {
        const ScopedLock cuLock(scopeSyncAudio.controlUpdateLock);
        audioControlUpdates.swapWith(scopeSyncAudio.controlUpdates);
        
        for (int i = 0; i < audioControlUpdates.size(); i++)
        {
            int   scopeSyncCode = audioControlUpdates[i].first;
            float newScopeValue = audioControlUpdates[i].second;

            int paramIdx = getParameterIdxFromScopeCode(scopeSyncCode);
            
            if (paramIdx != -1)
            {
                float oldHostValue = getParameterHostValue(paramIdx);
                float newHostValue = convertScopeFltToHostValue(paramIdx, newScopeValue);

                if (newHostValue != oldHostValue)
                {
                    DBG("ScopeSync::receiveUpdatesFromScope: " + String(scopeSyncCode) + ": newValue: " + String(newHostValue));

                    float newUIValue = convertHostToUIValue(paramIdx, newHostValue);

                    setParameterValues(paramIdx, newHostValue, newUIValue);
#ifndef __DLL_EFFECT__
                    pluginProcessor->updateListeners(paramIdx, newHostValue);
#endif // __DLL_EFFECT__
                }
            }
        }    
    }
    audioControlUpdates.clear();
}      

void ScopeSync::receiveUpdatesFromScopeAsync()
{
#ifdef __DLL_EFFECT__
    scopeSyncAsync.getAsyncUpdatesArray(asyncControlUpdates);

    int numUpdates = asyncControlUpdates.size();

    Array<int> processedParams;

    for (int i = numUpdates - 1; i >= 0 ; i--)
    {
        int scopeCode     = asyncControlUpdates[i].first;
        int newScopeValue = asyncControlUpdates[i].second;

        int paramIdx = getParameterIdxFromScopeCode(scopeCode);

        if (!(processedParams.contains(paramIdx)))
        {
            if (!(isAffectedByUI(paramIdx)))
            {
                float oldHostValue = getParameterHostValue(paramIdx);
                float newHostValue = convertScopeIntToHostValue(paramIdx, newScopeValue);

                DBG("ScopeSync::receiveUpdatesFromScopeAsync: " + String(paramIdx) + ": oldValue: " + String(oldHostValue));

                if (newHostValue != oldHostValue)
                {
                    DBG("ScopeSync::receiveUpdatesFromScopeAsync: " + String(paramIdx) + ": newValue: " + String(newHostValue));
                    float newUIValue = convertHostToUIValue(paramIdx, newHostValue);
                    setParameterValues(paramIdx, newHostValue, newUIValue);
                }
            }
            else
            {
                DBG("ScopeSync::receiveUpdatesFromScopeAsync: Parameter affected by UI since last update" + String(paramIdx));
            }

            processedParams.add(paramIdx);
        }
        else
        {
            DBG("ScopeSync::receiveUpdatesFromScopeAsync: throwing away extra update: " + String(paramIdx));
        }
    }
    asyncControlUpdates.clear();
#endif // __DLL_EFFECT__
} 

void ScopeSync::sendToScopeSyncAudio(int paramIdx, float newValue)
{
    if (paramIdx >= 0 && paramIdx < numParameters) {

        int scopeSyncId = getScopeSyncId(paramIdx);

        if (scopeSyncId != -1)
        {
            float newScopeValue  = convertHostToScopeFltValue(paramIdx, newValue);

            DBG("ScopeSync::sendToScopeSyncAudio: " + String(scopeSyncId) + ", orig value: " + String(newValue) + ", scaled value: " + String(newScopeValue));
            scopeSyncAudio.setControlValue(scopeSyncId, newScopeValue);
        }


    }
}

void ScopeSync::sendToScopeSyncAsync(int paramIdx, float newValue)
{
#ifdef __DLL_EFFECT__
    if (paramIdx >= 0 && paramIdx < numParameters) {

        // Try to grab the ScopeSyncId for the relevant parameter
        int scopeCode = getScopeSyncId(paramIdx);

        if (scopeCode == -1)
        {
            // We didn't find a ScopeSyncId, so check to see if there's
            // a ScopeLocalId instead
            scopeCode = getScopeLocalId(paramIdx);

            if (scopeCode != -1)
            {
                // We found one, so shift the value by the number of
                // ScopeSyncIds to generate a valid async ScopeCode
                scopeCode += numAsyncParameters;
            }
        }

        if (scopeCode != -1)
        {
            int newScopeValue = convertHostToScopeIntValue(paramIdx, newValue);

            DBG("ScopeSync::sendToScopeSyncAsync: " + String(scopeCode) + ", orig value: " + String(newValue) + ", scaled value: " + String(newScopeValue));
            scopeSyncAsync.setValue(scopeCode, newScopeValue);
        }
        else
        {
            DBG("ScopeSync::sendToScopeSyncAsync: couldn't find Scope code for parameter: " + String(paramIdx));
        }
    }
#else
(void)paramIdx;
(void)newValue;
#endif // __DLL_EFFECT__
}

void ScopeSync::timerCallback()
{
    if (inPluginContext())
        receiveUpdatesFromScopeAudio();
    else
        receiveUpdatesFromScopeAsync();
}

bool ScopeSync::guiNeedsReloading()
{
    const ScopedLock lock(flagLock);
    return reloadGUI;
};

void ScopeSync::setGUIReload(bool reloadGUIFlag)
{
    const ScopedLock lock(flagLock);
    reloadGUI = reloadGUIFlag;
};
    
void ScopeSync::valueChanged(Value& valueThatChanged)
{
    if (valueThatChanged.refersToSameSourceAs(configurationFilePath))
    {
        loadConfiguration(false, retainParameterState);
    }
}

int ScopeSync::getNumParametersForHost()
{
    int num;

    if (numParameters < minHostParameters)
        num = minHostParameters;
    else
        num = numParameters;

    DBG("ScopeSync::getNumHostParameters - " + String(num));
    return num;
}

int ScopeSync::getNumParameters()
{
    return numParameters;
}

float ScopeSync::getParameterHostValue(int index)
{
    if (index >= 0 && index < numParameters)
    {
        float value = deviceParameters.getChild(index).getProperty(paramHostValueId);
        return value;
    }
    else
    {
        return 0.0f;
    }
}

int ScopeSync::getParameterScopeIntValue(int index)
{
    if (index >= 0 && index < numParameters)
    {
        float  hostValue = deviceParameters.getChild(index).getProperty(paramHostValueId);
        int    value = convertHostToScopeIntValue(index, hostValue);
        //DBG("ScopeSync::getParameterScopeIntValue - index: " + String(index) + ", value: " + String(value));
        return value;
    }
    else
    {
        return 0;
    }
}

int ScopeSync::getParameterIdxByName(String& name)
{
    // DBG("ScopeSync::getParameterIdxByName: " + name + ", id: " + String(paramIdx));
    return deviceParameters.indexOf(deviceParameters.getChildWithProperty(paramNameId, name));
}

int ScopeSync::getParameterIdxFromScopeCode(int scopeCode)
{
    ValueTree parameter;

    if (scopeCode < numAsyncParameters)
        parameter = deviceParameters.getChildWithProperty(paramScopeSyncId, scopeCode);
    else
        parameter = deviceParameters.getChildWithProperty(paramScopeLocalId, (scopeCode - numAsyncParameters));

    if (parameter.isValid())
    {
        int paramIdx = deviceParameters.indexOf(parameter);
        return paramIdx;
    }
    else
        return -1;
}

void ScopeSync::getParameterNameForHost(int index, String& parameterName)
{
    if (index >= 0 && index < numParameters)
        parameterName = deviceParameters.getChild(index).getProperty(paramFullDescId);
}

void ScopeSync::setParameterFromHost(int index, float newHostValue)
{
    float newUIValue = convertHostToUIValue(index, newHostValue);

    setParameterValues(index, newHostValue, newUIValue);
#ifdef __DLL_EFFECT__
    sendToScopeSyncAsync(index, newHostValue);
#else
    sendToScopeSyncAudio(index, newHostValue);
#endif // __DLL_EFFECT__
}

void ScopeSync::setParameterFromGUI(int index, float newValue)
{
    float newHostValue = convertUIToHostValue(index, newValue);

    setParameterValues(index, newHostValue, newValue);
    
#ifdef __DLL_EFFECT__
    sendToScopeSyncAsync(index, newHostValue);
#else
    sendToScopeSyncAudio(index, newHostValue);
    pluginProcessor->updateListeners(index, newHostValue);
#endif // __DLL_EFFECT__
}

void ScopeSync::handleScopeSyncAsyncUpdate(Array<int>& asyncValues)
{
#ifdef __DLL_EFFECT__
    scopeSyncAsync.handleUpdate(asyncValues, initialiseScopeParameters);
    initialiseScopeParameters = false;
#else
    (void)asyncValues;
#endif // __DLL_EFFECT__
}

void ScopeSync::getSnapshot(Array<std::pair<int, int>>& snapshotSubset, int numElements)
{
#ifdef __DLL_EFFECT__
    scopeSyncAsync.getSnapshot(snapshotSubset, numElements);
#else
    (void)snapshotSubset;
    (void)numElements;
#endif // __DLL_EFFECT__
}

void ScopeSync::setAffectedByUI(int paramIdx, bool isAffected)
{
    ValueTree parameter = deviceParameters.getChild(paramIdx);

    if (parameter.isValid())
        parameter.setProperty(affectedByUIId, isAffected, nullptr);
}

bool ScopeSync::isAffectedByUI(int paramIdx)
{
    ValueTree parameter = deviceParameters.getChild(paramIdx);
    bool isAffected = false;

    if (parameter.isValid())
        isAffected = parameter.getProperty(affectedByUIId, false);

    return isAffected;
}

int ScopeSync::getScopeSyncId(int paramIdx)
{
    ValueTree parameter = deviceParameters.getChild(paramIdx);
    int scopeSyncId = -1;

    if (parameter.isValid())
        scopeSyncId = parameter.getProperty(paramScopeSyncId, -1);

    return scopeSyncId;
}

int ScopeSync::getScopeLocalId(int paramIdx)
{
    ValueTree parameter = deviceParameters.getChild(paramIdx);
    int scopeLocalId = -1;

    if (parameter.isValid())
        scopeLocalId = parameter.getProperty(paramScopeLocalId, -1);

    return scopeLocalId;
}

void ScopeSync::getParameterSettings(int paramIdx, ValueTree& settings)
{
    ValueTree parameter = deviceParameters.getChild(paramIdx);

    if (parameter.isValid())
    {
        ValueTree parameterType = parameter.getChildWithName(paramTypeId);

        if (parameterType.isValid())
            settings = parameterType.getChildWithName(paramTypeSettingsId);
    }
}

void ScopeSync::mapToParameterUIValue(int paramIdx, Value& valueToMapTo)
{
    ValueTree parameter = deviceParameters.getChild(paramIdx);
   
    if (parameter.isValid())
        valueToMapTo.referTo(parameter.getPropertyAsValue(paramUIValueId, nullptr));
}

void ScopeSync::getParameterDescriptions(int paramIdx, String& shortDesc, String& fullDesc)
{
    ValueTree parameter = deviceParameters.getChild(paramIdx);

    if (parameter.isValid())
    {
        shortDesc = parameter.getProperty(paramShortDescId, shortDesc).toString();
        fullDesc = parameter.getProperty(paramFullDescId, fullDesc).toString();
    }
}

void ScopeSync::getParameterUIRanges(int paramIdx, double& rangeMin, double& rangeMax, double& rangeInt, String& uiSuffix)
{
    ValueTree parameter = deviceParameters.getChild(paramIdx);

    if (parameter.isValid())
    {
        ValueTree parameterType = parameter.getChildWithName(paramTypeId);

        if (parameterType.isValid())
        {
            rangeMin = parameterType.getProperty(paramTypeUIRangeMinId, rangeMin);
            rangeMax = parameterType.getProperty(paramTypeUIRangeMaxId, rangeMax);
            rangeInt = parameterType.getProperty(paramTypeUIRangeIntervalId, rangeInt);
            uiSuffix = parameterType.getProperty(paramTypeUISuffixId, uiSuffix);
        }
    }
}

bool ScopeSync::getParameterUIResetValue(int paramIdx, double& uiResetValue)
{
    bool foundValue = false;
    ValueTree parameter = deviceParameters.getChild(paramIdx);
    
    if (parameter.isValid())
    {
        ValueTree parameterType = parameter.getChildWithName(paramTypeId);

        if (parameterType.isValid())
        {
            if (parameterType.hasProperty(paramTypeUIResetValueId))
            {
                uiResetValue = parameterType.getProperty(paramTypeUIResetValueId);
                foundValue = true;
            }
        }
    }
    return foundValue;
}

bool ScopeSync::getParameterUISkewFactor(int paramIdx, String& uiSkewFactorType, double& uiSkewFactor)
{
    bool foundValue = false;
    ValueTree parameter = deviceParameters.getChild(paramIdx);
    
    if (parameter.isValid())
    {
        ValueTree parameterType = parameter.getChildWithName(paramTypeId);

        if (parameterType.isValid())
        {
            if (parameterType.hasProperty(paramTypeUISkewFactorId))
            {
                uiSkewFactorType = parameterType.getProperty(paramTypeUISkewFactorTypeId, uiSkewFactorType);
                uiSkewFactor     = parameterType.getProperty(paramTypeUISkewFactorId);
                foundValue = true;
            }
        }
    }
    return foundValue;
}

void ScopeSync::setParameterValues(int index, float newHostValue, float newUIValue)
{
    ValueTree deviceParameter = deviceParameters.getChild(index);

    if (deviceParameter.isValid())
    {
        deviceParameter.setProperty(paramHostValueId, newHostValue, nullptr);
        deviceParameter.setProperty(paramUIValueId, newUIValue, nullptr);
    }
    //DBG("ScopeSync::setParameterValues - index: " + String(index) + ", newHostValue: " + String(newHostValue) + ", newUIValue: " + String(newUIValue));
}

float ScopeSync::convertUIToHostValue(int paramIdx, float value)
{
    ValueTree parameter = deviceParameters.getChild(paramIdx);
    ValueTree paramType = parameter.getChildWithName(paramTypeId);

    float  minUIValue = paramType.getProperty(paramTypeUIRangeMinId);
    float  maxUIValue = paramType.getProperty(paramTypeUIRangeMaxId);

    float  scaledValue = (float)scaleDouble(minUIValue, maxUIValue, 0.0f, 1.0f, value);

    return scaledValue;
}

float ScopeSync::convertHostToUIValue(int paramIdx, float value)
{
    ValueTree parameter = deviceParameters.getChild(paramIdx);
    ValueTree paramType = parameter.getChildWithName(paramTypeId);

    float  minUIValue = paramType.getProperty(paramTypeUIRangeMinId);
    float  maxUIValue = paramType.getProperty(paramTypeUIRangeMaxId);

    float  scaledValue = (float)scaleDouble(0.0f, 1.0f, minUIValue, maxUIValue, value);

    return scaledValue;
}

float ScopeSync::convertScopeFltToHostValue(int paramIdx, float value)
{
    float scaledValue   = 0.0f;

    ValueTree parameter = deviceParameters.getChild(paramIdx);
    ValueTree paramType = parameter.getChildWithName(paramTypeId);

    int parameterValueType = paramType.getProperty(paramTypeValueTypeId);

    if (parameterValueType == discrete)
    {
        ValueTree paramSettings = paramType.getChildWithName(paramTypeSettingsId);
        int nearestItem = findNearestParameterSetting(paramSettings, value);

        scaledValue = convertUIToHostValue(paramIdx, (float)nearestItem);
    }
    else
    {
        float  minScopeValue = paramType.getProperty(paramTypeScopeRangeMinFltId);
        float  maxScopeValue = paramType.getProperty(paramTypeScopeRangeMaxFltId);

        scaledValue = (float)scaleDouble(minScopeValue, maxScopeValue, 0.0f, 1.0f, value);
    }

    return scaledValue;
}

float ScopeSync::convertScopeIntToHostValue(int paramIdx, int value)
{
    float scaledValue   = 0.0f;

    ValueTree parameter = deviceParameters.getChild(paramIdx);
    ValueTree paramType = parameter.getChildWithName(paramTypeId);

    int minScopeValue = paramType.getProperty(paramTypeScopeRangeMinId);
    int maxScopeValue = paramType.getProperty(paramTypeScopeRangeMaxId);

    scaledValue = (float)scaleDouble(minScopeValue, maxScopeValue, 0.0f, 1.0f, value);

    int parameterValueType = paramType.getProperty(paramTypeValueTypeId);

    if (parameterValueType == discrete)
    {
        ValueTree paramSettings = paramType.getChildWithName(paramTypeSettingsId);
        int nearestItem = findNearestParameterSetting(paramSettings, scaledValue);

        scaledValue = convertUIToHostValue(paramIdx, (float)nearestItem);
    }

    return scaledValue;
}

float ScopeSync::convertHostToScopeFltValue(int paramIdx, float value)
{
    float scaledValue   = 0.0f;

    ValueTree parameter = deviceParameters.getChild(paramIdx);
    ValueTree paramType = parameter.getChildWithName(paramTypeId);

    int parameterValueType = paramType.getProperty(paramTypeValueTypeId);
    if (parameterValueType == discrete)
    {
        ValueTree paramSettings = paramType.getChildWithName(paramTypeSettingsId);
        int settingIdx = roundDoubleToInt(convertHostToUIValue(paramIdx, value));

        scaledValue = paramSettings.getChild(settingIdx).getProperty(paramTypeSettingValueId);
    }
    else
    {
        float  minScopeValue = paramType.getProperty(paramTypeScopeRangeMinFltId);
        float  maxScopeValue = paramType.getProperty(paramTypeScopeRangeMaxFltId);

        scaledValue = (float)scaleDouble(0.0f, 1.0f, minScopeValue, maxScopeValue, value);
    }

    return scaledValue;
}

int ScopeSync::convertHostToScopeIntValue(int paramIdx, float value)
{
    int   scaledValue = 0;

    ValueTree parameter = deviceParameters.getChild(paramIdx);
    ValueTree paramType = parameter.getChildWithName(paramTypeId);

    int parameterValueType = paramType.getProperty(paramTypeValueTypeId);
    
    if (parameterValueType == discrete)
    {
        ValueTree paramSettings = paramType.getChildWithName(paramTypeSettingsId);
        int settingIdx = roundDoubleToInt(convertHostToUIValue(paramIdx, value));

        value = paramSettings.getChild(settingIdx).getProperty(paramTypeSettingValueId);
    }
    
    int minScopeValue = paramType.getProperty(paramTypeScopeRangeMinId);
    int maxScopeValue = paramType.getProperty(paramTypeScopeRangeMaxId);
    
    scaledValue = (int)scaleDouble(0.0f, 1.0f, minScopeValue, maxScopeValue, value);
    
    return scaledValue;
}

int ScopeSync::findNearestParameterSetting(const ValueTree& settings, float value)
{
    int   nearestItem = 0;
    float smallestGap = 1.0f;

    for (int i = 0; i < settings.getNumChildren(); i++)
    {
        float settingValue = settings.getChild(i).getProperty(paramTypeSettingValueId);
        float gap = abs(value - settingValue);

        if (gap < FLT_EPSILON)
        {
            DBG("ScopeSync::findNearestParameterSetting - Found 'exact' match for setting: " + String(settings.getChild(i).getProperty(paramTypeSettingNameId)));

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

Value& ScopeSync::getConfigurationName()
{
    return configurationName;
};

Value& ScopeSync::getConfigurationFilePath()
{
    return configurationFilePath;
};

void ScopeSync::setConfigurationFilePath(const String& newFilePath, bool retainState)
{ 
    configurationFilePath = newFilePath;
    retainParameterState  = retainState;
};

PropertiesFile& ScopeSync::getAppProperties()
{
    PropertiesFile::Options options;
    options.applicationName     = ProjectInfo::projectName;
    options.folderName          = ProjectInfo::projectName;
    options.filenameSuffix      = "settings";
    options.osxLibrarySubFolder = "Application Support";
    appProperties.setStorageParameters(options);

    PropertiesFile* properties = appProperties.getUserSettings();

    return *properties;
}
    
bool ScopeSync::configurationIsLoading()
{
    const ScopedLock lock(flagLock);
    return configurationLoading;
}

void ScopeSync::setConfigurationLoading(bool configurationLoadingFlag)
{
    const ScopedLock lock(flagLock);
    configurationLoading = configurationLoadingFlag;
};

XmlElement* ScopeSync::getSystemLookAndFeels()
{
    XmlDocument lookAndFeelsDocument(systemLookAndFeels);
    XmlElement* lookAndFeelsElement = lookAndFeelsDocument.getDocumentElement();
    return lookAndFeelsElement;
}

bool ScopeSync::loadConfiguration(bool loadLoader, bool retainState)
{
    setConfigurationLoading(true);
    numParameters = 0;
        
    if (!loadSystemParameterTypes())
    {
        setConfigurationLoading(false);
        return false;
    }
    
    ScopedPointer<XmlElement> configElement;

    if (loadLoader)
    {
        XmlDocument configurationDocument(loaderConfiguration);
        configElement = configurationDocument.getDocumentElement();
    }
    else
    {
        File configurationFile(configurationFilePath.getValue());

        DBG("ScopeSync::loadConfiguration - Trying to load: " + configurationFile.getFullPathName());

        if (!(configurationFile.existsAsFile()))
        {
            AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Error", "Configuration file not found");
            setConfigurationLoading(false);
            return false;
        }
    
        XmlDocument configurationDocument(configurationFile);
        configElement = configurationDocument.getDocumentElement();
        
        if (configElement == nullptr)
        {
            AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Error", "Configuration XML Parsing error: " + configurationDocument.getLastParseError());
            setConfigurationLoading(false);
            return false;
        }
    }
    
    configurationXml  = XmlElement(*configElement);
    configurationName = configurationXml.getStringAttribute("name", "ScopeSync");

    XmlElement* paramTypesXml = configurationXml.getChildByName("paramtypes");
    if (paramTypesXml)
        overrideParameterTypes(*paramTypesXml);
    
    XmlElement* deviceXml = configurationXml.getChildByName("device");
    if (deviceXml)
        loadDeviceParameters(*deviceXml);
    
    XmlElement* mappingXml = configurationXml.getChildByName("mapping");
    if (mappingXml && mappingXml->hasAttribute("filename"))
        loadMappingFile(*mappingXml);
    
    XmlElement* layoutXml = configurationXml.getChildByName("layout");
    if (layoutXml && layoutXml->hasAttribute("filename"))
        loadLayoutFile(*layoutXml);
    
#ifndef __DLL_EFFECT__
    pluginProcessor->updateHostDisplay();
#endif // __DLL_EFFECT__

    if (retainState && inPluginContext())
        restoreParameterValues();
    else
        initialiseScopeParameters = true;

    setGUIReload(true);
    setConfigurationLoading(false);
    return true;
}

void ScopeSync::addBCMLookAndFeel(BCMLookAndFeel* bcmLookAndFeel)
{
    bcmLookAndFeels.add(bcmLookAndFeel);
}

BCMLookAndFeel* ScopeSync::getBCMLookAndFeelById(String id)
{
    //DBG("ScopeSync::getBCMLookAndFeelById: Looking for LookAndFeel by id: id = " + id);

    // Set LookAndFeel to system:default, in case we don't find one that matches, or a layout default
    BCMLookAndFeel* bcmLookAndFeel = bcmLookAndFeels[0];
    
    bool foundLnF = false;

    // Try to find the LookAndFeel with a matching id
    for (int i = 1; i < bcmLookAndFeels.size(); i++)
    {
        String foundId = bcmLookAndFeels[i]->getId();
        
        if (foundId.equalsIgnoreCase(id))
        {
            bcmLookAndFeel = bcmLookAndFeels[i];
            foundLnF       = true;
            break;
        }
    }

    if (!foundLnF)
    {
        // We didn't find a matching one, so try to find the layout default instead
        for (int i = 1; i < bcmLookAndFeels.size(); i++)
        {
            String foundId = bcmLookAndFeels[i]->getId();
        
            if (foundId.equalsIgnoreCase("default"))
            {
                bcmLookAndFeel = bcmLookAndFeels[i];
                break;
            }
        }
    }

    return bcmLookAndFeel;
}

void ScopeSync::clearBCMLookAndFeels()
{
    DBG("ScopeSync::clearBCMLookAndFeels: array currently contains " + String(bcmLookAndFeels.size()) + " entries");
    bcmLookAndFeels.clear();
    DBG("ScopeSync::clearBCMLookAndFeels: array now contains " + String(bcmLookAndFeels.size()) + " entries");
}

int ScopeSync::getNumBCMLookAndFeels()
{
    return bcmLookAndFeels.size();
}

void ScopeSync::storeParameterValues()
{
    Array<float> currentParameterValues;

    for (int i = 0; i < numParameters; i++)
        currentParameterValues.set(i, getParameterHostValue(i));

    parameterValueStore = XmlElement("parametervalues");
    parameterValueStore.addTextElement(String(floatArrayToString(currentParameterValues, numParameters)));

    DBG("ScopeSync::storeParameterValues - Storing XML: " + parameterValueStore.createDocument(""));
}

void ScopeSync::storeParameterValues(XmlElement& parameterValues)
{
    parameterValueStore = XmlElement(parameterValues);
    
    DBG("ScopeSync::storeParameterValues - Storing XML: " + parameterValueStore.createDocument(""));
}

void ScopeSync::restoreParameterValues()
{
    Array<float> parameterValues;

    String floatCSV = parameterValueStore.getAllSubText();
    int numParametersToRead = jmin(numParameters, stringToFloatArray(floatCSV, parameterValues, numParameters));

    for (int i = 0; i < numParametersToRead; i++)
    {
        setParameterFromHost(i, parameterValues[i]);
    }

    DBG("ScopeSync::restoreParameterValues - Restoring XML: " + parameterValueStore.createDocument(""));
}

bool ScopeSync::loadSystemParameterTypes()
{
    // Create the default parameter type
    ValueTree defaultParameterType(paramTypeId);
    defaultParameterType.setProperty(paramTypeNameId,             "default",         nullptr);
    defaultParameterType.setProperty(paramTypeUISuffixId,         "",                nullptr);
    defaultParameterType.setProperty(paramTypeScopeRangeMinId,    0,                 nullptr);
    defaultParameterType.setProperty(paramTypeScopeRangeMaxId,    MAX_SCOPE_INTEGER, nullptr);
    defaultParameterType.setProperty(paramTypeScopeRangeMinFltId, 0.0f,              nullptr);
    defaultParameterType.setProperty(paramTypeScopeRangeMaxFltId, 1.0f,              nullptr);
    defaultParameterType.setProperty(paramTypeUIRangeMinId,       0,                 nullptr);
    defaultParameterType.setProperty(paramTypeUIRangeMaxId,       100,               nullptr);
    defaultParameterType.setProperty(paramTypeUIRangeIntervalId,  0.0001,            nullptr);
    defaultParameterType.setProperty(paramTypeValueTypeId,        continuous,        nullptr);

    XmlDocument               parameterTypesDocument(systemParameterTypes);
    ScopedPointer<XmlElement> parameterTypesXml = parameterTypesDocument.getDocumentElement();
    
    // Initialise the parameterTypes Tree and add the default entry
    parameterTypes = ValueTree(paramTypesId);
    parameterTypes.addChild(defaultParameterType, 0, nullptr);

    if (parameterTypesXml != nullptr)
    {
        forEachXmlChildElementWithTagName(*parameterTypesXml, child, "parametertype")
        {
            ValueTree parameterType = defaultParameterType.createCopy();

            getParameterTypeFromXML(*child, parameterType);

            DBG("ScopeSync::loadSystemParameterTypes - Adding Parameter Type:" + newLine + parameterType.toXmlString());
            parameterTypes.addChild(parameterType, -1, nullptr);
        }
    }
    else
    {
        AlertWindow::showMessageBox(AlertWindow::WarningIcon,"Error","Problem reading System Parameter Types: " + parameterTypesDocument.getLastParseError());
        return false;
    }

    return true;
}

bool ScopeSync::overrideParameterTypes(XmlElement& parameterTypesXml)
{
    if (parameterTypesXml.hasAttribute("filename"))
    {
        File configurationFile(configurationFilePath.getValue());
        File configurationFileDir = configurationFile.getParentDirectory();

        File parameterTypesFile = configurationFileDir.getChildFile(parameterTypesXml.getStringAttribute("filename"));
        
        DBG("ScopeSync::overrideParameterTypes - Trying to load: " + parameterTypesFile.getFullPathName());

        XmlDocument               parameterTypesDocument(parameterTypesFile);
        ScopedPointer<XmlElement> loadedParameterTypesXML = parameterTypesDocument.getDocumentElement();

        if (loadedParameterTypesXML != nullptr)
        {
            parameterTypesXml = *loadedParameterTypesXML;
        }
        else
        {
            AlertWindow::showMessageBox(AlertWindow::WarningIcon,"Error","Failed to load Configuration's Parameter Types. Parsing error: " + parameterTypesDocument.getLastParseError());
            return false;
        }
    }

    forEachXmlChildElementWithTagName(parameterTypesXml, child, "parametertype")
    {
        // Initialise a parameter type value tree, in case this is a new one
        ValueTree parameterType = parameterTypes.getChild(0).createCopy();
        bool      newParameterType = true;
        
        String parameterTypeName = child->getStringAttribute(paramTypeNameId);

        for (int i = 0; i < parameterTypes.getNumChildren(); i++)
        {
            if (parameterTypes.getChild(i).getProperty(paramTypeNameId) == parameterTypeName)
            {
                parameterType    = parameterTypes.getChild(i);
                newParameterType = false;
                break;
            }
        }
        
        getParameterTypeFromXML(*child, parameterType);

        if (newParameterType) parameterTypes.addChild(parameterType, -1, nullptr);
    }
    return true;
}

void ScopeSync::getParameterTypeFromXML(XmlElement& xml, ValueTree& parameterType)
{
    forEachXmlChildElement(xml, child)
    {
        Identifier xmlId = child->getTagName();

        if (xmlId == paramTypeNameId || xmlId == paramTypeUISuffixId)
        {
            parameterType.setProperty(xmlId, child->getAllSubText(), nullptr);
        }
        else if (xmlId == paramTypeScopeRangeMinId || xmlId == paramTypeScopeRangeMaxId)
        {
            int intValue = 0;

            if (child->getAllSubText().toLowerCase() == "min_int")
                intValue = MIN_SCOPE_INTEGER;
            else if (child->getAllSubText().toLowerCase() == "max_int")
                intValue = MAX_SCOPE_INTEGER;
            else
                intValue = child->getAllSubText().getIntValue();

            parameterType.setProperty(xmlId, intValue, nullptr);

            float fltValue = 0.0f;

            if (intValue >= 0)
                fltValue = (float)intValue / (float)MAX_SCOPE_INTEGER;
            else
                fltValue = -((float)intValue / (float)MIN_SCOPE_INTEGER);  
            
            if (xmlId == paramTypeScopeRangeMinId)
                parameterType.setProperty(paramTypeScopeRangeMinFltId, fltValue, nullptr);
            else
                parameterType.setProperty(paramTypeScopeRangeMaxFltId, fltValue, nullptr);
        }
        else if
            (
                xmlId == paramTypeUIRangeMinId
             || xmlId == paramTypeUIRangeMaxId
             || xmlId == paramTypeUIRangeIntervalId
             || xmlId == paramTypeUIResetValueId
            )
        {
            float fltValue = child->getAllSubText().getFloatValue();
            parameterType.setProperty(xmlId, fltValue, nullptr);
        }
        else if (xmlId == paramTypeUISkewFactorId)
        {
            String uiSkewFactorType = child->getStringAttribute("type", "standard");

            if (uiSkewFactorType.compareIgnoreCase("frommidpoint") == 0)
                parameterType.setProperty(paramTypeUISkewFactorTypeId, "frommidpoint", nullptr);
            else
                parameterType.setProperty(paramTypeUISkewFactorTypeId, "standard", nullptr);
                            
            double dblValue = child->getAllSubText().getDoubleValue();
            parameterType.setProperty(xmlId, dblValue, nullptr);
        }
        else if (xmlId == paramTypeSettingsId)
        {
            // Remove any existing settings, in case we're overriding from the main parameter type
            parameterType.removeChild(parameterType.getChildWithName(paramTypeSettingsId), nullptr);
                
            ValueTree parameterSettings(paramTypeSettingsId);

            int numSettings = 0;

            // Need to find out how many settings we have, so we can calculate the
            // automatic values below
            forEachXmlChildElementWithTagName(*child, subChild, paramTypeSettingId)
            {
                numSettings++;
            }

            if (numSettings > 0)
            {
                float uiRangeMax = (float)numSettings - 1.0f;

                parameterType.setProperty(paramTypeUIRangeMinId, 0.0f, nullptr);
                parameterType.setProperty(paramTypeUIRangeMaxId, uiRangeMax, nullptr);

                int settingCounter = 0;

                parameterType.setProperty(paramTypeValueTypeId, discrete, nullptr);

                forEachXmlChildElementWithTagName(*child, subChild, paramTypeSettingId)
                {
                    ValueTree parameterSetting(paramTypeSettingId);

                    String    settingName = subChild->getAllSubText();
                    parameterSetting.setProperty(paramTypeSettingNameId, settingName, nullptr);

                    float autoSettingValue = (float)scaleDouble(0, numSettings - 1, 0, 1, settingCounter);
                    float settingValue = (float)(subChild->getDoubleAttribute(paramTypeSettingValueId, autoSettingValue));
                    parameterSetting.setProperty(paramTypeSettingValueId, settingValue, nullptr);

                    parameterSettings.addChild(parameterSetting, -1, nullptr);
                    settingCounter++;
                }

                parameterType.setProperty(ScopeSync::paramTypeUIRangeMinId, 0, nullptr);
                parameterType.setProperty(ScopeSync::paramTypeUIRangeMaxId, settingCounter - 1, nullptr);

                parameterType.setProperty(paramTypeValueTypeId, discrete, nullptr);
                parameterType.addChild(parameterSettings, -1, nullptr);
            }
            else
            {
                // In this case, i.e. we have an empty "settings" element, set the parameter type
                // as being "continuous" instead of "discrete"
                parameterType.setProperty(paramTypeValueTypeId, continuous, nullptr);
            }
        }
    }
}

bool ScopeSync::loadDeviceParameters(XmlElement& deviceXml)
{
    if (deviceXml.hasAttribute("filename"))
    {
        File configurationFile(configurationFilePath.getValue());
        File configurationFileDir = configurationFile.getParentDirectory();

        File deviceFile = configurationFileDir.getChildFile(deviceXml.getStringAttribute("filename"));
        
        DBG("ScopeSync::loadDeviceParameters - Trying to load: " + deviceFile.getFullPathName());

        XmlDocument               deviceDocument(deviceFile);
        ScopedPointer<XmlElement> loadedDeviceXML = deviceDocument.getDocumentElement();

        if (loadedDeviceXML != nullptr)
        {
            deviceXml = *loadedDeviceXML;
        }
        else
        {
            AlertWindow::showMessageBox(AlertWindow::WarningIcon,"Error","Problem reading Configuration's Device File: " + deviceDocument.getLastParseError());
            return false;
        }
    }

    deviceParameters = ValueTree(deviceId);
    numParameters = 0;
    
    forEachXmlChildElementWithTagName(deviceXml, child, "parameter")
    {
        ValueTree parameter     = ValueTree(paramId);
        bool scopeLocalParameter = false;
        
        parameter.setProperty(paramNameId, child->getStringAttribute(paramNameId, "__NO_NAME__").toLowerCase(), nullptr);
        
        // In case we don't find a match or no parameter type is supplied in the XML,
        // initialise to the default parameter type
        ValueTree parameterType = parameterTypes.getChild(0).createCopy();
            
        forEachXmlChildElement(*child, subChild)
        {
            Identifier xmlId = subChild->getTagName();

            if (xmlId == paramShortDescId || xmlId == paramFullDescId)
            {
                parameter.setProperty(xmlId, subChild->getAllSubText(), nullptr);
            }
            else if (xmlId == paramScopeSyncId)
            {
                // Ignore ScopeSync value if we already have a ScopeLocal value
                if (scopeLocalParameter)
                    continue;
                else
                {
                    int scopeSyncCode = scopeSyncCodes.indexOf(subChild->getAllSubText());
                
                    if (scopeSyncCode != -1)
                        parameter.setProperty(xmlId, scopeSyncCode, nullptr);
                }
            }
            else if (xmlId == paramScopeLocalId)
            {
                int scopeLocalCode = scopeLocalCodes.indexOf(subChild->getAllSubText());
                
                if (scopeLocalCode != -1 && inScopeFXContext())
                {
                    scopeLocalParameter = true;
                    parameter.setProperty(xmlId, scopeLocalCode, nullptr);
                    // Get rid of any ScopeSync value that may have been set
                    parameter.removeProperty(paramScopeSyncId, nullptr);
                }   
                else
                {
                    DBG("ScopeSync::loadDeviceParameters - Invalid ScopeLocal code or not in scopefx context, so ignoring parameter");
                    scopeLocalParameter = true;
                    break;
                }
            }
            else if (xmlId == paramTypeId)
            {
                String parameterTypeName = subChild->getStringAttribute(paramTypeNameId);

                for (int i = 0; i < parameterTypes.getNumChildren(); i++)
                {
                    if (parameterTypes.getChild(i).getProperty(paramTypeNameId) == parameterTypeName)
                    {
                        parameterType = parameterTypes.getChild(i).createCopy();
                        break;
                    }
                }

                // Apply any overrides
                getParameterTypeFromXML(*subChild, parameterType);
            }
        }

        if (scopeLocalParameter)
        {
            // Ignore ScopeLocal parameters if in a plugin context
            if (inPluginContext())
                continue;
        }

        numParameters++;
        
        parameter.addChild(parameterType, -1, nullptr);
        deviceParameters.addChild(parameter, -1, nullptr);

        // Set a sensible default value
        int   paramIdx;
        float uiInitValue;
        float hostInitValue;

        if (parameterType.hasProperty(paramTypeUIResetValueId))
            uiInitValue = parameterType.getProperty(paramTypeUIResetValueId);
        else
            uiInitValue = parameterType.getProperty(paramTypeUIRangeMinId);

        paramIdx      = deviceParameters.indexOf(parameter);
        hostInitValue = convertUIToHostValue(paramIdx, uiInitValue);

        setParameterValues(paramIdx, hostInitValue, uiInitValue);
    }

    //DeviceParameterComparator comparator;
    //deviceParameters.sort(comparator, nullptr, true);
    
    DBG("ScopeSync::loadDeviceParameters - Param definition");
    DBG("==================================================");
    for (int i = 0; i < deviceParameters.getNumChildren(); i++)
    {
        ValueTree param = deviceParameters.getChild(i);
        String paramDefinition = param.toXmlString();
        DBG(paramDefinition);
    }
    DBG("==================================================");
    
    return true;
}

bool ScopeSync::loadMappingFile(XmlElement& mappingXml)
{
    File configurationFile(configurationFilePath.getValue());
    File configurationFileDir = configurationFile.getParentDirectory();

    File mappingFile = configurationFileDir.getChildFile(mappingXml.getStringAttribute("filename"));
        
    DBG("ScopeSync::loadMappingFile - Trying to load: " + mappingFile.getFullPathName());

    XmlDocument               mappingDocument(mappingFile);
    ScopedPointer<XmlElement> loadedMappingXml = mappingDocument.getDocumentElement();

    if (loadedMappingXml != nullptr)
    {
        mappingXml = *loadedMappingXml;
    }
    else
    {
        AlertWindow::showMessageBox(AlertWindow::WarningIcon,"Error","Problem reading Configuration's Mapping File: " + mappingDocument.getLastParseError());
        return false;
    }

    return true;
}

bool ScopeSync::loadLayoutFile(XmlElement& layoutXml)
{
    File configurationFile(configurationFilePath.getValue());
    File configurationFileDir = configurationFile.getParentDirectory();

    File layoutFile = configurationFileDir.getChildFile(layoutXml.getStringAttribute("filename"));
        
    DBG("ScopeSync::loadLayoutFile - Trying to load: " + layoutFile.getFullPathName());

    XmlDocument               layoutDocument(layoutFile);
    ScopedPointer<XmlElement> loadedLayoutXml = layoutDocument.getDocumentElement();

    if (loadedLayoutXml != nullptr)
    {
        layoutXml = *loadedLayoutXml;
    }
    else
    {
        AlertWindow::showMessageBox(AlertWindow::WarningIcon,"Error","Problem reading Configuration's Layout File: " + layoutDocument.getLastParseError());
        return false;
    }

    return true;
}

const String ScopeSync::systemLookAndFeels =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<lookandfeels>\n"
"  <lookandfeel id=\"system:default\">\n"
"    <colours>\n"
"      <textbutton>\n"
"        <textcolouroffid>00ffffff</textcolouroffid>\n"
"        <textcolouronid>00ff0000</textcolouronid>\n"
"        <buttoncolourid>00ff0000</buttoncolourid>\n"
"      </textbutton>\n"
"    </colours>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:load_config_button\" parentid=\"system:default\">\n"
"    <images>\n"
"      <textbutton>\n"
"        <upfilename>loadConfigButton_off_png</upfilename>\n"
"        <downfilename>loadConfigButton_on_png</downfilename>\n"
"        <mouseoverupfilename>loadConfigButton_off_png</mouseoverupfilename>\n"
"        <mouseoverdownfilename>loadConfigButton_on_png</mouseoverdownfilename>\n"
"      </textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"</lookandfeels>\n";

const String ScopeSync::systemParameterTypes =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<parametertypes>\n"
"    <parametertype>\n"
"        <name>0-100%</name>\n"
"        <uisuffix> %</uisuffix>\n"
"        <scoperangemin>0</scoperangemin>\n"
"        <scoperangemax>MAX_INT</scoperangemax>\n"
"        <uirangemin>0</uirangemin>\n"
"        <uirangemax>100</uirangemax>\n"
"        <uirangeinterval>0.001</uirangeinterval>\n"
"        <uiresetvalue>45</uiresetvalue>\n"
"    </parametertype>\n"
"    <parametertype>\n"
"        <name>Freq (Hz)</name>\n"
"        <uisuffix> Hz</uisuffix>\n"
"        <scoperangemin>0</scoperangemin>\n"
"        <scoperangemax>MAX_INT</scoperangemax>\n"
"        <uirangemin>0</uirangemin>\n"
"        <uirangemax>24000</uirangemax>\n"
"        <uirangeinterval>0.01</uirangeinterval>\n"
"        <uiskewfactor type=\"standard\">2</uiskewfactor>\n"
"    </parametertype>\n"
"    <parametertype>\n"
"        <name>Env Time (s)</name>\n"
"        <uisuffix></uisuffix>\n"
"        <scoperangemin>0</scoperangemin>\n"
"        <scoperangemax>MAX_INT</scoperangemax>\n"
"        <uirangemin>0</uirangemin>\n"
"        <uirangemax>2147483647</uirangemax>\n"
"        <uirangeinterval>1</uirangeinterval>\n"
"    </parametertype>\n"
"    <parametertype>\n"
"        <name>MultimodeOsc Wave</name>\n"
"        <scoperangemin>0</scoperangemin>\n"
"        <scoperangemax>MAX_INT</scoperangemax>\n"
"        <settings>\n"
"            <setting>Sine</setting>\n"
"            <setting>Tri</setting>\n"
"            <setting>Saw Up</setting>\n"
"            <setting>Saw Dn</setting>\n"
"            <setting>Square</setting>\n"
"            <setting>Input</setting>\n"
"        </settings>\n"
"    </parametertype>\n"
"    <parametertype>\n"
"        <name>ON/OFF Toggle</name>\n"
"        <scoperangemin>0</scoperangemin>\n"
"        <scoperangemax>MAX_INT</scoperangemax>\n"
"        <settings>\n"
"            <setting>OFF</setting>\n"
"            <setting>ON</setting>\n"
"        </settings>\n"
"    </parametertype>\n"
"</parametertypes>";

const String ScopeSync::loaderConfiguration =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"\n"
"<configuration>\n"
"  <device>\n"
"    <parameter name=\"PARAM132\">\n"
"      <shortdescription>Param 132</shortdescription>\n"
"      <fulldescription>Load New Configuration</fulldescription>\n"
"      <parametertype name=\"ON/OFF Toggle\"/>\n"
"      <scopelocal>Z8</scopelocal>\n"
"    </parameter>\n"
"  </device>\n"
"  <mapping>\n"
"    <textbutton name=\"B69\"><parameter name=\"PARAM132\" settingdown=\"ON\" settingup=\"OFF\" type=\"notoggle\"/>\n"
"    </textbutton>\n"
"  </mapping>\n"
"  <layout>\n"
"    <component>\n"
"      <backgroundcolour>ff2b2e30</backgroundcolour>\n"
"      <position>\n"
"        <x>0</x>\n"
"        <y>0</y>\n"
"      </position>\n"
"      <size>\n"
"        <width>250</width>\n"
"        <height>40</height>\n"
"      </size>\n"
"      <textbutton lfid=\"system:load_config_button\" displaycontext=\"host\">\n"
"        <name>chooseconfiguration</name>\n"
"        <tooltip>Load New Configuration</tooltip>\n"
"        <position>\n"
"          <x>210</x>\n"
"          <y>10</y>\n"
"        </position>\n"
"        <size>\n"
"          <width>21</width>\n"
"          <height>21</height>\n"
"        </size>\n"
"      </textbutton>\n"
"      <textbutton lfid=\"system:load_config_button\" displaycontext=\"scope\">\n"
"        <name>B69</name>\n"
"        <tooltip>Load New Configuration</tooltip>\n"
"        <position>\n"
"          <x>210</x>\n"
"          <y>10</y>\n"
"        </position>\n"
"        <size>\n"
"          <width>21</width>\n"
"          <height>21</height>\n"
"        </size>\n"
"      </textbutton>\n"
"      <component>\n"
"        <backgroundcolour>00000000</backgroundcolour>\n"
"        <backgroundimage>scopeSync_logo_png</backgroundimage>\n"
"        <position>\n"
"          <x>10</x>\n"
"          <y>12</y>\n"
"        </position>\n"
"        <size>\n"
"          <width>151</width>\n"
"          <height>16</height>\n"
"        </size>\n"
"      </component>\n"
"    </component>\n"
"  </layout>\n"
"</configuration>\n";

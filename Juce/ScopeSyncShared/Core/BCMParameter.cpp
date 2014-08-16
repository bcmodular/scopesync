/*
  ==============================================================================

    BCMParameter.cpp
    Created: 15 Aug 2014 8:48:58am
    Author:  giles

  ==============================================================================
*/

#include "BCMParameter.h"
#include "../Utils/BCMMath.h"

const Identifier BCMParameter::paramTypesId                = "parametertypes";
const Identifier BCMParameter::paramTypeId                 = "parametertype";
const Identifier BCMParameter::paramTypeNameId             = "name";
const Identifier BCMParameter::paramTypeValueTypeId        = "valuetype";
const Identifier BCMParameter::paramTypeUISuffixId         = "uisuffix";
const Identifier BCMParameter::paramTypeUIRangeMinId       = "uirangemin";
const Identifier BCMParameter::paramTypeUIRangeMaxId       = "uirangemax";
const Identifier BCMParameter::paramTypeUIRangeIntervalId  = "uirangeinterval";
const Identifier BCMParameter::paramTypeUIResetValueId     = "uiresetvalue";
const Identifier BCMParameter::paramTypeUISkewFactorId     = "uiskewfactor";
const Identifier BCMParameter::paramTypeSkewUIOnlyId       = "skewuionly";
const Identifier BCMParameter::paramTypeScopeRangeMinId    = "scoperangemin";
const Identifier BCMParameter::paramTypeScopeRangeMaxId    = "scoperangemax";
const Identifier BCMParameter::paramTypeScopeRangeMinFltId = "scoperangeminflt";
const Identifier BCMParameter::paramTypeScopeRangeMaxFltId = "scoperangemaxflt";
const Identifier BCMParameter::paramTypeSettingsId         = "settings";
const Identifier BCMParameter::paramTypeSettingId          = "setting";
const Identifier BCMParameter::paramTypeSettingNameId      = "name";
const Identifier BCMParameter::paramTypeSettingValueId     = "value";
                                
const Identifier BCMParameter::deviceId          = "device";
const Identifier BCMParameter::paramId           = "parameter";
const Identifier BCMParameter::paramNameId       = "name";
const Identifier BCMParameter::paramShortDescId  = "shortdescription";
const Identifier BCMParameter::paramFullDescId   = "fulldescription";
const Identifier BCMParameter::paramScopeSyncId  = "scopesync";
const Identifier BCMParameter::paramScopeLocalId = "scopelocal";

BCMParameter::BCMParameter(int index, ValueTree parameterDefinition)
{
    hostIdx = index;
    definition = parameterDefinition;
    affectedByUI = false;
    putValuesInRange(true);
}

void BCMParameter::putValuesInRange(bool initialise)
{
    ValueTree parameterType = definition.getChildWithName(paramTypeId);
    float     uiMinValue    = parameterType.getProperty(paramTypeUIRangeMinId);
    float     uiMaxValue    = parameterType.getProperty(paramTypeUIRangeMaxId);
    
    if (initialise)
    {
        if (parameterType.hasProperty(paramTypeUIResetValueId))
            uiValue = parameterType.getProperty(paramTypeUIResetValueId);
        else
            uiValue = parameterType.getProperty(paramTypeUIRangeMinId);
    }
    else
    {
        if (float(uiValue.getValue()) < uiMinValue)
            uiValue = uiMinValue;
        else if (float(uiValue.getValue()) > uiMaxValue)
            uiValue = uiMaxValue;
    }

    linearNormalisedValue = (float)scaleDouble(uiMinValue, uiMaxValue, 0.0f, 1.0f, uiValue.getValue());
}

void BCMParameter::mapToUIValue(Value& valueToMapTo)
{
    valueToMapTo.referTo(uiValue);
}

void BCMParameter::setAffectedByUI(bool isAffected)
{
    affectedByUI = isAffected;
}

bool BCMParameter::isAffectedByUI()
{
    return affectedByUI;
}

String BCMParameter::getName()
{
    return definition.getProperty(paramNameId, String::empty).toString();
}

int BCMParameter::getScopeCode()
{
    int scopeCode = definition.getProperty(paramScopeSyncId, -1);

    if (scopeCode == -1)
    {
        scopeCode = definition.getProperty(paramScopeLocalId, -1);

        if (scopeCode != -1)
        {
            // We found one, so shift the value by the number of
            // ScopeSyncIds to generate a valid ScopeCode
            scopeCode += 128;
        }
    }
    
    return scopeCode;
}

void BCMParameter::getSettings(ValueTree& settings)
{
    ValueTree parameterType = definition.getChildWithName(paramTypeId);
    
    if (parameterType.isValid())
        settings = parameterType.getChildWithName(paramTypeSettingsId);
}

void BCMParameter::getDescriptions(String& shortDesc, String& fullDesc)
{
    shortDesc = definition.getProperty(paramShortDescId, shortDesc).toString();
    fullDesc  = definition.getProperty(paramFullDescId, fullDesc).toString();
}

void BCMParameter::getUIRanges(double& rangeMin, double& rangeMax, double& rangeInt, String& uiSuffix)
{
    ValueTree parameterType = definition.getChildWithName(paramTypeId);

    if (parameterType.isValid())
    {
        rangeMin = parameterType.getProperty(paramTypeUIRangeMinId, rangeMin);
        rangeMax = parameterType.getProperty(paramTypeUIRangeMaxId, rangeMax);
        rangeInt = parameterType.getProperty(paramTypeUIRangeIntervalId, rangeInt);
        uiSuffix = parameterType.getProperty(paramTypeUISuffixId, uiSuffix);
    }
}

bool BCMParameter::getUIResetValue(double& uiResetValue)
{
    bool      foundValue    = false;
    ValueTree parameterType = definition.getChildWithName(paramTypeId);

    if (parameterType.isValid())
    {
        if (parameterType.hasProperty(paramTypeUIResetValueId))
        {
            uiResetValue = parameterType.getProperty(paramTypeUIResetValueId);
            foundValue = true;
        }
    }

    return foundValue;
}

bool BCMParameter::getUISkewFactor(double& uiSkewFactor)
{
    bool      foundValue    = false;
    ValueTree parameterType = definition.getChildWithName(paramTypeId);

    if (parameterType.isValid())
    {
        if (parameterType.hasProperty(paramTypeUISkewFactorId))
        {
            uiSkewFactor = parameterType.getProperty(paramTypeUISkewFactorId);
            foundValue = true;
        }
    }
    
    return foundValue;
}

void BCMParameter::getUITextValue(String& textValue)
{
    ValueTree parameterType = definition.getChildWithName(paramTypeId);
        
    int parameterValueType = parameterType.getProperty(paramTypeValueTypeId);

    if (parameterValueType == discrete)
    {
        ValueTree paramSettings = parameterType.getChildWithName(paramTypeSettingsId);

        if (paramSettings.isValid())
        {
            int settingIdx = roundDoubleToInt(uiValue.getValue());
            String settingName = paramSettings.getChild(settingIdx).getProperty(paramTypeSettingNameId);
                    
            if (settingName.isNotEmpty())
                textValue = settingName;
        }
    }
    else
    {
        String uiSuffix = String::empty;

        if (parameterType.hasProperty(paramTypeUISuffixId))
            uiSuffix = parameterType.getProperty(paramTypeUISuffixId);
        
        textValue = uiValue.toString() + uiSuffix;
    }
    //DBG("BCMParameter::getUITextValue - " + definition.getProperty(paramNameId).toString() + ": " + textValue);
}

float BCMParameter::getHostValue()
{
    float hostValue = skewHostValue(linearNormalisedValue.getValue(), true);
    DBG("BCMParameter::getHostValue - " + definition.getProperty(paramNameId).toString() + ": " + String(hostValue));
    return hostValue;
}

float BCMParameter::getScopeFltValue()
{
    ValueTree parameterType = definition.getChildWithName(paramTypeId);
    float     minScopeValue = parameterType.getProperty(paramTypeScopeRangeMinFltId);
    float     maxScopeValue = parameterType.getProperty(paramTypeScopeRangeMaxFltId);

    float scopeValue = (float)scaleDouble(0.0f, 1.0f, minScopeValue, maxScopeValue, linearNormalisedValue.getValue());
    DBG("BCMParameter::getScopeFltValue - " + definition.getProperty(paramNameId).toString() + ": " + String(scopeValue));
    return scopeValue;
}

int BCMParameter::getScopeIntValue()
{
    ValueTree parameterType = definition.getChildWithName(paramTypeId);
    int minScopeValue       = parameterType.getProperty(paramTypeScopeRangeMinId);
    int maxScopeValue       = parameterType.getProperty(paramTypeScopeRangeMaxId);

    int scopeValue = roundDoubleToInt(scaleDouble(0.0f, 1.0f, minScopeValue, maxScopeValue, linearNormalisedValue.getValue()));
    DBG("BCMParameter::getScopeIntValue - " + definition.getProperty(paramNameId).toString() + ": " + String(scopeValue));
    return scopeValue;
}
    
float BCMParameter::convertLinearNormalisedToUIValue(float linearNormalisedValue)
{
    double minUIValue;
    double maxUIValue;
    double uiInterval;
    String uiSuffix;
    
    getUIRanges(minUIValue, maxUIValue, uiInterval, uiSuffix);
    
    return (float)scaleDouble(0.0f, 1.0f, minUIValue, maxUIValue, linearNormalisedValue);
}

void BCMParameter::setHostValue(float newValue)
{
    linearNormalisedValue = skewHostValue(newValue, false);
    uiValue               = convertLinearNormalisedToUIValue(linearNormalisedValue.getValue());
    DBG("BCMParameter::setHostValue - " + definition.getProperty(paramNameId).toString() + " linearNormalisedValue: " + linearNormalisedValue.toString() + ", uiValue: " + uiValue.toString());
}

void BCMParameter::setScopeFltValue(float newValue)
{
    ValueTree parameterType = definition.getChildWithName(paramTypeId);
    float     minScopeValue = parameterType.getProperty(paramTypeScopeRangeMinFltId);
    float     maxScopeValue = parameterType.getProperty(paramTypeScopeRangeMaxFltId);

    linearNormalisedValue = (float)scaleDouble(minScopeValue, maxScopeValue, 0.0f, 1.0f, newValue);
    uiValue               = convertLinearNormalisedToUIValue(linearNormalisedValue.getValue());
    DBG("BCMParameter::setScopeFltValue - " + definition.getProperty(paramNameId).toString() + " linearNormalisedValue: " + linearNormalisedValue.toString() + ", uiValue: " + uiValue.toString());
}

void BCMParameter::setScopeIntValue(int newValue)
{
    if (!affectedByUI)
    {
        ValueTree parameterType = definition.getChildWithName(paramTypeId);
        int minScopeValue       = parameterType.getProperty(paramTypeScopeRangeMinId);
        int maxScopeValue       = parameterType.getProperty(paramTypeScopeRangeMaxId);

        linearNormalisedValue = (float)scaleDouble(minScopeValue, maxScopeValue, 0.0f, 1.0f, newValue);
        uiValue               = convertLinearNormalisedToUIValue(linearNormalisedValue.getValue());
        DBG("BCMParameter::setScopeIntValue - " + definition.getProperty(paramNameId).toString() + " linearNormalisedValue: " + linearNormalisedValue.toString() + ", uiValue: " + uiValue.toString());
    }
    else
    {
        DBG("ScopeSync::receiveUpdatesFromScopeAsync: Parameter affected by UI since last update: " + definition.getProperty(paramNameId).toString());
    }
}

void BCMParameter::setUIValue(float newValue)
{
    uiValue = newValue;

    double minUIValue;
    double maxUIValue;
    double uiInterval;
    String uiSuffix;
    
    getUIRanges(minUIValue, maxUIValue, uiInterval, uiSuffix);
    
    linearNormalisedValue = (float)scaleDouble(minUIValue, maxUIValue, 0.0f, 1.0f, newValue);
    DBG("BCMParameter::setUIValue - " + definition.getProperty(paramNameId).toString() + " linearNormalisedValue: " + linearNormalisedValue.toString() + ", uiValue: " + uiValue.toString());
}

float BCMParameter::skewHostValue(float hostValue, bool invert)
{
    double skewedValue = hostValue;

    ValueTree parameterType = definition.getChildWithName(paramTypeId);

    if (parameterType.hasProperty(paramTypeUISkewFactorId) && !(parameterType.getProperty(paramTypeSkewUIOnlyId)))
    {
        double skewFactor = parameterType.getProperty(paramTypeUISkewFactorId);
        skewValue(skewedValue, skewFactor, 0.0f, 1.0f, invert);
    }
    
    return (float)skewedValue;
}

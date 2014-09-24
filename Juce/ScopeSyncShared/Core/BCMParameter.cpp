/*
  ==============================================================================

    BCMParameter.cpp
    Created: 15 Aug 2014 8:48:58am
    Author:  giles

  ==============================================================================
*/

#include "BCMParameter.h"
#include "../Utils/BCMMath.h"
#include "Global.h"

BCMParameter::BCMParameter(int index, ValueTree parameterDefinition)
{
    hostIdx = index;
    definition = parameterDefinition;
    affectedByUI = false;
    putValuesInRange(true);
    setNumDecimalPlaces();
}

void BCMParameter::setNumDecimalPlaces()
{
    float uiInterval = definition.getProperty(Ids::uiRangeInterval);
    
    // figure out the number of DPs needed to display all values at this
    // interval setting.
    numDecimalPlaces = 7;

    if (uiInterval != 0)
    {
        int v = std::abs (roundToInt(uiInterval * 10000000));

        while ((v % 10) == 0)
        {
            --numDecimalPlaces;
            v /= 10;
        }
    }
}

void BCMParameter::putValuesInRange(bool initialise)
{
    DBG("BCMParameter::putValuesInRange - " + String(getName()));
    float uiMinValue = definition.getProperty(Ids::uiRangeMin);
    float uiMaxValue = definition.getProperty(Ids::uiRangeMax);
    
    DBG("BCMParameter::putValuesInRange - uiMinValue: " + String(uiMinValue) + ", uiMaxValue: " + String(uiMaxValue));
    
    if (initialise)
    {
        DBG("BCMParameter::putValuesInRange - Initialise to: " + String(definition.getProperty(Ids::uiResetValue)));
        uiValue.setValue(definition.getProperty(Ids::uiResetValue));
    }
    else
    {
        if (float(uiValue.getValue()) < uiMinValue)
        {
            DBG("BCMParameter::putValuesInRange - Bumping up to: " + String(uiMinValue));
            uiValue.setValue(uiMinValue);
        }
        else if (float(uiValue.getValue()) > uiMaxValue)
        {
            DBG("BCMParameter::putValuesInRange - Dropping down to: " + String(uiMaxValue));
            uiValue.setValue(uiMaxValue);
        }
    }

    linearNormalisedValue = scaleDouble(uiMinValue, uiMaxValue, 0.0f, 1.0f, uiValue.getValue());
}

void BCMParameter::mapToUIValue(Value& valueToMapTo)
{
    DBG("BCMParameter::mapToUIValue - current uiValue: " + uiValue.getValue().toString());
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
    return definition.getProperty(Ids::name).toString();
}

int BCMParameter::getScopeCode()
{
    int scopeCode = definition.getProperty(Ids::scopeSync);

    if (scopeCode == -1)
    {
        scopeCode = definition.getProperty(Ids::scopeLocal);

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
    settings = definition.getChildWithName(Ids::settings);
}

void BCMParameter::getDescriptions(String& shortDesc, String& fullDesc)
{
    shortDesc = definition.getProperty(Ids::shortDescription).toString();
    fullDesc  = definition.getProperty(Ids::fullDescription).toString();
}

void BCMParameter::getUIRanges(double& rangeMin, double& rangeMax, double& rangeInt, String& uiSuffix)
{
    rangeMin = definition.getProperty(Ids::uiRangeMin);
    rangeMax = definition.getProperty(Ids::uiRangeMax);
    rangeInt = definition.getProperty(Ids::uiRangeInterval);
    uiSuffix = definition.getProperty(Ids::uiSuffix);
}

double BCMParameter::getUIResetValue()
{
    return definition.getProperty(Ids::uiResetValue);
}

double BCMParameter::getUISkewFactor()
{
    return definition.getProperty(Ids::uiSkewFactor);
}

void BCMParameter::getUITextValue(String& textValue)
{
    int parameterValueType = definition.getProperty(Ids::valueType);

    if (parameterValueType == discrete)
    {
        ValueTree paramSettings = definition.getChildWithName(Ids::settings);

        if (paramSettings.isValid())
        {
            int settingIdx = roundDoubleToInt(uiValue.getValue());
            String settingName = paramSettings.getChild(settingIdx).getProperty(Ids::name);
                    
            if (settingName.isNotEmpty())
                textValue = settingName;
        }
    }
    else
    {
        textValue = String(float(uiValue.getValue()), numDecimalPlaces) + definition.getProperty(Ids::uiSuffix);
    }
    //DBG("BCMParameter::getUITextValue - " + definition.getProperty(paramNameId).toString() + ": " + textValue);
}

float BCMParameter::getHostValue()
{
    float hostValue = skewHostValue(linearNormalisedValue.getValue(), true);
    DBG("BCMParameter::getHostValue - " + definition.getProperty(Ids::name).toString() + ": " + String(hostValue));
    return hostValue;
}

float BCMParameter::getScopeFltValue()
{
    float minScopeValue = definition.getProperty(Ids::scopeRangeMinFlt);
    float maxScopeValue = definition.getProperty(Ids::scopeRangeMaxFlt);

    double valueToScale = linearNormalisedValue.getValue();

    double ref        = definition.getProperty(Ids::scopeDBRef);
        
    if (ref != 0.0f)
    {
        double uiMinValue = definition.getProperty(Ids::uiRangeMin);
        double uiMaxValue = definition.getProperty(Ids::uiRangeMax);
        
        valueToScale = dbSkew(linearNormalisedValue.getValue(), ref, uiMinValue, uiMaxValue, true);
    }

    float scopeValue = (float)scaleDouble(0.0f, 1.0f, minScopeValue, maxScopeValue, valueToScale);
    DBG("BCMParameter::getScopeFltValue - " + definition.getProperty(Ids::name).toString() + ": " + String(scopeValue));
    return scopeValue;
}

void BCMParameter::getScopeRanges(int& min, int& max)
{
    min = definition.getProperty(Ids::scopeRangeMin);
    max = definition.getProperty(Ids::scopeRangeMax);
}

int BCMParameter::getScopeIntValue()
{
    int minScopeValue;
    int maxScopeValue;

    getScopeRanges(minScopeValue, maxScopeValue);

    double valueToScale = linearNormalisedValue.getValue();

    double ref        = definition.getProperty(Ids::scopeDBRef);
        
    if (ref != 0.0f)
    {
        double uiMinValue = definition.getProperty(Ids::uiRangeMin);
        double uiMaxValue = definition.getProperty(Ids::uiRangeMax);

        valueToScale = dbSkew(linearNormalisedValue.getValue(), ref, uiMinValue, uiMaxValue, true);
    }

    int scopeValue = roundDoubleToInt(scaleDouble(0.0f, 1.0f, minScopeValue, maxScopeValue, valueToScale));

    return scopeValue;
}
    
double BCMParameter::convertLinearNormalisedToUIValue(double linearNormalisedValue)
{
    double minUIValue;
    double maxUIValue;
    double uiInterval;
    String uiSuffix;
    
    getUIRanges(minUIValue, maxUIValue, uiInterval, uiSuffix);
    
    return scaleDouble(0.0f, 1.0f, minUIValue, maxUIValue, linearNormalisedValue);
}

void BCMParameter::setHostValue(float newValue)
{
    linearNormalisedValue = skewHostValue(newValue, false);
    uiValue               = convertLinearNormalisedToUIValue(linearNormalisedValue.getValue());
    DBG("BCMParameter::setHostValue - " + definition.getProperty(Ids::name).toString() + " linearNormalisedValue: " + linearNormalisedValue.toString() + ", uiValue: " + uiValue.toString());
}

void BCMParameter::setScopeFltValue(float newValue)
{
    float minScopeValue = definition.getProperty(Ids::scopeRangeMinFlt);
    float maxScopeValue = definition.getProperty(Ids::scopeRangeMaxFlt);

    linearNormalisedValue = scaleDouble(minScopeValue, maxScopeValue, 0.0f, 1.0f, newValue);

    double ref        = definition.getProperty(Ids::scopeDBRef);
        
    if (ref != 0.0f)
    {
        double uiMinValue = definition.getProperty(Ids::uiRangeMin);
        double uiMaxValue = definition.getProperty(Ids::uiRangeMax);
            
        linearNormalisedValue = dbSkew(linearNormalisedValue.getValue(), ref, uiMinValue, uiMaxValue, false);
    }

    uiValue = convertLinearNormalisedToUIValue(linearNormalisedValue.getValue());
    DBG("BCMParameter::setScopeFltValue - " + definition.getProperty(Ids::name).toString() + " linearNormalisedValue: " + linearNormalisedValue.toString() + ", uiValue: " + uiValue.toString());
}

void BCMParameter::setScopeIntValue(int newValue)
{
    if (!affectedByUI)
    {
        int minScopeValue;
        int maxScopeValue;

        getScopeRanges(minScopeValue, maxScopeValue);
        linearNormalisedValue = scaleDouble(minScopeValue, maxScopeValue, 0.0f, 1.0f, newValue);

        double ref        = definition.getProperty(Ids::scopeDBRef);
        
        if (ref != 0.0f)
        {
            double uiMinValue = definition.getProperty(Ids::uiRangeMin);
            double uiMaxValue = definition.getProperty(Ids::uiRangeMax);
            
            linearNormalisedValue = dbSkew(linearNormalisedValue.getValue(), ref, uiMinValue, uiMaxValue, false);
        }

        uiValue.setValue(convertLinearNormalisedToUIValue(linearNormalisedValue.getValue()));
        DBG("BCMParameter::setScopeIntValue - " + definition.getProperty(Ids::name).toString() + " linearNormalisedValue: " + linearNormalisedValue.toString() + ", uiValue: " + uiValue.toString());
    }
    else
    {
        DBG("ScopeSync::receiveUpdatesFromScopeAsync: Parameter affected by UI since last update: " + definition.getProperty(Ids::name).toString());
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
    
    linearNormalisedValue = scaleDouble(minUIValue, maxUIValue, 0.0f, 1.0f, newValue);
    DBG("BCMParameter::setUIValue - " + definition.getProperty(Ids::name).toString() + " linearNormalisedValue: " + linearNormalisedValue.toString() + ", uiValue: " + uiValue.toString());
}

float BCMParameter::skewHostValue(float hostValue, bool invert)
{
    double skewedValue = hostValue;
    double skewFactor = definition.getProperty(Ids::uiSkewFactor);

    if ((skewFactor != 1.0f) && !(definition.getProperty(Ids::skewUIOnly)))
    {
        skewValue(skewedValue, skewFactor, 0.0f, 1.0f, invert);
    }
    
    return (float)skewedValue;
}


double BCMParameter::dbSkew(double valueToSkew, double ref, double uiMinValue, double uiMaxValue, bool invert)
{
    if (!invert)
    {
        double minNormalised = ref * pow(10, (uiMinValue/20));

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

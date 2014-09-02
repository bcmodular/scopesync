/*
  ==============================================================================

    BCMParameter.h
    Created: 15 Aug 2014 8:48:58am
    Author:  giles

  ==============================================================================
*/

#ifndef BCMPARAMETER_H_INCLUDED
#define BCMPARAMETER_H_INCLUDED

#include <JuceHeader.h>

class BCMParameter
{
public:
    BCMParameter(int index, ValueTree parameterDefinition);
    ~BCMParameter() {};

    void    mapToUIValue(Value& valueToMapTo);
    void    setAffectedByUI(bool isAffected);
    bool    isAffectedByUI();
    String  getName();
    int     getHostIdx() { return hostIdx; };
    int     getScopeCode();
    void    getSettings(ValueTree& settings);
    void    getDescriptions(String& shortDesc, String& fullDesc);
    void    getUIRanges(double& rangeMin, double& rangeMax, double& rangeInt, String& uiSuffix);
    void    getScopeRanges(int& min, int& max);
    double  getUIResetValue();
    double  getUISkewFactor();

    void  getUITextValue(String& textValue);
    float getHostValue();
    float getScopeFltValue();
    int   getScopeIntValue();
    
    void setHostValue(float newValue);
    void setScopeFltValue(float newValue);
    void setScopeIntValue(int newValue);
    void setUIValue(float newValue);

    /* ============================ Enumerations ============================== */
    enum ParameterValueType {continuous, discrete}; // Possible types of Parameter Value
    
private:
    /* ====================== Private Parameter Methods ======================= */
    // Either on initialisation, or after ranges have changed, this method will
    // ensure the UI and LinearNormalised values are within the ranges
    void   putValuesInRange(bool initialise);
    void   setNumDecimalPlaces();
    float  skewHostValue(float hostValue, bool invert);
    double dbSkew(double valueToSkew, double ref, double uiMinValue, double uiMaxValue, bool invert);
    float  convertLinearNormalisedToUIValue(float linearNormalisedValue);

    void  setParameterValues(int index, float newHostValue, float newUIValue);
    float convertUIToHostValue(int paramIdx, float value);
    float convertHostToUIValue(int paramIdx, float value);
    float convertScopeFltToHostValue(int paramIdx, float value);
    float convertScopeIntToHostValue(int paramIdx, int value);
    float convertHostToScopeFltValue(int paramIdx, float value);
    int   convertHostToScopeIntValue(int paramIdx, float value);
    int   findNearestParameterSetting(const ValueTree& settings, float value);
    
    /* ===================== Private member variables ========================= */
    ValueTree definition;
    Value     uiValue;
    Value     linearNormalisedValue;
    bool      affectedByUI;
    int       hostIdx;
    int       numDecimalPlaces;
};

#endif  // BCMPARAMETER_H_INCLUDED

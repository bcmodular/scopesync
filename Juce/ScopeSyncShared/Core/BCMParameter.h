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
    bool    getUIResetValue(double& uiResetValue);
    bool    getUISkewFactor(double& uiSkewFactor);

    void  getUITextValue(String& textValue);
    float getHostValue();
    float getScopeFltValue();
    int   getScopeIntValue();
    
    void setHostValue(float newValue);
    void setScopeFltValue(float newValue);
    void setScopeIntValue(int newValue);
    void setUIValue(float newValue);

    /* =========================== Identifiers =============================== */
    static const Identifier paramTypesId;
    static const Identifier paramTypeId;
    static const Identifier paramTypeNameId;
    static const Identifier paramTypeValueTypeId;
    static const Identifier paramTypeHostRangeMinId;
    static const Identifier paramTypeHostRangeMaxId;
    static const Identifier paramTypeHostRangeIntervalId;
    static const Identifier paramTypeUISuffixId;
    static const Identifier paramTypeUIRangeMinId;
    static const Identifier paramTypeUIRangeMaxId;
    static const Identifier paramTypeUIRangeIntervalId;
    static const Identifier paramTypeUIResetValueId;
    static const Identifier paramTypeUISkewFactorId;
    static const Identifier paramTypeSkewUIOnlyId;
    static const Identifier paramTypeScopeRangeMinId;
    static const Identifier paramTypeScopeRangeMaxId;
    static const Identifier paramTypeScopeRangeMinFltId;
    static const Identifier paramTypeScopeRangeMaxFltId;
    static const Identifier paramTypeScopeDBRefId;
    static const Identifier paramTypeSettingsId;
    static const Identifier paramTypeSettingId;
    static const Identifier paramTypeSettingNameId;
    static const Identifier paramTypeSettingValueId;

    static const Identifier deviceId;
    static const Identifier paramId;
    static const Identifier paramNameId;
    static const Identifier paramShortDescId;
    static const Identifier paramFullDescId;
    static const Identifier paramScopeSyncId;
    static const Identifier paramScopeLocalId;
    
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

/*
  ==============================================================================

    BCMParameterController.h
    Created: 29 Nov 2015 8:33:20pm
    Author:  giles

  ==============================================================================
*/

#ifndef BCMPARAMETERCONTROLLER_H_INCLUDED
#define BCMPARAMETERCONTROLLER_H_INCLUDED

#include <JuceHeader.h>
#include "BCMParameter.h"
#include "ScopeSync.h"

class BCMParameterController : public Timer
{
public:

    BCMParameterController(ScopeSync* owner);
    ~BCMParameterController();
    
	/* ====================== Public Parameter Methods ======================= */
    // Returns the number of parameters to inform the host about. Actually returns
    // the "minHostParameters" value to prevent issues with switching between 
    // configurations that have different parameter counts.
    int  getNumParametersForHost();

    void reset();

    void addParameter(int index, ValueTree parameterDefinition, BCMParameter::ParameterType parameterType, bool fixedParameter = false);
    void setupHostParameters();

    BCMParameter* getParameterByName(const String& name);
    float getParameterHostValue(int hostIdx);
    void  setParameterFromHost(int hostIdx, float newValue);
    void  setParameterFromGUI(BCMParameter& parameter, float newValue);
    void  getParameterNameForHost(int hostIdx, String& parameterName);
    void  getParameterText(int hostIdx, String& parameterText);
    void  resetScopeCodeIndexes();
    void  snapshot();

    int  getOSCUID();
	void setOSCUID(int uid);
	void initOSCUID();
	void referToOSCUID(Value& valueToLink) { valueToLink.referTo(oscUID); }
	void updateHost(int hostIdx, float newValue);

    void toggleAsyncUpdates(bool enable) { shouldReceiveAsyncUpdates = enable; }
    
    void beginParameterChangeGesture(int hostIdx);
    void endParameterChangeGesture(int hostIdx);
    void endAllParameterChangeGestures();

    void receiveUpdatesFromScopeAsync();
    
    void         storeParameterValues();
    void         storeParameterValues(XmlElement& parameterValues);
    void         restoreParameterValues();
    XmlElement&  getParameterValueStore() { return parameterValueStore; };
    
    void timerCallback();

private:

	void addToParamIdxByScopeCodeId(BCMParameter* parameter, int index);

    OwnedArray<BCMParameter>   fixedParameters;
    OwnedArray<BCMParameter>   dynamicParameters;
    Array<BCMParameter*>       hostParameters;
    Array<BCMParameter*>       parameters;

    Array<int>                 paramIdxByScopeCodeId;  // Index of parameters by their scopeCodeId
    Value oscUID; // Unique OSC ID for the current instance
    XmlElement                 parameterValueStore;
    
    bool shouldReceiveAsyncUpdates;
    
    HashMap<int, int, DefaultHashFunctions, CriticalSection> asyncControlUpdates;    // Updates received from the ScopeFX async input to be passed on to the ScopeSync system
    
    ScopeSync* scopeSync;
    
    BigInteger changingParams;
    
    static const int minHostParameters;       // Minimum parameter count to return to host
    static const int maxHostParameters;       // Minimum parameter count to return to host
    static const int timerFrequency;
};

#endif  // BCMPARAMETERCONTROLLER_H_INCLUDED

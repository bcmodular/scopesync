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
	static int  getNumParametersForHost();

    void reset();

    void addParameter(ValueTree parameterDefinition, bool fixedParameter = false, bool oscAble = true);
    void addFixedScopeParameter(const String& scopeCode);
    void setupHostParameters();

	BCMParameter* getParameterByName(const String& name) const;
    BCMParameter* getParameterByScopeCode(const String& scopeCode) const;

    float getParameterHostValue(int hostIdx) const;
    void  setParameterFromHost(int hostIdx, float newValue) const;
	static void  setParameterFromGUI(BCMParameter& parameter, float newValue);
    void  getParameterNameForHost(int hostIdx, String& parameterName) const;
    void  getParameterText(int hostIdx, String& parameterText) const;
    void  resetScopeCodeIndexes();
    void  snapshot() const;

	void initOSCUID() const;
    void referToOSCUID(Value& valueToLink) const;
    int  getOSCUID() const;

	void updateHost(int hostIdx, float newValue) const;

    void toggleAsyncUpdates(bool enable) { shouldReceiveAsyncUpdates = enable; }
    
    void beginParameterChangeGesture(int hostIdx);
    void endParameterChangeGesture(int hostIdx);
    void endAllParameterChangeGestures();

    void receiveUpdatesFromScopeAsync();
    
    void         storeParameterValues();
    void         storeParameterValues(XmlElement& parameterValues);
    void         restoreParameterValues() const;
    XmlElement&  getParameterValueStore() { return parameterValueStore; };
    
    void timerCallback() override;

private:

	void addToParametersByScopeCodeId(BCMParameter* parameter, int scopeCodeId);

    OwnedArray<BCMParameter>    fixedParameters;
    OwnedArray<BCMParameter>    dynamicParameters;
    Array<BCMParameter*>        hostParameters;
    Array<BCMParameter*>        parameters;

    HashMap<int, BCMParameter*> parametersByScopeCodeId;  // Index of parameters by their scopeCodeId
    XmlElement                  parameterValueStore;
    
    bool shouldReceiveAsyncUpdates;
    
    HashMap<int, int, DefaultHashFunctions, CriticalSection> asyncControlUpdates;    // Updates received from the ScopeFX async input to be passed on to the ScopeSync system
    
    ScopeSync* scopeSync;
    
    BigInteger changingParams;
    
    static const int minHostParameters;       // Minimum parameter count to return to host
    static const int maxHostParameters;       // Minimum parameter count to return to host
    static const int timerFrequency;
};

#endif  // BCMPARAMETERCONTROLLER_H_INCLUDED

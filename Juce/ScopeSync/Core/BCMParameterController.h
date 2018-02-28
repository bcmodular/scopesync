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

class BCMParameterController
{
public:

    BCMParameterController(ScopeSync* owner);
    
	/* ====================== Public Parameter Methods ======================= */
    // Returns the number of parameters to inform the host about. Actually returns
    // the "minHostParameters" value to prevent issues with switching between 
    // configurations that have different parameter counts.
	static int  getNumParametersForHost();

    void reset();

    void addParameter(ValueTree parameterDefinition, bool fixedParameter = false);
    void addFixedScopeParameter(const String& name, int scopeParamId);
    void setupHostParameters();

	ScopeSync* getScopeSync() {return scopeSync;}

	BCMParameter* getParameterByName(StringRef name) const;
	
    float getParameterHostValue(int hostIdx) const;
    void  setParameterFromHost(int hostIdx, float newValue) const;
	static void  setParameterFromGUI(BCMParameter& parameter, float newValue);
    void  getParameterNameForHost(int hostIdx, String& parameterName) const;
    void  getParameterText(int hostIdx, String& parameterText) const;
    void  snapshot() const;

	void updateHost(int hostIdx, float newValue) const;

    void beginParameterChangeGesture(int hostIdx);
    void endParameterChangeGesture(int hostIdx);
    void endAllParameterChangeGestures();
    
    void         storeParameterValues();
    void         storeParameterValues(XmlElement& parameterValues);
    void         restoreParameterValues() const;
    XmlElement&  getParameterValueStore() { return parameterValueStore; };
    
private:

	OwnedArray<BCMParameter>    fixedParameters;
    OwnedArray<BCMParameter>    dynamicParameters;
    Array<BCMParameter*>        hostParameters;
    Array<BCMParameter*>        parameters;

    HashMap<String, BCMParameter*> parametersByName;
    XmlElement                     parameterValueStore;
    
    ScopeSync* scopeSync;
    
    BigInteger changingParams;
    
    static const int hostParameterCount;       // Parameter count to return to host
};

#endif  // BCMPARAMETERCONTROLLER_H_INCLUDED

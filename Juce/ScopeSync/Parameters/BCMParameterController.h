/**
 * Sets up all parameters and then provides access to them for
 * other objects as necessary
 *
 *  (C) Copyright 2014 bcmodular (http://www.bcmodular.co.uk/)
 *
 * This file is part of ScopeSync.
 *
 * ScopeSync is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef BCMPARAMETERCONTROLLER_H_INCLUDED
#define BCMPARAMETERCONTROLLER_H_INCLUDED

#include <JuceHeader.h>
#include "BCMParameter.h"
#include "../Core/ScopeSync.h"
#include "HostParameter.h"

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
	
    static void  setParameterFromGUI(BCMParameter& parameter, float newValue);
    void  snapshot() const;

    void         storeParameterValues();
    void         storeParameterValues(XmlElement& parameterValues);
    void         restoreParameterValues() const;
    XmlElement&  getParameterValueStore() { return parameterValueStore; };
    
private:

	OwnedArray<BCMParameter>    fixedParameters;
    OwnedArray<BCMParameter>    dynamicParameters;
#ifndef __DLL_EFFECT__
    Array<HostParameter*>       hostParameters;
#endif // __DLL_EFFECT__
    Array<BCMParameter*>        parameters;

    HashMap<String, BCMParameter*> parametersByName;
    XmlElement                     parameterValueStore;
    
    ScopeSync* scopeSync;
};

#endif  // BCMPARAMETERCONTROLLER_H_INCLUDED

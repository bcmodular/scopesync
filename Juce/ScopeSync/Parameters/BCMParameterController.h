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

class BCMParameterController : public Timer
{
public:

    BCMParameterController(ScopeSync* owner);
    
	void initialise();
    void reset();

    void addParameter(ValueTree parameterDefinition, bool fixedParameter = false);
    void addFixedScopeParameter(const String& name, int scopeParamId);
    void setupHostParameters();

	ScopeSync* getScopeSync() {return scopeSync;}

	BCMParameter* getParameterByName(StringRef name) const;

	BCMParameter * getFixedParameterByName(StringRef name) const;
	
    static void  setParameterFromGUI(BCMParameter& parameter, float newValue);
    void  snapshot();

	void sendAllCurrentValues();
	void sendAllMinValues();
	void sendAllMaxValues();

    void         storeParameterValues();
    void         storeParameterValues(XmlElement& parameterValues);
    void         restoreParameterValues();
    XmlElement&  getParameterValueStore() { return parameterValueStore; };
    
private:

	OwnedArray<BCMParameter>    fixedParameters;
    OwnedArray<BCMParameter>    dynamicParameters;
    Array<BCMParameter*>        parameters;

    HashMap<String, BCMParameter*> parametersByName;
    XmlElement                     parameterValueStore;
    
    ScopeSync* scopeSync;

	enum SnapshotStage {sendMin, sendMax, sendCurrent};
	SnapshotStage currentStage;

	void timerCallback() override;
};

#endif  // BCMPARAMETERCONTROLLER_H_INCLUDED

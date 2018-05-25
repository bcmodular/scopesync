/**
* Models a Host (i.e. Plugin) parameter
*
*  (C) Copyright 2018 bcmodular (http://www.bcmodular.co.uk/)
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

#pragma once
#ifndef __DLL_EFFECT__
#include <JuceHeader.h>
#include "../Parameters/BCMParameter.h"

class HostParameter : public AudioProcessorParameter
{
public:
	HostParameter() {}
	~HostParameter() { masterReference.clear(); }

	void   setBCMParameter(BCMParameter* bcmP) { bcmParameter = bcmP; }

	// Pure virtuals for AudioProcessorParameter
	float  getValue() const override;
	void   setValue(float newValue) override;
	float  getDefaultValue() const override;
	String getName(int maximumStringLength) const override;
	String getLabel() const override;
	float  getValueForText(const String & text) const override;

	// Other overrides for AudioProcessorParameter
	int  getNumSteps() const override;
	bool isDiscrete() const override;
	String getText(float value, int);

private:
	JUCE_DECLARE_WEAK_REFERENCEABLE(HostParameter)

	WeakReference<BCMParameter> bcmParameter;

};
#endif // __DLL_EFFECT__

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

#include "HostParameter.h"
#ifndef __DLL_EFFECT__

float HostParameter::getValue() const
{
	if (bcmParameter != nullptr)
		return bcmParameter->getHostValue();

	return 0.0f;
}

void HostParameter::setValue(float newValue)
{
	if (bcmParameter != nullptr)
		bcmParameter->setHostValue(newValue);
}

float HostParameter::getDefaultValue() const
{
	if (bcmParameter != nullptr)
		return bcmParameter->getDefaultHostValue();
	
	return 0.0f;
}

String HostParameter::getName(int maximumStringLength) const
{
	// TODO: need to do something with the maximumStringLength
	if (bcmParameter != nullptr)
	{
		String shortDesc;
		String fullDesc;
		bcmParameter->getDescriptions(shortDesc, fullDesc);

		return fullDesc;
	}
	
	//return String();
	return "Dummy Param";
}

String HostParameter::getLabel() const
{
	if (bcmParameter != nullptr)
		return bcmParameter->getUISuffix();

	return String();
}

float HostParameter::getValueForText(const String & text) const
{
	if (bcmParameter != nullptr)
		return text.getFloatValue();

	return 0.0f;
}

int HostParameter::getNumSteps() const
{
	if (bcmParameter != nullptr && bcmParameter->isDiscrete())
		return bcmParameter->getNumSettings();

	return AudioProcessor::getDefaultNumParameterSteps();
}

bool HostParameter::isDiscrete() const
{
	if (bcmParameter != nullptr)
		return bcmParameter->isDiscrete();

	return false;
}

#endif // __DLL_EFFECT__

/**
 *  Utility classes for holding Scope IDs
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

#include "ScopeIDs.h"

ScopeOSCParamID::ScopeOSCParamID(int pg, int pId) 
	: paramGroup(pg), paramId(pId)
{}

ScopeCodeMapper::ScopeCodeMapper()
{
	// Don't tab these out, or it breaks the StringArray!
	const StringArray scopeCodes = StringArray::fromTokens(
"A1,A2,A3,A4,A5,A6,A7,A8,\
B1,B2,B3,B4,B5,B6,B7,B8,\
C1,C2,C3,C4,C5,C6,C7,C8,\
D1,D2,D3,D4,D5,D6,D7,D8,\
E1,E2,E3,E4,E5,E6,E7,E8,\
F1,F2,F3,F4,F5,F6,F7,F8,\
G1,G2,G3,G4,G5,G6,G7,G8,\
H1,H2,H3,H4,H5,H6,H7,H8,\
I1,I2,I3,I4,I5,I6,I7,I8,\
J1,J2,J3,J4,J5,J6,J7,J8,\
K1,K2,K3,K4,K5,K6,K7,K8,\
L1,L2,L3,L4,L5,L6,L7,L8,\
M1,M2,M3,M4,M5,M6,M7,M8,\
N1,N2,N3,N4,N5,N6,N7,N8,\
O1,O2,O3,O4,O5,O6,O7,O8,\
P1,P2,P3,P4,P5,P6,P7,P8",
",",""
);

	int paramGroup = 1;
	int paramId    = 1;

	for (const auto& scopeCode : scopeCodes)
	{
		ScopeOSCParamID* scopeID = new ScopeOSCParamID(paramGroup, paramId);
		scopeOSCParamIDsByCode.set(scopeCode, scopeID);
		
		if (paramId == 16)
		{
			++paramGroup;
			paramId = 1;
		}
		else
			++paramId;

		DBG("ScopeCodeMapper::ScopeCodeMapper - created mapping for scopeCode: " + scopeCode + ", to: " + String(scopeID->paramGroup) + ":" + String(scopeID->paramId));
	}
}

ScopeOSCParamID ScopeCodeMapper::getScopeOSCParamIDByCode(StringRef code) const
{
	return *scopeOSCParamIDsByCode[code];
}

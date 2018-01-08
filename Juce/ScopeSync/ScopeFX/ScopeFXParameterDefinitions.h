/**
 * Description of the scopefx input and output parameters, used
 * by the Scope "Effect" class for communication between Scope
 * applications and the ScopeSync DLL.
 *
 *  (C) Copyright 2014 bcmodular (http://www.bcmodular.co.uk/)
 *
 * This file is part of ScopeSync.
 *
 * ScopeSync is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#ifndef SCOPEFXPARAMETERDEFINITIONS_H_INCLUDED
#define SCOPEFXPARAMETERDEFINITIONS_H_INCLUDED
#include <effclass.h>

namespace ScopeFXParameterDefinitions {

    /** the async input numbers*/
    enum asyncInPads {
		INPAD_OSCUID,
		NUM_ASYNCINPADS
    };

    /** so far no sync inputs used*/
    enum syncInPads {
        NUM_SYNCINPADS
    };

    /** each control that should be connected to the sdk circuit needs an async outpad*/
    enum asyncOutPads {
        OUTPAD_OSCUID,
        OUTPAD_SNAPSHOT,
		OUTPAD_LOADED,
		NUM_ASYNCOUTPADS
    };

    enum syncOutPads {
        NUM_SYNCOUTPADS
    };

   //-------------------------- Pad Definitions -------------------------------

    static PadType inputPadTypes[NUM_ASYNCINPADS + NUM_SYNCINPADS] = {
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },                        // INPAD_OSCUID
	};

    static PadType outputPadTypes[NUM_ASYNCOUTPADS + NUM_SYNCOUTPADS] = {
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_OSCUID
		{ DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_SNAPSHOT
		{ DTYPE_INT, 0, FRAC_MAX },         // OUTPAD_LOADED
	};

    //--------------------------------------------------------------------------
    static nameDesc inputNameDescs[NUM_ASYNCINPADS + NUM_SYNCINPADS] = {
        { "osc",  "osc uid" },
	};

    static nameDesc outputNameDescs[NUM_ASYNCOUTPADS + NUM_SYNCOUTPADS] = {
        { "osc",  "osc uid" },
        { "snap", "snapshot" },
		{ "load", "ScopeSync Loaded" },
};

	//--------------------------------------------------------------------------
    static EffectDescription effectDescription = {

        NUM_ASYNCINPADS,   // int32    asyncinPads;
        NUM_SYNCINPADS,    // int32    syncinPads;
        // 
        NUM_ASYNCOUTPADS,  // int32    asyncoutPads;
        NUM_SYNCOUTPADS,   // int32    syncoutPads;
        // 
        inputPadTypes,     // PadType *typeOfInPad;
        outputPadTypes,    // PadType *typeOfOutPad;
        // 
        MODFLAG_HASASYNC | MODFLAG_SINGLE,  // int32    flags;
        "ScopeSync 1.0.3", // char    *shortName;
        "ScopeSync 1.0.3", // char    *longName;
        NULL,              // int32   *delay;      // internal delay of output pads relative to input
        //                 // this delay is not part of the effect but has to be
        //                 // compensated
        0,                 // int32    cpuUsage;   // some measure for cpu usage
        inputNameDescs,    // char   **inPadNames  // array of short and long names of inpads
        outputNameDescs,   // char   **outPadNames
    };
}

#endif  // SCOPEFXPARAMETERDEFINITIONS_H_INCLUDED

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

#ifndef SCOPEFXPARAMETERDEFINITIONS_H_INCLUDED
#define SCOPEFXPARAMETERDEFINITIONS_H_INCLUDED
#include <effclass.h>

namespace ScopeFXParameterDefinitions {

    /** the async input numbers*/
    enum asyncInPads {
		INPAD_DEVICE_INSTANCE,
		NUM_ASYNCINPADS
    };

    /** so far no sync inputs used*/
    enum syncInPads {
        NUM_SYNCINPADS
    };

    /** each control that should be connected to the sdk circuit needs an async outpad*/
    enum asyncOutPads {
        OUTPAD_DEVICE_INSTANCE,
        OUTPAD_SNAPSHOT,
		OUTPAD_PLUGIN_HOST_OCT1,
		OUTPAD_PLUGIN_HOST_OCT2,
		OUTPAD_PLUGIN_HOST_OCT3,
		OUTPAD_PLUGIN_HOST_OCT4,
		OUTPAD_PLUGIN_LISTENER_PORT,
		OUTPAD_SCOPESYNC_LISTENER_PORT,
		OUTPAD_SYNC_SCOPE,
		NUM_ASYNCOUTPADS
    };

    enum syncOutPads {
        NUM_SYNCOUTPADS
    };

   //-------------------------- Pad Definitions -------------------------------

    static PadType inputPadTypes[NUM_ASYNCINPADS + NUM_SYNCINPADS] = {
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },	// INPAD_DEVICE_INSTANCE
	};

    static PadType outputPadTypes[NUM_ASYNCOUTPADS + NUM_SYNCOUTPADS] = {
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_DEVICE_INSTANCE
		{ DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_SNAPSHOT
		{ DTYPE_INT, 0, 255 },  // OUTPAD_PLUGIN_HOST_OCT1
		{ DTYPE_INT, 0, 255 },  // OUTPAD_PLUGIN_HOST_OCT2
		{ DTYPE_INT, 0, 255 },  // OUTPAD_PLUGIN_HOST_OCT3
		{ DTYPE_INT, 0, 255 },  // OUTPAD_PLUGIN_HOST_OCT4
		{ DTYPE_INT, 1, 65535 },  // OUTPAD_PLUGIN_LISTENER_PORT
		{ DTYPE_INT, 1, 65535 },  // OUTPAD_SCOPESYNC_LISTENER_PORT
		{ DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_SYNC_SCOPE
	};

    //--------------------------------------------------------------------------
    static nameDesc inputNameDescs[NUM_ASYNCINPADS + NUM_SYNCINPADS] = {
		{ "Inst", "Device Instance" },
	};

    static nameDesc outputNameDescs[NUM_ASYNCOUTPADS + NUM_SYNCOUTPADS] = {
		{ "Inst", "Device Instance" },
		{ "snap", "snapshot" },
		{ "oct1", "plugin host oct1" },
		{ "oct2", "plugin host oct2" },
		{ "oct3", "plugin host oct3" },
		{ "oct4", "plugin host oct4" },
		{ "pprt", "plugin listener port" },
		{ "sprt", "scopesync listener port" },
		{ "sync", "sync Scope" },
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

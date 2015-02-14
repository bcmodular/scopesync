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

namespace ScopeFXParameterDefinitions {

    /** the async input numbers*/
    enum asyncInPads {
		INPAD_PARAMS,
        INPAD_LOCALS,
        INPAD_FEEDBACK,
        INPAD_X,
		INPAD_Y,
		INPAD_SHOW,
		INPAD_CONFIGUID,
		INPAD_OSCUID,
		INPAD_PERFORMANCE_MODE,
		NUM_ASYNCINPADS
    };

    /** so far no sync inputs used*/
    enum syncInPads {
        NUM_SYNCINPADS
    };

    /** each control that should be connected to the sdk circuit needs an async outpad*/
    enum asyncOutPads {
        OUTPAD_PARAM_1,   OUTPAD_PARAM_2,   OUTPAD_PARAM_3,   OUTPAD_PARAM_4,   OUTPAD_PARAM_5,   OUTPAD_PARAM_6,   OUTPAD_PARAM_7,   OUTPAD_PARAM_8,
        OUTPAD_PARAM_9,   OUTPAD_PARAM_10,  OUTPAD_PARAM_11,  OUTPAD_PARAM_12,  OUTPAD_PARAM_13,  OUTPAD_PARAM_14,  OUTPAD_PARAM_15,  OUTPAD_PARAM_16,
        OUTPAD_PARAM_17,  OUTPAD_PARAM_18,  OUTPAD_PARAM_19,  OUTPAD_PARAM_20,  OUTPAD_PARAM_21,  OUTPAD_PARAM_22,  OUTPAD_PARAM_23,  OUTPAD_PARAM_24,
        OUTPAD_PARAM_25,  OUTPAD_PARAM_26,  OUTPAD_PARAM_27,  OUTPAD_PARAM_28,  OUTPAD_PARAM_29,  OUTPAD_PARAM_30,  OUTPAD_PARAM_31,  OUTPAD_PARAM_32,
        OUTPAD_PARAM_33,  OUTPAD_PARAM_34,  OUTPAD_PARAM_35,  OUTPAD_PARAM_36,  OUTPAD_PARAM_37,  OUTPAD_PARAM_38,  OUTPAD_PARAM_39,  OUTPAD_PARAM_40,
        OUTPAD_PARAM_41,  OUTPAD_PARAM_42,  OUTPAD_PARAM_43,  OUTPAD_PARAM_44,  OUTPAD_PARAM_45,  OUTPAD_PARAM_46,  OUTPAD_PARAM_47,  OUTPAD_PARAM_48,
        OUTPAD_PARAM_49,  OUTPAD_PARAM_50,  OUTPAD_PARAM_51,  OUTPAD_PARAM_52,  OUTPAD_PARAM_53,  OUTPAD_PARAM_54,  OUTPAD_PARAM_55,  OUTPAD_PARAM_56,
        OUTPAD_PARAM_57,  OUTPAD_PARAM_58,  OUTPAD_PARAM_59,  OUTPAD_PARAM_60,  OUTPAD_PARAM_61,  OUTPAD_PARAM_62,  OUTPAD_PARAM_63,  OUTPAD_PARAM_64,
        OUTPAD_PARAM_65,  OUTPAD_PARAM_66,  OUTPAD_PARAM_67,  OUTPAD_PARAM_68,  OUTPAD_PARAM_69,  OUTPAD_PARAM_70,  OUTPAD_PARAM_71,  OUTPAD_PARAM_72,
        OUTPAD_PARAM_73,  OUTPAD_PARAM_74,  OUTPAD_PARAM_75,  OUTPAD_PARAM_76,  OUTPAD_PARAM_77,  OUTPAD_PARAM_78,  OUTPAD_PARAM_79,  OUTPAD_PARAM_80,
        OUTPAD_PARAM_81,  OUTPAD_PARAM_82,  OUTPAD_PARAM_83,  OUTPAD_PARAM_84,  OUTPAD_PARAM_85,  OUTPAD_PARAM_86,  OUTPAD_PARAM_87,  OUTPAD_PARAM_88,
        OUTPAD_PARAM_89,  OUTPAD_PARAM_90,  OUTPAD_PARAM_91,  OUTPAD_PARAM_92,  OUTPAD_PARAM_93,  OUTPAD_PARAM_94,  OUTPAD_PARAM_95,  OUTPAD_PARAM_96,
        OUTPAD_PARAM_97,  OUTPAD_PARAM_98,  OUTPAD_PARAM_99,  OUTPAD_PARAM_100, OUTPAD_PARAM_101, OUTPAD_PARAM_102, OUTPAD_PARAM_103, OUTPAD_PARAM_104,
        OUTPAD_PARAM_105, OUTPAD_PARAM_106, OUTPAD_PARAM_107, OUTPAD_PARAM_108, OUTPAD_PARAM_109, OUTPAD_PARAM_110, OUTPAD_PARAM_111, OUTPAD_PARAM_112,
        OUTPAD_PARAM_113, OUTPAD_PARAM_114, OUTPAD_PARAM_115, OUTPAD_PARAM_116, OUTPAD_PARAM_117, OUTPAD_PARAM_118, OUTPAD_PARAM_119, OUTPAD_PARAM_120,
        OUTPAD_PARAM_121, OUTPAD_PARAM_122, OUTPAD_PARAM_123, OUTPAD_PARAM_124, OUTPAD_PARAM_125, OUTPAD_PARAM_126, OUTPAD_PARAM_127, OUTPAD_PARAM_128,
        OUTPAD_LOCAL_1,   OUTPAD_LOCAL_2,   OUTPAD_LOCAL_3,   OUTPAD_LOCAL_4,   OUTPAD_LOCAL_5,   OUTPAD_LOCAL_6,   OUTPAD_LOCAL_7,   OUTPAD_LOCAL_8,
        OUTPAD_LOCAL_9,   OUTPAD_LOCAL_10,  OUTPAD_LOCAL_11,  OUTPAD_LOCAL_12,  OUTPAD_LOCAL_13,  OUTPAD_LOCAL_14,  OUTPAD_LOCAL_15,  OUTPAD_LOCAL_16,
        OUTPAD_X,
		OUTPAD_Y,
        OUTPAD_SHOW,
		OUTPAD_CONFIGUID,
		OUTPAD_OSCUID,
		OUTPAD_PERFORMANCE_MODE,
		NUM_ASYNCOUTPADS
    };

    enum syncOutPads {
        NUM_SYNCOUTPADS
    };

   //-------------------------- Pad Definitions -------------------------------

    static PadType inputPadTypes[NUM_ASYNCINPADS + NUM_SYNCINPADS] = {
		{ DTYPE_INT, FRAC_MIN, FRAC_MAX, 128 }, // INPAD_PARAMS
        { DTYPE_INT, FRAC_MIN, FRAC_MAX, 16 },  // INPAD_LOCALS
        { DTYPE_INT, FRAC_MIN, FRAC_MAX, 128 }, // INPAD_FEEDBACK
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },      // INPAD_X
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },      // INPAD_Y
		{ DTYPE_INT, 0, 1 },                    // INPAD_SHOW
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },      // INPAD_CONFIGUID
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },      // INPAD_OSCUID
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },      // INPAD_PERFORMANCE_MODE
    };

    static PadType outputPadTypes[NUM_ASYNCOUTPADS + NUM_SYNCOUTPADS] = {
		{ DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_1
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_2
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_3
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_4
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_5
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_6
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_7
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_8
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_9
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_10
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_11
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_12
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_13
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_14
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_15
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_16
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_17
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_18
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_19
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_20
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_21
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_22
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_23
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_24
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_25
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_26
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_27
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_28
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_29
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_30
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_31
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_32
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_33
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_34
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_35
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_36
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_37
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_38
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_39
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_40
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_41
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_42
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_43
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_44
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_45
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_46
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_47
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_48
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_49
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_50
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_51
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_52
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_53
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_54
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_55
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_56
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_57
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_58
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_59
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_60
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_61
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_62
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_63
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_64
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_65
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_66
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_67
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_68
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_69
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_70
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_71
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_72
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_73
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_74
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_75
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_76
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_77
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_78
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_79
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_80
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_81
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_82
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_83
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_84
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_85
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_86
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_87
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_88
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_89
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_90
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_91
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_92
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_93
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_94
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_95
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_96
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_97
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_98
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_99
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_100
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_101
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_102
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_103
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_104
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_105
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_106
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_107
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_108
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_109
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_110
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_111
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_112
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_113
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_114
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_115
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_116
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_117
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_118
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_119
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_120
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_121
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_122
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_123
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_124
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_125
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_126
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_127
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PARAM_128
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_LOCAL_1
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_LOCAL_2
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_LOCAL_3
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_LOCAL_4
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_LOCAL_5
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_LOCAL_6
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_LOCAL_7
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_LOCAL_8
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_LOCAL_9
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_LOCAL_10
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_LOCAL_11
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_LOCAL_12
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_LOCAL_13
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_LOCAL_14
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_LOCAL_15
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_LOCAL_16
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_X
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_Y
        { DTYPE_INT, 0, 1 },				// OUTPAD_SHOW
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_CONFIGUID
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_OSCUID
        { DTYPE_INT, FRAC_MIN, FRAC_MAX },  // OUTPAD_PERFORMANCE_MODE
    };

    //--------------------------------------------------------------------------
    static nameDesc inputNameDescs[NUM_ASYNCINPADS + NUM_SYNCINPADS] = {
		{ "PI",   "Parameter Inputs" },
        { "LI",   "Local Inputs" },
        { "FI",   "Feedback Inputs" },
        { "X",    "X position" },
        { "Y",    "Y position" },
		{ "show", "show window" },
        { "cfg",  "configuration uid" },
        { "osc",  "osc uid" },
        { "pm",   "performance mode" },
    };

    static nameDesc outputNameDescs[NUM_ASYNCOUTPADS + NUM_SYNCOUTPADS] = {
		{ "P1",   "Parameter Output 1" },
        { "P2",   "Parameter Output 2" },
        { "P3",   "Parameter Output 3" },
        { "P4",   "Parameter Output 4" },
        { "P5",   "Parameter Output 5" },
        { "P6",   "Parameter Output 6" },
        { "P7",   "Parameter Output 7" },
        { "P8",   "Parameter Output 8" },
        { "P9",   "Parameter Output 9" },
        { "P10",  "Parameter Output 10" },
        { "P11",  "Parameter Output 11" },
        { "P12",  "Parameter Output 12" },
        { "P13",  "Parameter Output 13" },
        { "P14",  "Parameter Output 14" },
        { "P15",  "Parameter Output 15" },
        { "P16",  "Parameter Output 16" },
        { "P17",  "Parameter Output 17" },
        { "P18",  "Parameter Output 18" },
        { "P19",  "Parameter Output 19" },
        { "P20",  "Parameter Output 20" },
        { "P21",  "Parameter Output 21" },
        { "P22",  "Parameter Output 22" },
        { "P23",  "Parameter Output 23" },
        { "P24",  "Parameter Output 24" },
        { "P25",  "Parameter Output 25" },
        { "P26",  "Parameter Output 26" },
        { "P27",  "Parameter Output 27" },
        { "P28",  "Parameter Output 28" },
        { "P29",  "Parameter Output 29" },
        { "P30",  "Parameter Output 30" },
        { "P31",  "Parameter Output 31" },
        { "P32",  "Parameter Output 32" },
        { "P33",  "Parameter Output 33" },
        { "P34",  "Parameter Output 34" },
        { "P35",  "Parameter Output 35" },
        { "P36",  "Parameter Output 36" },
        { "P37",  "Parameter Output 37" },
        { "P38",  "Parameter Output 38" },
        { "P39",  "Parameter Output 39" },
        { "P40",  "Parameter Output 40" },
        { "P41",  "Parameter Output 41" },
        { "P42",  "Parameter Output 42" },
        { "P43",  "Parameter Output 43" },
        { "P44",  "Parameter Output 44" },
        { "P45",  "Parameter Output 45" },
        { "P46",  "Parameter Output 46" },
        { "P47",  "Parameter Output 47" },
        { "P48",  "Parameter Output 48" },
        { "P49",  "Parameter Output 49" },
        { "P50",  "Parameter Output 50" },
        { "P51",  "Parameter Output 51" },
        { "P52",  "Parameter Output 52" },
        { "P53",  "Parameter Output 53" },
        { "P54",  "Parameter Output 54" },
        { "P55",  "Parameter Output 55" },
        { "P56",  "Parameter Output 56" },
        { "P57",  "Parameter Output 57" },
        { "P58",  "Parameter Output 58" },
        { "P59",  "Parameter Output 59" },
        { "P60",  "Parameter Output 60" },
        { "P61",  "Parameter Output 61" },
        { "P62",  "Parameter Output 62" },
        { "P63",  "Parameter Output 63" },
        { "P64",  "Parameter Output 64" },
        { "P65",  "Parameter Output 65" },
        { "P66",  "Parameter Output 66" },
        { "P67",  "Parameter Output 67" },
        { "P68",  "Parameter Output 68" },
        { "P69",  "Parameter Output 69" },
        { "P70",  "Parameter Output 70" },
        { "P71",  "Parameter Output 71" },
        { "P72",  "Parameter Output 72" },
        { "P73",  "Parameter Output 73" },
        { "P74",  "Parameter Output 74" },
        { "P75",  "Parameter Output 75" },
        { "P76",  "Parameter Output 76" },
        { "P77",  "Parameter Output 77" },
        { "P78",  "Parameter Output 78" },
        { "P79",  "Parameter Output 79" },
        { "P80",  "Parameter Output 80" },
        { "P81",  "Parameter Output 81" },
        { "P82",  "Parameter Output 82" },
        { "P83",  "Parameter Output 83" },
        { "P84",  "Parameter Output 84" },
        { "P85",  "Parameter Output 85" },
        { "P86",  "Parameter Output 86" },
        { "P87",  "Parameter Output 87" },
        { "P88",  "Parameter Output 88" },
        { "P89",  "Parameter Output 89" },
        { "P90",  "Parameter Output 90" },
        { "P91",  "Parameter Output 91" },
        { "P92",  "Parameter Output 92" },
        { "P93",  "Parameter Output 93" },
        { "P94",  "Parameter Output 94" },
        { "P95",  "Parameter Output 95" },
        { "P96",  "Parameter Output 96" },
        { "P97",  "Parameter Output 97" },
        { "P98",  "Parameter Output 98" },
        { "P99",  "Parameter Output 99" },
        { "P100", "Parameter Output 100" },
        { "P101", "Parameter Output 101" },
        { "P102", "Parameter Output 102" },
        { "P103", "Parameter Output 103" },
        { "P104", "Parameter Output 104" },
        { "P105", "Parameter Output 105" },
        { "P106", "Parameter Output 106" },
        { "P107", "Parameter Output 107" },
        { "P108", "Parameter Output 108" },
        { "P109", "Parameter Output 109" },
        { "P110", "Parameter Output 110" },
        { "P111", "Parameter Output 111" },
        { "P112", "Parameter Output 112" },
        { "P113", "Parameter Output 113" },
        { "P114", "Parameter Output 114" },
        { "P115", "Parameter Output 115" },
        { "P116", "Parameter Output 116" },
        { "P117", "Parameter Output 117" },
        { "P118", "Parameter Output 118" },
        { "P119", "Parameter Output 119" },
        { "P120", "Parameter Output 120" },
        { "P121", "Parameter Output 121" },
        { "P122", "Parameter Output 122" },
        { "P123", "Parameter Output 123" },
        { "P124", "Parameter Output 124" },
        { "P125", "Parameter Output 125" },
        { "P126", "Parameter Output 126" },
        { "P127", "Parameter Output 127" },
        { "P128", "Parameter Output 128" },
        { "L1",   "Local Output 1" },
        { "L2",   "Local Output 2" },
        { "L3",   "Local Output 3" },
        { "L4",   "Local Output 4" },
        { "L5",   "Local Output 5" },
        { "L6",   "Local Output 6" },
        { "L7",   "Local Output 7" },
        { "L8",   "Local Output 8" },
        { "L9",   "Local Output 9" },
        { "L10",  "Local Output 10" },
        { "L11",  "Local Output 11" },
        { "L12",  "Local Output 12" },
        { "L13",  "Local Output 13" },
        { "L14",  "Local Output 14" },
        { "L15",  "Local Output 15" },
        { "L16",  "Local Output 16" },
        { "X",    "X position" },
        { "Y",    "Y position" },
        { "show", "show window" },
        { "cfg",  "configuration uid" },
        { "pm",   "performance mode" },
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
        MODFLAG_HASASYNC,  // int32    flags;
        "ScopeSync 1.0.2", // char    *shortName;
        "ScopeSync 1.0.2", // char    *longName;
        NULL,              // int32   *delay;      // internal delay of output pads relative to input
        //                 // this delay is not part of the effect but has to be
        //                 // compensated
        0,                 // int32    cpuUsage;   // some measure for cpu usage
        inputNameDescs,    // char   **inPadNames  // array of short and long names of inpads
        outputNameDescs,   // char   **outPadNames
    };
}

#endif  // SCOPEFXPARAMETERDEFINITIONS_H_INCLUDED

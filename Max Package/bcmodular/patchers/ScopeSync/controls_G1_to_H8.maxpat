{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 6,
			"minor" : 1,
			"revision" : 7,
			"architecture" : "x86"
		}
,
		"rect" : [ 34.0, 88.0, 694.0, 642.0 ],
		"bglocked" : 0,
		"openinpresentation" : 1,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 0,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 0,
		"statusbarvisible" : 0,
		"toolbarvisible" : 1,
		"boxanimatetime" : 200,
		"imprint" : 0,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"description" : "",
		"digest" : "",
		"tags" : "",
		"boxes" : [ 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-6",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 58.0, 321.5, 94.0, 18.0 ],
					"text" : "r ---scope_update",
					"varname" : "u437001377[2]"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-10",
					"maxclass" : "newobj",
					"numinlets" : 9,
					"numoutlets" : 9,
					"outlettype" : [ "", "", "", "", "", "", "", "", "" ],
					"patching_rect" : [ 58.0, 356.5, 469.75, 18.0 ],
					"text" : "route H1 H2 H3 H4 H5 H6 H7 H8",
					"varname" : "u437001377[3]"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-2",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 121.821411, 28.5, 94.0, 18.0 ],
					"text" : "r ---scope_update",
					"varname" : "u437001377[1]"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-4",
					"maxclass" : "newobj",
					"numinlets" : 9,
					"numoutlets" : 9,
					"outlettype" : [ "", "", "", "", "", "", "", "", "" ],
					"patching_rect" : [ 121.821411, 63.5, 459.928589, 18.0 ],
					"text" : "route G1 G2 G3 G4 G5 G6 G7 G8",
					"varname" : "u437001377"
				}

			}
, 			{
				"box" : 				{
					"color" : [ 0.415686, 0.239216, 0.109804, 1.0 ],
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-7",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 256.285706, 589.0, 107.0, 18.0 ],
					"text" : "s ---queue_message"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-46",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 441.75, 546.0, 37.0, 16.0 ],
					"text" : "H8 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-48",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 386.928589, 546.0, 37.0, 16.0 ],
					"text" : "H7 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-49",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 332.107178, 546.0, 37.0, 16.0 ],
					"text" : "H6 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-50",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 277.285706, 546.0, 37.0, 16.0 ],
					"text" : "H5 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-51",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 222.464294, 546.0, 37.0, 16.0 ],
					"text" : "H4 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-52",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 167.642822, 546.0, 37.0, 16.0 ],
					"text" : "H3 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-53",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 112.821411, 546.0, 37.0, 16.0 ],
					"text" : "H2 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-54",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 58.0, 546.0, 37.0, 16.0 ],
					"text" : "H1 $1"
				}

			}
, 			{
				"box" : 				{
					"color" : [ 0.415686, 0.239216, 0.109804, 1.0 ],
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-9",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 300.0, 285.0, 107.0, 18.0 ],
					"text" : "s ---queue_message"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-55",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 489.75, 243.0, 38.0, 16.0 ],
					"text" : "G8 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-56",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 434.928589, 243.0, 38.0, 16.0 ],
					"text" : "G7 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-57",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 380.107178, 243.0, 38.0, 16.0 ],
					"text" : "G6 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-58",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 325.285706, 243.0, 38.0, 16.0 ],
					"text" : "G5 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-59",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 270.464294, 243.0, 38.0, 16.0 ],
					"text" : "G4 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-60",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 215.642822, 243.0, 38.0, 16.0 ],
					"text" : "G3 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-61",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 160.821411, 243.0, 38.0, 16.0 ],
					"text" : "G2 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-62",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 106.0, 243.0, 38.0, 16.0 ],
					"text" : "G1 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-1",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 4,
					"outlettype" : [ "", "", "", "" ],
					"patching_rect" : [ 479.0, 14.0, 59.5, 18.0 ],
					"restore" : 					{
						"G1" : [ 0.0 ],
						"G2" : [ 0.0 ],
						"G3" : [ 0.0 ],
						"G4" : [ 0.0 ],
						"G5" : [ 0.0 ],
						"G6" : [ 0.0 ],
						"G7" : [ 0.0 ],
						"G8" : [ 0.0 ],
						"H1" : [ 0.0 ],
						"H2" : [ 0.0 ],
						"H3" : [ 0.0 ],
						"H4" : [ 0.0 ],
						"H5" : [ 0.0 ],
						"H6" : [ 0.0 ],
						"H7" : [ 0.0 ],
						"H8" : [ 0.0 ]
					}
,
					"text" : "autopattr",
					"varname" : "u424002320"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-3",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "bang", "bang" ],
					"patching_rect" : [ 27.0, 65.0, 67.0, 18.0 ],
					"text" : "bangbang 2"
				}

			}
, 			{
				"box" : 				{
					"comment" : "",
					"id" : "obj-5",
					"maxclass" : "inlet",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 27.0, 21.5, 25.0, 25.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-8",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 8,
					"outlettype" : [ "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang" ],
					"patching_rect" : [ 27.0, 395.5, 402.749878, 18.0 ],
					"text" : "bangbang 8"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-20",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 8,
					"outlettype" : [ "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang" ],
					"patching_rect" : [ 75.0, 104.0, 402.75, 18.0 ],
					"text" : "bangbang 8"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-82",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 410.75, 436.0, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 353.0, 83.0, 50.0, 80.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "H8",
							"parameter_shortname" : "H8",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 64
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "H8"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-83",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 355.928589, 436.0, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 303.0, 83.0, 50.0, 80.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "H7",
							"parameter_shortname" : "H7",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 63
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "H7"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-84",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 301.107178, 436.0, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 253.0, 83.0, 50.0, 80.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "H6",
							"parameter_shortname" : "H6",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 62
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "H6"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-85",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 246.285706, 436.0, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 203.0, 83.0, 50.0, 80.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "H5",
							"parameter_shortname" : "H5",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 61
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "H5"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-86",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 191.464294, 436.0, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 153.0, 83.0, 50.0, 80.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "H4",
							"parameter_shortname" : "H4",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 60
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "H4"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-87",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 136.642822, 436.0, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 103.0, 83.0, 50.0, 80.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "H3",
							"parameter_shortname" : "H3",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 59
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "H3"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-88",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 81.821411, 436.0, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 53.0, 83.0, 50.0, 80.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "H2",
							"parameter_shortname" : "H2",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 58
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "H2"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-89",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 27.0, 436.0, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 3.0, 83.0, 50.0, 80.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "H1",
							"parameter_shortname" : "H1",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 57
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "H1"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-90",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 458.75, 141.5, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 353.0, 3.0, 50.0, 80.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "G8",
							"parameter_shortname" : "G8",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 56
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "G8"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-91",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 403.928589, 141.5, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 303.0, 3.0, 50.0, 80.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "G7",
							"parameter_shortname" : "G7",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 55
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "G7"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-92",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 349.107178, 141.5, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 253.0, 3.0, 50.0, 80.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "G6",
							"parameter_shortname" : "G6",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 54
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "G6"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-93",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 294.285706, 141.5, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 203.0, 3.0, 50.0, 80.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "G5",
							"parameter_shortname" : "G5",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 53
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "G5"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-94",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 239.464294, 141.5, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 153.0, 3.0, 50.0, 80.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "G4",
							"parameter_shortname" : "G4",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 52
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "G4"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-95",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 184.642822, 141.5, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 103.0, 3.0, 50.0, 80.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "G3",
							"parameter_shortname" : "G3",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 51
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "G3"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-96",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 129.821411, 141.5, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 53.0, 3.0, 50.0, 80.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "G2",
							"parameter_shortname" : "G2",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 50
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "G2"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-97",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 75.0, 141.5, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 3.0, 3.0, 50.0, 80.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "G1",
							"parameter_shortname" : "G1",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 49
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "G1"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-82", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-10", 7 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-83", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-10", 6 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-84", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-10", 5 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-85", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-10", 4 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-86", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-10", 3 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-87", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-10", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-88", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-10", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-89", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-10", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-2", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-90", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 468.25, 123.0, 468.25, 123.0 ],
					"source" : [ "obj-20", 7 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-91", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 413.428558, 123.0, 413.428589, 123.0 ],
					"source" : [ "obj-20", 6 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-92", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 358.607147, 123.0, 358.607178, 123.0 ],
					"source" : [ "obj-20", 5 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-93", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 303.785706, 123.0, 303.785706, 123.0 ],
					"source" : [ "obj-20", 4 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-94", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 248.964279, 123.0, 248.964294, 123.0 ],
					"source" : [ "obj-20", 3 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-95", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 194.142853, 123.0, 194.142822, 123.0 ],
					"source" : [ "obj-20", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-96", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 139.321426, 123.0, 139.321411, 123.0 ],
					"source" : [ "obj-20", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-97", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 84.5, 123.0, 84.5, 123.0 ],
					"source" : [ "obj-20", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-20", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 84.5, 84.0, 84.5, 84.0 ],
					"source" : [ "obj-3", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-8", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 36.5, 84.0, 36.5, 84.0 ],
					"source" : [ "obj-3", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-90", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-4", 7 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-91", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-4", 6 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-92", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-4", 5 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-93", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-4", 4 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-94", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-4", 3 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-95", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-4", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-96", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-4", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-97", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-4", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-7", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 451.25, 576.0, 265.785706, 576.0 ],
					"source" : [ "obj-46", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-7", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 396.428589, 576.0, 265.785706, 576.0 ],
					"source" : [ "obj-48", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-7", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 341.607178, 576.0, 265.785706, 576.0 ],
					"source" : [ "obj-49", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-3", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 36.5, 48.0, 36.5, 48.0 ],
					"source" : [ "obj-5", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-7", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 286.785706, 576.0, 265.785706, 576.0 ],
					"source" : [ "obj-50", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-7", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 231.964294, 576.0, 265.785706, 576.0 ],
					"source" : [ "obj-51", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-7", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 177.142822, 576.0, 265.785706, 576.0 ],
					"source" : [ "obj-52", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-7", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 122.321411, 576.0, 265.785706, 576.0 ],
					"source" : [ "obj-53", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-7", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 67.5, 576.0, 265.785706, 576.0 ],
					"source" : [ "obj-54", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-9", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 499.25, 270.0, 309.5, 270.0 ],
					"source" : [ "obj-55", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-9", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 444.428589, 270.0, 309.5, 270.0 ],
					"source" : [ "obj-56", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-9", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 389.607178, 270.0, 309.5, 270.0 ],
					"source" : [ "obj-57", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-9", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 334.785706, 270.0, 309.5, 270.0 ],
					"source" : [ "obj-58", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-9", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 279.964294, 270.0, 309.5, 270.0 ],
					"source" : [ "obj-59", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-10", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-6", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-9", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 225.142822, 270.0, 309.5, 270.0 ],
					"source" : [ "obj-60", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-9", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 170.321411, 270.0, 309.5, 270.0 ],
					"source" : [ "obj-61", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-9", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 115.5, 270.0, 309.5, 270.0 ],
					"source" : [ "obj-62", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-82", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 420.249878, 414.0, 420.25, 414.0 ],
					"source" : [ "obj-8", 7 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-83", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 365.428467, 414.0, 365.428589, 414.0 ],
					"source" : [ "obj-8", 6 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-84", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 310.607056, 414.0, 310.607178, 414.0 ],
					"source" : [ "obj-8", 5 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-85", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 255.785645, 414.0, 255.785706, 414.0 ],
					"source" : [ "obj-8", 4 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-86", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 200.964233, 414.0, 200.964294, 414.0 ],
					"source" : [ "obj-8", 3 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-87", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 146.142822, 414.0, 146.142822, 414.0 ],
					"source" : [ "obj-8", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-88", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 91.321411, 414.0, 91.321411, 414.0 ],
					"source" : [ "obj-8", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-89", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 36.5, 414.0, 36.5, 414.0 ],
					"source" : [ "obj-8", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-46", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 451.25, 516.0, 451.25, 516.0 ],
					"source" : [ "obj-82", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-48", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 396.428589, 516.0, 396.428589, 516.0 ],
					"source" : [ "obj-83", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-49", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 341.607178, 516.0, 341.607178, 516.0 ],
					"source" : [ "obj-84", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-50", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 286.785706, 516.0, 286.785706, 516.0 ],
					"source" : [ "obj-85", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-51", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 231.964294, 516.0, 231.964294, 516.0 ],
					"source" : [ "obj-86", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-52", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 177.142822, 516.0, 177.142822, 516.0 ],
					"source" : [ "obj-87", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-53", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 122.321411, 516.0, 122.321411, 516.0 ],
					"source" : [ "obj-88", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-54", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 67.5, 516.0, 67.5, 516.0 ],
					"source" : [ "obj-89", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-55", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 499.25, 222.0, 499.25, 222.0 ],
					"source" : [ "obj-90", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-56", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 444.428589, 222.0, 444.428589, 222.0 ],
					"source" : [ "obj-91", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-57", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 389.607178, 222.0, 389.607178, 222.0 ],
					"source" : [ "obj-92", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-58", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 334.785706, 222.0, 334.785706, 222.0 ],
					"source" : [ "obj-93", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-59", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 279.964294, 222.0, 279.964294, 222.0 ],
					"source" : [ "obj-94", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-60", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 225.142822, 222.0, 225.142822, 222.0 ],
					"source" : [ "obj-95", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-61", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 170.321411, 222.0, 170.321411, 222.0 ],
					"source" : [ "obj-96", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-62", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 115.5, 222.0, 115.5, 222.0 ],
					"source" : [ "obj-97", 1 ]
				}

			}
 ],
		"parameters" : 		{
			"obj-97" : [ "G1", "G1", 0 ],
			"obj-92" : [ "G6", "G6", 0 ],
			"obj-87" : [ "H3", "H3", 0 ],
			"obj-82" : [ "H8", "H8", 0 ],
			"obj-96" : [ "G2", "G2", 0 ],
			"obj-91" : [ "G7", "G7", 0 ],
			"obj-86" : [ "H4", "H4", 0 ],
			"obj-95" : [ "G3", "G3", 0 ],
			"obj-90" : [ "G8", "G8", 0 ],
			"obj-85" : [ "H5", "H5", 0 ],
			"obj-94" : [ "G4", "G4", 0 ],
			"obj-89" : [ "H1", "H1", 0 ],
			"obj-84" : [ "H6", "H6", 0 ],
			"obj-93" : [ "G5", "G5", 0 ],
			"obj-88" : [ "H2", "H2", 0 ],
			"obj-83" : [ "H7", "H7", 0 ]
		}
,
		"dependency_cache" : [  ]
	}

}

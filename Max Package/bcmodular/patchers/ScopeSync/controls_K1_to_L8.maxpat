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
		"rect" : [ 34.0, 88.0, 710.0, 833.0 ],
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
					"id" : "obj-3",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 80.0, 331.0, 94.0, 18.0 ],
					"text" : "r ---scope_update",
					"varname" : "u437001377[2]"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-7",
					"maxclass" : "newobj",
					"numinlets" : 9,
					"numoutlets" : 9,
					"outlettype" : [ "", "", "", "", "", "", "", "", "" ],
					"patching_rect" : [ 80.0, 371.0, 465.5, 18.0 ],
					"text" : "route L1 L2 L3 L4 L5 L6 L7 L8",
					"varname" : "u437001377[3]"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-4",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 135.714279, 55.0, 94.0, 18.0 ],
					"text" : "r ---scope_update",
					"varname" : "u437001377[1]"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-5",
					"maxclass" : "newobj",
					"numinlets" : 9,
					"numoutlets" : 9,
					"outlettype" : [ "", "", "", "", "", "", "", "", "" ],
					"patching_rect" : [ 135.714279, 91.0, 424.0, 18.0 ],
					"text" : "route K1 K2 K3 K4 K5 K6 K7 K8",
					"varname" : "u437001377[4]"
				}

			}
, 			{
				"box" : 				{
					"color" : [ 0.415686, 0.239216, 0.109804, 1.0 ],
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-138",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 275.428589, 614.0, 107.0, 18.0 ],
					"text" : "s ---queue_message"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-208",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 460.5, 565.5, 37.0, 16.0 ],
					"text" : "L8 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-209",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 406.142853, 565.5, 37.0, 16.0 ],
					"text" : "L7 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-210",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 351.785706, 565.5, 37.0, 16.0 ],
					"text" : "L6 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-211",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 297.428589, 565.5, 37.0, 16.0 ],
					"text" : "L5 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-212",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 243.071411, 565.5, 37.0, 16.0 ],
					"text" : "L4 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-213",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 188.714279, 565.5, 37.0, 16.0 ],
					"text" : "L3 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-214",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 134.0, 565.5, 37.0, 16.0 ],
					"text" : "L2 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-215",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 80.0, 565.5, 37.0, 16.0 ],
					"text" : "L1 $1"
				}

			}
, 			{
				"box" : 				{
					"color" : [ 0.415686, 0.239216, 0.109804, 1.0 ],
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-136",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 313.535706, 306.0, 107.0, 18.0 ],
					"text" : "s ---queue_message"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-180",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 508.5, 259.5, 37.0, 16.0 ],
					"text" : "K8 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-181",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 454.142853, 259.5, 37.0, 16.0 ],
					"text" : "K7 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-177",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 399.785706, 259.5, 37.0, 16.0 ],
					"text" : "K6 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-178",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 345.428589, 259.5, 37.0, 16.0 ],
					"text" : "K5 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-179",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 291.071411, 259.5, 37.0, 16.0 ],
					"text" : "K4 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-176",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 236.714279, 259.5, 37.0, 16.0 ],
					"text" : "K3 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-175",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 182.0, 259.5, 37.0, 16.0 ],
					"text" : "K2 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-173",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 128.0, 259.5, 37.0, 16.0 ],
					"text" : "K1 $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-2",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 4,
					"outlettype" : [ "", "", "", "" ],
					"patching_rect" : [ 519.25, 3.0, 56.0, 18.0 ],
					"restore" : 					{
						"K1" : [ 0.0 ],
						"K2" : [ 0.0 ],
						"K3" : [ 0.0 ],
						"K4" : [ 0.0 ],
						"K5" : [ 0.0 ],
						"K6" : [ 0.0 ],
						"K7" : [ 0.0 ],
						"K8" : [ 0.0 ],
						"L1" : [ 0.0 ],
						"L2" : [ 0.0 ],
						"L3" : [ 0.0 ],
						"L4" : [ 0.0 ],
						"L5" : [ 0.0 ],
						"L6" : [ 0.0 ],
						"L7" : [ 0.0 ],
						"L8" : [ 0.0 ]
					}
,
					"text" : "autopattr",
					"varname" : "u654002538"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-1",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "bang", "bang" ],
					"patching_rect" : [ 49.0, 73.0, 67.0, 18.0 ],
					"text" : "bangbang 2"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-134",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 8,
					"outlettype" : [ "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang" ],
					"patching_rect" : [ 49.0, 416.5, 399.5, 18.0 ],
					"text" : "bangbang 8"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Bold",
					"fontsize" : 10.0,
					"id" : "obj-133",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 8,
					"outlettype" : [ "bang", "bang", "bang", "bang", "bang", "bang", "bang", "bang" ],
					"patching_rect" : [ 97.0, 122.0, 399.5, 18.0 ],
					"text" : "bangbang 8"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-6",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 429.5, 453.5, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 353.0, 83.0, 50.0, 80.0 ],
					"prototypename" : "M4L.dial.tiny",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "L8",
							"parameter_shortname" : "L8",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 96
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "L8"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-23",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 375.142853, 453.5, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 303.0, 83.0, 50.0, 80.0 ],
					"prototypename" : "M4L.dial.tiny",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "L7",
							"parameter_shortname" : "L7",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 95
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "L7"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-24",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 320.785706, 453.5, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 253.0, 83.0, 50.0, 80.0 ],
					"prototypename" : "M4L.dial.tiny",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "L6",
							"parameter_shortname" : "L6",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 94
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "L6"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-25",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 266.428589, 453.5, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 203.0, 83.0, 50.0, 80.0 ],
					"prototypename" : "M4L.dial.tiny",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "L5",
							"parameter_shortname" : "L5",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 93
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "L5"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-26",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 212.071411, 453.5, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 153.0, 83.0, 50.0, 80.0 ],
					"prototypename" : "M4L.dial.tiny",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "L4",
							"parameter_shortname" : "L4",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 92
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "L4"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-27",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 157.714279, 453.5, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 103.0, 83.0, 50.0, 80.0 ],
					"prototypename" : "M4L.dial.tiny",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "L3",
							"parameter_shortname" : "L3",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 91
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "L3"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-28",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 103.0, 453.5, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 53.0, 83.0, 50.0, 80.0 ],
					"prototypename" : "M4L.dial.tiny",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "L2",
							"parameter_shortname" : "L2",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 90
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "L2"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-29",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 49.0, 453.5, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 3.0, 83.0, 50.0, 80.0 ],
					"prototypename" : "M4L.dial.tiny",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "L1",
							"parameter_shortname" : "L1",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 89
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "L1"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-16",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 477.5, 158.0, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 353.0, 3.0, 50.0, 80.0 ],
					"prototypename" : "M4L.dial.tiny",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "K8",
							"parameter_shortname" : "K8",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 88
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "K8"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-17",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 423.142853, 158.0, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 303.0, 3.0, 50.0, 80.0 ],
					"prototypename" : "M4L.dial.tiny",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "K7",
							"parameter_shortname" : "K7",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 87
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "K7"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-18",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 368.785706, 158.0, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 253.0, 3.0, 50.0, 80.0 ],
					"prototypename" : "M4L.dial.tiny",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "K6",
							"parameter_shortname" : "K6",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 86
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "K6"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-19",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 314.428589, 158.0, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 203.0, 3.0, 50.0, 80.0 ],
					"prototypename" : "M4L.dial.tiny",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "K5",
							"parameter_shortname" : "K5",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 85
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "K5"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-15",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 260.071411, 158.0, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 153.0, 3.0, 50.0, 80.0 ],
					"prototypename" : "M4L.dial.tiny",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "K4",
							"parameter_shortname" : "K4",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 84
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "K4"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-14",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 205.714279, 158.0, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 103.0, 3.0, 50.0, 80.0 ],
					"prototypename" : "M4L.dial.tiny",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "K3",
							"parameter_shortname" : "K3",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 83
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "K3"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-13",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 151.0, 158.0, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 53.0, 3.0, 50.0, 80.0 ],
					"prototypename" : "M4L.dial.tiny",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "K2",
							"parameter_shortname" : "K2",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 82
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "K2"
				}

			}
, 			{
				"box" : 				{
					"activeneedlecolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"appearance" : 2,
					"focusbordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-12",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"panelcolor" : [ 0.262745, 0.298039, 0.317647, 1.0 ],
					"parameter_enable" : 1,
					"patching_rect" : [ 97.0, 158.0, 50.0, 80.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 3.0, 3.0, 50.0, 80.0 ],
					"prototypename" : "M4L.dial.tiny",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_linknames" : 1,
							"parameter_longname" : "K1",
							"parameter_shortname" : "K1",
							"parameter_type" : 0,
							"parameter_mmax" : 100.0,
							"parameter_unitstyle" : 9,
							"parameter_units" : "%0.4f%",
							"parameter_speedlim" : 0.0,
							"parameter_mapping_index" : 81
						}

					}
,
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"varname" : "K1"
				}

			}
, 			{
				"box" : 				{
					"comment" : "",
					"id" : "obj-122",
					"maxclass" : "inlet",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 49.0, 20.0, 25.0, 25.0 ]
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-133", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-1", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-134", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-1", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-173", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-12", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-122", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-175", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-13", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-12", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-133", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-133", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-14", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-133", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-15", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-133", 3 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-16", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-133", 7 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-133", 6 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-18", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-133", 5 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-19", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-133", 4 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-23", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-134", 6 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-24", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-134", 5 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-25", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-134", 4 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-26", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-134", 3 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-27", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-134", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-28", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-134", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-29", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-134", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-6", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-134", 7 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-176", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-14", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-179", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-15", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-180", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-16", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-181", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-17", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-136", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 137.5, 293.0, 323.035706, 293.0 ],
					"source" : [ "obj-173", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-136", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 191.5, 293.0, 323.035706, 293.0 ],
					"source" : [ "obj-175", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-136", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 246.214279, 293.0, 323.035706, 293.0 ],
					"source" : [ "obj-176", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-136", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 409.285706, 293.0, 323.035706, 293.0 ],
					"source" : [ "obj-177", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-136", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 354.928589, 293.0, 323.035706, 293.0 ],
					"source" : [ "obj-178", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-136", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 300.571411, 293.0, 323.035706, 293.0 ],
					"source" : [ "obj-179", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-177", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-18", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-136", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 518.0, 293.0, 323.035706, 293.0 ],
					"source" : [ "obj-180", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-136", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 463.642853, 293.0, 323.035706, 293.0 ],
					"source" : [ "obj-181", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-178", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-19", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-138", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 470.0, 599.0, 284.928589, 599.0 ],
					"source" : [ "obj-208", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-138", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 415.642853, 599.0, 284.928589, 599.0 ],
					"source" : [ "obj-209", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-138", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 361.285706, 599.0, 284.928589, 599.0 ],
					"source" : [ "obj-210", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-138", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 306.928589, 599.0, 284.928589, 599.0 ],
					"source" : [ "obj-211", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-138", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 252.571411, 599.0, 284.928589, 599.0 ],
					"source" : [ "obj-212", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-138", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 198.214279, 599.0, 284.928589, 599.0 ],
					"source" : [ "obj-213", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-138", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 143.5, 599.0, 284.928589, 599.0 ],
					"source" : [ "obj-214", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-138", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"midpoints" : [ 89.5, 599.0, 284.928589, 599.0 ],
					"source" : [ "obj-215", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-209", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-23", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-210", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-24", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-211", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-25", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-212", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-26", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-213", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-27", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-214", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-28", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-215", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-29", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-7", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-3", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-5", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-4", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-12", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-5", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-5", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-14", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-5", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-15", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-5", 3 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-16", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-5", 7 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-5", 6 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-18", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-5", 5 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-19", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-5", 4 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-208", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-6", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-23", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-7", 6 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-24", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-7", 5 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-25", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-7", 4 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-26", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-7", 3 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-27", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-7", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-28", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-7", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-29", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-7", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-6", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-7", 7 ]
				}

			}
 ],
		"parameters" : 		{
			"obj-19" : [ "K5", "K5", 0 ],
			"obj-28" : [ "L2", "L2", 0 ],
			"obj-23" : [ "L7", "L7", 0 ],
			"obj-12" : [ "K1", "K1", 0 ],
			"obj-18" : [ "K6", "K6", 0 ],
			"obj-27" : [ "L3", "L3", 0 ],
			"obj-6" : [ "L8", "L8", 0 ],
			"obj-13" : [ "K2", "K2", 0 ],
			"obj-17" : [ "K7", "K7", 0 ],
			"obj-26" : [ "L4", "L4", 0 ],
			"obj-14" : [ "K3", "K3", 0 ],
			"obj-16" : [ "K8", "K8", 0 ],
			"obj-25" : [ "L5", "L5", 0 ],
			"obj-15" : [ "K4", "K4", 0 ],
			"obj-29" : [ "L1", "L1", 0 ],
			"obj-24" : [ "L6", "L6", 0 ]
		}
,
		"dependency_cache" : [  ]
	}

}

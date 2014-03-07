/**
 * ScopeSync mxj~ Control class
 *
 *  (C) Copyright ${year} bcmodular (http://www.bcmodular.co.uk/)
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
package uk.co.bcmodular.scopesync;
import java.util.*;

public class Control {
	public int     control_id;   // Identifier (0-127) for the Control
	public float   dest_value;   // Value to be put into the audio stream to identify the Control
	public float   last_data_value_sent = 0.0f; // The data value previously sent in the audio stream
	public float   data_value           = 0.0f; // The current value of the Control
	public boolean snapshot             = false;// Indicates whether a snapshot has been requested. Used to force re-send of all Control values
	static public  HashMap<String,int[]> controlDestCodeMap = new HashMap<String,int[]>(128); // Mapping between the input String and the destination code (1-144)

	// Set up the dest to port mapping
	static {
		final String[] controls = {"A1","A2","A3","A4","A5","A6","A7","A8",
				                   "B1","B2","B3","B4","B5","B6","B7","B8",
				                   "C1","C2","C3","C4","C5","C6","C7","C8",
				                   "D1","D2","D3","D4","D5","D6","D7","D8",
				                   "E1","E2","E3","E4","E5","E6","E7","E8",
				                   "F1","F2","F3","F4","F5","F6","F7","F8",
				                   "G1","G2","G3","G4","G5","G6","G7","G8",
				                   "H1","H2","H3","H4","H5","H6","H7","H8",
				                   "I1","I2","I3","I4","I5","I6","I7","I8",
				                   "J1","J2","J3","J4","J5","J6","J7","J8",
				                   "K1","K2","K3","K4","K5","K6","K7","K8",
				                   "L1","L2","L3","L4","L5","L6","L7","L8",
				                   "M1","M2","M3","M4","M5","M6","M7","M8",
				                   "N1","N2","N3","N4","N5","N6","N7","N8",
				                   "O1","O2","O3","O4","O5","O6","O7","O8",
				                   "P1","P2","P3","P4","P5","P6","P7","P8"};
		final int[]  destCodes = {  1,  2,  3,  4,  5,  6,  7,  8,
				                    9, 10, 11, 12, 13, 14, 15, 16,
				                   19, 20, 21, 22, 23, 24, 25, 26,
				                   27, 28, 29, 30, 31, 32, 33, 34,
				                   37, 38, 39, 40, 41, 42, 43, 44,
				                   45, 46, 47, 48, 49, 50, 51, 52,
				                   55, 56, 57, 58, 59, 60, 61, 62,
				                   63, 64, 65, 66, 67, 68, 69, 70,
				                   73, 74, 75, 76, 77, 78, 79, 80,
				                   81, 82, 83, 84, 85, 86, 87, 88,
				                   91, 92, 93, 94, 95, 96, 97, 98,
				                   99,100,101,102,103,104,105,106,
				                  109,110,111,112,113,114,115,116,
				                  117,118,119,120,121,122,123,124,
				                  127,128,129,130,131,132,133,134,
				                  135,136,137,138,139,140,141,142};

		for (int i=0; i<controls.length; i++){
			controlDestCodeMap.put(controls[i], new int[]{i,destCodes[i]});
		}
	}

	public Control(String control) {
		int[] control_info;
		control_info = controlDestCodeMap.get(control);
		control_id = control_info[0];
		dest_value = (float)control_info[1] / 144.0f;
	}
}

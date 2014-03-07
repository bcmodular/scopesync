/**
 * ScopeSync mxj~ Main class
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
 * 	Will Ellis
 *  Jessica Brandt
 */
package uk.co.bcmodular.scopesync;

import java.util.*;

import com.cycling74.max.*;
import com.cycling74.msp.*;

public class ScopeSync extends MSPPerformer {

	private static final boolean  debug_mode  = false;						// Debug mode setting
	private LinkedList<MIDIEvent> MIDI_buffer = new LinkedList<MIDIEvent>();// MIDI Event queue
	private MIDIDest[]            MIDI_dests;		// MIDI Destinations
	private Control[]             controls;			// Array of Controls being tracked
	private int        num_controls;				// Number of controls being tracked
	private int        last_midi_dest_used;			// Array position of the last MIDI Destination used
	private double     last_sigv_time;				// Time the previous Signal Vector was received
	private int        last_sigv_control_id;		// Array position of the last Control whose change was sent
	private double     sigv_size;					// Size of signal vectors
	private double     sampling_rate;				// Audio sampling rate
	private float      samples_per_ms;				// Audio sampling rate vs ms
	private float      sigv_length;					// Length of a signal vector in ms

	// Debug poster
	private void debug_post(String s) {
		if (debug_mode) {
			post(s);
		}
	}

	// Constructor
	public ScopeSync(Atom[] args) {
		declareInlets(new int[]{DataTypes.ALL});
		declareOutlets(new int[]{SIGNAL,SIGNAL});
		setInletAssist(new String[]{"Incoming Data"});
		setOutletAssist(new String[]{"MIDI Data Signal", "MIDI Dest Signal"});
		createInfoOutlet(false);

		// If no arguments are provided, use basic defaults (1 MIDI port and 16 control destinations)
		if (args.length == 0) {
			Atom[] a = Atom.newAtom(new String[]{"A","B"});
			args = a;
		}

		setupDests(args);
	}

	// Create Destinations if arguments have been supplied
	private void setupDests(Atom[] dest) {
		int i = 0;
		int num_MIDI_dests = (dest.length / 2);
		num_controls = dest.length * 8;
		last_sigv_control_id = num_controls - 1;
		char midi_dest_code;
		String control_dest_code;
		int control_id;
		MIDIDest md;
		Control cd;

		// Initialise the arrays.
		MIDI_dests = new MIDIDest[num_MIDI_dests];
		controls   = new Control[num_controls];

		// Add the Destinations to the arrays, based on the provided arguments
		while (i < (num_MIDI_dests * 2)) {
			midi_dest_code = dest[i].getString().charAt(0);

			if (i % 2 == 0) {
				// Add MIDI dest to array (throwing away alternative dests, to avoid
				// using more than one from the same S2M handler in Scope, which doesn't
				// like that)
				md = new MIDIDest(midi_dest_code);
				MIDI_dests[i/2] = md;
			}
			// Add control dests to array
			for (int j = 1; j < 9; j++) {
				control_dest_code = midi_dest_code+Integer.toString(j);
				debug_post("Adding control dest to array: "+control_dest_code);
				cd = new Control(control_dest_code);
				control_id = cd.control_id;
				debug_post("control_id= "+control_id);
				controls[control_id] = cd;
			}
			i++;
		}
	}

	// Handle incoming lists (expected to be MIDI Events)
	public void list(Atom[] list_input) {
		double    event_time;
		int       inlet_num     = getInlet();
		int       list_length   = list_input.length;
		boolean   invalid_input = true;
		MIDIEvent MIDI_event;

		if (inlet_num == 0) {
			if (list_length > 0 && list_length < 4) {
				// Create new MIDI event and put it in the buffer
				event_time = MaxSystem.sysTimerGetTime();
				MIDI_event = new MIDIEvent(list_input,event_time);
				MIDI_buffer.add(MIDI_event);
				debug_post("MIDI event coming in: "+MIDI_event.MIDI_data+", arriving at time:"+event_time);
				invalid_input = false;
			}
		}

		if (invalid_input) {
			post("********************************************");
			post("Invalid input to ScopeSync, inlet: "+inlet_num);
			Atom a;
			for (int i = 0; i < list_length; i++) {
				a = list_input[i];
				post("List element "+i+" has value"+a.getInt());
			}
			post("********************************************");
		}
	}

	// Handle incoming Messages (expected to be Control Data or Snapshot messages)
	public void anything(String msg, Atom[] control_data) {
		int       inlet_num     = getInlet();
		int       list_length   = control_data.length;
		boolean   invalid_input = true;
		Control   control;
		float     control_data_value;
		int[]     control_info;
		int       control_id;
		double    event_time;
		MIDIEvent MIDI_event;

		if (list_length == 0) {
			if (msg.equals("snapshot")) {
				for (int i = 0; i < num_controls; i++) {
					controls[i].snapshot = true;
				}
				invalid_input = false;
			} else if (msg.equals("midipanic")) {
				event_time = MaxSystem.sysTimerGetTime();
				for (int j = 1; j < 17; j++){
					MIDI_event = new MIDIEvent(j,event_time);
					MIDI_buffer.add(MIDI_event);
				}
				invalid_input = false;
			}
		}
		else if (list_length == 1) {
			// Process a Control Change
			control_data_value = control_data[0].getFloat();

			// Firstly look up the control_id given the supplied control_dest_code
			control_info = Control.controlDestCodeMap.get(msg);
			control_id   = control_info[0];

			// Then grab that element from the control destination array
			control = controls[control_id];
			control.data_value = control_data_value;
			invalid_input = false;
		}

		if (invalid_input) {
			post("********************************************");
			post("Invalid message input to ScopeSync, message: "+msg+", inlet: "+inlet_num);
			post("List length: "+list_length);
			Atom a;
			for (int i = 0; i < list_length; i++) {
				a = control_data[i];
				post("List element "+i+" has value"+a.getInt());
			}
			post("********************************************");
		}
	}

	// DSP Initialisation Routine (run whenever audio engine is restarted)
	public void dspsetup(MSPSignal[] in, MSPSignal[] out) {
		sampling_rate  = out[0].sr;
		samples_per_ms = (float)sampling_rate / 1000;
		sigv_size      = out[0].n;
		sigv_length    = (1000 * (float)sigv_size) / (float)sampling_rate;
		last_sigv_time = MaxSystem.sysTimerGetTime();
		debug_post("*Sig Init* sampling_rate: "+sampling_rate+", sigv_size: "+sigv_size+", last_sigv_time: "+last_sigv_time);
		debug_post("*Sig Init* samples_per_ms: "+samples_per_ms+", sigv_length: "+sigv_length);
	}

	// Process next signal vector
	public void perform(MSPSignal[] in, MSPSignal[] out) {
		double    sigv_time = MaxSystem.sysTimerGetTime(); // Time current signal vector arrived
		MIDIEvent MIDI_event;  			// MIDI event to be processed
		float     MIDI_event_MIDI_data; // Actual MIDI data to be sent
		MIDIDest  MIDI_dest;   			// MIDI dest to be used
		int       midi_dest_to_use;     // Array position for MIDI dest to be used (in MIDI_dests ArrayList)
		boolean   MIDI_dest_found;		// Flag to indicate that a valid MIDI dest has been found for this MIDI event
		float     MIDI_dest_value;      // Value of MIDI dest
		double    midi_time_since_sigv; // Time difference between time last signal vector arrived
										// and the MIDI event being handled
		int       midi_sample_offset;	// Minimum number of samples between last sample used and
										// when MIDI event should be output
		int       dest_sample_offset;	// Minimum number of samples before a MIDI dest can be used
		int       sample_offset;		// Actual number of samples moved on between events
		int       last_sample_used = -1;// Last sample to have a value set into it
		int       sample_to_use = 0;	// Next sample to set a value into
		int       midi_events_set = 0;	// Number of MIDI events set into the current signal vector
		int       i = 0;				// Utility counter

		if (out[0].connected && out[1].connected) {
			//float[]   in1   = in[0].vec; 	// Input signal vector
			float[]   out1  = out[0].vec; 	// Output signal vector for the MIDI Data
			float[]   out2  = out[1].vec; 	// Output signal vector for the MIDI Destination

			// Blank the output vectors
			for (i=0; i < sigv_size; i++) {
				out1[i] = 0.0f;
				out2[i] = 0.0f;
			}

			if (debug_mode) {
				if (!MIDI_buffer.isEmpty()) {
					post("****************************************************");
					post("Processing new signal vector with active MIDI events");
				}
			}

			while (last_sample_used < (sigv_size - 1) && !MIDI_buffer.isEmpty()) {
				MIDI_event           = MIDI_buffer.getFirst();
				MIDI_event_MIDI_data = MIDI_event.MIDI_data / 2147483648.F;
				midi_time_since_sigv = MIDI_event.event_time - last_sigv_time;
				debug_post("==================================================");
				debug_post("MIDI event grabbed from queue: "+MIDI_event_MIDI_data+", time since sigv:"+midi_time_since_sigv);
				debug_post("MIDI event time is: "+MIDI_event.event_time+", last_sigv_time: "+last_sigv_time);
				debug_post("==================================================");

				if (midi_time_since_sigv <= 0 || midi_time_since_sigv >= sigv_length ) {
					// If the event happened at or before the beginning of the previous signal vector (e.g.
					// it's queued up), or it was received after the end of the previous signal vector
					// (signal has been disconnected?) then set it as soon as possible
					midi_sample_offset = 0;
					debug_post("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
					debug_post("midi_sample_offset: 0 (edge case)");
				} else {
					// Calculate minimum number of samples since the last sample was set before the MIDI event
					// should be set
					midi_sample_offset = (int)Math.round(midi_time_since_sigv * samples_per_ms) - last_sample_used;
					debug_post("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
					debug_post("midi_sample_offset: "+ midi_sample_offset);
				}

				// Grab the next MIDI dest to use
				midi_dest_to_use = last_midi_dest_used + 1;

				debug_post("Length of MIDI_dests: " + MIDI_dests.length);
				if (midi_dest_to_use >= MIDI_dests.length) {
					midi_dest_to_use = 0;
				}
				MIDI_dest = MIDI_dests[midi_dest_to_use];
				debug_post("Next MIDI dest we should be using: " + midi_dest_to_use);

				MIDI_dest_found = false;

				// Check that it didn't send the same message last time (the Scope module will ignore it if it tries
				// to send it again)
				debug_post("last_MIDI_data_sent: "+MIDI_dest.last_MIDI_data_sent);
				debug_post("MIDI_event_MIDI_data: "+MIDI_event_MIDI_data);
				if (MIDI_dest.last_MIDI_data_sent == MIDI_event_MIDI_data) {
					// Now we have to search for one that didn't send that message
					debug_post("Duplicate MIDI message, must search for another dest");
					i = 0;
					while (i < MIDI_dests.length) {
						if (i != midi_dest_to_use) {
							MIDI_dest = MIDI_dests[i];
							if (MIDI_dest.last_MIDI_data_sent != MIDI_event_MIDI_data){
								debug_post("Found replacement MIDI dest: "+midi_dest_to_use);
								MIDI_dest_found = true;
								break;
							}
						}
						i++;
					}
				} else {
					MIDI_dest_found = true;
				}

				if (!MIDI_dest_found) {
					// No MIDI destination available, so this must be a duplicate message
					MIDI_buffer.removeFirst();
					debug_post("Removed duplicate message from MIDI queue");
				} else {
					// Find the smallest gap before the MIDI dest can be used
					MIDI_dest_value    = MIDI_dest.dest_value;
					dest_sample_offset = MIDI_dest.samples_before_use;
					debug_post("------------------------------");
					debug_post("dest_sample_offset: "+ dest_sample_offset);

					// Work out the earliest point we can put a value
					sample_offset = Math.max(Math.max(midi_sample_offset, dest_sample_offset), 1);
					debug_post("sample_offset: "+ sample_offset);
					sample_to_use = last_sample_used + sample_offset;
					debug_post("sample_to_use: "+ sample_to_use);
					debug_post("------------------------------");

					// If we've not run out of samples to use then write out the MIDI value
					if (sample_to_use < sigv_size) {
						// Stick value into the signal
						out1[sample_to_use] = MIDI_event_MIDI_data;
						out2[sample_to_use] = MIDI_dest_value;
						debug_post("Set MIDI data: "+MIDI_event_MIDI_data+" and MIDI dest:"+MIDI_dest_value+" into sample: "+sample_to_use);

						// Decrement all the MIDI Dest samples_before_use values
						for (i=0; i < MIDI_dests.length; i++) {
							MIDI_dests[i].decSamples(sample_offset);
						}

						// Reset sample gap for this destination
						MIDI_dest.resetSamplesBeforeUse();

						// Update the last sent value
						MIDI_dest.last_MIDI_data_sent = MIDI_event_MIDI_data;

						// Remove MIDI Event from buffer
						MIDI_buffer.removeFirst();

						debug_post("new size of MIDI event buffer: "+MIDI_buffer.size());
						last_midi_dest_used = midi_dest_to_use;
						last_sample_used = sample_to_use;
						debug_post("last_sample_used: "+last_sample_used+", sigv_size - 1 = " + (sigv_size - 1));
						midi_events_set++;
					} else {
						debug_post("Ran out of samples to fill in this vector");
						// Just decrement all of the MIDI Dest samples_before_use values
						for (i=0; i < MIDI_dests.length; i++) {
							MIDI_dests[i].decSamples((int)(sigv_size - last_sample_used));
						}
						break;
					}
				}
			}

			// If we haven't put any MIDI events into this vector, then just decrement the samples_to_use values for all destinations
			if (midi_events_set == 0) {
				for (i=0; i < MIDI_dests.length; i++) {
					MIDI_dests[i].decSamples((int)(sigv_size));
				}
			}

			// Now fill in gaps with any control changes that have happened
			Control c;
			int control_id = -1;

			// Loop through each of the signal vector items
			for (i=0; i < sigv_size; i++) {

				if (out2[i] == 0.0f) {
					// We've found a gap, so try to find a Control to put into it
					// Loop through the Controls, starting at the one after the
					// last one we looked at
					while (control_id != last_sigv_control_id) {

						if (control_id == -1 ) {
							// We've entered for the first time, so start with the next one
							// after the last one reached in the previous signal vector
							control_id = (last_sigv_control_id + 1) % num_controls;
						} else {
							// Increment, but need to loop around the Controls
							control_id = (control_id + 1) % num_controls;
						}

						c = controls[control_id];

						if ((c.data_value != c.last_data_value_sent) || c.snapshot) {
							// The value has changed since we last sent one for this
							// Control, so we should send an update
							out1[i] = c.data_value;
							out2[i] = c.dest_value;

							c.last_data_value_sent = c.data_value;
							c.snapshot = false;
							break;
						}
					}
				}
				if (control_id == last_sigv_control_id) {
					break;
				}
			}

			last_sigv_control_id = control_id;
			last_sigv_time       = sigv_time;
		}
	}
}

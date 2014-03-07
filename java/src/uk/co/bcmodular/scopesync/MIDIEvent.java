/**
 * ScopeSync mxj~ MIDIEvent class
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
package uk.co.bcmodular.scopesync;

import com.cycling74.max.Atom;

// Object to store an incoming MIDI event
public class MIDIEvent {
	public float  MIDI_data;
	public double event_time;

	public MIDIEvent(Atom[] MIDI_input, Double event_time){
		int MIDI_data   = 0;
		int list_length = MIDI_input.length;
		this.event_time = event_time;

		if (list_length == 1) {
			MIDI_data = (MIDI_input[0].getInt() << 16);
		} else if (list_length == 2) {
			MIDI_data = ((MIDI_input[0].getInt() << 16) + (MIDI_input[1].getInt() << 8));
		} else if (list_length == 3) {
			MIDI_data = ((MIDI_input[0].getInt() << 16) + (MIDI_input[1].getInt() << 8) + MIDI_input[2].getInt());
		}
		this.MIDI_data = (float)MIDI_data;
	}

	// Create a MIDI all notes off event
	public MIDIEvent(int MIDI_channel, Double event_time){
		int MIDI_data   = 0;
		this.event_time = event_time;

		MIDI_data = ((MIDI_channel + 175) << 16) + (123 << 8);
		this.MIDI_data = (float)MIDI_data;
	}
}

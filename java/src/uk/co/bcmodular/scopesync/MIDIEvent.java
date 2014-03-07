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

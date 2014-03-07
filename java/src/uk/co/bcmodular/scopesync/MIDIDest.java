package uk.co.bcmodular.scopesync;

import java.util.*;

public class MIDIDest {
	public int    dest;
	public float  dest_value;
	public int    samples_before_use = 0;
	public float  last_MIDI_data_sent = 0.0f;
	static final private int sample_gap = 16;
	static private HashMap<Character,Integer> portDestMap = new HashMap<Character,Integer>(16);

	// Set up the dest to port mapping
	static {
		char[] ports = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P'}; 
		int[]  dests = {17,18,35,36,53,54,71,72,89,90,107,108,125,126,143,144};
		for (int i=0; i<ports.length; i++){
			portDestMap.put(ports[i], dests[i]);
		}
	}
	
	public MIDIDest(char port){
		dest = portDestMap.get(port);
		dest_value = (float)dest / 144.0f;
	}
	
	public void decSamples(int num_samples){
		samples_before_use -= num_samples;

		if (samples_before_use < 0) {
			samples_before_use = 0;
		}
	}
	
	public void resetSamplesBeforeUse(){
		samples_before_use = sample_gap;
	}
}
package de.tudresden.ws.container;

import java.util.LinkedList;

/**
 * 
 * @author Mario Krumnow
 * @author Anja Liebscher
 *
 */

public class SumoVehicleSignal {

	LinkedList<Integer> ll_states;
	
	public SumoVehicleSignal(int code){
		
		String s1 = this.getDual(code);
		String[] tmp = s1.split("");
		
		//init
		this.ll_states = new LinkedList<Integer>();
		for(int i=0; i<14; i++){this.ll_states.add(0);}

		for(int i = tmp.length-1; i>0; i--){
			int pos = tmp.length-i-1;
			this.ll_states.set(pos, Integer.valueOf(tmp[i]));		
		}
		
	}
	
	public boolean getState(SumoVehicleSignalState s){
		
		boolean out = false;
		if(this.ll_states.get(s.getPos()) == 1){out = true;}
		return out;
		
	}
	
	
	private String getDual(int code) {
		if (code < 2) {return "" + code;} 
		else {return getDual(code / 2) + code % 2;
	
		}
	}
	
}

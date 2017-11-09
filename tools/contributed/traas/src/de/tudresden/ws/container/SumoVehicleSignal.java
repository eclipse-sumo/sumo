/*   
    Copyright (C) 2017 Mario Krumnow, Dresden University of Technology

    This file is part of TraaS.

    TraaS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    TraaS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TraaS.  If not, see <http://www.gnu.org/licenses/>.
*/

package de.tudresden.ws.container;

import java.util.LinkedList;

/**
 * 
 * @author Mario Krumnow
 * @author Anja Liebscher
 *
 */

public class SumoVehicleSignal implements SumoObject {

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

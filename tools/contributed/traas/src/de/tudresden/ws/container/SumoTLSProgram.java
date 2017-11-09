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
 *
 */

public class SumoTLSProgram implements SumoObject {

	public String subID;
	public int type;
	public int subParameter;
	public int currentPhaseIndex;
	public LinkedList<SumoTLSPhase> phases;
		
	public SumoTLSProgram(){
		
		this.subID = "unkown";
		this.type = -1;
		this.subParameter = -1;
		this.currentPhaseIndex = -1;
		this.phases = new LinkedList<SumoTLSPhase>();
	}
	
	public SumoTLSProgram(String subID, int type, int subParameter, int currentPhaseIndex){
		
		this.subID = subID;
		this.type = type;
		this.subParameter = subParameter;
		this.currentPhaseIndex = currentPhaseIndex;
		this.phases = new LinkedList<SumoTLSPhase>();
		
	}
	
	public void add(SumoTLSPhase phase){
		this.phases.add(phase);
	}
	
	public String toString(){
		StringBuilder sb = new StringBuilder();
		sb.append(this.subID+"#");
		sb.append(this.type+"#");
		sb.append(this.subParameter+"#");
		sb.append(this.currentPhaseIndex+"#");
		for(SumoTLSPhase sp : this.phases){
			sb.append(sp.toString()+"#");
		}	
		return sb.toString();
	}
	
}

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

/**
 * 
 * @author Mario Krumnow
 *
 */

public class SumoTLSPhase implements SumoObject {

	public int duration;
	public int duration1;
	public int duration2;
	public String phasedef;
	
	public SumoTLSPhase(){
		
		this.duration = 0;
		this.duration1 = 0;
		this.duration2 = 0;
		this.phasedef = "r";
		
	}
	
	public SumoTLSPhase(int duration, int duration1, int duration2, String phasedef){
		
		this.duration = duration;
		this.duration1 = duration1;
		this.duration2 = duration2;
		this.phasedef = phasedef;
		
	}
	
	public SumoTLSPhase(int duration, String phasedef){
		
		this.duration = duration;
		this.duration1 = duration;
		this.duration2 = duration;
		this.phasedef = phasedef;
		
	}
	
	public String toString(){
		return this.phasedef+"#"+this.duration+"#"+this.duration1+"#"+this.duration2;
	}
	
}

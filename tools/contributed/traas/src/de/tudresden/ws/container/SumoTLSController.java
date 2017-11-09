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

import java.util.HashMap;

/**
 * 
 * @author Mario Krumnow
 *
 */

public class SumoTLSController implements SumoObject {

	public HashMap<String, SumoTLSProgram> programs;
		
	public SumoTLSController(){
		this.programs = new HashMap<String, SumoTLSProgram>();
	}
	
	public void addProgram(SumoTLSProgram logic){
		this.programs.put(logic.subID, logic);
	}
	
	public SumoTLSProgram get(String prog_name){
		
		SumoTLSProgram tl = null;
		if(this.programs.containsKey(prog_name)){
			tl = this.programs.get(prog_name);
		}
		
		return tl;
		
	}
	
	public String toString(){
		return "";
	}
	
}

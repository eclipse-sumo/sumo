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

public class SumoNextTLS implements SumoObject {

	public LinkedList<NextTLS> ll;
	
	public SumoNextTLS(){
		this.ll = new LinkedList<NextTLS>();
	}
	
	public void add(String tlsID, int ix, double dist, String state){
		this.ll.add(new NextTLS(tlsID, ix, dist, state));
	}
	
	public String toString(){
		
		return "";
		//return this.phasedef+"#"+this.duration+"#"+this.duration1+"#"+this.duration2;
	}
	
	public class NextTLS{
		
		public String tlsID;
		int ix;
		double dist;
		String state;
		
		public NextTLS(String tlsID, int ix, double dist, String state){
			this.tlsID = tlsID;
			this.ix = ix;
			this.dist = dist;
			this.state = state;
		}
	}
	
	
}

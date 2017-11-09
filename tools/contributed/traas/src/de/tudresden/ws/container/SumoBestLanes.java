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

public class SumoBestLanes implements SumoObject {

	public LinkedList<SumoBestLane> ll;
	
	public SumoBestLanes(){this.ll = new LinkedList<SumoBestLane>();}
	
	public void add(String laneID, double length, double occupation, int offset, int allowsContinuation, LinkedList<String> ll){
		this.ll.add(new SumoBestLane(laneID, length, occupation, offset, allowsContinuation, ll));
	}
	
	public String toString(){
		return "";
	}
	
	public class SumoBestLane{
		
		public LinkedList<String> ll;
		public String laneID;
		public double length;
		public double occupation;
		public int offset;
		public int allowsContinuation;
		
		public SumoBestLane(String laneID, double length, double occupation, int offset, int allowsContinuation, LinkedList<String> ll){
			this.laneID = laneID;
			this.length = length;
			this.occupation = occupation;
			this.offset = offset;
			this.allowsContinuation = allowsContinuation;
			this.ll = ll;
		}
	}
}

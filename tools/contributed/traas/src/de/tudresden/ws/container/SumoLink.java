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

public class SumoLink implements SumoObject {

	public String from;
	public String to;
	public String over;
	
	//2nd 
	public String notInternalLane;
	public String internalLane;
	public String state;
	public String direction;
	public byte hasPriority;
	public byte isOpen;
	public byte hasApproachingFoe;
	public double length;
	
	int type = 0;
	
	//1st constructor
	public SumoLink(String from, String to,	String over){
		this.from = from;
		this.to = to;
		this.over = over;
		this.type = 0;
	}
	
	//2nd constructor
	public SumoLink(String notInternal, String internal, byte priority, byte isOpen, byte hasFoe, double length, String state, String direction)
	{
		this.notInternalLane = notInternal;
		this.internalLane = internal;
		this.hasPriority = priority;
		this.isOpen = isOpen;
		this.hasApproachingFoe = hasFoe;
		this.length = length;
		this.state = state;
		this.direction = direction;
		this.type = 1;
	}
	
	
	
	public String toString(){
		
		if(this.type == 0){
			return this.from+"#"+this.over+"#"+this.to;
		}else{
			return this.notInternalLane+"#"+this.internalLane+"#"+this.hasPriority+"#"+this.isOpen+"#"+this.hasApproachingFoe+"#"+this.length+"#"+this.state+"#"+this.direction;
		}
		
	}
	
	
}

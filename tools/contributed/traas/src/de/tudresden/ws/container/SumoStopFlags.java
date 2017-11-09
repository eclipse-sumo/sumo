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

public class SumoStopFlags implements SumoObject {

	 public boolean stopped = false;
	 public boolean parking = false;
	 public boolean triggered = false;
	 public boolean containerTriggered = false;
	 public boolean isBusStop = false;
	 public boolean isContainerStop = false;
	
	 public SumoStopFlags(boolean parking, boolean triggered, boolean containerTriggered, boolean isBusStop, boolean isContainerStop){
		 this.parking = parking;
		 this.triggered = triggered;
		 this.containerTriggered = containerTriggered;
		 this.isBusStop = isBusStop;
		 this.isContainerStop = isContainerStop;
	 }
	 
	 public SumoStopFlags(byte id){
		 
		 byte[] m = {1, 2, 4, 8, 16, 32};
		 this.stopped = (id & m[0]) == m[0] ? true : false;
		 this.parking = (id & m[1]) == m[1] ? true : false;
		 this.triggered = (id & m[2]) == m[2] ? true : false;
		 this.containerTriggered = (id & m[3]) == m[3] ? true : false;
		 this.isBusStop = (id & m[4]) == m[4] ? true : false;
		 this.isContainerStop = (id & m[5]) == m[5] ? true : false;
		 
	 }

	 public byte getID(){
		 
		 StringBuilder sb = new StringBuilder();
		 sb.append(isContainerStop ? "1" : "0");
		 sb.append(isBusStop ? "1" : "0");
		 sb.append(containerTriggered ? "1" : "0");
		 sb.append(triggered ? "1" : "0");
		 sb.append(parking ? "1" : "0");
		 return (byte)Integer.parseInt(sb.toString(), 2);
		 
	 }
	 
	
}

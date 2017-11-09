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

public class SumoVehicleData implements SumoObject {

public LinkedList<VehicleData> ll;
	
	public SumoVehicleData(){
		this.ll = new LinkedList<VehicleData>();
	}
	
	public void add(String vehID, double length, double entry_time, double leave_time, String typeID){
		this.ll.add(new VehicleData(vehID, length, entry_time, leave_time, typeID));
	}
	
	public static class VehicleData{
		
		public String vehID;
		public double length;
		public double entry_time;
		public double leave_time;
		public String typeID;
		
		public VehicleData(String vehID, double length, double entry_time, double leave_time, String typeID){
			this.vehID = vehID;
			this.length = length;
			this.entry_time = entry_time;
			this.leave_time = leave_time;
			this.typeID = typeID;
		}
		
		
	}
	
}

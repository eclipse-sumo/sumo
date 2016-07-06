package de.tudresden.ws.container;

import java.util.LinkedList;

public class SumoVehicleData {

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

package de.tudresden.sumo.subscription;

import de.tudresden.sumo.config.Constants;

public enum SumoDomain {

	inductive_loops(Constants.CMD_GET_INDUCTIONLOOP_VARIABLE),	
	lanes(Constants.CMD_GET_LANE_VARIABLE),
	vehicles(Constants.CMD_GET_VEHICLE_VARIABLE),
	poi(Constants.CMD_GET_POI_VARIABLE),
	polygons(Constants.CMD_GET_POLYGON_VARIABLE),
	junction(Constants.CMD_GET_JUNCTION_VARIABLE),
	edges(Constants.CMD_GET_EDGE_VARIABLE);
	
	int id;
	
	SumoDomain(int id){this.id = id;}
	
	public int getID(){return this.id;}
	
}

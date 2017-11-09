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

package de.tudresden.sumo.subscription;

import de.tudresden.sumo.config.Constants;

public enum SubscribtionVariable {

	inductive_loop(Constants.CMD_SUBSCRIBE_INDUCTIONLOOP_VARIABLE),	
	areal_detector(Constants.CMD_SUBSCRIBE_LANEAREA_VARIABLE),	
	me_detector(Constants.CMD_SUBSCRIBE_MULTIENTRYEXIT_VARIABLE),	
	trafficlight(Constants.CMD_SUBSCRIBE_TL_VARIABLE),	
	lane(Constants.CMD_SUBSCRIBE_LANE_VARIABLE),
	vehicle(Constants.CMD_SUBSCRIBE_VEHICLE_VARIABLE),
	vehicleType(Constants.CMD_SUBSCRIBE_VEHICLETYPE_VARIABLE),
	route(Constants.CMD_SUBSCRIBE_ROUTE_VARIABLE),
	poi(Constants.CMD_SUBSCRIBE_POI_VARIABLE),
	polygon(Constants.CMD_SUBSCRIBE_POLYGON_VARIABLE),
	person(Constants.CMD_SUBSCRIBE_PERSON_VARIABLE),
	junction(Constants.CMD_SUBSCRIBE_JUNCTION_VARIABLE),
	simulation(Constants.CMD_SUBSCRIBE_SIM_VARIABLE),
	gui(Constants.CMD_SUBSCRIBE_GUI_VARIABLE),
	edge(Constants.CMD_SUBSCRIBE_EDGE_VARIABLE);
	
	
	int id;
	SubscribtionVariable(int id){this.id = id;}
	
	public int getID(){return this.id;}
	
}

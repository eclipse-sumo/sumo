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

public enum SumoDomain {

	inductive_loops(Constants.CMD_GET_INDUCTIONLOOP_VARIABLE),	
	lanes(Constants.CMD_GET_LANE_VARIABLE),
	vehicles(Constants.CMD_GET_VEHICLE_VARIABLE),
	persons(Constants.CMD_GET_PERSON_VARIABLE),
	poi(Constants.CMD_GET_POI_VARIABLE),
	polygons(Constants.CMD_GET_POLYGON_VARIABLE),
	junction(Constants.CMD_GET_JUNCTION_VARIABLE),
	edges(Constants.CMD_GET_EDGE_VARIABLE);
	
	int id;
	
	SumoDomain(int id){this.id = id;}
	
	public int getID(){return this.id;}
	
}

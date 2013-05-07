/*   
    Copyright (C) 2013 Mario Krumnow, Dresden University of Technology

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

package de.tudresden.sumo.cmd;
import de.tudresden.sumo.config.Constants;
import de.tudresden.sumo.util.SumoCommand;
import de.tudresden.ws.container.SumoStringList;

public class Lane {

	//getter methods

	/**
	 * getAllowed
	 */

	public static SumoCommand getAllowed(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LANE_ALLOWED, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the CO2 emission in mg for the last time step on the given lane.
	 */

	public static SumoCommand getCO2Emission(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_CO2EMISSION, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the CO emission in mg for the last time step on the given lane.
	 */

	public static SumoCommand getCOEmission(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_COEMISSION, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * getDisallowed
	 */

	public static SumoCommand getDisallowed(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LANE_DISALLOWED, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * getEdgeID
	 */

	public static SumoCommand getEdgeID(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LANE_EDGE_ID, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns the fuel consumption in ml for the last time step on the given lane.
	 */

	public static SumoCommand getFuelConsumption(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_FUELCONSUMPTION, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the HC emission in mg for the last time step on the given lane.
	 */

	public static SumoCommand getHCEmission(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_HCEMISSION, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns a list of all lanes in the network.
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the total number of halting vehicles for the last time step on the given lane.
	 */

	public static SumoCommand getLastStepHaltingNumber(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LAST_STEP_VEHICLE_HALTING_NUMBER, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * Returns the total vehicle length in m for the last time step on the given lane.
	 */

	public static SumoCommand getLastStepLength(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LAST_STEP_LENGTH, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the average speed in m/s for the last time step on the given lane.
	 */

	public static SumoCommand getLastStepMeanSpeed(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LAST_STEP_MEAN_SPEED, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the occupancy in % for the last time step on the given lane.
	 */

	public static SumoCommand getLastStepOccupancy(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LAST_STEP_OCCUPANCY, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the subscription results for the last time step and the given lane.
	 */

	public static SumoCommand getLastStepVehicleIDs(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LAST_STEP_VEHICLE_ID_LIST, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the total number of vehicles for the last time step on the given lane.
	 */

	public static SumoCommand getLastStepVehicleNumber(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LAST_STEP_VEHICLE_NUMBER, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * getLength
	 */

	public static SumoCommand getLength(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_LENGTH, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * getLinkNumber
	 */

	public static SumoCommand getLinkNumber(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LANE_LINK_NUMBER, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * getLinks
	 */

	public static SumoCommand getLinks(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LANE_LINKS, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * getMaxSpeed
	 */

	public static SumoCommand getMaxSpeed(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_MAXSPEED, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the NOx emission in mg for the last time step on the given lane.
	 */

	public static SumoCommand getNOxEmission(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_NOXEMISSION, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the noise emission in db for the last time step on the given lane.
	 */

	public static SumoCommand getNoiseEmission(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_NOISEEMISSION, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the particular matter emission in mg for the last time step on the given lane.
	 */

	public static SumoCommand getPMxEmission(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_PMXEMISSION, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * getShape
	 */

	public static SumoCommand getShape(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_SHAPE, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_POLYGON);
	}

	/**
	 * Returns the estimated travel time in s for the last time step on the given lane.
	 */

	public static SumoCommand getTraveltime(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_CURRENT_TRAVELTIME, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * getWidth
	 */

	public static SumoCommand getWidth(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_WIDTH, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	//setter methods

	/**
	 * setAllowed
	 */

	public static SumoCommand setAllowed(String laneID, SumoStringList allowedClasses){

		return new SumoCommand(Constants.CMD_SET_LANE_VARIABLE, Constants.LANE_ALLOWED, laneID, allowedClasses);
	}

	/**
	 * setDisallowed
	 */

	public static SumoCommand setDisallowed(String laneID, SumoStringList disallowedClasses){

		return new SumoCommand(Constants.CMD_SET_LANE_VARIABLE, Constants.LANE_DISALLOWED, laneID, disallowedClasses);
	}

	/**
	 * setLength
	 */

	public static SumoCommand setLength(String laneID, double length){

		return new SumoCommand(Constants.CMD_SET_LANE_VARIABLE, Constants.VAR_LENGTH, laneID, length);
	}

	/**
	 * setMaxSpeed
	 */

	public static SumoCommand setMaxSpeed(String laneID, double speed){

		return new SumoCommand(Constants.CMD_SET_LANE_VARIABLE, Constants.VAR_MAXSPEED, laneID, speed);
	}


}
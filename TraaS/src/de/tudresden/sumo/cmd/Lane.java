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
	 * Returns a list of allowed vehicle classes. An empty list means all vehicles are allowed.
	 */

	public static SumoCommand getAllowed(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LANE_ALLOWED, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the CO2 emissions (in mg) on the given lane during the last time step.
	 */

	public static SumoCommand getCO2Emission(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_CO2EMISSION, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the CO emissions (in mg) on the given lane during the last time step.
	 */

	public static SumoCommand getCOEmission(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_COEMISSION, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns a list of disallowed vehicle classes.
	 */

	public static SumoCommand getDisallowed(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LANE_DISALLOWED, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the ID of the edge this lane belongs to.
	 */

	public static SumoCommand getEdgeID(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LANE_EDGE_ID, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns the fuel consumption (in ml) on the given lane during the last time step.
	 */

	public static SumoCommand getFuelConsumption(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_FUELCONSUMPTION, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the HC emissions (in mg) on the given lane during the last time step.
	 */

	public static SumoCommand getHCEmission(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_HCEMISSION, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns a list of IDs of all lanes in the network.
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the total number of halting vehicles on the given lane within the last time step.
	 */

	public static SumoCommand getLastStepHaltingNumber(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LAST_STEP_VEHICLE_HALTING_NUMBER, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * Returns the total vehicle length (in m) on the given lane during the last time step.
	 */

	public static SumoCommand getLastStepLength(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LAST_STEP_LENGTH, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the mean speed (in m/s) on the given lane within the last time step.
	 */

	public static SumoCommand getLastStepMeanSpeed(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LAST_STEP_MEAN_SPEED, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the occupancy (in %) on the given lane during the last time step.
	 */

	public static SumoCommand getLastStepOccupancy(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LAST_STEP_OCCUPANCY, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the IDs of the vehicles on the given lane within the last time step.
	 */

	public static SumoCommand getLastStepVehicleIDs(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LAST_STEP_VEHICLE_ID_LIST, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns number of vehicles on this lane within the last time step.
	 */

	public static SumoCommand getLastStepVehicleNumber(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LAST_STEP_VEHICLE_NUMBER, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * Returns the length (in m) of the named lane.
	 */

	public static SumoCommand getLength(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_LENGTH, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the number of links outgoing from this lane.
	 */

	public static SumoCommand getLinkNumber(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LANE_LINK_NUMBER, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * Returns descriptions of the links (in m) outgoing from this lane.
	 */

	public static SumoCommand getLinks(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.LANE_LINKS, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the maximum speed (in m/s) allowed on this lane.
	 */

	public static SumoCommand getMaxSpeed(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_MAXSPEED, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the NOx emissions (in mg) on the given lane during the last time step.
	 */

	public static SumoCommand getNOxEmission(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_NOXEMISSION, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the noise emissions (in db) on the given lane during the last time step.
	 */

	public static SumoCommand getNoiseEmission(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_NOISEEMISSION, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the particular matter emissions (in mg) on the given lane during the last time step.
	 */

	public static SumoCommand getPMxEmission(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_PMXEMISSION, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns this lane's shape.
	 */

	public static SumoCommand getShape(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_SHAPE, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_POLYGON);
	}

	/**
	 * Returns the estimated travel time (in s) on the given lane during the last time step.
	 */

	public static SumoCommand getTraveltime(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_CURRENT_TRAVELTIME, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the width (in m) of the named lane.
	 */

	public static SumoCommand getWidth(String laneID){
		return new SumoCommand(Constants.CMD_GET_LANE_VARIABLE, Constants.VAR_WIDTH, laneID, Constants.RESPONSE_GET_LANE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	//setter methods

	/**
	 * Set a list of allowed vehicle classes.
	 */

	public static SumoCommand setAllowed(String laneID, SumoStringList allowedClasses){

		return new SumoCommand(Constants.CMD_SET_LANE_VARIABLE, Constants.LANE_ALLOWED, laneID, allowedClasses);
	}

	/**
	 * Set a list of disallowed vehicle classes.
	 */

	public static SumoCommand setDisallowed(String laneID, SumoStringList disallowedClasses){

		return new SumoCommand(Constants.CMD_SET_LANE_VARIABLE, Constants.LANE_DISALLOWED, laneID, disallowedClasses);
	}

	/**
	 * Set the length (in m) of the named lane.
	 */

	public static SumoCommand setLength(String laneID, double length){

		return new SumoCommand(Constants.CMD_SET_LANE_VARIABLE, Constants.VAR_LENGTH, laneID, length);
	}

	/**
	 * Set the maximum speed (in m/s) allowed on this lane.
	 */

	public static SumoCommand setMaxSpeed(String laneID, double speed){

		return new SumoCommand(Constants.CMD_SET_LANE_VARIABLE, Constants.VAR_MAXSPEED, laneID, speed);
	}


}
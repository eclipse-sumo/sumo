/*   
    Copyright (C) 2013 Mario Krumnow, Evamarie Wießner, Dresden University of Technology

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

public class Edge {


	/**
	 * Returns the travel time value (in s) used for (re-)routing.
	 */

	public static SumoCommand getAdaptedTraveltime(String edgeID, double time){
		Object[] array = new Object[]{time};
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.VAR_EDGE_TRAVELTIME, edgeID, array, Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the CO2 emissions (in mg) on the given edge during the last time step.
	 */

	public static SumoCommand getCO2Emission(String edgeID){
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.VAR_CO2EMISSION, edgeID, Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the CO emissions (in mg) on the given edge during the last time step.
	 */

	public static SumoCommand getCOEmission(String edgeID){
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.VAR_COEMISSION, edgeID, Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the effort value used for (re-)routing.
	 */

	public static SumoCommand getEffort(String edgeID, double time){
		Object[] array = new Object[]{time};
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.VAR_EDGE_EFFORT, edgeID, array, Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the fuel consumption (in ml) on the given edge during the last time step.
	 */

	public static SumoCommand getFuelConsumption(String edgeID){
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.VAR_FUELCONSUMPTION, edgeID, Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the HC emissions (in mg) on the given edge during the last time step.
	 */

	public static SumoCommand getHCEmission(String edgeID){
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.VAR_HCEMISSION, edgeID, Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the number of edges in the network.
	 */

	public static SumoCommand getIDCount(){
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.ID_COUNT, "", Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * Returns a list of all edges in the network.
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the total number of halting vehicles on the given edge within the last time step.
	 */

	public static SumoCommand getLastStepHaltingNumber(String edgeID){
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.LAST_STEP_VEHICLE_HALTING_NUMBER, edgeID, Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * Returns the total vehicle length (in m) on the given edge within the last time step.
	 */

	public static SumoCommand getLastStepLength(String edgeID){
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.LAST_STEP_LENGTH, edgeID, Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the mean speed of vehicles (in m/s) that were on the named induction loop within the last simulation step.
	 */

	public static SumoCommand getLastStepMeanSpeed(String edgeID){
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.LAST_STEP_MEAN_SPEED, edgeID, Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the percentage of time (in %) the detector was occupied by a vehicle.
	 */

	public static SumoCommand getLastStepOccupancy(String edgeID){
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.LAST_STEP_OCCUPANCY, edgeID, Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the list of ids of the vehicles on the given edge within the last simulation step.
	 */

	public static SumoCommand getLastStepVehicleIDs(String edgeID){
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.LAST_STEP_VEHICLE_ID_LIST, edgeID, Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the total number of vehicles on this edge within the last time step.
	 */

	public static SumoCommand getLastStepVehicleNumber(String edgeID){
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.LAST_STEP_VEHICLE_NUMBER, edgeID, Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * Returns the NOx emissions (in mg) on the given edge during the last time step.
	 */

	public static SumoCommand getNOxEmission(String edgeID){
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.VAR_NOXEMISSION, edgeID, Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the noise emissions (in db)on the given edge within the last time step.
	 */

	public static SumoCommand getNoiseEmission(String edgeID){
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.VAR_NOISEEMISSION, edgeID, Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the particular matter emissions (in mg) on the given edge on this edge during the last time step.
	 */

	public static SumoCommand getPMxEmission(String edgeID){
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.VAR_PMXEMISSION, edgeID, Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the estimated travel time (in s) on the given edge on this edge within the last time step.
	 */

	public static SumoCommand getTraveltime(String edgeID){
		return new SumoCommand(Constants.CMD_GET_EDGE_VARIABLE, Constants.VAR_CURRENT_TRAVELTIME, edgeID, Constants.RESPONSE_GET_EDGE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	//setter methods

	/**
	 * Adapt the travel time value (in s) used for (re-)routing on the given edge.
	 */

	public static SumoCommand adaptTraveltime(String edgeID, int time){

		return new SumoCommand(Constants.CMD_SET_EDGE_VARIABLE, Constants.VAR_EDGE_TRAVELTIME, edgeID, time);
	}

	/**
	 * Adapt the effort value used for (re-)routing on the given edge.
	 */

	public static SumoCommand setEffort(String edgeID, double effort){

		return new SumoCommand(Constants.CMD_SET_EDGE_VARIABLE, Constants.VAR_EDGE_EFFORT, edgeID, effort);
	}

	/**
	 * Set a new maximum speed (in m/s) for all lanes of the edge.
	 */

	public static SumoCommand setMaxSpeed(String edgeID, double speed){

		return new SumoCommand(Constants.CMD_SET_EDGE_VARIABLE, Constants.VAR_MAXSPEED, edgeID, speed);
	}


}
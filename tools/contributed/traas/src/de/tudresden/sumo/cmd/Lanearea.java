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

package de.tudresden.sumo.cmd;
import de.tudresden.sumo.config.Constants;
import de.tudresden.sumo.util.SumoCommand;

/**
 * 
 * @author Mario Krumnow
 * @author Evamarie Wiessner
 *
 */

public class Lanearea {

	//getter methods

	/**
	 * Returns a list of all areal detectors in the network.
	 * @return a list of IDs of all areal detectors
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_LANEAREA_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_LANEAREA_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	
	/**
	 * Returns the number of vehicles on the detector
	 * @param loopID a string identifying the loop
	 * @return numeber of vehicles
	 */

	public static SumoCommand getLastStepVehicleNumber(String loopID){
		return new SumoCommand(Constants.CMD_GET_LANEAREA_VARIABLE, Constants.LAST_STEP_VEHICLE_NUMBER, "", Constants.RESPONSE_GET_LANEAREA_VARIABLE, Constants.TYPE_INTEGER);
	}
	
	/**
	 * Returns the number of vehicles which were halting during the last time step.
	 * @param loopID a string identifying the loop
	 * @return numeber of vehicles
	 */

	public static SumoCommand getLastStepHaltingNumber(String loopID){
		return new SumoCommand(Constants.CMD_GET_LANEAREA_VARIABLE, Constants.LAST_STEP_VEHICLE_HALTING_NUMBER, "", Constants.RESPONSE_GET_LANEAREA_VARIABLE, Constants.TYPE_INTEGER);
	}
	
	
	/**
	 *  Returns the list of ids of vehicles that were on the named detector in the last simulation step.
     * @param loopID a string identifying the loop
	 * @return a list of all vehicles on the detector
	 */

	public static SumoCommand getLastStepVehicleIDs(String loopID){
		return new SumoCommand(Constants.CMD_GET_LANEAREA_VARIABLE, Constants.LAST_STEP_VEHICLE_ID_LIST, "", Constants.RESPONSE_GET_LANEAREA_VARIABLE, Constants.TYPE_STRINGLIST);
	}
	
	/**
	 * Returns the number of the areal detectors in the network.
	 * @return the number of areal detectors
	 */

	public static SumoCommand getIDCount(){
		return new SumoCommand(Constants.CMD_GET_LANEAREA_VARIABLE, Constants.ID_COUNT, "", Constants.RESPONSE_GET_LANEAREA_VARIABLE, Constants.TYPE_INTEGER);
	}

	
	/**
	 * getJamLengthVehicle
	 * @param loopID a string identifying the loop
	 * @return jam length in vehicles
	 */


	public static SumoCommand getJamLengthVehicle(String loopID){
		return new SumoCommand(Constants.CMD_GET_LANEAREA_VARIABLE, Constants.JAM_LENGTH_VEHICLE, loopID, Constants.RESPONSE_GET_LANEAREA_VARIABLE, Constants.TYPE_INTEGER);
	}
	
	/**
	 * getJamLengthMeters
	 * @param loopID a string identifying the loop
	 * @return  jam length in [m]
	 */

	public static SumoCommand getJamLengthMeters(String loopID){
		return new SumoCommand(Constants.CMD_GET_LANEAREA_VARIABLE, Constants.JAM_LENGTH_METERS, loopID, Constants.RESPONSE_GET_LANEAREA_VARIABLE, Constants.TYPE_DOUBLE);
	}
	
	/**
	 * getLastStepMeanSpeed
	 * @param loopID a string identifying the loop
	 * @return last step mean speed
	 */

	public static SumoCommand getLastStepMeanSpeed(String loopID){
		return new SumoCommand(Constants.CMD_GET_LANEAREA_VARIABLE, Constants.LAST_STEP_MEAN_SPEED, loopID, Constants.RESPONSE_GET_LANEAREA_VARIABLE, Constants.TYPE_DOUBLE);
	}

	
	/**
	 * getLastStepOccupancy
	 * @param loopID a string identifying the loop
	 * @return last step occupancy
	 */

	public static SumoCommand getLastStepOccupancy(String loopID){
		return new SumoCommand(Constants.CMD_GET_LANEAREA_VARIABLE, Constants.LAST_STEP_OCCUPANCY, loopID, Constants.RESPONSE_GET_LANEAREA_VARIABLE, Constants.TYPE_DOUBLE);
	}
	
	
	/**
	 * getPosition
	 * @param loopID a string identifying the loop
	 * @return position of the vehicle in m
	 */

	public static SumoCommand getPosition(String loopID){
		return new SumoCommand(Constants.CMD_GET_LANEAREA_VARIABLE, Constants.VAR_POSITION, loopID, Constants.RESPONSE_GET_LANEAREA_VARIABLE, Constants.TYPE_DOUBLE);
	}
	
	/**
	 * getLaneID
	 * @param loopID a string identifying the loop
	 * @return lane ID of the detector
	 */

	public static SumoCommand getLaneID(String loopID){
		return new SumoCommand(Constants.CMD_GET_LANEAREA_VARIABLE, Constants.VAR_LANE_ID, loopID, Constants.RESPONSE_GET_LANEAREA_VARIABLE, Constants.TYPE_STRING);
	}
	
	/**
	 * getlength
	 * @param loopID a string identifying the loop
	 * @return lane ID of the detector
	 */

	public static SumoCommand getLength(String loopID){
		return new SumoCommand(Constants.CMD_GET_LANEAREA_VARIABLE, Constants.VAR_LENGTH, loopID, Constants.RESPONSE_GET_LANEAREA_VARIABLE, Constants.TYPE_DOUBLE);
	}
	
	
}
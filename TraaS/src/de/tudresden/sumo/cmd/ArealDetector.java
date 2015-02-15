/*   
    Copyright (C) 2015 Mario Krumnow, Dresden University of Technology

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

public class ArealDetector {

	//getter methods

	/**
	 * Returns a list of all areal detectors in the network.
	 * @return a list of IDs of all areal detectors
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_AREAL_DETECTOR_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_AREAL_DETECTOR_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the number of the areal detectors in the network.
	 * @return the number of areal detectors
	 */

	public static SumoCommand getIDCount(){
		return new SumoCommand(Constants.CMD_GET_AREAL_DETECTOR_VARIABLE, Constants.ID_COUNT, "", Constants.RESPONSE_GET_AREAL_DETECTOR_VARIABLE, Constants.TYPE_INTEGER);
	}

	
	/**
	 * getJamLengthVehicle
	 * @param loopID a string identifying the loop
	 * @return jam length in vehicles
	 */


	public static SumoCommand getJamLengthVehicle(String loopID){
		return new SumoCommand(Constants.CMD_GET_AREAL_DETECTOR_VARIABLE, Constants.JAM_LENGTH_VEHICLE, loopID, Constants.RESPONSE_GET_AREAL_DETECTOR_VARIABLE, Constants.TYPE_INTEGER);
	}
	
	/**
	 * getJamLengthMeters
	 * @param loopID a string identifying the loop
	 * @return  jam length in [m]
	 */

	public static SumoCommand getJamLengthMeters(String loopID){
		return new SumoCommand(Constants.CMD_GET_AREAL_DETECTOR_VARIABLE, Constants.JAM_LENGTH_METERS, loopID, Constants.RESPONSE_GET_AREAL_DETECTOR_VARIABLE, Constants.TYPE_DOUBLE);
	}
	
	/**
	 * getLastStepMeanSpeed
	 * @param loopID a string identifying the loop
	 * @return last step mean speed
	 */

	public static SumoCommand getLastStepMeanSpeed(String loopID){
		return new SumoCommand(Constants.CMD_GET_AREAL_DETECTOR_VARIABLE, Constants.LAST_STEP_MEAN_SPEED, loopID, Constants.RESPONSE_GET_AREAL_DETECTOR_VARIABLE, Constants.TYPE_DOUBLE);
	}

	
	/**
	 * getLastStepOccupancy
	 * @param loopID a string identifying the loop
	 * @return last step occupancy
	 */

	public static SumoCommand getLastStepOccupancy(String loopID){
		return new SumoCommand(Constants.CMD_GET_AREAL_DETECTOR_VARIABLE, Constants.LAST_STEP_OCCUPANCY, loopID, Constants.RESPONSE_GET_AREAL_DETECTOR_VARIABLE, Constants.TYPE_DOUBLE);
	}
	
}
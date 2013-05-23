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

public class Inductionloop {

	//getter methods

	/**
	 * Returns a list of IDs of all induction loops within the scenario.
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_INDUCTIONLOOP_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_INDUCTIONLOOP_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the ID of the lane the loop is placed at.
	 */

	public static SumoCommand getLaneID(String loopID){
		return new SumoCommand(Constants.CMD_GET_INDUCTIONLOOP_VARIABLE, Constants.VAR_LANE_ID, loopID, Constants.RESPONSE_GET_INDUCTIONLOOP_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns the mean length of vehicles (in m) which were on the detector in the last step. 
	 */

	public static SumoCommand getLastStepMeanLength(String loopID){
		return new SumoCommand(Constants.CMD_GET_INDUCTIONLOOP_VARIABLE, Constants.LAST_STEP_LENGTH, loopID, Constants.RESPONSE_GET_INDUCTIONLOOP_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the mean speed of vehicles (in m/s) that were on the named induction loop within the last simulation step.
	 */

	public static SumoCommand getLastStepMeanSpeed(String loopID){
		return new SumoCommand(Constants.CMD_GET_INDUCTIONLOOP_VARIABLE, Constants.LAST_STEP_MEAN_SPEED, loopID, Constants.RESPONSE_GET_INDUCTIONLOOP_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the percentage of time (in %) the detector was occupied by a vehicle within the last time step.
	 */

	public static SumoCommand getLastStepOccupancy(String loopID){
		return new SumoCommand(Constants.CMD_GET_INDUCTIONLOOP_VARIABLE, Constants.LAST_STEP_OCCUPANCY, loopID, Constants.RESPONSE_GET_INDUCTIONLOOP_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the list of IDs of vehicles that were on the named induction loop in the last simulation step.
	 */

	public static SumoCommand getLastStepVehicleIDs(String loopID){
		return new SumoCommand(Constants.CMD_GET_INDUCTIONLOOP_VARIABLE, Constants.LAST_STEP_VEHICLE_ID_LIST, loopID, Constants.RESPONSE_GET_INDUCTIONLOOP_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the number of vehicles that were on the named induction loop within the last simulation step.
	 */

	public static SumoCommand getLastStepVehicleNumber(String loopID){
		return new SumoCommand(Constants.CMD_GET_INDUCTIONLOOP_VARIABLE, Constants.LAST_STEP_VEHICLE_NUMBER, loopID, Constants.RESPONSE_GET_INDUCTIONLOOP_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * Returns the position (in m) of the induction loop at it's lane, counted from the lane's begin.
	 */

	public static SumoCommand getPosition(String loopID){
		return new SumoCommand(Constants.CMD_GET_INDUCTIONLOOP_VARIABLE, Constants.VAR_POSITION, loopID, Constants.RESPONSE_GET_INDUCTIONLOOP_VARIABLE, Constants.POSITION_2D);
	}

	/**
	 * Return the time (in s) since last detection.
	 */

	public static SumoCommand getTimeSinceDetection(String loopID){
		return new SumoCommand(Constants.CMD_GET_INDUCTIONLOOP_VARIABLE, Constants.LAST_STEP_TIME_SINCE_DETECTION, loopID, Constants.RESPONSE_GET_INDUCTIONLOOP_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * A complex structure containing several information about vehicles which passed the detector.
	 */

	public static SumoCommand getVehicleData(SumoStringList loopID){
		return new SumoCommand(Constants.CMD_GET_INDUCTIONLOOP_VARIABLE, Constants.LAST_STEP_VEHICLE_DATA, loopID, Constants.RESPONSE_GET_INDUCTIONLOOP_VARIABLE, Constants.TYPE_INTEGER);
	}

	//setter methods
	
}
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

public class Multientryexit {

	//getter methods

	/**
	 * Returns a list of all e3 detectors in the network.
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the subscription results for the last time step and the given detector.
	 */

	public static SumoCommand getLastStepHaltingNumber(String detID){
		return new SumoCommand(Constants.CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, Constants.LAST_STEP_VEHICLE_HALTING_NUMBER, detID, Constants.RESPONSE_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * getLastStepMeanSpeed
	 */

	public static SumoCommand getLastStepMeanSpeed(String detID){
		return new SumoCommand(Constants.CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, Constants.LAST_STEP_MEAN_SPEED, detID, Constants.RESPONSE_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * getLastStepVehicleIDs
	 */

	public static SumoCommand getLastStepVehicleIDs(String detID){
		return new SumoCommand(Constants.CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, Constants.LAST_STEP_VEHICLE_ID_LIST, detID, Constants.RESPONSE_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * getLastStepVehicleNumber
	 */

	public static SumoCommand getLastStepVehicleNumber(String detID){
		return new SumoCommand(Constants.CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, Constants.LAST_STEP_VEHICLE_NUMBER, detID, Constants.RESPONSE_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, Constants.TYPE_INTEGER);
	}

	//setter methods


}
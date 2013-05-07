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

public class Simulation {

	//getter methods

	/**
	 * convert2D
	 */

	public static SumoCommand convert2D(String edgeID, double pos, int laneIndex, String toGeo){
		Object[] array = new Object[]{pos, laneIndex, toGeo};
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.POSITION_CONVERSION, edgeID, array, Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * convertGeo
	 */

	public static SumoCommand convertGeo(double x, double y, String fromGeo){
		Object[] array = new Object[]{y, fromGeo};
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.POSITION_CONVERSION, x, array, Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * convertRoad
	 */

	public static SumoCommand convertRoad(double x, double y, String isGeo){
		Object[] array = new Object[]{y, isGeo};
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.VAR_POSITION, x, array, Constants.RESPONSE_GET_SIM_VARIABLE, Constants.POSITION_2D);
	}

	/**
	 * getArrivedIDList
	 */

	public static SumoCommand getArrivedIDList(){
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.VAR_ARRIVED_VEHICLES_IDS, "", Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * getArrivedNumber
	 */

	public static SumoCommand getArrivedNumber(){
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.VAR_ARRIVED_VEHICLES_NUMBER, "", Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * getCurrentTime
	 */

	public static SumoCommand getCurrentTime(){
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.VAR_TIME_STEP, "", Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * getDeltaT
	 */

	public static SumoCommand getDeltaT(){
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.VAR_DELTA_T, "", Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * getDepartedIDList
	 */

	public static SumoCommand getDepartedIDList(){
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.VAR_DEPARTED_VEHICLES_IDS, "", Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * getDepartedNumber
	 */

	public static SumoCommand getDepartedNumber(){
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.VAR_DEPARTED_VEHICLES_NUMBER, "", Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * getDistance2D
	 */

	public static SumoCommand getDistance2D(double x1, double y1, double x2, double y2, String isGeo, String isDriving){
		Object[] array = new Object[]{y1, x2, y2, isGeo, isDriving};
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.POSITION_LAT_LON, x1, array, Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the subscription results for the last time step.
	 */

	public static SumoCommand getDistanceRoad(String edgeID1, double pos1, String edgeID2, double pos2, String isDriving){
		Object[] array = new Object[]{pos1, edgeID2, pos2, isDriving};
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.REQUEST_DRIVINGDIST, edgeID1, array, Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * getEndingTeleportIDList
	 */

	public static SumoCommand getEndingTeleportIDList(){
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.VAR_TELEPORT_ENDING_VEHICLES_IDS, "", Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * getEndingTeleportNumber
	 */

	public static SumoCommand getEndingTeleportNumber(){
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.VAR_TELEPORT_ENDING_VEHICLES_NUMBER, "", Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * getLoadedIDList
	 */

	public static SumoCommand getLoadedIDList(){
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.VAR_LOADED_VEHICLES_IDS, "", Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * getLoadedNumber
	 */

	public static SumoCommand getLoadedNumber(){
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.VAR_LOADED_VEHICLES_NUMBER, "", Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * getMinExpectedNumber
	 */

	public static SumoCommand getMinExpectedNumber(){
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.VAR_MIN_EXPECTED_VEHICLES, "", Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * getNetBoundary
	 */

	public static SumoCommand getNetBoundary(){
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.VAR_NET_BOUNDING_BOX, "", Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * getStartingTeleportIDList
	 */

	public static SumoCommand getStartingTeleportIDList(){
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.VAR_TELEPORT_STARTING_VEHICLES_IDS, "", Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * getStartingTeleportNumber
	 */

	public static SumoCommand getStartingTeleportNumber(){
		return new SumoCommand(Constants.CMD_GET_SIM_VARIABLE, Constants.VAR_TELEPORT_STARTING_VEHICLES_NUMBER, "", Constants.RESPONSE_GET_SIM_VARIABLE, Constants.TYPE_INTEGER);
	}

	//setter methods


}
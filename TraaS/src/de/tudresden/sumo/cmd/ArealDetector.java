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
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_AREAL_DETECTOR_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_AREAL_DETECTOR_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the number of the areal detectors in the network.
	 */

	public static SumoCommand getIDCount(){
		return new SumoCommand(Constants.CMD_GET_AREAL_DETECTOR_VARIABLE, Constants.ID_COUNT, "", Constants.RESPONSE_GET_AREAL_DETECTOR_VARIABLE, Constants.TYPE_INTEGER);
	}

	
	/**
	 * getJamLengthVehicle
	 */

	public static SumoCommand getJamLengthVehicle(String loopID){
		return new SumoCommand(Constants.CMD_GET_AREAL_DETECTOR_VARIABLE, Constants.JAM_LENGTH_VEHICLE, loopID, Constants.RESPONSE_GET_AREAL_DETECTOR_VARIABLE, Constants.TYPE_INTEGER);
	}
	
	/**
	 * getJamLengthMeters
	 */

	public static SumoCommand getJamLengthMeters(String loopID){
		return new SumoCommand(Constants.CMD_GET_AREAL_DETECTOR_VARIABLE, Constants.JAM_LENGTH_METERS, loopID, Constants.RESPONSE_GET_AREAL_DETECTOR_VARIABLE, Constants.TYPE_DOUBLE);
	}
	

}
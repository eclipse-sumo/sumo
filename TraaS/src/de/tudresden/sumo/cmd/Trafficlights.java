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
import de.tudresden.ws.container.SumoTLSLogic;

public class Trafficlights {

	//getter methods

	/**
	 * getCompleteRedYellowGreenDefinition
	 */

	public static SumoCommand getCompleteRedYellowGreenDefinition(String tlsID){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.TL_COMPLETE_DEFINITION_RYG, tlsID, Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_COMPOUND);
	}

	/**
	 * getControlledLanes
	 */

	public static SumoCommand getControlledLanes(String tlsID){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.TL_CONTROLLED_LANES, tlsID, Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * getControlledLinks
	 */

	public static SumoCommand getControlledLinks(String tlsID){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.TL_CONTROLLED_LINKS, tlsID, Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns a list of all traffic lights in the network.
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the subscription results for the last time step and the given traffic light.
	 */

	public static SumoCommand getNextSwitch(String tlsID){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.TL_NEXT_SWITCH, tlsID, Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * getPhase
	 */

	public static SumoCommand getPhase(String tlsID){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.TL_CURRENT_PHASE, tlsID, Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * getProgram
	 */

	public static SumoCommand getProgram(String tlsID){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.TL_CURRENT_PROGRAM, tlsID, Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * getRedYellowGreenState
	 */

	public static SumoCommand getRedYellowGreenState(String tlsID){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.TL_RED_YELLOW_GREEN_STATE, tlsID, Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_STRING);
	}

	//setter methods

	/**
	 * setCompleteRedYellowGreenDefinition
	 */

	public static SumoCommand setCompleteRedYellowGreenDefinition(String tlsID, SumoTLSLogic tls){

		return new SumoCommand(Constants.CMD_SET_TL_VARIABLE, Constants.TL_COMPLETE_PROGRAM_RYG, tlsID, tls);
	}

	/**
	 * setPhase
	 */

	public static SumoCommand setPhase(String tlsID, int index){

		return new SumoCommand(Constants.CMD_SET_TL_VARIABLE, Constants.TL_PHASE_INDEX, tlsID, index);
	}

	/**
	 * setPhaseDuration
	 */

	public static SumoCommand setPhaseDuration(String tlsID, int phaseDuration){

		return new SumoCommand(Constants.CMD_SET_TL_VARIABLE, Constants.TL_PHASE_DURATION, tlsID, phaseDuration);
	}

	/**
	 * setProgram
	 */

	public static SumoCommand setProgram(String tlsID, String programID){

		return new SumoCommand(Constants.CMD_SET_TL_VARIABLE, Constants.TL_PROGRAM, tlsID, programID);
	}

	/**
	 * setRedYellowGreenState
	 */

	public static SumoCommand setRedYellowGreenState(String tlsID, String state){

		return new SumoCommand(Constants.CMD_SET_TL_VARIABLE, Constants.TL_RED_YELLOW_GREEN_STATE, tlsID, state);
	}


}
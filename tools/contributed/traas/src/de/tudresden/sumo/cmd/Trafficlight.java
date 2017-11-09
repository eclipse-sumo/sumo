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
import de.tudresden.ws.container.SumoTLSProgram;

/**
 * 
 * @author Mario Krumnow
 * @author Evamarie Wiessner
 *
 */


public class Trafficlight {

	//getter methods

	/**
	 * Returns the complete traffic light program.
	 * 
	 * @param tlsID a string identifying the traffic light
	 * @return program definition
	 */

	public static SumoCommand getCompleteRedYellowGreenDefinition(String tlsID){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.TL_COMPLETE_DEFINITION_RYG, tlsID, Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_COMPOUND);
	}

	
	/**
	 * Returns the number of all traffic lights in the network.
	 * @return number of traffic lights
	 */
	
	public static SumoCommand getIDCount(){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.ID_COUNT, "", Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_INTEGER);
	}
	
	/**
	 * getPhaseDuration
	 * @param tlsID a string identifying the traffic light
	 * @return phase duration
	 */

	public static SumoCommand getPhaseDuration(String tlsID){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.TL_PHASE_DURATION, tlsID, Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_INTEGER);
	}
	
	/**
	 * Returns the list of lanes which are controlled by the named traffic light.
	 * 
	 * @param tlsID a string identifying the traffic light
	 * @return list of controlled lanes
	 */
	
	
	public static SumoCommand getControlledLanes(String tlsID){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.TL_CONTROLLED_LANES, tlsID, Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the links controlled by the traffic light, sorted by the signal index and described by giving the incoming, outgoing, and via lane.
	 * 
	 * @param tlsID a string identifying the traffic light
	 * @return list of controlled links
	 */

	public static SumoCommand getControlledLinks(String tlsID){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.TL_CONTROLLED_LINKS, tlsID, Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_COMPOUND);
	}

	/**
	 * Returns the external state
	 * 
	 * @param tlsID a string identifying the traffic light
	 * @return compound of the external state
	 */

	public static SumoCommand getExternalState(String tlsID){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.TL_EXTERNAL_STATE, tlsID, Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_COMPOUND);
	}
	
	/**
	 * Returns a list of IDs of all traffic lights within the scenario.
	 * 
	 * @return a list of IDs of all TLS
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the assumed time (in ms) at which the TLS changes the phase.
	 * 
	 * @param tlsID a string identifying the traffic light
	 * @return absolute time, counting from simulation start, at which TLS may change
	 */

	public static SumoCommand getNextSwitch(String tlsID){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.TL_NEXT_SWITCH, tlsID, Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * Returns the chosen parameter
	 *
	 *  @param tlsID a string identifying the traffic light
	 *  @param param a string identifying the parameter
	 *  
	 * @return the specific parameter
	 */

	public static SumoCommand getParameter(String tlsID, String param){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.VAR_PARAMETER, tlsID, Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_STRING);
	}
	
	/**
	 * Returns the index of the current phase in the current program.
	 * 
	 * @param tlsID a string identifying the traffic light
	 * @return index of the current phase
	 */

	public static SumoCommand getPhase(String tlsID){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.TL_CURRENT_PHASE, tlsID, Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * Returns the id of the current program.
	 * 
	 * @param tlsID a string identifying the traffic light
	 * @return index of the current program
	 */

	public static SumoCommand getProgram(String tlsID){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.TL_CURRENT_PROGRAM, tlsID, Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns the named tl's state as a tuple of light definitions from rRgGyYoO, for red, green, yellow, off, where lower case letters mean that the stream has to decelerate.
	 * 
	 * @param tlsID a string identifying the traffic light
	 * @return SumoCommand
	 */

	public static SumoCommand getRedYellowGreenState(String tlsID){
		return new SumoCommand(Constants.CMD_GET_TL_VARIABLE, Constants.TL_RED_YELLOW_GREEN_STATE, tlsID, Constants.RESPONSE_GET_TL_VARIABLE, Constants.TYPE_STRING);
	}

	//setter methods

	/**
	 * Set the complete traffic light program.
	 * 
	 * @param tlsID a string identifying the traffic light
	 * @param tls complete program definition
	 * @return SumoCommand
	 */

	public static SumoCommand setCompleteRedYellowGreenDefinition(String tlsID, SumoTLSProgram tls){
		return new SumoCommand(Constants.CMD_SET_TL_VARIABLE, Constants.TL_COMPLETE_PROGRAM_RYG, tlsID, tls);
	}

	/**
	 * Set the index of the current phase in the current program.
	 * 
	 * @param tlsID a string identifying the traffic light
	 * @param index an integer identifying the phase (it must be between 0 and the number of phases known to the current program of the tls - 1)
	 * @return SumoCommand
	 */

	public static SumoCommand setPhase(String tlsID, int index){

		return new SumoCommand(Constants.CMD_SET_TL_VARIABLE, Constants.TL_PHASE_INDEX, tlsID, index);
	}

	/**
	 * Set the duration of the currently active phase (in s?).
	 * 
	 * @param tlsID a string identifying the traffic light
	 * @param phaseDuration remaining duration of the current phase
	 * @return SumoCommand
	 */

	public static SumoCommand setPhaseDuration(String tlsID, int phaseDuration){

		return new SumoCommand(Constants.CMD_SET_TL_VARIABLE, Constants.TL_PHASE_DURATION, tlsID, phaseDuration);
	}

	/**
	 * Set the id of the current program.
	 * 
	 * @param tlsID a string identifying the traffic light
	 * @param programID a string identifying the program
	 * @return SumoCommand
	 */

	public static SumoCommand setProgram(String tlsID, String programID){
		return new SumoCommand(Constants.CMD_SET_TL_VARIABLE, Constants.TL_PROGRAM, tlsID, programID);
	}

	/**
	 * Set the named tl's state as a tuple of light definitions.
	 * 
	 * @param tlsID a string identifying the traffic light
	 * @param state as a tuple of light definitions from rRgGyYoO, for red, green, yellow, off, where lower case letters mean that the stream has to decelerate
	 * @return SumoCommand
	 */

	public static SumoCommand setRedYellowGreenState(String tlsID, String state){
		return new SumoCommand(Constants.CMD_SET_TL_VARIABLE, Constants.TL_RED_YELLOW_GREEN_STATE, tlsID, state);
	}

}
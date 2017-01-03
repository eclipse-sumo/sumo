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
 *
 */

public class Person {

	//getter methods

	
	/**
	 * Returns the number of all persons in the network.
	 * @return the number of persons in the network
	 */

	public static SumoCommand getIDCount(){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.ID_COUNT, "", Constants.RESPONSE_GET_PERSON_VARIABLE, Constants.TYPE_INTEGER);
	}
	
	
	/**
	 * Returns a list of personIDs of all persons
	 * 
	 * @return list of IDs of all persons
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_PERSON_VARIABLE, Constants.TYPE_STRINGLIST);
	}
	
	/**
	 * If the person is walking, returns the next edge on the persons route (including crossing and walkingareas). If there is no further edge or the person is in another stage, returns the empty string.
	 * 
	 * @param personID
	 *            a string personIDentifying the person
	 * @return next edge
	 */

	public static SumoCommand getNextEdge(String personID){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_NEXT_EDGE, personID, Constants.RESPONSE_GET_PERSON_VARIABLE, Constants.TYPE_STRING);
	}
	


	/**
	 * Returns the chosen parameter
	 *
	 *  @param personID a string personIDentifying the person
	 *  @param param a string personIDentifying the parameter
	 *  
	 * @return the specific parameter
	 */

	public static SumoCommand getParameter(String personID, String param){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_PARAMETER, personID, Constants.RESPONSE_GET_PERSON_VARIABLE, Constants.TYPE_STRING);
	}
	
	/**
	 * get speed
	 * 
	 * @param personID
	 *            a string personIDentifying the person
	 * @return the speed in m/s
	 */
	
	public static SumoCommand getSpeed(String personID){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_SPEED, personID, Constants.RESPONSE_GET_PERSON_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * get position
	 * 
	 * @param personID
	 *            a string personIDentifying the person
	 * @return the position 2D
	 */
	
	public static SumoCommand getPosition(String personID){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_POSITION, personID, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.POSITION_2D);
	}
	
	/**
	 * get position3D
	 * 
	 * @param personID
	 *            a string personIDentifying the person
	 * @return the position 3D
	 */
	
	public static SumoCommand getPosition3D(String personID){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_POSITION3D, personID, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.POSITION_3D);
	}
	
	/**
	 * get angle
	 * 
	 * @param personID
	 *            a string personIDentifying the person
	 * @return the angle
	 */
	
	public static SumoCommand getAngle(String personID){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_ANGLE, personID, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_DOUBLE);
	}
	
	/**
	 * get road ID
	 * 
	 * @param personID
	 *            a string personIDentifying the person
	 * @return the personID of the road
	 */

	public static SumoCommand getRoadID(String personID){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_ROAD_ID, personID, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_STRING);
	}

	/**
	 * get type ID
	 * 
	 * @param personID
	 *            a string personIDentifying the person
	 * @return the type ID of the person
	 */
	
	public static SumoCommand getTypeID(String personID){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_TYPE, personID, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_STRING);
	}
	
	/**
	 * get lane position 
	 * 	 
	 * @param personID
	 *            a string personIDentifying the person
	 * @return the position on the lane
	 */
	
	public static SumoCommand getLanePosition(String personID){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_LANEPOSITION, personID, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_DOUBLE);
	}
	
	/**
	 * get color 
	 * 	 
	 * @param personID
	 *            a string personIDentifying the person
	 * @return the color of the person in the GUI
	 */
	
	public static SumoCommand getColor(String personID){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_COLOR, personID, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_COLOR);
	}
	
	
	/**
	 * get person number
	 * 	 
	 * @param personID
	 *            a string personIDentifying the person
	 * @return the person number
	 */
	
	public static SumoCommand getPersonNumber(String personID){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_PERSON_NUMBER, personID, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_INTEGER);
	}
	
	/**
	 * get length
	 * 	 
	 * @param personID
	 *            a string personIDentifying the person
	 * @return the length of the person
	 */
	
	public static SumoCommand getLength(String personID){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_LENGTH, personID, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_DOUBLE);
	}
	
	/**
	 * get waiting time
	 * 	 
	 * @param personID
	 *            a string personIDentifying the person
	 * @return the waiting time of the person
	 */
	
	
	public static SumoCommand getWaitingTime(String personID){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_WAITING_TIME, personID, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_DOUBLE);
	}
	
	/**
	 * get wpersonIDth
	 * 	 
	 * @param personID
	 *            a string identifying the person
	 * @return the width of the person
	 */
	
	public static SumoCommand getWidth(String personID){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_WIDTH, personID, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_DOUBLE);
	}
	
	/**
	 * get minGap
	 * 	 
	 * @param personID
	 *            a string personIDentifying the person
	 * @return the value for the minimum Ggap of the person
	 */
	
	public static SumoCommand getMinGap(String personID){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_MINGAP, personID, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_DOUBLE);
	}
	
}
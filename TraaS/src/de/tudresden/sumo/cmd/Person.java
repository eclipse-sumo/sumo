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
	 * Returns a list of ids of all persons
	 * 
	 * @return list of IDs of all persons
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_PERSON_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * get speed
	 * 
	 * @param id
	 *            a string identifying the person
	 * @return the speed in m/s
	 */
	
	public static SumoCommand getSpeed(String id){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_SPEED, id, Constants.RESPONSE_GET_PERSON_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * get position
	 * 
	 * @param id
	 *            a string identifying the person
	 * @return the position 2D
	 */
	
	public static SumoCommand getPosition(String id){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_POSITION, id, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.POSITION_2D);
	}
	
	/**
	 * get position3D
	 * 
	 * @param id
	 *            a string identifying the person
	 * @return the position 3D
	 */
	
	public static SumoCommand getPosition3D(String id){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_POSITION3D, id, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.POSITION_3D);
	}
	
	/**
	 * get angle
	 * 
	 * @param id
	 *            a string identifying the person
	 * @return the angle
	 */
	
	public static SumoCommand getAngle(String id){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_ANGLE, id, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_DOUBLE);
	}
	
	/**
	 * get road ID
	 * 
	 * @param id
	 *            a string identifying the person
	 * @return the id of the road
	 */

	public static SumoCommand getRoadID(String id){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_ROAD_ID, id, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_STRING);
	}

	/**
	 * get type ID
	 * 
	 * @param id
	 *            a string identifying the person
	 * @return the type ID of the person
	 */
	
	public static SumoCommand getTypeID(String id){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_TYPE, id, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_STRING);
	}
	
	/**
	 * get lane position 
	 * 	 
	 * @param id
	 *            a string identifying the person
	 * @return the position on the lane
	 */
	
	public static SumoCommand getLanePosition(String id){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_LANEPOSITION, id, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_DOUBLE);
	}
	
	/**
	 * get color 
	 * 	 
	 * @param id
	 *            a string identifying the person
	 * @return the color of the person in the GUI
	 */
	
	public static SumoCommand getColor(String id){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_COLOR, id, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_COLOR);
	}
	
	
	/**
	 * get person number
	 * 	 
	 * @param id
	 *            a string identifying the person
	 * @return the person number
	 */
	
	public static SumoCommand getPersonNumber(String id){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_PERSON_NUMBER, id, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_INTEGER);
	}
	
	/**
	 * get length
	 * 	 
	 * @param id
	 *            a string identifying the person
	 * @return the length of the person
	 */
	
	public static SumoCommand getLength(String id){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_LENGTH, id, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_DOUBLE);
	}
	
	/**
	 * get waiting time
	 * 	 
	 * @param id
	 *            a string identifying the person
	 * @return the waiting time of the person
	 */
	
	
	public static SumoCommand getWaitingTime(String id){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_WAITING_TIME, id, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_DOUBLE);
	}
	
	/**
	 * get width
	 * 	 
	 * @param id
	 *            a string identifying the person
	 * @return the width of the person
	 */
	
	public static SumoCommand getWidth(String id){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_WIDTH, id, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_DOUBLE);
	}
	
	/**
	 * get minGap
	 * 	 
	 * @param id
	 *            a string identifying the person
	 * @return the value for the minimum Ggap of the person
	 */
	
	public static SumoCommand getMinGap(String id){
		return new SumoCommand(Constants.CMD_GET_PERSON_VARIABLE, Constants.VAR_MINGAP, id, Constants.RESPONSE_GET_PERSON_VARIABLE,Constants.TYPE_DOUBLE);
	}
	
	//setter methods


	/* not yet implemented!

	public static SumoCommand add(String id, String typeID, SumoStringList edges, int depart, double pos, double speed){

		Object[] array = new Object[]{typeID, edges, depart, pos, speed};
		return new SumoCommand(Constants.CMD_SET_PERSON_VARIABLE, Constants.ADD, id, array);
	}
	
	public static SumoCommand setColor(String id, SumoColor color){
		return new SumoCommand(Constants.CMD_SET_PERSON_VARIABLE, Constants.VAR_COLOR, id, color);
	}
	
	public static SumoCommand setSpeed(String id, double  speed){
		return new SumoCommand(Constants.CMD_SET_PERSON_VARIABLE, Constants.VAR_SPEED, id, speed);
	}
	
	public static SumoCommand remove(String id, byte reason){
		return new SumoCommand(Constants.CMD_SET_PERSON_VARIABLE, Constants.REMOVE, id, reason);
	}
	
	*/
}
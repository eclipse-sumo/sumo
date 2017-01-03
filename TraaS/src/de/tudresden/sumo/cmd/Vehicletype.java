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
import de.tudresden.ws.container.SumoColor;

/**
 * 
 * @author Mario Krumnow
 * @author Evamarie Wiessner
 *
 */

public class Vehicletype {

	//getter methods

	/**
	 * Returns the maximum acceleration of the named vehicle class.
	 * @param typeID  type id
	 * @return acceleration
	 */
	public static SumoCommand getAccel(String typeID){
		return new SumoCommand(Constants.CMD_GET_VEHICLETYPE_VARIABLE, Constants.VAR_ACCEL, typeID, Constants.RESPONSE_GET_VEHICLETYPE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the number of all vehicle types in the network.
	 * @return number of vehicle types 
	 */

	public static SumoCommand getIDCount(){
		return new SumoCommand(Constants.CMD_GET_VEHICLETYPE_VARIABLE, Constants.ID_COUNT, "", Constants.RESPONSE_GET_VEHICLETYPE_VARIABLE, Constants.TYPE_INTEGER);
	} 
	
	/**
	 *  Returns the vehicle type's color.
	 * @param typeID  type id
	 * @return color color
	 */
	public static SumoCommand getColor(String typeID){
		return new SumoCommand(Constants.CMD_GET_VEHICLETYPE_VARIABLE, Constants.VAR_COLOR, typeID, Constants.RESPONSE_GET_VEHICLETYPE_VARIABLE, Constants.TYPE_COLOR);
	}

	/**
	 * Returns the maximum deceleration of the names vehicle type.
	 * @param typeID  type id
	 * @return deceleration
	 */

	public static SumoCommand getDecel(String typeID){
		return new SumoCommand(Constants.CMD_GET_VEHICLETYPE_VARIABLE, Constants.VAR_DECEL, typeID, Constants.RESPONSE_GET_VEHICLETYPE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the emission class of the named vehicle type.
	 * @param typeID  type id
	 * @return emission class
	 */

	public static SumoCommand getEmissionClass(String typeID){
		return new SumoCommand(Constants.CMD_GET_VEHICLETYPE_VARIABLE, Constants.VAR_EMISSIONCLASS, typeID, Constants.RESPONSE_GET_VEHICLETYPE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns a list of all known vehicle types.
	 * @return list of vechicle types
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_VEHICLETYPE_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_VEHICLETYPE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * getImperfection
	 * @param typeID type id
	 * @return imperfection
	 */

	public static SumoCommand getImperfection(String typeID){
		return new SumoCommand(Constants.CMD_GET_VEHICLETYPE_VARIABLE, Constants.VAR_IMPERFECTION, typeID, Constants.RESPONSE_GET_VEHICLETYPE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the length of the named vehicle type.
	 * @param typeID type id
	 * @return length
	 */

	public static SumoCommand getLength(String typeID){
		return new SumoCommand(Constants.CMD_GET_VEHICLETYPE_VARIABLE, Constants.VAR_LENGTH, typeID, Constants.RESPONSE_GET_VEHICLETYPE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Return the maximum speed of the named vehicle type.
	 * @param typeID type id
	 * @return max speed
	 */

	public static SumoCommand getMaxSpeed(String typeID){
		return new SumoCommand(Constants.CMD_GET_VEHICLETYPE_VARIABLE, Constants.VAR_MAXSPEED, typeID, Constants.RESPONSE_GET_VEHICLETYPE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the minimum headway gap of the named vehicle type.
	 * @param typeID type id
	 * @return min gap
	 */

	public static SumoCommand getMinGap(String typeID){
		return new SumoCommand(Constants.CMD_GET_VEHICLETYPE_VARIABLE, Constants.VAR_MINGAP, typeID, Constants.RESPONSE_GET_VEHICLETYPE_VARIABLE, Constants.TYPE_DOUBLE);
	}
	
	/**
	 * Returns the chosen parameter
	 *
	 *  @param typeID a string identifying the vehicle type
	 *  @param param a string identifying the parameter
	 *  
	 * @return the specific parameter
	 */

	public static SumoCommand getParameter(String typeID, String param){
		return new SumoCommand(Constants.CMD_GET_VEHICLETYPE_VARIABLE, Constants.VAR_PARAMETER, typeID, Constants.RESPONSE_GET_VEHICLETYPE_VARIABLE, Constants.TYPE_STRING);
	}
	

	/**
	 * getShapeClass
	 * @param typeID type id
	 * @return shape class
	 */

	public static SumoCommand getShapeClass(String typeID){
		return new SumoCommand(Constants.CMD_GET_VEHICLETYPE_VARIABLE, Constants.VAR_SHAPECLASS, typeID, Constants.RESPONSE_GET_VEHICLETYPE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns the speed deviation of the named vehicle type.
	 * @param typeID type id
	 * @return speed deviation
	 */

	public static SumoCommand getSpeedDeviation(String typeID){
		return new SumoCommand(Constants.CMD_GET_VEHICLETYPE_VARIABLE, Constants.VAR_SPEED_DEVIATION, typeID, Constants.RESPONSE_GET_VEHICLETYPE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Return the speed factor of the names vehicle type.
	 * @param typeID type id
	 * @return speed factor
	 */

	public static SumoCommand getSpeedFactor(String typeID){
		return new SumoCommand(Constants.CMD_GET_VEHICLETYPE_VARIABLE, Constants.VAR_SPEED_FACTOR, typeID, Constants.RESPONSE_GET_VEHICLETYPE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * getTau
	 * @param typeID type id
	 * @return tau
	 */

	public static SumoCommand getTau(String typeID){
		return new SumoCommand(Constants.CMD_GET_VEHICLETYPE_VARIABLE, Constants.VAR_TAU, typeID, Constants.RESPONSE_GET_VEHICLETYPE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the vehicle class of the names vehicle type.
	 * @param typeID type id
	 * @return vehicle class
	 */

	public static SumoCommand getVehicleClass(String typeID){
		return new SumoCommand(Constants.CMD_GET_VEHICLETYPE_VARIABLE, Constants.VAR_VEHICLECLASS, typeID, Constants.RESPONSE_GET_VEHICLETYPE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns the width of the named vehicle type.
	 * @param typeID type id
	 * @return width
	 */

	public static SumoCommand getWidth(String typeID){
		return new SumoCommand(Constants.CMD_GET_VEHICLETYPE_VARIABLE, Constants.VAR_WIDTH, typeID, Constants.RESPONSE_GET_VEHICLETYPE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	//setter methods

	/**
	 * Sets the vehicle type's wished maximum acceleration to the given value.
	 * @param typeID type id
	 * @param accel acceleration
	 * @return SumoCommand
	 */
	public static SumoCommand setAccel(String typeID, double accel){

		return new SumoCommand(Constants.CMD_SET_VEHICLETYPE_VARIABLE, Constants.VAR_ACCEL, typeID, accel);
	}

	/**
	 * Sets the vehicle type's color.
	 * @param typeID type id
	 * @param color color
	 * @return SumoCommand
	 */
	public static SumoCommand setColor(String typeID, SumoColor color){

		return new SumoCommand(Constants.CMD_SET_VEHICLETYPE_VARIABLE, Constants.VAR_COLOR, typeID, color);
	}

	/**
	 * Sets the vehicle type's wished maximum deceleration to the given value.

	 * @param typeID type id
	 * @param decel deceleration
	 * @return SumoCommand
	 */
	public static SumoCommand setDecel(String typeID, double decel){

		return new SumoCommand(Constants.CMD_SET_VEHICLETYPE_VARIABLE, Constants.VAR_DECEL, typeID, decel);
	}

	/**
	 * Sets the vehicle type's emission class to the given value.
	 * @param typeID type id
	 * @param clazz emission class
	 * @return SumoCommand
	 */
	public static SumoCommand setEmissionClass(String typeID, String clazz){

		return new SumoCommand(Constants.CMD_SET_VEHICLETYPE_VARIABLE, Constants.VAR_EMISSIONCLASS, typeID, clazz);
	}

	/**
	 * setImperfection
	 * @param typeID type id
	 * @param imperfection imperfection
	 * @return SumoCommand
	 */
	public static SumoCommand setImperfection(String typeID, double imperfection){

		return new SumoCommand(Constants.CMD_SET_VEHICLETYPE_VARIABLE, Constants.VAR_IMPERFECTION, typeID, imperfection);
	}

	/**
	 * Sets the vehicle type's length to the given value.
	 * @param typeID type id
	 * @param length length
	 * @return SumoCommand
	 */
	public static SumoCommand setLength(String typeID, double length){
		return new SumoCommand(Constants.CMD_SET_VEHICLETYPE_VARIABLE, Constants.VAR_LENGTH, typeID, length);
	}

	/**
	 * Sets the vehicle type's maximum speed to the given value.
	 * @param typeID type id
	 * @param speed speed
	 * @return SumoCommand
	 */
	
	public static SumoCommand setMaxSpeed(String typeID, double speed){
	return new SumoCommand(Constants.CMD_SET_VEHICLETYPE_VARIABLE, Constants.VAR_MAXSPEED, typeID, speed);
	}

	/**
	 * Sets the vehicle type's minimum headway gap to the given value.
	 * @param typeID type id
	 * @param minGap minimum gap
	 * @return SumoCommand
	 */
	public static SumoCommand setMinGap(String typeID, double minGap){
		return new SumoCommand(Constants.CMD_SET_VEHICLETYPE_VARIABLE, Constants.VAR_MINGAP, typeID, minGap);
	}

	/**
	 * setShapeClass
	 * @param typeID type id
	 * @param clazz shape class
	 * @return SumoCommand
	 */
	public static SumoCommand setShapeClass(String typeID, String clazz){
		return new SumoCommand(Constants.CMD_SET_VEHICLETYPE_VARIABLE, Constants.VAR_SHAPECLASS, typeID, clazz);
	}

	/**
	 * Sets the vehicle type's speed deviation to the given value.
	 * @param typeID type id
	 * @param deviation deviation
	 * @return SumoCommand
	 */
	public static SumoCommand setSpeedDeviation(String typeID, double deviation){
		return new SumoCommand(Constants.CMD_SET_VEHICLETYPE_VARIABLE, Constants.VAR_SPEED_DEVIATION, typeID, deviation);
	}

	/**
	 * Sets the vehicle type's speed factor to the given value.
	 * @param typeID type id
	 * @param factor factor
	 * @return SumoCommand
	 */
	public static SumoCommand setSpeedFactor(String typeID, double factor){
		return new SumoCommand(Constants.CMD_SET_VEHICLETYPE_VARIABLE, Constants.VAR_SPEED_FACTOR, typeID, factor);
	}

	/**
	 * setTau
	 * @param typeID type id
	 * @param tau tau
	 * @return SumoCommand
	 */
	public static SumoCommand setTau(String typeID, double tau){
		return new SumoCommand(Constants.CMD_SET_VEHICLETYPE_VARIABLE, Constants.VAR_TAU, typeID, tau);
	}

	/**
	 * Sets the vehicle type's vehicle class to the given value.
	 * @param typeID type id
	 * @param clazz vehicle class
	 * @return SumoCommand
	 */
	public static SumoCommand setVehicleClass(String typeID, String clazz){
		return new SumoCommand(Constants.CMD_SET_VEHICLETYPE_VARIABLE, Constants.VAR_VEHICLECLASS, typeID, clazz);
	}

	/**
	 * Sets the vehicle type's width to the given value.
	 * @param typeID type id
	 * @param width width
	 * @return SumoCommand
	 */
	public static SumoCommand setWidth(String typeID, double width){

		return new SumoCommand(Constants.CMD_SET_VEHICLETYPE_VARIABLE, Constants.VAR_WIDTH, typeID, width);
	}


}
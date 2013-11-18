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
import de.tudresden.ws.container.SumoColor;
import de.tudresden.ws.container.SumoStringList;

/**
 * 
 * @author Mario Krumnow
 * @author Evamarie Wiessner
 *
 */

public class Vehicle {

	//getter methods

	/**
	 * Returns the acceleration of the named vehicle within the last step.
	 */

	public static SumoCommand getAccel(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_ACCEL, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the number of all vehicles in the network.
	 */

	public static SumoCommand getIDCount(){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.ID_COUNT, "", Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_INTEGER);
	}
	
	
	/**
	 * Returns the edge travel time for the given time as stored in the vehicle's internal container.
	 */

	public static SumoCommand getAdaptedTraveltime(String vehID, double time, String edgeID){
		Object[] array = new Object[]{time, edgeID};
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_EDGE_TRAVELTIME, vehID, array, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the angle of the named vehicle within the last time step.
	 */

	public static SumoCommand getAngle(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_ANGLE, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	
	/**
	 * Returns the maximum allowed speed on the current lane regarding speed factor in m/s for this vehicle.
	 */

	public static SumoCommand getAllowedSpeed(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_ALLOWED_SPEED, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}
	
	
	/**
	 * getPersonNumber
	 */

	public static SumoCommand getPersonNumber(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_PERSON_NUMBER, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_INTEGER);
	}
	

	/**
	 * getBestLanes
	 */

	public static SumoCommand getBestLanes(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_BEST_LANES, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the CO2 emission in mg for the last time step.
	 */

	public static SumoCommand getCO2Emission(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_CO2EMISSION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the CO emission in mg for the last time step.
	 */

	public static SumoCommand getCOEmission(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_COEMISSION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the vehicle's color (RGBA).
	 */

	public static SumoCommand getColor(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_COLOR, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_COLOR);
	}

	/**
	 * Returns the deceleration of the named vehicle within the last time step.
	 */

	public static SumoCommand getDecel(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_DECEL, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the driving distance (in m) of the named vehicle within the last tine step.
	 */

	public static SumoCommand getDrivingDistance(String vehID, String edgeID, double pos, int laneID){
		Object[] array = new Object[]{edgeID, pos, laneID};
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.DISTANCE_REQUEST, vehID, array, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * getDrivingDistance2D
	 */

	public static SumoCommand getDrivingDistance2D(String vehID, double x, double y){
		Object[] array = new Object[]{x, y};
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.DISTANCE_REQUEST, vehID, array, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Get the distance to the starting point acts as a odometer 
	 */

	public static SumoCommand getDistance(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_DISTANCE, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	
	

	/**
	 * Returns the edge effort for the given time as stored in the vehicle's internal container.
	 */

	public static SumoCommand getEffort(String vehID, double time, String edgeID){
		Object[] array = new Object[]{time, edgeID};
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_EDGE_EFFORT, vehID, array, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the emission class of the named vehicle.
	 */

	public static SumoCommand getEmissionClass(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_EMISSIONCLASS, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns the fuel consumption in ml for the last time step.
	 */

	public static SumoCommand getFuelConsumption(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_FUELCONSUMPTION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the HC emission (in mg) during the last time step.
	 */

	public static SumoCommand getHCEmission(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_HCEMISSION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns a list of ids of all vehicles currently running within the scenario.
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * getImperfection
	 */

	public static SumoCommand getImperfection(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_IMPERFECTION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the id of the lane the named vehicle was at within the last step.
	 */

	public static SumoCommand getLaneID(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_LANE_ID, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns the index of the lane the named vehicle was at within the last step.
	 */

	public static SumoCommand getLaneIndex(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_LANE_INDEX, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * Returns the position of the vehicle along the lane (in m).
	 */

	public static SumoCommand getLanePosition(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_LANEPOSITION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the length (in m) of the named vehicle.
	 */

	public static SumoCommand getLength(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_LENGTH, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the maximum speed(in m/s) of the named vehicle.
	 */

	public static SumoCommand getMaxSpeed(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_MAXSPEED, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the minimum gap (in m) between this vehicle and the vehicle before it.
	 */

	public static SumoCommand getMinGap(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_MINGAP, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the NOx emissions (in mg) generated by the vehicle during the last time step.
	 */

	public static SumoCommand getNOxEmission(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_NOXEMISSION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the noise emission (in db) generated by the vehicle during the last time step.
	 */

	public static SumoCommand getNoiseEmission(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_NOISEEMISSION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the particular matter emissions (in mg) generated by the vehicle during the last time step.
	 */

	public static SumoCommand getPMxEmission(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_PMXEMISSION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the position of the named vehicle within the last step [m,m].
	 */

	public static SumoCommand getPosition(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_POSITION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.POSITION_2D);
	}

	/**
	 * Returns the id of the edge the named vehicle was at within the last step.
	 */

	public static SumoCommand getRoadID(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_ROAD_ID, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns the ids of the edges the vehicle's route is made of.
	 */

	public static SumoCommand getRoute(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_EDGES, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the id of the route of the named vehicle.
	 */

	public static SumoCommand getRouteID(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_ROUTE_ID, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * getShapeClass
	 */

	public static SumoCommand getShapeClass(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_SHAPECLASS, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * getSignals
	 */

	public static SumoCommand getSignals(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_SIGNALS, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * Returns the speed (in m/s) of the named vehicle within the last step.
	 */

	public static SumoCommand getSpeed(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_SPEED, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the standard deviation of the estimated maximum speed (see speed factor) divided by this speed.
	 */

	public static SumoCommand getSpeedDeviation(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_SPEED_DEVIATION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the factor by which the driver multiplies the speed read from street signs to estimate "real" maximum allowed speed.
	 */

	public static SumoCommand getSpeedFactor(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_SPEED_FACTOR, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * getSpeedWithoutTraCI
	 */

	public static SumoCommand getSpeedWithoutTraCI(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_SPEED_WITHOUT_TRACI, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * getTau
	 */

	public static SumoCommand getTau(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_TAU, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * getWaitingTime
	 */

	public static SumoCommand getWaitingTime(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_WAITING_TIME, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}
	
	
	/**
	 * Returns the id of the type of the named vehicle.
	 */

	public static SumoCommand getTypeID(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_TYPE, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns the vehicle class.
	 */

	public static SumoCommand getVehicleClass(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_VEHICLECLASS, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns the vehicle's width (in m).
	 */

	public static SumoCommand getWidth(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_WIDTH, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	
	/**
	 * Returns whether the Route is valid.
	 */

	public static SumoCommand isRouteValid(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_ROUTE_VALID, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_UBYTE);
	}

	/**
	 * Sets the IDs of the edges the vehicle's route is made of.
	 */

	public static SumoCommand setRoute(String vehID, SumoStringList edgeList){
		Object[] array = new Object[]{edgeList};
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_ROUTE, vehID, array, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * Reduces the speed to the given for the given amount of time.
	 */

	public static SumoCommand slowDown(String vehID, double speed, int duration){
		Object[] array = new Object[]{speed, duration};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.CMD_SLOWDOWN, vehID, array, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_INTEGER);
	}

	//setter methods

	/**
	 * Add a new vehicle.
	 */

	public static SumoCommand add(String vehID, String typeID, String routeID, int depart, double pos, double speed, byte lane){

		Object[] array = new Object[]{typeID, routeID, depart, pos, speed, lane};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.ADD, vehID, array);
	}

	/**
	 * Forces a lane change to the lane with the given index; if successful, the lane will be chosen for the given amount of time.
	 */

	public static SumoCommand changeLane(String vehID, byte laneIndex, int duration){

		Object[] array = new Object[]{laneIndex, duration};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.CMD_CHANGELANE, vehID, array);
	}

	/**
	 * The vehicle's destination edge is set to the given. The route is rebuilt.
	 */

	public static SumoCommand changeTarget(String vehID, String edgeID){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.CMD_CHANGETARGET, vehID, edgeID);
	}

	/**
	 * Moves the vehicle to a new position.
	 */

	public static SumoCommand moveTo(String vehID, String laneID, double pos){

		Object[] array = new Object[]{laneID, pos};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_MOVE_TO, vehID, array);
	}

	/**
	 * moveToVTD
	 */

	public static SumoCommand moveToVTD(String vehID, String edgeID, int lane, double x, double y){

		Object[] array = new Object[]{edgeID, lane, x, y};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_MOVE_TO_VTD, vehID, array);
	}

	/**
	 * Removes vehicle with the given ID for the given reason. Reasons are defined in module constants and start with REMOVE_
	 */

	public static SumoCommand remove(String vehID, byte reason){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.REMOVE, vehID, reason);
	}

	/**
	 * Computes a new route using the vehicle's internal and the global edge effort information.
	 */

	public static SumoCommand rerouteEffort(String vehID){

		Object[] array = new Object[]{vehID};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.CMD_REROUTE_EFFORT, vehID, array);
	}

	/**
	 * Computes a new route using the vehicle's internal and the global edge travel time information.
	 */

	public static SumoCommand rerouteTraveltime(String vehID){

		Object[] array = new Object[]{vehID};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.CMD_REROUTE_TRAVELTIME, vehID, array);
	}

	/**
	 * Sets the acceleration of the named vehicle.
	 */

	public static SumoCommand setAccel(String vehID, double accel){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_ACCEL, vehID, accel);
	}

	/**
	 * Sets the edge travel time of the named vehicle.
	 */

	public static SumoCommand setAdaptedTraveltime(String vehID, int begTime, int endTime, String edgeID, double time){

		Object[] array = new Object[]{begTime, endTime, edgeID, time};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_EDGE_TRAVELTIME, vehID, array);
	}

	/**
	 * Sets the vehicle's color (RGBA).
	 */

	public static SumoCommand setColor(String vehID, SumoColor color){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_COLOR, vehID, color);
	}

	/**
	 * Sets the deceleration of the named vehicle.
	 */

	public static SumoCommand setDecel(String vehID, double decel){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_DECEL, vehID, decel);
	}

	/**
	 * Sets the vehicle's lane change mode as a bitset.
	 */

	public static SumoCommand setLaneChangeMode(String vehID, int lcm){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_LANECHANGE_MODE, vehID, lcm);
	}
	
	
	/**
	 * Sets the id of the type for the named vehicle.
	 */

	public static SumoCommand setType(String vehID, String typeID){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_TYPE, vehID, typeID);
	}

	
	
	/**
	 * Sets the edge effort for the given time as stored in the vehicle's internal container.
	 */

	public static SumoCommand setEffort(String vehID, int begTime, int endTime, String edgeID, double effort){

		Object[] array = new Object[]{begTime, endTime, edgeID, effort};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_EDGE_EFFORT, vehID, array);
	}

	/**
	 * Sets the emission class of the named vehicle.
	 */

	public static SumoCommand setEmissionClass(String vehID, String clazz){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_EMISSIONCLASS, vehID, clazz);
	}

	/**
	 * setImperfection
	 */

	public static SumoCommand setImperfection(String vehID, double imperfection){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_IMPERFECTION, vehID, imperfection);
	}

	/**
	 * Sets the length (in m) of the named vehicle.
	 */

	public static SumoCommand setLength(String vehID, double length){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_LENGTH, vehID, length);
	}

	/**
	 * Sets the maximum speed (in m/s) of the named vehicle.
	 */

	public static SumoCommand setMaxSpeed(String vehID, double speed){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_MAXSPEED, vehID, speed);
	}

	/**
	 * Sets the minimum gap (in m) between this vehicle and the vehicle before it.
	 */

	public static SumoCommand setMinGap(String vehID, double minGap){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_MINGAP, vehID, minGap);
	}

	/**
	 * Sets the id of the route of the named vehicle.
	 */

	public static SumoCommand setRouteID(String vehID, String routeID){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_ROUTE_ID, vehID, routeID);
	}

	/**
	 * setShapeClass
	 */

	public static SumoCommand setShapeClass(String vehID, String clazz){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_SHAPECLASS, vehID, clazz);
	}

	/**
	 * setSignals
	 */

	public static SumoCommand setSignals(String vehID, String signals){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_SIGNALS, vehID, signals);
	}

	/**
	 * Sets the speed (in m/s) of the named vehicle.
	 */

	public static SumoCommand setSpeed(String vehID, double speed){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_SPEED, vehID, speed);
	}

	/**
	 *  Sets the standard deviation of the estimated maximum speed.
	 */

	public static SumoCommand setSpeedDeviation(String vehID, double deviation){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_SPEED_DEVIATION, vehID, deviation);
	}

	/**
	 * Sets the factor by which the driver multiplies the speed read from street signs to estimate "real" maximum allowed speed.
	 */

	public static SumoCommand setSpeedFactor(String vehID, double factor){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_SPEED_FACTOR, vehID, factor);
	}

	/**
	 * Lets the vehicle stop at the given edge, at the given position and lane. The vehicle will stop for the given duration. Re-issuing a stop command with the same lane and position allows changing the duration.
	 */

	public static SumoCommand setStop(String vehID, String edgeID, double pos, byte laneIndex, int duration){

		Object[] array = new Object[]{edgeID, pos, laneIndex, duration};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.CMD_STOP, vehID, array);
	}

	/**
	 * setTau
	 */

	public static SumoCommand setTau(String vehID, double tau){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_TAU, vehID, tau);
	}

	/**
	 * Sets the vehicle class.
	 */

	public static SumoCommand setVehicleClass(String vehID, String clazz){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_VEHICLECLASS, vehID, clazz);
	}

	/**
	 * Sets the vehicle's width (in m).
	 */

	public static SumoCommand setWidth(String vehID, double width){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_WIDTH, vehID, width);
	}


}
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
import de.tudresden.ws.container.SumoStopFlags;
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
	 * @param vehID id of the vehicle
	 * @return acceleration
	 */

	public static SumoCommand getAccel(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_ACCEL, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the number of all vehicles in the network.
	 * @return current number of vehicles in the network
	 */

	public static SumoCommand getIDCount(){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.ID_COUNT, "", Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_INTEGER);
	}
	
	
	/**
	 * Returns the edge travel time for the given time as stored in the vehicle's internal container.
	 * @param vehID id of the vehicle
	 * @param time time
	 * @param edgeID id of the edge
	 * @return SumoCommand
	 */
	public static SumoCommand getAdaptedTraveltime(String vehID, int time, String edgeID){
		Object[] array = new Object[]{time, edgeID};
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_EDGE_TRAVELTIME, vehID, array, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the angle of the named vehicle within the last time step.
	 * @param vehID id of the vehicle
	 * @return angle
	 */

	public static SumoCommand getAngle(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_ANGLE, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	
	/**
	 * Returns the maximum allowed speed on the current lane regarding speed factor in m/s for this vehicle.
	 * @param vehID id of the vehicle
	 * @return allowed speed
	 */

	public static SumoCommand getAllowedSpeed(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_ALLOWED_SPEED, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}
	
	/**
	 * Returns the height in m of this vehicle.
	 * @param vehID id of the vehicle
     * @return height in m
	 */

	public static SumoCommand getHeight(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_HEIGHT, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}
	
	/**
	 * Return the leading vehicle id together with the distance.
      The dist parameter defines the maximum lookahead, 0 calculates a lookahead from the brake gap.
     * @param vehID id of the vehicle
     * @param dist distance
	 * @return the leading vehicle
	 */

	public static SumoCommand getLeader(String vehID, double dist){
		Object[] array = new Object[]{dist};
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_LEADER, vehID, array, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_COMPOUND);
	}

	
	/**
	 * getPersonNumber
	 * @param vehID id of the vehicle
	 * @return person number
	 */

	public static SumoCommand getPersonNumber(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_PERSON_NUMBER, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_INTEGER);
	}
	

	/**
	 * getBestLanes
	 * @param vehID id of the vehicle
	 * @return list of the best lanes
	 */

	public static SumoCommand getBestLanes(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_BEST_LANES, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_COMPOUND);
	}

	/**
	 * Returns the CO2 emission in mg for the last time step.
	 * @param vehID id of the vehicle
	 * @return value for co2
	 */

	public static SumoCommand getCO2Emission(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_CO2EMISSION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the CO emission in mg for the last time step.
 	 * @param vehID id of the vehicle
	 * @return value for co2
	 */

	public static SumoCommand getCOEmission(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_COEMISSION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the vehicle's color (RGBA).
	 * @param vehID id of the vehicle
	 * @return color
	 */

	public static SumoCommand getColor(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_COLOR, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_COLOR);
	}

	/**
	 * Returns the deceleration of the named vehicle within the last time step.
	 * @param vehID id of the vehicle
	 * @return deceleration
	 */

	public static SumoCommand getDecel(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_DECEL, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the driving distance (in m) of the named vehicle within the last tine step.
	 * @param vehID id
	 * @param edgeID edge id
	 * @param pos pos
	 * @param laneID lane id
	 * @return the distance
	 */
	public static SumoCommand getDrivingDistance(String vehID, String edgeID, double pos, byte laneID){
		Object[] array = new Object[]{edgeID, pos, laneID};
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.DISTANCE_REQUEST, vehID, array, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * getDrivingDistance2D
	 * @param vehID id of the vehicle
	 * @param x x
	 * @param y y
	 * @return the distance
	 */
	public static SumoCommand getDrivingDistance2D(String vehID, double x, double y){
		Object[] array = new Object[]{x, y};
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.DISTANCE_REQUEST, vehID, array, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Get the distance to the starting point acts as a odometer 
	 * @param vehID id of the vehicle
	 * @return the current distance of the vehicle (odometer)
	 */

	public static SumoCommand getDistance(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_DISTANCE, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	
	/**
	 * Returns the edge effort for the given time as stored in the vehicle's internal container.
	 * @param vehID id of the vehicle
	 * @param time time
	 * @param edgeID edge id
	 * @return the effort
	 */
	public static SumoCommand getEffort(String vehID, int time, String edgeID){
		Object[] array = new Object[]{time, edgeID};
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_EDGE_EFFORT, vehID, array, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the emission class of the named vehicle.
	 *  @param vehID id of the vehicle
	 *  @return emission class
	 */

	public static SumoCommand getEmissionClass(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_EMISSIONCLASS, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns the electricity consumption in ml for the last time step.
	 *  @param vehID id of the vehicle
	 *  @return energy consumption
	 */

	public static SumoCommand getElectricityConsumption(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_ELECTRICITYCONSUMPTION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}
	
	/**
	 * Returns the fuel consumption in ml for the last time step.
	 *  @param vehID id of the vehicle
	 *  @return fuel consumption
	 */

	public static SumoCommand getFuelConsumption(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_FUELCONSUMPTION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the HC emission (in mg) during the last time step.
	 * @param vehID id of the vehicle
	 * @return value for hc
	 */

	public static SumoCommand getHCEmission(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_HCEMISSION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns a list of ids of all vehicles currently running within the scenario.
	 * @return list of vehicles
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * getImperfection
	 * @param vehID id of the vehicle
	 *  @return value for imperfection
	 */

	public static SumoCommand getImperfection(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_IMPERFECTION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the id of the lane the named vehicle was at within the last step.
	 * @param vehID id of the vehicle
	 *  @return lane id
	 */

	public static SumoCommand getLaneID(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_LANE_ID, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns the index of the lane the named vehicle was at within the last step.
	 * @param vehID id of the vehicle
	 * @return lane index
	 */

	public static SumoCommand getLaneIndex(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_LANE_INDEX, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * Returns the position of the vehicle along the lane (in m).
	 * @param vehID id of the vehicle
	 * @return lane position
	 */

	public static SumoCommand getLanePosition(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_LANEPOSITION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the length (in m) of the named vehicle.
	 * @param vehID id of the vehicle
	 * @return length
	 */

	public static SumoCommand getLength(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_LENGTH, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the length (in m) of the named vehicle.
	 * @param vehID id of the vehicle
	 * @return length
	 */

	public static SumoCommand getLine(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_LINE, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRING);
	}
	
	/**
	 * Returns the maximum speed(in m/s) of the named vehicle.
	 * @param vehID id of the vehicle
	 * @return max speed
	 */

	public static SumoCommand getMaxSpeed(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_MAXSPEED, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the minimum gap (in m) between this vehicle and the vehicle before it.
	 * @param vehID id of the vehicle
	 * @return min gap
	 */

	public static SumoCommand getMinGap(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_MINGAP, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}
	
	
	/**
	 * Return list of upcoming traffic lights [(tlsID, tlsIndex, distance, state), ...]
	 * @param vehID id of the vehicle
	 * @return next traffic lights (compound)
	 */
	public static SumoCommand getNextTLS(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_NEXT_TLS, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_COMPOUND);
	}
	
	

	/**
	 * Returns the NOx emissions (in mg) generated by the vehicle during the last time step.
	 * @param vehID id of the vehicle
	 * @return value for nox
	 */

	public static SumoCommand getNOxEmission(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_NOXEMISSION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the noise emission (in db) generated by the vehicle during the last time step.
	 * @param vehID id of the vehicle
	 * @return value for noise emission
	 */

	public static SumoCommand getNoiseEmission(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_NOISEEMISSION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}
	
	/**
	 * Returns the chosen parameter
	 *
	 *  @param vehID a string identifying the vehicle
	 *  @param param a string identifying the parameter
	 *  
	 * @return the specific parameter
	 */

	public static SumoCommand getParameter(String vehID, String param){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_PARAMETER, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRING);
	}
	

	/**
	 * Returns the particular matter emissions (in mg) generated by the vehicle during the last time step.
	 * @param vehID id of the vehicle
	 * @return value for pmx
	 */

	public static SumoCommand getPMxEmission(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_PMXEMISSION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the position of the named vehicle within the last step [m,m].
	 * @param vehID id of the vehicle
	 * @return position 2D
	 */

	public static SumoCommand getPosition(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_POSITION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.POSITION_2D);
	}

	/**
	 * Returns the position of the named vehicle within the last step [m,m,m].
	 * @param vehID id of the vehicle
	 * @return position 3D
	 */

	public static SumoCommand getPosition3D(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_POSITION3D, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.POSITION_3D);
	}

	
	/**
	 * Returns the id of the edge the named vehicle was at within the last step.
	 * @param vehID id of the vehicle
	 * @return road id
	 */

	public static SumoCommand getRoadID(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_ROAD_ID, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns the ids of the edges the vehicle's route is made of.
	 * @param vehID id of the vehicle
	 * @return route
	 */

	public static SumoCommand getRoute(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_EDGES, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the id of the route of the named vehicle.
	 * @param vehID id of the vehicle
	 * @return route id
	 */

	public static SumoCommand getRouteID(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_ROUTE_ID, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 *  Returns the index of the current edge within the vehicles route or -1 if the vehicle has not yet departed
	 * @param vehID id of the vehicle
	 * @return route index
	 */

	public static SumoCommand getRouteIndex(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_ROUTE_INDEX, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_INTEGER);
	}
	
	/**
	 * getShapeClass
	 * @param vehID id of the vehicle
	 * @return shape class
	 */

	public static SumoCommand getShapeClass(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_SHAPECLASS, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * getSignals
	 * @param vehID id of the vehicle
	 * @return the signal state of the vehicle
	 */

	public static SumoCommand getSignals(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_SIGNALS, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_INTEGER);
	}

	/**
	 * getSlope
	 * @param vehID id of the vehicle
	 * @return  the slope at the current position of the vehicle in degrees
	 */

	public static SumoCommand getSlope(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_SLOPE, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	
	/**
	 * Returns the speed (in m/s) of the named vehicle within the last step.
	 * @param vehID id of the vehicle
	 * @return speed in m/s
	 */

	public static SumoCommand getSpeed(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_SPEED, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the standard deviation of the estimated maximum speed (see speed factor) divided by this speed.
	 * @param vehID id of the vehicle
	 * @return speed deviation
	 */

	public static SumoCommand getSpeedDeviation(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_SPEED_DEVIATION, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the factor by which the driver multiplies the speed read from street signs to estimate "real" maximum allowed speed.
	 * @param vehID id of the vehicle
	 * @return speed factor
	 */

	public static SumoCommand getSpeedFactor(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_SPEED_FACTOR, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the speed mode of a vehicle 
	 * @param vehID id of the vehicle
	 * @return speed mode
	 */

	public static SumoCommand getSpeedMode(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_SPEEDSETMODE, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_INTEGER);
	}

	
	/**
	 * getSpeedWithoutTraCI
	 * @param vehID id of the vehicle
	 * @return speed in m/s
	 */

	public static SumoCommand getSpeedWithoutTraCI(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_SPEED_WITHOUT_TRACI, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 *  Returns information in regard to stopping: The returned integer is defined as 1 * stopped + 2 * parking + 4 * personTriggered + 8 * containerTriggered + 16 * isBusStop + 32 * isContainerStop with each of these flags defined as 0 or 1
	 * @param vehID id of the vehicle
	 * @return speed in m/s
	 */

	public static SumoCommand getStopState(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_STOPSTATE, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_UBYTE);
	}
	
	
	/**
	 * getTau
	 * @param vehID id of the vehicle
	 * @return tau
	 */

	public static SumoCommand getTau(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_TAU, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	/**
	 * Returns the ids of via edges for this vehicle
	 * @param vehID id of the vehicle
	 * @return ids of via edges for this vehicle
	 */

	public static SumoCommand getVia(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_VIA, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRINGLIST);
	}
	
	/**
	 * The waiting time of a vehicle is defined as the time (in seconds) spent with a speed below 0.1m/s since the last time it was faster than 0.1m/s. (basically, the waiting time of a vehicle is reset to 0 every time it moves).
	 * @param vehID id of the vehicle
	 * @return waiting time
	 */

	public static SumoCommand getWaitingTime(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_WAITING_TIME, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}
	
	
	/**
	 * Returns the id of the type of the named vehicle.
	 * @param vehID id of the vehicle
	 * @return type id
	 */

	public static SumoCommand getTypeID(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_TYPE, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns the vehicle class.
	 * @param vehID id of the vehicle
	 * @return vehicle class
	 */

	public static SumoCommand getVehicleClass(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_VEHICLECLASS, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns the vehicle's width (in m).
	 * @param vehID id of the vehicle
	 * @return width
	 */

	public static SumoCommand getWidth(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_WIDTH, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_DOUBLE);
	}

	
	/**
	 * isStopped
	 * @param vehID id of the vehicle
	 * @return stop
	 */

	public static SumoCommand isStopped(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_STOPSTATE, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_UBYTE, "isStopped");
	}
	
	/**
	 * Return whether the vehicle is stopped and waiting for a person or container
	 * @param vehID id of the vehicle
	 * @return stop
	 */

	public static SumoCommand isStoppedTriggered(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_STOPSTATE, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_UBYTE, "isStoppedTriggered");
	}
	
	/**
	 * Return whether the vehicle is stopped at a container stop
	 * @param vehID id of the vehicle
	 * @return stop
	 */

	public static SumoCommand isAtContainerStop(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_STOPSTATE, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_UBYTE, "isAtContainerStop");
	}
	
	/**
	 * isStoppedParking
	 * @param vehID id of the vehicle
	 * @return stop
	 */

	public static SumoCommand isStoppedParking(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_STOPSTATE, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_UBYTE, "isStoppedParking");
	}
	
	/**
	 * isAtBusStop
	 * @param vehID id of the vehicle
	 * @return stop
	 */

	public static SumoCommand isAtBusStop(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_STOPSTATE, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_UBYTE, "isAtBusStop");
	}
	
	
	
	/**
	 * Returns whether the Route is valid.
	 * @param vehID id of the vehicle
	 * @return route valid
	 */

	public static SumoCommand isRouteValid(String vehID){
		return new SumoCommand(Constants.CMD_GET_VEHICLE_VARIABLE, Constants.VAR_ROUTE_VALID, vehID, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_UBYTE);
	}

	/**
	 * Reduces the speed to the given for the given amount of time.
	 * @param vehID id of the vehicle
	 * @param speed speed
	 * @param duration duration
	 * @return SumoCommand
	 */

	public static SumoCommand slowDown(String vehID, double speed, int duration){
		Object[] array = new Object[]{speed, duration};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.CMD_SLOWDOWN, vehID, array, Constants.RESPONSE_GET_VEHICLE_VARIABLE, Constants.TYPE_INTEGER);
	}

	//setter methods

	/**
	 * Add a new vehicle.

	 * @param vehID vehicle id
	 * @param typeID type id
	 * @param routeID route id
	 * @param depart depart
	 * @param pos position
	 * @param speed speed
	 * @param lane lane
	 * @return SumoCommand
	 */
	public static SumoCommand add(String vehID, String typeID, String routeID, int depart, double pos, double speed, byte lane){

		Object[] array = new Object[]{typeID, routeID, depart, pos, speed, lane};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.ADD, vehID, array);
	}

	
	/**
	 * Add a new vehicle.

	 * @return SumoCommand
	 */
	public static SumoCommand addFull(String vehID, String routeID, String typeID, String depart, String departLane, String departPosition, 
									  String departSpeed, String arrivalLane, String arrivalPosition, String arrivalSpeed,  String fromTAZ, String toTAZ, String line, int person_capacity, int person_number){

		Object[] array = new Object[]{ routeID, typeID, depart, departLane, departPosition, departSpeed, arrivalLane, arrivalPosition, arrivalSpeed, fromTAZ, toTAZ, line, person_capacity, person_number};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.ADD_FULL, vehID, array);
	}
	
	
	/**
	 * Forces a lane change to the lane with the given index; if successful, the lane will be chosen for the given amount of time.

	 * @param vehID vehicle id
	 * @param laneIndex lane index
	 * @param duration duration
	 * @return SumoCommand
	 */
	public static SumoCommand changeLane(String vehID, byte laneIndex, int duration){

		Object[] array = new Object[]{laneIndex, duration};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.CMD_CHANGELANE, vehID, array);
	}

	/**
	 * The vehicle's destination edge is set to the given. The route is rebuilt.
	 * @param vehID vehicle id
	 * @param edgeID edge id
	 * @return SumoCommand
	 */
	public static SumoCommand changeTarget(String vehID, String edgeID){

		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.CMD_CHANGETARGET, vehID, edgeID);
	}

	/**
	 * Moves the vehicle to a new position.

	 * @param vehID vehicle id
	 * @param laneID lane id
	 * @param pos position
	 * @return SumoCommand
	 */
	
	public static SumoCommand moveTo(String vehID, String laneID, double pos){

		Object[] array = new Object[]{laneID, pos};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_MOVE_TO, vehID, array);
	}

	
	/**
	 * Place vehicle at the given x,y coordinates and force it's angle to
        the given value (for drawing). If keepRoute is set to 1, the closest position
        within the existing route is taken. If keepRoute is set to 0, the vehicle may move to
        any edge in the network but it's route then only consists of that edge.
        If keepRoute is set to 2 the vehicle has all the freedom of keepRoute=1
        but in addition to that may even move outside the road network.
        edgeID and lane are optional placement hints to resolve ambiguities

	 * @param vehID vehicle id
	 * @param edgeID edge id
	 * @param lane lane
	 * @param x x
	 * @param y y
	 * @param angle angle
	 * @param  keepRoute keepRoute 
	 * @return SumoCommand
	 */
	public static SumoCommand moveToXY(String vehID, String edgeID, int lane, double x, double y, double angle, byte keepRoute){

		Object[] array = new Object[]{edgeID, lane, x, y, angle, keepRoute};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_MOVE_TO_VTD, vehID, array);
	}
	
	
	/**
	 * Removes vehicle with the given ID for the given reason. Reasons are defined in module constants and start with REMOVE_

	 * @param vehID vehicle id
	 * @param reason reason
	 * @return SumoCommand
	 */
	public static SumoCommand remove(String vehID, byte reason){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.REMOVE, vehID, reason);
	}

	/**
	 * Computes a new route using the vehicle's internal and the global edge effort information.
	 * @param vehID id of the vehicle
	 * @return SumoCommand
	 */

	public static SumoCommand rerouteEffort(String vehID){
		Object[] array = new Object[]{vehID};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.CMD_REROUTE_EFFORT, vehID, array);
	}

	/**
	 * Computes a new route using the vehicle's internal and the global edge travel time information.
	 * @param vehID id of the vehicle
	 * @return SumoCommand
	 */

	public static SumoCommand rerouteTraveltime(String vehID){
		Object[] array = new Object[]{vehID};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.CMD_REROUTE_TRAVELTIME, vehID, array);
	}

	/**
	 * Sets the acceleration of the named vehicle.
	 * @param vehID id of the vehicle
	 * @param accel acceleration
	 * @return SumoCommand
	 */

	public static SumoCommand setAccel(String vehID, double accel){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_ACCEL, vehID, accel);
	}

	/**
	 * Sets the edge travel time of the named vehicle.
	 * @param vehID id of the vehicle
	 * @param begTime begin time
	 * @param endTime end time
	 * @param edgeID edge id
	 * @param time time
	 * @return SumoCommand
	 */
	public static SumoCommand setAdaptedTraveltime(String vehID, int begTime, int endTime, String edgeID, double time){

		Object[] array = new Object[]{begTime, endTime, edgeID, time};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_EDGE_TRAVELTIME, vehID, array);
	}

	
	/**
	 * Sets the vehicle's color (RGBA).

	 * @param vehID id of the vehicle
	 * @param color color
	 * @return SumoCommand
	 */
	public static SumoCommand setColor(String vehID, SumoColor color){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_COLOR, vehID, color);
	}

	
	/**
	 * Sets the deceleration of the named vehicle.
	 * @param vehID id of the vehicle
	 * @param decel deceleration
	 * @return SumoCommand
	 */
	public static SumoCommand setDecel(String vehID, double decel){
	return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_DECEL, vehID, decel);
	}

	/**
	 *  Sets the height in m for this vehicle.
	 * @param vehID id of the vehicle
	 * @param height in m
	 * @return SumoCommand
	 */
	public static SumoCommand setHeight(String vehID, double height){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_HEIGHT, vehID, height);
	}
	
	/**
	 * Sets the vehicle's lane change mode as a bitset.
	 * @param vehID id of the vehicle
	 * @param lcm lane change model
	 * @return SumoCommand
	 */
	public static SumoCommand setLaneChangeMode(String vehID, int lcm){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_LANECHANGE_MODE, vehID, lcm);
	}
	
	
	/**
	 * Sets the IDs of the edges the vehicle's route is made of.
	 * @param vehID id of the vehicle
	 * @param edgeList edges list
	 * @return SumoCommand
	 */
	public static SumoCommand setRoute(String vehID, SumoStringList edgeList){
		Object[] array = new Object[]{edgeList};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_ROUTE, vehID, array);
	}
	
	/**
	 * Sets the id of the type for the named vehicle.
	 * @param vehID id of the vehicle
	 * @param typeID type id
	 * @return SumoCommand
	 */
	public static SumoCommand setType(String vehID, String typeID){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_TYPE, vehID, typeID);
	}

	
	
	/**
	 * Sets the edge effort for the given time as stored in the vehicle's internal container.
	 * @param vehID id of the vehicle
	 * @param begTime begin time
	 * @param endTime end time
	 * @param edgeID edge id
	 * @param effort effort
	 * @return SumoCommand
	 */
	public static SumoCommand setEffort(String vehID, int begTime, int endTime, String edgeID, double effort){

		Object[] array = new Object[]{begTime, endTime, edgeID, effort};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_EDGE_EFFORT, vehID, array);
	}

	/**
	 * Sets the emission class of the named vehicle.

	 * @param vehID id of the vehicle
	 * @param clazz emission class
	 * @return SumoCommand
	 */
	public static SumoCommand setEmissionClass(String vehID, String clazz){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_EMISSIONCLASS, vehID, clazz);
	}

	/**
	 * setImperfection
	 * @param vehID vehicle id
	 * @param imperfection imperfection
	 * @return SumoCommand
	 */
	public static SumoCommand setImperfection(String vehID, double imperfection){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_IMPERFECTION, vehID, imperfection);
	}

	/**
	 * Sets the length (in m) of the named vehicle.
	 * @param vehID vehicle id
	 * @param length length
	 * @return SumoCommand
	 */
	public static SumoCommand setLength(String vehID, double length){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_LENGTH, vehID, length);
	}

	/**
	 * Sets the line information for this vehicle.
	 * @param vehID vehicle id
	 * @param line line
	 * @return SumoCommand
	 */
	public static SumoCommand setLine(String vehID, String line){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_LINE, vehID, line);
	}
	
	/**
	 * Sets the line information for this vehicle.
	 *  changes the via edges to the given edges list (to be used during
        subsequent rerouting calls).

        Note: a single edgeId as argument is allowed as shorthand for a list of length 1
	 * @param vehID vehicle id
	 * @param edgeList edges
	 * @return SumoCommand
	 */
	public static SumoCommand setVia(String vehID, SumoStringList edgeList){
		Object[] array = new Object[]{edgeList};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_VIA, vehID, array);
	}
	
	
	/**
	 * Sets the maximum speed (in m/s) of the named vehicle.
	 * @param vehID vehicle id
	 * @param speed speed
	 * @return SumoCommand
	 */
	public static SumoCommand setMaxSpeed(String vehID, double speed){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_MAXSPEED, vehID, speed);
	}

	/**
	 * Sets the minimum gap (in m) between this vehicle and the vehicle before it.
	 * @param vehID vehicle id
	 * @param minGap minimum gap
	 * @return SumoCommand
	 */
	public static SumoCommand setMinGap(String vehID, double minGap){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_MINGAP, vehID, minGap);
	}

	/**
	 * Sets the id of the route of the named vehicle.
	 * @param vehID vehicle id
	 * @param routeID route id
	 * @return SumoCommand
	 */

	public static SumoCommand setRouteID(String vehID, String routeID){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_ROUTE_ID, vehID, routeID);
	}

	/**
	 * setShapeClass
	 * @param vehID vehicle id
	 * @param clazz shape class
	 * @return SumoCommand
	 */
	public static SumoCommand setShapeClass(String vehID, String clazz){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_SHAPECLASS, vehID, clazz);
	}

	/**
	 * setSignals
	 * @param vehID vehicle id
	 * @param signals signals
	 * @return SumoCommand
	 */
	public static SumoCommand setSignals(String vehID, int signals){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_SIGNALS, vehID, signals);
	}

	/**
	 * Sets the speed (in m/s) of the named vehicle.
	 * @param vehID vehicle id
	 * @param speed speed
	 * @return SumoCommand
	 */
	
	public static SumoCommand setSpeed(String vehID, double speed){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_SPEED, vehID, speed);
	}
	
	/**
	 *  Sets the standard deviation of the estimated maximum speed.
	 * @param vehID vehicle id
	 * @param deviation deviation
	 * @return SumoCommand
	 */
	public static SumoCommand setSpeedDeviation(String vehID, double deviation){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_SPEED_DEVIATION, vehID, deviation);
	}

	/**
	 * Sets the factor by which the driver multiplies the speed read from street signs to estimate "real" maximum allowed speed.
	 * @param vehID vehicle id
	 * @param factor factor
	 * @return SumoCommand
	 */
	
	public static SumoCommand setSpeedFactor(String vehID, double factor){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_SPEED_FACTOR, vehID, factor);
	}


	/**
	 * Sets the vehicle's speed mode as a bitset.
	 * @param vehID vehicle id
	 * @param sm speed mode
	 * @return SumoCommand
	 */
	public static SumoCommand setSpeedMode(String vehID, int sm){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_SPEEDSETMODE, vehID, sm);
	}
	
	/**
	 * Lets the vehicle stop at the given edge, at the given position and lane. The vehicle will stop for the given duration. Re-issuing a stop command with the same lane and position allows changing the duration.
	 * @param vehID vehicle id
	 * @param edgeID edge id
	 * @param pos positon
	 * @param laneIndex lane index
	 * @param duration duration
	 * @param sf stop flags
	 * @return SumoCommand
	 */
	public static SumoCommand setStop(String vehID, String edgeID, double pos, byte laneIndex, int duration, SumoStopFlags sf){

		Object[] array = new Object[]{edgeID, pos, laneIndex, duration, sf};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.CMD_STOP, vehID, array);
	}

	/**
	 * Adds or modifies a bus stop with the given parameters. The duration and the until attribute are in milliseconds.
	 * @param vehID id of the vehicle
	 * @param stopID
	 * @param duration
	 * @param until
	 * @return SumoCommand
	 */
	public static SumoCommand setBusStop(String vehID, String stopID, int duration, int until){
		SumoStopFlags sf = new SumoStopFlags(false, false, false, true, false);
    	return setStop(vehID, stopID, 1, (byte) 0, duration, sf);
	}
	

	/**
	 *  Adds or modifies a container stop with the given parameters. The duration and the until attribute are
	    in milliseconds.

	 * @param vehID id of the vehicle
	 * @param stopID
	 * @param duration
	 * @param until
	 * @return SumoCommand
	 */
	public static SumoCommand setContainerStop(String vehID, String stopID, int duration, int until){
		SumoStopFlags sf = new SumoStopFlags(false, false, false, false, true);
		return setStop(vehID, stopID, 1, (byte) 0, duration, sf);
	}
	
	/**
	 * Continue after a stop
	 * @param vehID vehicle id
	 * @return SumoCommand
	 */
	public static SumoCommand resume(String vehID){
		Object[] array = new Object[]{vehID};
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.CMD_RESUME, vehID, array);
	}

	/**
	 * setTau
	 * @param vehID vehicle id
	 * @param tau tau
	 * @return SumoCommand
	 */
	public static SumoCommand setTau(String vehID, double tau){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_TAU, vehID, tau);
	}

	/**
	 * Sets the vehicle class.
	 * @param vehID vehicle id
	 * @param clazz vehicle class
	 * @return SumoCommand
	 */
	public static SumoCommand setVehicleClass(String vehID, String clazz){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_VEHICLECLASS, vehID, clazz);
	}

	/**
	 * Sets the vehicle's width (in m).
	 * @param vehID vehicle id
	 * @param width width
	 * @return SumoCommand
	 */

	public static SumoCommand setWidth(String vehID, double width){
		return new SumoCommand(Constants.CMD_SET_VEHICLE_VARIABLE, Constants.VAR_WIDTH, vehID, width);
	}

}
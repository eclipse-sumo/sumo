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


package de.tudresden.sumo.config;

/**
 * 
 * @author Mario Krumnow
 *
 */

public class Constants {
	
	public static final int CMD_SETORDER = 0x03;

	public static final int COPY = 0x88;

	public static final int VAR_ACCUMULATED_WAITING_TIME = 0x87;
	
	public static final int DEPARTFLAG_SPEED_RANDOM = -0x02;
	
	public static final int RESPONSE_SUBSCRIBE_MULTIENTRYEXIT_VARIABLE = 0xe1;
	
	public static final int CMD_GET_LANEAREA_VARIABLE = 0xad;
	
	public static final int RESPONSE_SUBSCRIBE_LANEAREA_CONTEXT = 0x9d;
	
	public static final int CMD_LOAD = 0x01;
	
	public static final int DEPARTFLAG_LANE_BEST_FREE = -0x05;
	
	public static final int MOVE_TO_XY = 0xb4;
	
	public static final int DEPARTFLAG_LANE_FIRST_ALLOWED = -0x06;
	
	public static final int STOP_CONTAINER_TRIGGERED = 0x04;
	
	public static final int CMD_SUBSCRIBE_MULTIENTRYEXIT_CONTEXT = 0x81;
	
	public static final int APPEND_STAGE = 0xc4;
	
	public static final int VAR_VEHICLE = 0xc3;
	
	public static final int DEPARTFLAG_SPEED_MAX = -0x03;
	
	public static final int DEPARTFLAG_TRIGGERED = -0x01;
	
	public static final int VAR_STAGES_REMAINING = 0xc2;
	
	public static final int VAR_APPARENT_DECEL = 0x7c;
	
	public static final int STOP_BUS_STOP = 0x08;
	
	public static final int REMOVE_STAGE = 0xc5;
	
	public static final int CMD_SUBSCRIBE_LANEAREA_CONTEXT = 0x8d;
	
	public static final int DEPARTFLAG_CONTAINER_TRIGGERED = -0x02;
	
	public static final int RESPONSE_GET_LANEAREA_VARIABLE = 0xbd;
	
	public static final int STOP_PARKING_AREA = 0x40;
	
	public static final int CMD_SUBSCRIBE_LANEAREA_VARIABLE = 0xdd;
	
	public static final int VAR_EMERGENCY_DECEL = 0x7b;
	
	public static final int DEPARTFLAG_LANE_ALLOWED_FREE = -0x04;
	
	public static final int CMD_SUBSCRIBE_MULTIENTRYEXIT_VARIABLE = 0xd1;
	
	public static final int CMD_SIMSTEP = 0x02;
	
	public static final int STOP_PARKING = 0x01;
	
	public static final int RESPONSE_SUBSCRIBE_LANEAREA_VARIABLE = 0xed;
	
	public static final int RESPONSE_SUBSCRIBE_MULTIENTRYEXIT_CONTEXT = 0x91;
	
	public static final int DEPARTFLAG_LANE_FREE = -0x03;
	
	public static final int STOP_CONTAINER_STOP = 0x10;
	
	public static final int CMD_GET_MULTIENTRYEXIT_VARIABLE = 0xa1;
	
	public static final int STAGE_WAITING_FOR_DEPART = 0x00;
	
	public static final int DEPARTFLAG_LANE_RANDOM = -0x02;
	
	public static final int STOP_CHARGING_STATION = 0x20;
	
	public static final int DEPARTFLAG_NOW = -0x03;
	
	public static final int STOP_TRIGGERED = 0x02;
	
	public static final int STOP_DEFAULT = 0x00;
	
	public static final int RESPONSE_GET_MULTIENTRYEXIT_VARIABLE = 0xb1;
	
	public static final int INVALID_DOUBLE_VALUE = -1001;
	
	public static final int INVALID_INT_VALUE = -1;
	
	public static final int ADD_FULL = 0x85;
	
	public static final int CMD_CLEAR_PENDING_VEHICLES = 0x94;
	
	public static final int CMD_SAVE_SIMSTATE = 0x95;
	
	public static final int CMD_CHANGESUBLANE = 0x15;
	
	public static final int VAR_LATALIGNMENT = 0xb9;
	
	public static final int VAR_MINGAP_LAT = 0xbb;
	
	public static final int VAR_NEXT_TLS = 0x70;
	
	public static final int VAR_LANEPOSITION_LAT = 0xb8;
	
	public static final int VAR_MAXSPEED_LAT = 0xba;
	
	public static final int VAR_LEADER = 0x68;

	public static final int AUTOMATIC_VARIABLES_SUBSCRIPTION = 0x02;

	public static final int VAR_ELECTRICITYCONSUMPTION = 0x71;
	
	public static final int AUTOMATIC_CONTEXT_SUBSCRIPTION = 0x03;
	
	public static final int GENERIC_ATTRIBUTE = 0x03;
	
	public static final int VAR_SLOPE = 0x36;
	
	public static final int VAR_VIA = 0xbe;
	
	public static final int VAR_LINE = 0xbd;
	
	
	
	/**sets/retrieves abstract parameter*/
	public static final int VAR_PARAMETER = 0x7e;
	
	/**edge index in current route (get: vehicle)*/
	public static final int VAR_ROUTE_INDEX = 0x69;
	
	/**person riding / container being transported*/
	public static final int STAGE_DRIVING = 0x03;
	
	/**person / container stopping*/
	public static final int STAGE_WAITING = 0x01;
	
	/**person walking / container transhiping*/
	public static final int STAGE_WALKING = 0x02;
	
	/**retrieve information regarding the next edge including crossings and walkingAreas (pedestrians only)*/
	public static final int VAR_NEXT_EDGE = 0xc1;
	
	/** how lane changing is performed (set: vehicle)*/
	public static final int VAR_LANECHANGE_MODE = 0xb6;

	/**maximum speed regarding max speed on the current lane and speed factor (get: vehicle)*/
	public static final int VAR_ALLOWED_SPEED = 0xb7;

	/**retrieve information regarding the current person/container stage*/
	public static final int VAR_STAGE = 0xc0;
	
	/**last step person list (get: edges)*/
	public static final int LAST_STEP_PERSON_ID_LIST = 0x1a;
	
	
	
	
	/** number of vehicles starting to park (get: simulation)*/
	public static final int VAR_STOP_STARTING_VEHICLES_NUMBER = 0x68;

	/**ids of vehicles starting to park (get: simulation)*/
	public static final int VAR_STOP_STARTING_VEHICLES_IDS = 0x69;

	/**number of vehicles ending to park (get: simulation)*/
	public static final int VAR_STOP_ENDING_VEHICLES_NUMBER = 0x6a;

	/**ids of vehicles ending to park (get: simulation)*/
	public static final int VAR_STOP_ENDING_VEHICLES_IDS = 0x6b;

	/**number of vehicles starting to park (get: simulation)*/
	public static final int VAR_PARKING_STARTING_VEHICLES_NUMBER = 0x6c;

	/**ids of vehicles starting to park (get: simulation)*/
	public static final int VAR_PARKING_STARTING_VEHICLES_IDS = 0x6d;

	/**number of vehicles ending to park (get: simulation)*/
	public static final int VAR_PARKING_ENDING_VEHICLES_NUMBER = 0x6e;

	/**ids of vehicles ending to park (get: simulation)*/
	public static final int VAR_PARKING_ENDING_VEHICLES_IDS = 0x6f;

	/**vehicle finished route during teleport*/
	public static final int REMOVE_TELEPORT_ARRIVED = 0x04;

	/**subscribe junction variable*/
	public static final int CMD_SUBSCRIBE_JUNCTION_VARIABLE = 0xd9;

	/**yellow phase*/
	public static final int TLPHASE_YELLOW = 0x02;

	/**response: get simulation variable*/
	public static final int RESPONSE_GET_SIM_VARIABLE = 0xbb;

	/**position (1D along lane) (get: vehicle)*/
	public static final int VAR_LANEPOSITION = 0x56;

	/**controlled junctions (get: traffic lights)*/
	public static final int TL_CONTROLLED_JUNCTIONS = 0x2a;

	/**remove an instance (poi, polygon)*/
	public static final int REMOVE = 0x81;

	
	/**stop node*/
	public static final int CMD_STOP = 0x12;

	/**response: subscribe edge variable*/
	public static final int RESPONSE_SUBSCRIBE_EDGE_VARIABLE = 0xea;

	/**ids of vehicles starting to teleport (get: simulation)*/
	public static final int VAR_TELEPORT_STARTING_VEHICLES_IDS = 0x76;

	/**result type: not implemented*/
	public static final int RTYPE_NOTIMPLEMENTED = 0x01;

	/**signals state (get/set: vehicle)*/
	public static final int VAR_SIGNALS = 0x5b;

	/**validates current route (vehicles)*/
	public static final int VAR_ROUTE_VALID = 0x92;

	/**Position on road map*/
	public static final int POSITION_ROADMAP = 0x04;

	/**response: subscribe polygon context*/
	public static final int RESPONSE_SUBSCRIBE_POLYGON_CONTEXT = 0x98;

	/**get version*/
	public static final int CMD_GETVERSION = 0x00;

	/**delta t (get: simulation)*/
	public static final int VAR_DELTA_T = 0x7b;

	/**3D cartesian coordinates*/
	public static final int POSITION_3D = 0x03;

	/**view schema*/
	public static final int VAR_VIEW_SCHEMA = 0xa2;

	/**edges (get: routes)*/
	public static final int VAR_EDGES = 0x54;

	/**set route variable*/
	public static final int CMD_SET_ROUTE_VARIABLE = 0xc6;

	/**32 bit signed integer*/
	public static final int TYPE_INTEGER = 0x09;

	/**last step travel time (get: edge, lane)*/
	public static final int VAR_CURRENT_TRAVELTIME = 0x5a;

	/**signed byte*/
	public static final int TYPE_BYTE = 0x08;

	/**road id (get: vehicles)*/
	public static final int VAR_ROAD_ID = 0x50;

	/**response: subscribe edge context*/
	public static final int RESPONSE_SUBSCRIBE_EDGE_CONTEXT = 0x9a;

	/**get vehicle type variable*/
	public static final int CMD_GET_VEHICLETYPE_VARIABLE = 0xa5;

	/**route (set: vehicles)*/
	public static final int VAR_ROUTE = 0x57;

	/**last step vehicle halting number (get: multi-entry/multi-exit detector, lanes, edges)*/
	public static final int LAST_STEP_VEHICLE_HALTING_NUMBER = 0x14;

	/**force rerouting based on effort (vehicles)*/
	public static final int CMD_REROUTE_EFFORT = 0x91;

	/**8 bit ASCII string*/
	public static final int TYPE_STRING = 0x0c;

	/**name of the current program (get: traffic lights)*/
	public static final int TL_CURRENT_PROGRAM = 0x29;
	
	/**current state, using external signal names (get: traffic lights)*/
	public static final int TL_EXTERNAL_STATE = 0x2e;

	/**set simulation variable*/
	public static final int CMD_SET_SIM_VARIABLE = 0xcb;

	/**subscribe induction loop (e1) context*/
	public static final int CMD_SUBSCRIBE_INDUCTIONLOOP_CONTEXT = 0x80;

	/**command: get person variable*/
	public static final int  CMD_GET_PERSON_VARIABLE = 0xae;
	
	/**command: set person variable*/
	public static final int  CMD_SET_PERSON_VARIABLE = 0xce;
	
	/**response: get person variable*/
	public static final int  RESPONSE_GET_PERSON_VARIABLE = 0xbe;
	
	/**command: subscribe person context*/
	public static final int  CMD_SUBSCRIBE_PERSON_CONTEXT = 0x8e;
	
	/**command: subscribe person variable*/
	public static final int  CMD_SUBSCRIBE_PERSON_VARIABLE = 0xde;
	
	/**response: subscribe person context*/
	public static final int  RESPONSE_SUBSCRIBE_PERSON_CONTEXT = 0x9e;
	
	/**response: subscribe person variable*/
	public static final int  RESPONSE_SUBSCRIBE_PERSON_VARIABLE = 0xee;
	
	
	/**route id (get and set: vehicles)*/
	public static final int VAR_ROUTE_ID = 0x53;

	/**response: subscribe poi context*/
	public static final int RESPONSE_SUBSCRIBE_POI_CONTEXT = 0x97;

	/**2D cartesian coordinates*/
	public static final int POSITION_2D = 0x01;

	/**number of vehicles starting to teleport (get: simulation)*/
	public static final int VAR_TELEPORT_STARTING_VEHICLES_NUMBER = 0x75;

	/**response: get lane variable*/
	public static final int RESPONSE_GET_LANE_VARIABLE = 0xb3;

	/**view by boundary*/
	public static final int VAR_VIEW_BOUNDARY = 0xa3;

	
	/**result type: error*/
	public static final int RTYPE_ERR = 0xff;

	/**response: get polygon variable*/
	public static final int RESPONSE_GET_POLYGON_VARIABLE = 0xb8;

	/**response: subscribe GUI context*/
	public static final int RESPONSE_SUBSCRIBE_GUI_CONTEXT = 0x9c;

	/**outgoing link number (get: lanes)*/
	public static final int LANE_LINK_NUMBER = 0x30;

	/**current person number (get: vehicle)*/
	public static final int VAR_BUS_STOP_WAITING = 0x67;

	/**response: subscribe junction variable*/
	public static final int RESPONSE_SUBSCRIBE_JUNCTION_VARIABLE = 0xe9;

	/**width (get: vehicle types, lanes)*/
	public static final int VAR_WIDTH = 0x4d;
	
	/**height (get: vehicle)*/
	public static final int VAR_HEIGHT = 0xbc;

	/**set GUI variable*/
	public static final int CMD_SET_GUI_VARIABLE = 0xcc;

	/**controlled lanes (get: traffic lights)*/
	public static final int TL_CONTROLLED_LANES = 0x26;

	/**response: get induction loop (e1) variable*/
	public static final int RESPONSE_GET_INDUCTIONLOOP_VARIABLE = 0xb0;

	/**subscribe vehicle type context*/
	public static final int CMD_SUBSCRIBE_VEHICLETYPE_CONTEXT = 0x85;

	/**entry times*/
	public static final int LAST_STEP_VEHICLE_DATA = 0x17;

	/**traffic light program (set: traffic lights)*/
	public static final int TL_PROGRAM = 0x23;

	/**force rerouting based on travel time (vehicles)*/
	public static final int CMD_REROUTE_TRAVELTIME = 0x90;

	/**response: subscribe vehicle variable*/
	public static final int RESPONSE_SUBSCRIBE_VEHICLE_VARIABLE = 0xe4;

	/**get edge variable*/
	public static final int CMD_GET_EDGE_VARIABLE = 0xaa;

	/**color (four ubytes)*/
	public static final int TYPE_COLOR = 0x11;

	/**response: subscribe induction loop (e1) variable*/
	public static final int RESPONSE_SUBSCRIBE_INDUCTIONLOOP_VARIABLE = 0xe0;

	/**index of the current phase (get: traffic lights)*/
	public static final int TL_CURRENT_PHASE = 0x28;

	/**subscribe traffic lights variable*/
	public static final int CMD_SUBSCRIBE_TL_VARIABLE = 0xd2;

	/**speed factor (set: vehicle)*/
	public static final int VAR_SPEED_FACTOR = 0x5e;

	/**set vehicle type variable*/
	public static final int CMD_SET_VEHICLETYPE_VARIABLE = 0xc5;

	/**shape (get: polygons)*/
	public static final int VAR_SHAPE = 0x4e;

	/**complete definition (get: traffic lights)*/
	public static final int TL_COMPLETE_DEFINITION_RYG = 0x2b;

	/**response: subscribe induction loop (e1) context*/
	public static final int RESPONSE_SUBSCRIBE_INDUCTIONLOOP_CONTEXT = 0x90;

	/**set lane variable*/
	public static final int CMD_SET_LANE_VARIABLE = 0xc3;

	/**response: get route variable*/
	public static final int RESPONSE_GET_ROUTE_VARIABLE = 0xb6;

	/**lane index (get: vehicles)*/
	public static final int VAR_LANE_INDEX = 0x52;

	/**subscribe poi context*/
	public static final int CMD_SUBSCRIBE_POI_CONTEXT = 0x87;

	/**Position in geo-coordinates with altitude*/
	public static final int POSITION_LON_LAT_ALT = 0x02;

	
	/**set poi variable*/
	public static final int CMD_SET_POI_VARIABLE = 0xc7;

	/**track vehicle*/
	public static final int VAR_TRACK_VEHICLE = 0xa6;

	/**driver imperfection (set: vehicle)*/
	public static final int VAR_IMPERFECTION = 0x5d;

	/**add an instance (poi, polygon, vehicle, route)*/
	public static final int ADD = 0x80;

	/**Polygon (2*n doubles)*/
	public static final int TYPE_POLYGON = 0x06;

	/**subscribe polygon variable*/
	public static final int CMD_SUBSCRIBE_POLYGON_VARIABLE = 0xd8;

	/**subscribe lane context*/
	public static final int CMD_SUBSCRIBE_LANE_CONTEXT = 0x83;

	/**stop state*/
	public static final int VAR_STOPSTATE = 0xb5;
	
	/**waiting time*/	
	public static final int VAR_WAITING_TIME = 0x7a;
	
	public static final int VAR_TAU = 0x48;

	/**set junction variable*/
	public static final int CMD_SET_JUNCTION_VARIABLE = 0xc9;

	/**loaded vehicle ids (get: simulation)*/
	public static final int VAR_LOADED_VEHICLES_IDS = 0x72;

	/**get traffic lights variable*/
	public static final int CMD_GET_TL_VARIABLE = 0xa2;

	/**new lane/position along (set: vehicle)*/
	public static final int VAR_MOVE_TO = 0x5c;

	/**compound object*/
	public static final int TYPE_COMPOUND = 0x0f;

	/**response: get junction variable*/
	public static final int RESPONSE_GET_JUNCTION_VARIABLE = 0xb9;

	/**assumed time to next switch (get: traffic lights)*/
	public static final int TL_NEXT_SWITCH = 0x2d;

	/**change target*/
	public static final int CMD_CHANGETARGET = 0x31;

	/**close sumo*/
	public static final int CMD_CLOSE = 0x7f;

	/**subscribe induction loop (e1) variable*/
	public static final int CMD_SUBSCRIBE_INDUCTIONLOOP_VARIABLE = 0xd0;

	/**zoom*/
	public static final int VAR_VIEW_ZOOM = 0xa0;

	/**number of departed vehicle (get: simulation)*/
	public static final int VAR_DEPARTED_VEHICLES_NUMBER = 0x73;

	/**tl is off and not blinking*/
	public static final int TLPHASE_NOSIGNAL = 0x05;

	/**response: get vehicle type variable*/
	public static final int RESPONSE_GET_VEHICLETYPE_VARIABLE = 0xb5;

	/**filled? (get: polygons)*/
	public static final int VAR_FILL = 0x55;

	/**ids of vehicles ending to teleport (get: simulation)*/
	public static final int VAR_TELEPORT_ENDING_VEHICLES_IDS = 0x78;

	/**number of vehicles ending to teleport (get: simulation)*/
	public static final int VAR_TELEPORT_ENDING_VEHICLES_NUMBER = 0x77;

	/**subscribe GUI context*/
	public static final int CMD_SUBSCRIBE_GUI_CONTEXT = 0x8c;

	/**set lane*/
	public static final int CMD_CHANGELANE = 0x13;

	/**subscribe route context*/
	public static final int CMD_SUBSCRIBE_ROUTE_CONTEXT = 0x86;

	/**convert coordinates*/
	public static final int POSITION_CONVERSION = 0x82;

	/**response: get GUI variable*/
	public static final int RESPONSE_GET_GUI_VARIABLE = 0xbc;

	/**get poi variable*/
	public static final int CMD_GET_POI_VARIABLE = 0xa7;

	/**green phase*/
	public static final int TLPHASE_GREEN = 0x03;

	/**minimum gap (get: vehicle types)*/
	public static final int VAR_MINGAP = 0x4c;

	/**response: subscribe route context*/
	public static final int RESPONSE_SUBSCRIBE_ROUTE_CONTEXT = 0x96;

	/**type id (get: vehicles, polygons, pois)*/
	public static final int VAR_TYPE = 0x4f;

	/**last step vehicle number (get: induction loops, multi-entry/multi-exit detector, lanes, edges)*/
	public static final int LAST_STEP_VEHICLE_NUMBER = 0x10;

	/**last step occupancy (get: induction loops, lanes, edges)*/
	public static final int LAST_STEP_OCCUPANCY = 0x13;

	
	/** last step jam length in meters */
	public static final int JAM_LENGTH_VEHICLE = 0x18;

	/** last step jam length in meters */
	public static final int  JAM_LENGTH_METERS = 0x19;
	
	
	/**set polygon variable*/
	public static final int CMD_SET_POLYGON_VARIABLE = 0xc8;

	/**tl is blinking*/
	public static final int TLPHASE_BLINKING = 0x04;

	/**current HC emission of a node (get: vehicle, lane, edge)*/
	public static final int VAR_HCEMISSION = 0x62;

	/**response: subscribe GUI variable*/
	public static final int RESPONSE_SUBSCRIBE_GUI_VARIABLE = 0xec;

	/**air distance*/
	public static final int REQUEST_AIRDIST = 0x00;

	/**double*/
	public static final int TYPE_DOUBLE = 0x0b;

	/**shape class (get: vehicle types)*/
	public static final int VAR_SHAPECLASS = 0x4b;

	/**response: get poi variable*/
	public static final int RESPONSE_GET_POI_VARIABLE = 0xb7;

	/**subscribe simulation context*/
	public static final int CMD_SUBSCRIBE_SIM_CONTEXT = 0x8b;

	/**response: subscribe poi variable*/
	public static final int RESPONSE_SUBSCRIBE_POI_VARIABLE = 0xe7;

	/**response: get edge variable*/
	public static final int RESPONSE_GET_EDGE_VARIABLE = 0xba;

	/**response: get vehicle variable*/
	public static final int RESPONSE_GET_VEHICLE_VARIABLE = 0xb4;

	/**vehicle class (get: vehicle types)*/
	public static final int VAR_VEHICLECLASS = 0x49;

	/**driving distance*/
	public static final int REQUEST_DRIVINGDIST = 0x01;

	/**set traffic lights variable*/
	public static final int CMD_SET_TL_VARIABLE = 0xc2;

	/**vehicle started teleport*/
	public static final int REMOVE_TELEPORT = 0x00;

	/**subscribe edge variable*/
	public static final int CMD_SUBSCRIBE_EDGE_VARIABLE = 0xda;

	/**complete program (set: traffic lights)*/
	public static final int TL_COMPLETE_PROGRAM_RYG = 0x2c;

	/**get simulation variable*/
	public static final int CMD_GET_SIM_VARIABLE = 0xab;

	/**response: subscribe vehicle context*/
	public static final int RESPONSE_SUBSCRIBE_VEHICLE_CONTEXT = 0x94;

	/**subscribe poi variable*/
	public static final int CMD_SUBSCRIBE_POI_VARIABLE = 0xd7;

	/**response: subscribe vehicle type variable*/
	public static final int RESPONSE_SUBSCRIBE_VEHICLETYPE_VARIABLE = 0xe5;

	/**get vehicle variable*/
	public static final int CMD_GET_VEHICLE_VARIABLE = 0xa4;

	/**response: get traffic lights variable*/
	public static final int RESPONSE_GET_TL_VARIABLE = 0xb2;

	/**angle (get: vehicle types, lanes, set: lanes)*/
	public static final int VAR_LENGTH = 0x44;

	/**resume*/
	public static final int CMD_RESUME = 0x19;
	
	/**set edge variable*/
	public static final int CMD_SET_EDGE_VARIABLE = 0xca;

	/**list of strings*/
	public static final int TYPE_STRINGLIST = 0x0e;

	/**position (2D) (get: vehicle, poi, set: poi)*/
	public static final int VAR_POSITION = 0x42;

	/**position (3D) (get: vehicle, poi, set: poi)*/
	public static final int VAR_POSITION3D = 0x39;
	
	/**ids of arrived vehicles (get: simulation)*/
	public static final int VAR_ARRIVED_VEHICLES_IDS = 0x7a;

	/**last step time since last detection (get: induction loops)*/
	public static final int LAST_STEP_TIME_SINCE_DETECTION = 0x16;

	/**current noise emission of a node (get: vehicle, lane, edge)*/
	public static final int VAR_NOISEEMISSION = 0x66;

	/**Boundary Box (4 doubles)*/
	public static final int TYPE_BOUNDINGBOX = 0x05;

	/**response: subscribe vehicle type context*/
	public static final int RESPONSE_SUBSCRIBE_VEHICLETYPE_CONTEXT = 0x95;

	/**index of the phase (set: traffic lights)*/
	public static final int TL_PHASE_INDEX = 0x22;

	/**response: subscribe lane variable*/
	public static final int RESPONSE_SUBSCRIBE_LANE_VARIABLE = 0xe3;

	/**controlled links (get: traffic lights)*/
	public static final int TL_CONTROLLED_LINKS = 0x27;

	/**number of loaded vehicles (get: simulation)*/
	public static final int VAR_LOADED_VEHICLES_NUMBER = 0x71;

	/**get lane variable*/
	public static final int CMD_GET_LANE_VARIABLE = 0xa3;

	/**get GUI variable*/
	public static final int CMD_GET_GUI_VARIABLE = 0xac;

	/**slow down*/
	public static final int CMD_SLOWDOWN = 0x14;

	/**speed deviation (set: vehicle)*/
	public static final int VAR_SPEED_DEVIATION = 0x5f;

	/**last step vehicle number (get: induction loops, multi-entry/multi-exit detector, lanes, edges)*/
	public static final int LAST_STEP_MEAN_SPEED = 0x11;

	/**get junction variable*/
	public static final int CMD_GET_JUNCTION_VARIABLE = 0xa9;

	/**subscribe GUI variable*/
	public static final int CMD_SUBSCRIBE_GUI_VARIABLE = 0xdc;

	/**subscribe traffic lights context*/
	public static final int CMD_SUBSCRIBE_TL_CONTEXT = 0x82;

	/**count of instances (get: all)*/
	public static final int ID_COUNT = 0x01;

	/**red phase*/
	public static final int TLPHASE_RED = 0x01;

	/**subscribe lane variable*/
	public static final int CMD_SUBSCRIBE_LANE_VARIABLE = 0xd3;

	/**current CO emission of a node (get: vehicle, lane, edge)*/
	public static final int VAR_COEMISSION = 0x61;

	/**list of allowed vehicle classes (get and set: lanes)*/
	public static final int LANE_ALLOWED = 0x34;

	/**subscribe edge context*/
	public static final int CMD_SUBSCRIBE_EDGE_CONTEXT = 0x8a;

	/**subscribe polygon context*/
	public static final int CMD_SUBSCRIBE_POLYGON_CONTEXT = 0x88;

	/**best lanes (get: vehicle)*/
	public static final int VAR_BEST_LANES = 0xb2;

	/**response: subscribe junction context*/
	public static final int RESPONSE_SUBSCRIBE_JUNCTION_CONTEXT = 0x99;

	/**response: subscribe simulation context*/
	public static final int RESPONSE_SUBSCRIBE_SIM_CONTEXT = 0x9b;

	/**vehicle was vaporized*/
	public static final int REMOVE_VAPORIZED = 0x03;

	/**list of traffic light phases*/
	public static final int TYPE_TLPHASELIST = 0x0d;

	/**minimum number of expected vehicles (get: simulation)*/
	public static final int VAR_MIN_EXPECTED_VEHICLES = 0x7d;

	/**list of instances' ids (get: all)*/
	public static final int ID_LIST = 0x00;

	/**result type: Ok*/
	public static final int RTYPE_OK = 0x00;

	/**traffic light states, encoded as rRgGyYoO tuple (get: traffic lights)*/
	public static final int TL_RED_YELLOW_GREEN_STATE = 0x20;

	/**response: subscribe polygon variable*/
	public static final int RESPONSE_SUBSCRIBE_POLYGON_VARIABLE = 0xe8;

	/**effort information (get and set: vehicle)*/
	public static final int VAR_EDGE_EFFORT = 0x59;

	/**subscribe simulation variable*/
	public static final int CMD_SUBSCRIBE_SIM_VARIABLE = 0xdb;

	/**view position*/
	public static final int VAR_VIEW_OFFSET = 0xa1;

	/**current fuel consumption of a node (get: vehicle, lane, edge)*/
	public static final int VAR_FUELCONSUMPTION = 0x65;

	/**speed (get: vehicle)*/
	public static final int VAR_SPEED = 0x40;

	/**emission class (get: vehicle types)*/
	public static final int VAR_EMISSIONCLASS = 0x4a;

	/**response: subscribe traffic lights variable*/
	public static final int RESPONSE_SUBSCRIBE_TL_VARIABLE = 0xe2;

	/**id of parent edge (get: lanes)*/
	public static final int LANE_EDGE_ID = 0x31;

	/**departed vehicle ids (get: simulation)*/
	public static final int VAR_DEPARTED_VEHICLES_IDS = 0x74;

	/**outgoing link definitions (get: lanes)*/
	public static final int LANE_LINKS = 0x33;

	/**list of not allowed vehicle classes (get and set: lanes)*/
	public static final int LANE_DISALLOWED = 0x35;

	/**response: subscribe lane context*/
	public static final int RESPONSE_SUBSCRIBE_LANE_CONTEXT = 0x93;

	/**get polygon variable*/
	public static final int CMD_GET_POLYGON_VARIABLE = 0xa8;

	/**angle (get: vehicle)*/
	public static final int VAR_ANGLE = 0x43;

	/**get induction loop (e1) variable*/
	public static final int CMD_GET_INDUCTIONLOOP_VARIABLE = 0xa0;

	/**response: subscribe traffic lights context*/
	public static final int RESPONSE_SUBSCRIBE_TL_CONTEXT = 0x92;

	/**screenshot*/
	public static final int VAR_SCREENSHOT = 0xa5;

	/**current CO2 emission of a node (get: vehicle, lane, edge)*/
	public static final int VAR_CO2EMISSION = 0x60;

	/**current time step (get: simulation)*/
	public static final int VAR_TIME_STEP = 0x70;

	/**phase duration (set: traffic lights)*/
	public static final int TL_PHASE_DURATION = 0x24;

	/**current NOx emission of a node (get: vehicle, lane, edge)*/
	public static final int VAR_NOXEMISSION = 0x64;

	/**get route variable*/
	public static final int CMD_GET_ROUTE_VARIABLE = 0xa6;

	/**Position in geo-coordinates*/
	public static final int POSITION_LON_LAT = 0x00;
	
	/**vehicle arrived*/
	public static final int REMOVE_ARRIVED = 0x02;

	/**lane id (get: vehicles)*/
	public static final int VAR_LANE_ID = 0x51;

	/**bounding box (get: simulation)*/
	public static final int VAR_NET_BOUNDING_BOX = 0x7c;

	/**subscribe vehicle type variable*/
	public static final int CMD_SUBSCRIBE_VEHICLETYPE_VARIABLE = 0xd5;

	/**float*/
	public static final int TYPE_FLOAT = 0x0a;

	/**subscribe junction context*/
	public static final int CMD_SUBSCRIBE_JUNCTION_CONTEXT = 0x89;

	/**maximum allowed/possible speed (get: vehicle types, lanes, set: edges, lanes)*/
	public static final int VAR_MAXSPEED = 0x41;

	/**how speed is set (set: vehicle)*/
	public static final int VAR_SPEEDSETMODE = 0xb3;

	/**unsigned byte*/
	public static final int TYPE_UBYTE = 0x07;

	/**color (get: vehicles, vehicle types, polygons, pois)*/
	public static final int VAR_COLOR = 0x45;

	/**subscribe route variable*/
	public static final int CMD_SUBSCRIBE_ROUTE_VARIABLE = 0xd6;

	/**speed without TraCI influence (get: vehicle)*/
	public static final int VAR_SPEED_WITHOUT_TRACI = 0xb1;

	/**response: subscribe route variable*/
	public static final int RESPONSE_SUBSCRIBE_ROUTE_VARIABLE = 0xe6;

	/**set vehicle variable*/
	public static final int CMD_SET_VEHICLE_VARIABLE = 0xc4;

	/**last step mean vehicle length (get: induction loops, lanes, edges)*/
	public static final int LAST_STEP_LENGTH = 0x15;

	/**max. acceleration (get: vehicle types)*/
	public static final int VAR_ACCEL = 0x46;

	/**current person number (get: vehicle)*/
	public static final int VAR_PERSON_NUMBER = 0x67;

	/**subscribe vehicle variable*/
	public static final int CMD_SUBSCRIBE_VEHICLE_VARIABLE = 0xd4;

	/**max. deceleration (get: vehicle types)*/
	public static final int VAR_DECEL = 0x47;

	/**vehicle removed while parking*/
	public static final int REMOVE_PARKING = 0x01;

	/**subscribe vehicle context*/
	public static final int CMD_SUBSCRIBE_VEHICLE_CONTEXT = 0x84;

	/**number of arrived vehicles (get: simulation)*/
	public static final int VAR_ARRIVED_VEHICLES_NUMBER = 0x79;

	/**distance between points or vehicles*/
	public static final int DISTANCE_REQUEST = 0x83;

	/**current distance in m*/
	public static final int VAR_DISTANCE = 0x84;

	/**last step vehicle number (get: induction loops, multi-entry/multi-exit detector, lanes, edges)*/
	public static final int LAST_STEP_VEHICLE_ID_LIST = 0x12;

	/**response: subscribe simulation variable*/
	public static final int RESPONSE_SUBSCRIBE_SIM_VARIABLE = 0xeb;

	/**travel time information (get and set: vehicle)*/
	public static final int VAR_EDGE_TRAVELTIME = 0x58;

	/**current PMx emission of a node (get: vehicle, lane, edge)*/
	public static final int VAR_PMXEMISSION = 0x63;
	
}
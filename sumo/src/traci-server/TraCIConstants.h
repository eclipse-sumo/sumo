/****************************************************************************/
/// @file    TraCIConstants.h
/// @author  Axel Wegener <wegener@itm.uni-luebeck.de>
/// @date    2007/10/24
/// @version $Id$
///
/// holds codes used for TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TRACICONSTANTS_H
#define TRACICONSTANTS_H


// ****************************************
// COMMANDS
// ****************************************
// command: simulation step
#define CMD_SIMSTEP 0x01

// command: set maximum speed
#define CMD_SETMAXSPEED 0x11

// command: stop node
#define CMD_STOP 0x12

// command: set lane
#define CMD_CHANGELANE 0x13

// command: slow down
#define CMD_SLOWDOWN 0x14

// command: change route
#define CMD_CHANGEROUTE 0x30

// command: change target
#define CMD_CHANGETARGET 0x31

// command: subscribe lifecycles
#define CMD_SUBSCRIBELIFECYCLES 0x61

// command: unsubscribe lifecycles
#define CMD_UNSUBSCRIBELIFECYCLES 0x62

// command: object creation
#define CMD_OBJECTCREATION 0x63

// command: object destruction
#define CMD_OBJECTDESTRUCTION 0x64

// command: object domain subscription
#define CMD_SUBSCRIBEDOMAIN 0x65

// command: object domain unsubscription
#define CMD_UNSUBSCRIBEDOMAIN 0x66

// command: object update
#define CMD_UPDATEOBJECT 0x67

// command: Simulation Parameter
#define CMD_SIMPARAMETER 0x70

// command: Position Conversion
#define CMD_POSITIONCONVERSION 0x71

// command: Distance Request
#define CMD_DISTANCEREQUEST 0x72

// command: Scenario
#define CMD_SCENARIO 0x73

// command: move node
#define CMD_MOVENODE 0x80

// command: close sumo
#define CMD_CLOSE 0x7F

// command:
#define CMD_UPDATECALIBRATOR 0x50

// command: get all traffic light ids
#define CMD_GETALLTLIDS 0x40

// command: get traffic light status
#define CMD_GETTLSTATUS 0x41

// command: report traffic light id
#define CMD_TLIDLIST 0x90

// command: report traffic light status switch
#define CMD_TLSWITCH 0x91

// command: get induction loop (e1) variable
#define CMD_GET_INDUCTIONLOOP_VARIABLE 0xa0
// response: get induction loop (e1) variable
#define RESPONSE_GET_INDUCTIONLOOP_VARIABLE 0xb0

// command: get multi-entry/multi-exit detector (e3) variable
#define CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE 0xa1
// response: get areal detector (e3) variable
#define RESPONSE_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE 0xb1

// command: get traffic lights variable
#define CMD_GET_TL_VARIABLE 0xa2
// response: get traffic lights variable
#define RESPONSE_GET_TL_VARIABLE 0xb2
// command: set traffic lights variable
#define CMD_SET_TL_VARIABLE 0xc2

// command: get lane variable
#define CMD_GET_LANE_VARIABLE 0xa3
// response: get lane variable
#define RESPONSE_GET_LANE_VARIABLE 0xb3
// command: set lane variable
#define CMD_SET_LANE_VARIABLE 0xc3

// command: get vehicle variable
#define CMD_GET_VEHICLE_VARIABLE 0xa4
// response: get vehicle variable
#define RESPONSE_GET_VEHICLE_VARIABLE 0xb4
// command: set vehicle variable
#define CMD_SET_VEHICLE_VARIABLE 0xc4

// command: get vehicle type variable
#define CMD_GET_VEHICLETYPE_VARIABLE 0xa5
// response: get vehicle type variable
#define RESPONSE_GET_VEHICLETYPE_VARIABLE 0xb5
// command: set vehicle type variable
#define CMD_SET_VEHICLETYPE_VARIABLE 0xc5

// command: get route variable
#define CMD_GET_ROUTE_VARIABLE 0xa6
// response: get route variable
#define RESPONSE_GET_ROUTE_VARIABLE 0xb6
// command: set route variable
#define CMD_SET_ROUTE_VARIABLE 0xc6

// command: get poi variable
#define CMD_GET_POI_VARIABLE 0xa7
// response: get poi variable
#define RESPONSE_GET_POI_VARIABLE 0xb7
// command: set poi variable
#define CMD_SET_POI_VARIABLE 0xc7

// command: get polygon variable
#define CMD_GET_POLYGON_VARIABLE 0xa8
// response: get polygon variable
#define RESPONSE_GET_POLYGON_VARIABLE 0xb8
// command: set polygon variable
#define CMD_SET_POLYGON_VARIABLE 0xc8

// command: get junction variable
#define CMD_GET_JUNCTION_VARIABLE 0xa9
// response: get junction variable
#define RESPONSE_GET_JUNCTION_VARIABLE 0xb9
// command: set junction variable
#define CMD_SET_JUNCTION_VARIABLE 0xc9



// ****************************************
// POSITION REPRESENTATIONS
// ****************************************
// Omit position
#define POSITION_NONE 0x00
// 2D cartesian coordinates
#define POSITION_2D 0x01
// 2.5D cartesian coordinates
#define POSITION_2_5D 0x02
// 3D cartesian coordinates
#define POSITION_3D 0x03
// Position on road map
#define POSITION_ROADMAP 0x04



// ****************************************
// DATA TYPES
// ****************************************
// Boundary Box
#define TYPE_BOUNDINGBOX 0x05
// Polygon
#define TYPE_POLYGON 0x06
// unsigned byte
#define TYPE_UBYTE 0x07
// signed byte
#define TYPE_BYTE 0x08
// 32 bit integer
#define TYPE_INTEGER 0x09
// float
#define TYPE_FLOAT 0x0A
// double
#define TYPE_DOUBLE 0x0B
// 8 bit ASCII string
#define TYPE_STRING 0x0C
// list of traffic light phases
#define TYPE_TLPHASELIST 0x0D
// list of strings
#define TYPE_STRINGLIST 0x0E
// compound object
#define TYPE_COMPOUND 0x0F
// 2D-position (two floats)
#define TYPE_POSITION2D 0x10
// color (four ubytes)
#define TYPE_COLOR 0x11



// ****************************************
// RESULT TYPES
// ****************************************
// result type: Ok
#define RTYPE_OK 0x00
// result type: not implemented
#define RTYPE_NOTIMPLEMENTED 0x01
// result type: error
#define RTYPE_ERR 0xFF



// ****************************************
// DOMAIN IDs (FOR SCENARIO COMMAND)
// ****************************************
// road map domain
#define DOM_ROADMAP 0x00
// vehicle domain
#define DOM_VEHICLE 0x01
// traffic lights domain
#define DOM_TRAFFICLIGHTS 0x02
// points of interest domain
#define DOM_POI 0x03
// polygon domain
#define DOM_POLYGON 0x04



// ****************************************
// VARIABLE IDs (FOR SCENARIO COMMAND)
// ****************************************
// count of domain objects
#define DOMVAR_COUNT 0x01
// position of a domain object
#define DOMVAR_POSITION 0x02
// boundaries of simulation net
#define DOMVAR_BOUNDINGBOX 0x03
// speed of a node
#define DOMVAR_SPEED 0x04
// actual phase of a traffic light
#define DOMVAR_CURTLPHASE 0x05
// next phase of a traffic light
#define DOMVAR_NEXTTLPHASE 0x06
// type of a domain object (poi, polygon)
#define DOMVAR_TYPE 0x07
// layer a domain object is located at (poi, polygon)
#define DOMVAR_LAYER 0x08
// shape of a polygon
#define DOMVAR_SHAPE 0x09
// max count of vehicles
#define DOMVAR_MAXCOUNT 0x0A
// count of TraCI vehicles
#define DOMVAR_EQUIPPEDCOUNT 0x0B
// max count of TraCI vehicles
#define DOMVAR_EQUIPPEDCOUNTMAX 0x0C
// id string of a domain object
#define DOMVAR_NAME 0x0D
// route, a car plans to drive
#define DOMVAR_ROUTE 0x0E
// maximum allowed speed of a node
#define DOMVAR_ALLOWED_SPEED 0x0F
// air distance from a certain object to a position
#define DOMVAR_AIRDISTANCE 0x10
// driving distance from a certain object to a position
#define DOMVAR_DRIVINGDISTANCE 0x11
// external integer id of a certain object
#define DOMVAR_EXTID 0x12
// angle of a certain object, in degrees [0..360)
#define DOMVAR_ANGLE 0x13
// current simulation time
#define DOMVAR_SIMTIME 0x14
// current CO2 emission of a node
#define DOMVAR_CO2EMISSION 0x20
// current CO emission of a node
#define DOMVAR_COEMISSION 0x21
// current HC emission of a node
#define DOMVAR_HCEMISSION 0x22
// current PMx emission of a node
#define DOMVAR_PMXEMISSION 0x23
// current NOx emission of a node
#define DOMVAR_NOXEMISSION 0x24
// current fuel consumption of a node
#define DOMVAR_FUELCONSUMPTION 0x25
// current noise emission of a node
#define DOMVAR_NOISEEMISSION 0x26



// ****************************************
// TRAFFIC LIGHT PHASES
// ****************************************
// red phase
#define TLPHASE_RED 0x01
// yellow phase
#define TLPHASE_YELLOW 0x02
// green phase
#define TLPHASE_GREEN 0x03
// tl is blinking
#define TLPHASE_BLINKING 0x04
// tl is off and not blinking
#define TLPHASE_NOSIGNAL 0x05



// ****************************************
// DIFFERENT DISTANCE REQUESTS
// ****************************************
// air distance
#define REQUEST_AIRDIST 0x00
// driving distance
#define REQUEST_DRIVINGDIST 0x01



// ****************************************
// VARIABLE TYPES (for CMD_GET_*_VARIABLE)
// ****************************************
// list of instances' ids (get: induction loops, areal detector, traffic lights)
#define ID_LIST 0x00

// last step vehicle number (get: induction loops, multi-entry/multi-exit detector)
#define LAST_STEP_VEHICLE_NUMBER 0x10

// last step vehicle number (get: induction loops, multi-entry/multi-exit detector)
#define LAST_STEP_MEAN_SPEED 0x11

// last step vehicle number (get: induction loops, multi-entry/multi-exit detector)
#define LAST_STEP_VEHICLE_ID_LIST 0x12

// last step occupancy (get: induction loops)
#define LAST_STEP_OCCUPANCY 0x13

// last step vehicle halting number (get: multi-entry/multi-exit detector)
#define LAST_STEP_VEHICLE_HALTING_NUMBER 0x14

// last step mean vehicle length (get: induction loops)
#define LAST_STEP_LENGTH 0x15

// last step time since last detection (get: induction loops)
#define LAST_STEP_TIME_SINCE_DETECTION 0x16


// traffic light states, encoded as rRgGyYoO tuple (get: traffic lights)
#define TL_RED_YELLOW_GREEN_STATE 0x20

// traffic light states, encoded phase, brake, and yellow tuple (get: traffic lights, set: traffic lights)
#define TL_PHASE_BRAKE_YELLOW_STATE 0x21

// index of the phase (set: traffic lights)
#define TL_PHASE_INDEX 0x22

// traffic light program (set: traffic lights)
#define TL_PROGRAM 0x23

// phase duration (set: traffic lights)
#define TL_PHASE_DURATION 0x24

// complete definition (get: traffic lights)
#define TL_COMPLETE_DEFINITION_PBY 0x25

// controlled lanes (get: traffic lights)
#define TL_CONTROLLED_LANES 0x26

// controlled links (get: traffic lights)
#define TL_CONTROLLED_LINKS 0x27

// index of the current phase (get: traffic lights)
#define TL_CURRENT_PHASE 0x28

// name of the current program (get: traffic lights)
#define TL_CURRENT_PROGRAM 0x29

// controlled junctions (get: traffic lights)
#define TL_CONTROLLED_JUNCTIONS 0x2a

// complete definition (get: traffic lights)
#define TL_COMPLETE_DEFINITION_RYG 0x2b

// complete program (set: traffic lights)
#define TL_COMPLETE_PROGRAM_RYG 0x2c

// assumed time to next switch (get: traffic lights)
#define TL_NEXT_SWITCH 0x2d



// outgoing link number (get: lanes)
#define LANE_LINK_NUMBER 0x30

// id of parent edge (get: lanes)
#define LANE_EDGE_ID 0x31

// outgoing link definitions (get: lanes)
#define LANE_LINKS 0x33

// list of allowed vehicle classes (get&set: lanes)
#define LANE_ALLOWED 0x34

// list of not allowed vehicle classes (get&set: lanes)
#define LANE_DISALLOWED 0x35


// speed (get: vehicle)
#define VAR_SPEED 0x40

// maximum allowed/possible speed (get: vehicle types, lanes)
#define VAR_MAXSPEED 0x41

// position (2D) (get: vehicle, poi, set: poi)
#define VAR_POSITION 0x42

// angle (get: vehicle)
#define VAR_ANGLE 0x43

// angle (get: vehicle types, lanes)
#define VAR_LENGTH 0x44

// color (get: vehicles, vehicle types, polygons, pois)
#define VAR_COLOR 0x45

// max. acceleration (get: vehicle types)
#define VAR_ACCEL 0x46

// max. deceleration (get: vehicle types)
#define VAR_DECEL 0x47

// driver reaction time (get: vehicle types)
#define VAR_TAU 0x48

// vehicle class (get: vehicle types)
#define VAR_VEHICLECLASS 0x49

// emission class (get: vehicle types)
#define VAR_EMISSIONCLASS 0x4a

// shape class (get: vehicle types)
#define VAR_SHAPECLASS 0x4b

// offset (brake gap) (get: vehicle types)
#define VAR_GUIOFFSET 0x4c

// width (get: vehicle types)
#define VAR_WIDTH 0x4d

// shape (get: polygons)
#define VAR_SHAPE 0x4e

// type id (get: vehicles, polygons, pois)
#define VAR_TYPE 0x4f

// road id (get: vehicles)
#define VAR_ROAD_ID 0x50

// lane id (get: vehicles)
#define VAR_LANE_ID 0x51

// lane index (get: vehicles)
#define VAR_LANE_INDEX 0x52

// route id (get & set: vehicles)
#define VAR_ROUTE_ID 0x53

// edges (get: routes)
#define VAR_EDGES 0x54

// filled? (get: polygons)
#define VAR_FILL 0x55

// position (1D along lane) (get: vehicle)
#define VAR_LANEPOSITION 0x56

// route (set: vehicles)
#define VAR_ROUTE 0x57

// travel time information (get&set: vehicle)
#define VAR_EDGE_TRAVELTIME 0x58

// effort information (get&set: vehicle)
#define VAR_EDGE_EFFORT 0x59


// current CO2 emission of a node (get: vehicle)
#define VAR_CO2EMISSION 0x60

// current CO emission of a node (get: vehicle)
#define VAR_COEMISSION 0x61

// current HC emission of a node (get: vehicle)
#define VAR_HCEMISSION 0x62

// current PMx emission of a node (get: vehicle)
#define VAR_PMXEMISSION 0x63

// current NOx emission of a node (get: vehicle)
#define VAR_NOXEMISSION 0x64

// current fuel consumption of a node (get: vehicle)
#define VAR_FUELCONSUMPTION 0x65

// current noise emission of a node (get: vehicle)
#define VAR_NOISEEMISSION 0x66


// add an instance (poi, polygon)
#define ADD 0x80

// remove an instance (poi, polygon)
#define REMOVE 0x81



#endif




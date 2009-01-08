/****************************************************************************/
/// @file    TraCIConstants.h
/// @author  Axel Wegener <wegener@itm.uni-luebeck.de>
/// @date    2007/10/24
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
#define CMD_CLOSE   0x7F

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

// ****************************************
// POSITION REPRESENTATIONS
// ****************************************

// Omit position
#define POSITION_NONE    0x00
// 2D cartesian coordinates
#define POSITION_2D      0x01
// 2.5D cartesian coordinates
#define POSITION_2_5D    0x02
// 3D cartesian coordinates
#define POSITION_3D      0x03
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


// ****************************************
// RESULT TYPES
// ****************************************

// result type: Ok
#define RTYPE_OK    0x00
// result type: not implemented
#define RTYPE_NOTIMPLEMENTED  0x01
// result type: error
#define RTYPE_ERR   0xFF

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

#endif




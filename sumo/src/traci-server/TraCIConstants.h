/****************************************************************************/
/// @file    TraCIConstants.h
/// @author  Axel Wegener <wegener@itm.uni-luebeck.de>
/// @date    2007/10/24
///
/// holds codes used for TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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

// command: change route
#define CMD_CHANGEROUTE 0x30

// command: change target
#define CMD_CHANGETARGET 0x31

// command: Simulation Parameter
#define CMD_SIMPARAMETER 0x70

#define CMD_POSITIONCONVERSION 0x71

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

// command: slow down
#define CMD_SLOWDOWN 0x60 // -> 0x14

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
// RESULT TYPES
// ****************************************

// result type: Ok
#define RTYPE_OK    0x00
// result type: not implemented
#define RTYPE_NOTIMPLEMENTED  0x01
// result type: error
#define RTYPE_ERR   0xFF

#endif




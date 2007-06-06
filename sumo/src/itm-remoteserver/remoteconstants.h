/****************************************************************************/
/// @file    remoteconstants.h
/// @author  Thimor Bohn <bohn@itm.uni-luebeck.de>
/// @date    2007/03/13
/// @version $Id$
///
/// holds codes used for transmission protocol
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
#ifndef REMOTECONSTANTS_H
#define REMOTECONSTANTS_H



// ****************************************
// COMMANDS
// ****************************************

// command: simulation step
#define CMD_SIMSTEP 0x01
// command: close sumo
#define CMD_CLOSE   0xFF
// result type: no result


// ****************************************
// RESULT TYPES
// ****************************************

// result type: none
#define RTYPE_NONE  0x00
// result type: absolute position (x/y)
#define RTYPE_ABS   0x01
// result type: relative position (road/road pos)
#define RTYPE_REL   0x02
// result type: error
#define RTYPE_ERR   0xFF

#endif


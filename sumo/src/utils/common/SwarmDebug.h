/****************************************************************************/
/// @file    SwarmDebug.h
/// @author  Riccardo Belletti
/// @date    Apr 2013
/// @version $Id: SwarmDebug.h 1 2014-02-22 15:00:00Z riccardo_belletti $
///
// Used for additional optional debug messages
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#pragma once


//#ifndef ASSERT_H
//#define ASSERT_H
#include <assert.h>
//#endif


#ifndef SWARM_DEBUG
#define DBG(X) {}
#else
#define DBG(X) {X}
#endif/* DEBUG_H_ */

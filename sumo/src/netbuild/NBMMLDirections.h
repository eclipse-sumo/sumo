/****************************************************************************/
/// @file    NBMMLDirections.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// 	Possible directional types of lane-to-lane connections
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBMMLDirections_h
#define NBMMLDirections_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// enumerations
// ===========================================================================
/**
 * @enum NBMMLDirection
 * @brief Possible directional types of lane-to-lane connections
 *
 * A link between an incoming and an outgoing lane has an abstract direction.
 *  This direction is used within the gui to show the arrows at a lane's end.
 *
 * During network building, this direction information is used when building
 *  internal edges. In fact, this should be rechecked.
 *
 * @todo recheck usage within the computation of internal lanes, regard using something different
 */
enum NBMMLDirection {
    /// dead-end
    MMLDIR_NODIR,
    /// The link goes straight
    MMLDIR_STRAIGHT,
    /// The link is a turnaround link
    MMLDIR_TURN,
    /// The link turns left
    MMLDIR_LEFT,
    /// The link turns right
    MMLDIR_RIGHT,
    /// The link turns left, but not too much
    MMLDIR_PARTLEFT,
    /// The link turns right, but not too much
    MMLDIR_PARTRIGHT

};


#endif

/****************************************************************************/


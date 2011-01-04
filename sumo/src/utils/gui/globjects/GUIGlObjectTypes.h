/****************************************************************************/
/// @file    GUIGlObjectTypes.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A list of object types which may be displayed within the gui
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
#ifndef GUIGlObjectTypes_h
#define GUIGlObjectTypes_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// definitions
// ===========================================================================
/**
 * ChooseableArtifact
 * Enumeration to differ to show the list of which atrifact
 * The order is important during the selection of items for displaying their
 * tooltips; the item with the lowest type value (beside the network which
 * does not cause a hit as no "network" is being drawn) will be chosen.
 */
enum GUIGlObjectType {
    /// The network - empty
    GLO_NETWORK = 0,

    /// an edge
    GLO_EDGE = 1,

    /// a lane
    GLO_LANE = 2,

    /// a junction
    GLO_JUNCTION = 4,

    /// a tl-logic
    GLO_TLLOGIC = 8,

    /// a detector
    GLO_DETECTOR = 16,

    /// an emitter
    GLO_EMITTER = 32,

    /// a lane speed trigger,
    GLO_TRIGGER = 64,

    /// a shape
    GLO_SHAPE = 128,

    /// a vehicles
    GLO_VEHICLE = 256,

    /// compound additional
    GLO_ADDITIONAL = GLO_DETECTOR | GLO_EMITTER | GLO_TRIGGER,

    /// empty max
    GLO_MAX = 512

};


#endif

/****************************************************************************/


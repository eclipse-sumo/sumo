#ifndef GUIGlObjectType_h
#define GUIGlObjectType_h
//---------------------------------------------------------------------------//
//                        GUIGlObjectTypes.h -
//  A list of object types which may be displayed within the gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.4  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.3  2003/07/16 15:20:36  dkrajzew
// types enumeration is now using powers of two to allow enum combination
//
// Revision 1.2  2003/06/05 11:37:30  dkrajzew
// class templates applied
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


/* =========================================================================
 * definitions
 * ======================================================================= */
/**
 * ChooseableArtifact
 * Enumeration to differ to show the list of which atrifact
 * By now, one can display edges, junctions and vehicles
 */
enum GUIGlObjectType {
    /// The network - empty
    GLO_NETWORK = 0,
    /// a vehicles
    GLO_VEHICLE = 1,
    /// a detector
    GLO_DETECTOR = 2,
    /// an emitter
    GLO_EMITTER = 4,
    /// a lane
    GLO_LANE = 8,
    /// an edge
    GLO_EDGE = 16,
    /// a junction
    GLO_JUNCTION = 32
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIGlObjectTypes.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


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
    /// a vehicles
    GLO_VEHICLE = 0,
    /// a detector
    GLO_DETECTOR = 1,
    /// an emitter
    GLO_EMITTER = 2,
    /// a lane
    GLO_LANE = 3,
    /// an edges
    GLO_EDGE = 4,
    /// a junctions
    GLO_JUNCTION = 5
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIGlObjectTypes.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


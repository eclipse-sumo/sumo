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
// Revision 1.4  2005/10/07 11:45:32  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.3  2005/09/15 12:19:44  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/05/04 09:19:51  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.1  2004/11/23 10:38:31  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:50  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.6  2004/07/02 08:36:25  dkrajzew
// vss added
//
// Revision 1.5  2003/11/12 13:47:18  dkrajzew
// visualisation of tl-logics added
//
// Revision 1.4  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.3  2003/07/16 15:20:36  dkrajzew
// types enumeration is now using powers of two to allow enum combination
//
// Revision 1.2  2003/06/05 11:37:30  dkrajzew
// class templates applied
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H


/* =========================================================================
 * definitions
 * ======================================================================= */
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

    /// a lane
    GLO_LANE = 1,

    /// an edge
    GLO_EDGE = 2,

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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


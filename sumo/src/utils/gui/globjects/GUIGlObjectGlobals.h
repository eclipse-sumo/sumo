#ifndef GUIGlObjectGlobals_h
#define GUIGlObjectGlobals_h
//---------------------------------------------------------------------------//
//                        GUIGlObjectGlobals.h -
//  Some global variables (yep)
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.3  2005/09/15 12:19:44  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/05/04 09:18:58  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.1  2004/11/23 10:38:31  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:50  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.4  2004/08/02 11:49:11  dkrajzew
// gradients added; documentation added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <utils/gui/globjects/GUIGlObjectStorage.h>


/* =========================================================================
 * variable definitions
 * ======================================================================= */
/** @brief A container for numerical ids of objects
    in order to make them grippable by openGL */
extern GUIGlObjectStorage gIDStorage;


extern GUIGlObject *gNetWrapper;


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


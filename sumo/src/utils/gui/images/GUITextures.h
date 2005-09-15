#ifndef GUITextures_h
#define GUITextures_h
//---------------------------------------------------------------------------//
//                        GUITextures.h -
//  The base class for a view
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
// Revision 1.3  2005/09/15 12:19:55  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/07/12 11:55:37  dkrajzew
// fonts are now drawn using polyfonts; dialogs have icons; searching for structures improved;
//
// Revision 1.2  2005/06/14 11:29:50  dksumo
// documentation added
//

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <microsim/MSLink.h>


/* =========================================================================
 * enumeration
 * ======================================================================= */
enum GUITexture {
    TEXTURE_LINKDIR_STRAIGHT = MSLink::LINKDIR_STRAIGHT ,
    TEXTURE_LINKDIR_TURN = MSLink::LINKDIR_TURN,
    TEXTURE_LINKDIR_LEFT = MSLink::LINKDIR_LEFT,
    TEXTURE_LINKDIR_RIGHT = MSLink::LINKDIR_RIGHT,
    TEXTURE_LINKDIR_PARTLEFT = MSLink::LINKDIR_PARTLEFT,
    TEXTURE_LINKDIR_PARTRIGHT = MSLink::LINKDIR_PARTRIGHT,
    TEXTURE_MAX
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

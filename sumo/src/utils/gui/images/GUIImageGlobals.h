#ifndef GUIImageGlobals_h
#define GUIImageGlobals_h
//---------------------------------------------------------------------------//
//                        GUIImageGlobals.h -
//  A tooltip floating over a window
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
// Revision 1.5  2006/10/26 10:27:31  dkrajzew
// debugging
//
// Revision 1.6  2005/11/29 13:01:04  dksumo
// debugging
//
// Revision 1.5  2005/09/20 06:13:03  dksumo
// floats and doubles replaced by SUMOReal; warnings removed
//
// Revision 1.4  2005/09/15 12:27:57  dksumo
// code style changes
//
// Revision 1.3  2005/06/14 11:29:50  dksumo
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H


/* =========================================================================
 * variable declarations
 * ======================================================================= */
extern bool gAllowTextures;

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

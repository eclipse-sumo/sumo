#ifndef GUIExcp_VehicleIsInvisible_h
#define GUIExcp_VehicleIsInvisible_h
//---------------------------------------------------------------------------//
//                        GUIExcp_VehicleIsInvisible.h -
//  Exception thrown if an unvisible vehicle shall be taken into sight
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jun 2004
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
// Revision 1.3  2005/10/07 11:44:53  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.2  2005/09/15 12:18:59  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2004/11/23 10:38:29  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:47  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.1  2004/07/02 08:34:15  dkrajzew
// added to debug vehicle zoom
//
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

#include <exception>


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIExcp_VehicleIsInvisible : public std::exception {
public:
    GUIExcp_VehicleIsInvisible() { }
    ~GUIExcp_VehicleIsInvisible() throw() { }
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


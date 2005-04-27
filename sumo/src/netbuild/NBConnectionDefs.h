#ifndef NBConnectionDefs_h
#define NBConnectionDefs_h
//---------------------------------------------------------------------------//
//                        NBConnectionDefs.h -
//  Some conatiner for connections
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
// Revision 1.5  2005/04/27 11:48:25  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.4  2003/06/05 11:43:34  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <map>
#include "NBConnection.h"


/* =========================================================================
 * definitions
 * ======================================================================= */
/// Definition of a connection vector
typedef std::vector<NBConnection> NBConnectionVector;


/** @brief Definition of a container for connection block dependencies
    Includes a list of all connections which prohibit the key connection */
typedef std::map<NBConnection, NBConnectionVector> NBConnectionProhibits;


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


//---------------------------------------------------------------------------//
//                        MSTrigger.cpp -
//  The basic class for triggering artifacts
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 26.04.2004
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
// Revision 1.1  2004/07/02 09:56:40  dkrajzew
// debugging while implementing the vss visualisation
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSTrigger.h"
#include <utils/common/NamedObjectContSingleton.h>


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSTrigger::MSTrigger(const std::string &id)
    : Named(id)
{
//    NamedObjectContSingleton<MSTrigger*>::getInstance().add(id, this);
}


MSTrigger::~MSTrigger()
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


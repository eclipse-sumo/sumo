#ifndef MSTrigger_h
#define MSTrigger_h
//---------------------------------------------------------------------------//
//                        MSTrigger.h -
//  The basic class for triggering artifacts
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
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <utils/common/Named.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class MSTrigger : public Named {
public:
    MSTrigger(const std::string &id) : Named(id) { }
    ~MSTrigger() { }
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "MSTrigger.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


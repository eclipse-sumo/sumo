#ifndef ROEdgeCont_h
#define ROEdgeCont_h
//---------------------------------------------------------------------------//
//                        ROEdgeCont.h -
//  A container for routable edges
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
// Revision 1.4  2003/02/07 10:45:07  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <map>
#include <string>
#include "ROEdge.h"
#include <utils/common/NamedObjectCont.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class ROEdgeCont : public NamedObjectCont<ROEdge*> {
public:
	ROEdgeCont();
	~ROEdgeCont();
    void postloadInit();
    void init();
private:
    /// we made the copy constructor invalid
    ROEdgeCont(const ROEdgeCont &src);
    /// we made the assignment operator invalid
    ROEdgeCont &operator=(const ROEdgeCont &src);
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROEdgeCont.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


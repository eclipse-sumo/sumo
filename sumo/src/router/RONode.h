#ifndef RONode_h
#define RONode_h
//---------------------------------------------------------------------------//
//                        RONode.h -
//  A single router's node
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
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include "ROEdgeVector.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RONode
 * !!! not used ??
 */
class RONode {
private:
    ROEdgeVector _edges;
public:
    RONode(const std::string &id, const ROEdgeVector &edges);
    ~RONode();
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "RONode.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


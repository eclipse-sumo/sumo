#ifndef IntVector_h
#define IntVector_h
//---------------------------------------------------------------------------//
//                        IntVector.h -
//  A simple vector of integers
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
// Revision 1.3  2003/06/05 11:54:48  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>


/* =========================================================================
 * definitions
 * ======================================================================= */
typedef std::vector<int> IntVector;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class IntVectorHelper {
public:
    static void removeDouble(IntVector &v);

    /// Returns the information whether at least one element is within both vectors
    static bool subSetExists(const IntVector &v1, const IntVector &v2);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "IntVector.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


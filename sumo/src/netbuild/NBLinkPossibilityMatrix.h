#ifndef NBLinkPossibilityMatrix_h
#define NBLinkPossibilityMatrix_h
//---------------------------------------------------------------------------//
//                        NBLinkPossibilityMatrix.h -
//  A matric to describe whether two links are foes to each other
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
// Revision 1.3  2004/01/12 15:09:28  dkrajzew
// some work on the documentation
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <bitset>
#include <vector>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NBLinkPossibilityMatrix
 * Storing the information which links may be used simultanously, this matrix
 * is simply made by a vector of bitsets
 */
typedef std::vector<std::bitset<64> > NBLinkPossibilityMatrix;


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NBLinkPossibilityMatrix.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


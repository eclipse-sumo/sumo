#ifndef DoubleVector_h
#define DoubleVector_h
//---------------------------------------------------------------------------//
//                        DoubleVector.h -
//  A simple vector of doubles
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
// Revision 1.7  2003/08/20 11:46:28  dkrajzew
// some further methods added needed for the computation of node shapes
//
// Revision 1.6  2003/06/05 11:54:48  dkrajzew
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
typedef std::vector<double> DoubleVector;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class DoubleVectorHelper {
public:
    static double sum(const DoubleVector &dv);
    static void normalise(DoubleVector &dv, double sum=1.0);
    static void div(DoubleVector &dv, double by);
    static void removeDouble(DoubleVector &v);
    static void set(DoubleVector &dv, double to);
    static double max(const DoubleVector &dv);
    static double min(const DoubleVector &dv);
    static void remove_smaller_than(DoubleVector &v, double swell);
    static void remove_larger_than(DoubleVector &v, double swell);
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "DoubleVector.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


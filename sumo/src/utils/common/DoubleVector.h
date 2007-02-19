/****************************************************************************/
/// @file    DoubleVector.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A simple vector of SUMOReals
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef DoubleVector_h
#define DoubleVector_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>


// ===========================================================================
// definitions
// ===========================================================================
typedef std::vector<SUMOReal> DoubleVector;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class DoubleVectorHelper
{
public:
    static SUMOReal sum(const DoubleVector &dv);
    static void normalise(DoubleVector &dv, SUMOReal sum=1.0);
    static void div(DoubleVector &dv, SUMOReal by);
    static void removeDouble(DoubleVector &v);
    static void set(DoubleVector &dv, SUMOReal to);
    static SUMOReal maxValue(const DoubleVector &dv);
    static SUMOReal minValue(const DoubleVector &dv);
    static void remove_smaller_than(DoubleVector &v, SUMOReal swell);
    static void remove_larger_than(DoubleVector &v, SUMOReal swell);
    static void add2All(DoubleVector &v, SUMOReal what);

};


#endif

/****************************************************************************/


/****************************************************************************/
/// @file    IntVector.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// A simple vector of integers
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
#ifndef IntVector_h
#define IntVector_h
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
#include <iostream>


// ===========================================================================
// definitions
// ===========================================================================
typedef std::vector<int> IntVector;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class IntVectorHelper
{
public:
    static void removeDouble(IntVector &v);

    /// Returns the information whether at least one element is within both vectors
    static bool subSetExists(const IntVector &v1, const IntVector &v2);

};

std::ostream &operator<<(std::ostream &os, const IntVector &iv);


#endif

/****************************************************************************/


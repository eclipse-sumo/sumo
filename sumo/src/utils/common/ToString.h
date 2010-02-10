/****************************************************************************/
/// @file    ToString.h
/// @author  Christian Roessel
/// @date    Wed, 23 Sep 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ToString_h
#define ToString_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <sstream>
#include <string>
#include <iomanip>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * Template for conversions from origin format to string representation
 * (when supplied by c++/the stl)
 */
template <class T>
inline std::string toString(const T& t, std::streamsize accuracy=OUTPUT_ACCURACY) {
    std::ostringstream oss;
    oss.setf(std::ios::fixed , std::ios::floatfield);
    oss << std::setprecision(accuracy);
    oss << t;
    return oss.str();
}


#endif

/****************************************************************************/


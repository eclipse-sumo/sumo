/****************************************************************************/
/// @file    IDSupplier.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A class that generates enumerated and prefixed string-ids
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <sstream>
#include "IDSupplier.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
IDSupplier::IDSupplier(const std::string &prefix, long begin)
        : myCurrent(begin), myPrefix(prefix) {}


IDSupplier::~IDSupplier() {}


std::string
IDSupplier::getNext() {
    std::ostringstream strm;
    strm << myPrefix << myCurrent++;
    return strm.str();
}



/****************************************************************************/


/****************************************************************************/
/// @file    Distribution_MeanDev.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A distribution described by the mean value and std-dev amount
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "Distribution.h"
#include "Distribution_MeanDev.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
Distribution_MeanDev::Distribution_MeanDev(const std::string &id,
        SUMOReal mean, SUMOReal deviation) throw()
        : Distribution(id), myMeanValue(mean), myDeviation(deviation) {}


Distribution_MeanDev::~Distribution_MeanDev() throw() {}


SUMOReal
Distribution_MeanDev::getMax() const {
    return myMeanValue + myDeviation;
}



/****************************************************************************/


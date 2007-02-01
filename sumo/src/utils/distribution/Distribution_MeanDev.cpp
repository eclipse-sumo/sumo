/****************************************************************************/
/// @file    Distribution_MeanDev.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// A distribution described by the mean value and std-dev amount
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

#include "Distribution.h"
#include "Distribution_MeanDev.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// method definitions
// ===========================================================================
Distribution_MeanDev::Distribution_MeanDev(const std::string &id,
        SUMOReal mean, SUMOReal deviation)
        : Distribution(id), myMeanValue(mean), myDeviation(deviation)
{}


Distribution_MeanDev::~Distribution_MeanDev()
{}


SUMOReal
Distribution_MeanDev::getMax() const
{
    return myMeanValue + myDeviation;
}



/****************************************************************************/


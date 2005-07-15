///
/// @file    MSE3Collector.cpp
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Tue Dec 02 2003 22:17 CET
/// @version
///
/// @brief
///
///

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2005/07/15 07:18:40  dkrajzew
// code style applied
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "MSE3Collector.h"


/* =========================================================================
 * variable declarations
 * ======================================================================= */
std::string MSE3Collector::xmlHeaderM(
"<?xml version=\"1.0\" standalone=\"yes\"?>\n\n"
"<!--\n"
"- nVehicles [veh]\n"
"- meanTraveltime [s]\n"
"- meanNHaltsPerVehicle [n]\n"
"-->\n\n");


std::string MSE3Collector::infoEndM = std::string("</detector>");

namespace E3
{
    DetType& operator++( DetType& det ) {
        return det = ( ALL == det ) ? MEAN_TRAVELTIME : DetType( det + 1 );
    }

    Containers& operator++( Containers& cont ) {
        return cont =
            ( TRAVELTIME == cont ) ? VEHICLES : Containers( cont + 1 );
    }
}
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

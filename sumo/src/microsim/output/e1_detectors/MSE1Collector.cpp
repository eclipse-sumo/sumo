///
/// @file    MSE1Collector.cpp
/// @author  Christian Roessel <christian.roessel@gmx.de>
/// @date    Started Wed Jun 9 16:40:56 CEST 2004
/// @version $Id$
///
/// @brief
///

/* Copyright (C) 2004 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "MSE1Collector.h"

std::string MSE1Collector::xmlHeaderM(
"<?xml version=\"1.0\" standalone=\"yes\"?>\n\n"
"<!--\n"
"- meanSpeed [m/s]\n"
"- nVehicles [veh]\n"
"- flow [veh/h]\n"
"- meanOccupancy [%]\n"
"- meanVehLength [m]\n"
"-->\n\n");


std::string MSE1Collector::infoEndM = std::string("</detector>");

namespace E1
{
    DetType& operator++( DetType& det ) {
        return det = ( ALL == det ) ? MEAN_SPEED : DetType( det + 1 );
    }

//     Containers& operator++( Containers& cont ) {
//         return cont =
//             ( TRAVELTIME == cont ) ? VEHICLES : Containers( cont + 1 );
//     }
}

// int main()
// {
//     MSLane* lane = 0;
//     MSCrossSection crossSec = MSCrossSection( lane, 250 );
//     MSE1Collector("test",
//                   crossSec );
// }

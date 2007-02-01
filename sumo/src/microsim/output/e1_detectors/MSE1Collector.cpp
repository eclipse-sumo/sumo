/****************************************************************************/
/// @file    MSE1Collector.cpp
/// @author  Christian Roessel
/// @date    Wed Jun 9 16:40:56 CEST 2004
/// @version $Id: $
///
// / @author  Christian Roessel <christian.roessel@gmx.de>
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

#include "MSE1Collector.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

// ===========================================================================
// variable declarations
// ===========================================================================

std::string MSE1Collector::xmlHeaderM(
    "<?xml version=\"1.0\" standalone=\"yes\"?>\n\n"
    "<!--\n"
    "- meanSpeed [m/s]\n"
    "- nVehicles [veh]\n"
    "- flow [veh/h]\n"
    "- meanOccupancy [%]\n"
    "- meanVehLength [m]\n"
    "-->\n\n");


std::string MSE1Collector::infoEndM = "</detector>";

namespace E1
{
DetType& operator++(DetType& det)
{
    return det = (ALL == det) ? MEAN_SPEED : DetType(det + 1);
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



/****************************************************************************/


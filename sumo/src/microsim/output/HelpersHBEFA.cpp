/****************************************************************************/
/// @file    HelpersHBEFA.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id:HelpersHBEFA.cpp 4976 2008-01-30 14:23:39Z dkrajzew $
///
// Helper methods for HBEFA-based emission computation
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "HelpersHBEFA.h"
#include <limits>

#ifdef HAVE_MESOSIM
#include <microsim/MSGlobals.h>
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
SUMOReal 
HelpersHBEFA::computeCO(SUMOEmissionClass c, double v, double a) throw()
{
    return 0;
}


SUMOReal 
HelpersHBEFA::computeCO2(SUMOEmissionClass c, double v, double a) throw()
{
    double c0, cav1, cav2, c1, c2, c3;
    switch(c) {
    case SVE_UNKNOWN:
    case SVE_PASSENGER_EURO4__1_4__2l:
        c0 = 2848.06755056818;
        cav1 = 19.3232413452727;
        cav2 = 0.524796713753243;
        c1 = 102.470056970178;
        c2 = -0.523312751898079;
        c3 = 0.00805027554844549;
        break;
    case SVE_BUS_CITY:
        c0 = 991.101219412531;
        cav1 = 179.294485797466;
        cav2 = 9.17486361318017;
        c1 = 1316.33892721299;
        c2 = -11.9281543905269;
        c3 = -0.0403471773282253;
        break;
    case SVE_BUS_OVERLAND:
        c0 = 8553.23162570527;
        cav1 = 237.462164857203;
        cav2 = 13.1274968958206;
        c1 = 969.964377231991;
        c2 = -7.06185996371277;
        c3 = 0.0428354657150744;
        break;
    case SVE_HDV_7_5t__EURO4:
        c0 = 583.08186029781;
        cav1 = 61.8659020296918;
        cav2 = 2.70352119127361;
        c1 = 360.764731290949;
        c2 = -3.92381946077386;
        c3 = 0.0389758421245056;
        break;
    default:
        c0 = 2848.06755056818;
        cav1 = 19.3232413452727;
        cav2 = 0.524796713753243;
        c1 = 102.470056970178;
        c2 = -0.523312751898079;
        c3 = 0.00805027554844549;
        break;
    }
    return (SUMOReal) (c0 + cav1*a*v + cav2*pow(a,2.)*v + c1*v + c2*pow(v,2.) + c3*pow(v,3.));
}


SUMOReal 
HelpersHBEFA::computeNOx(SUMOEmissionClass c, double v, double a) throw()
{
    return 0;
}


SUMOReal 
HelpersHBEFA::computePMx(SUMOEmissionClass c, double v, double a) throw()
{
    return 0;
}


SUMOReal 
HelpersHBEFA::computeFuel(SUMOEmissionClass c, double v, double a) throw()
{
    return 0;
}


/****************************************************************************/


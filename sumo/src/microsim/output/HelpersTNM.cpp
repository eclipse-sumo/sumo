/****************************************************************************/
/// @file    HelpersTNM.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id: HelpersTNM.cpp 6711 2009-02-02 14:44:15Z dkrajzew $
///
// Helper methods for TNM-based noise emission computation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include "HelpersTNM.h"
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
// static definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// function parameter
// ---------------------------------------------------------------------------
double 
HelpersTNM::myFunctionParameter[2][14] =
{
    // passenger
    { 58.1, 58.1, 56.4, 58.4, 61.3, 64, 66.4, 68.5, 70.4, 72, 73.5, 74.9, 76.2, 77.4 },
    // heavy trucks
    { 82.7, 82.7, 79.4, 77.8, 77.3, 77.5, 78.3, 79.2, 80.3, 81.4, 82.5, 83.6, 84.6, 85.6 },
};


// ===========================================================================
// method definitions
// ===========================================================================
SUMOReal
HelpersTNM::computeNoise(SUMOEmissionClass c, double v) throw()
{
    double *vals;
    if(c>=SVE_HDV_3_1 && c<=SVE_HDV_12_12) {
        vals = myFunctionParameter[1];
    } else if(c!=SVE_ZERO_EMISSIONS) {
        vals = myFunctionParameter[0];
    } else {
        return 0;
    }
    //
    double val = vals[13];
    v = v * 3.6;
    if(v<=0) {
        return vals[0];
    } else if(v<130.) {
        int sm = v / 10;
        return vals[sm] + (vals[sm+1]-vals[sm])*(v-double(sm)*10.)/10.;
    } else {
        return vals[13];
    }
}


/****************************************************************************/


/****************************************************************************/
/// @file    RORDGenerator_Random.cpp
/// @author  Daniel Krajzewicz
/// @date    SUMOTime begin,
/// @version $Id$
///
// A "trip loader" for random trip generation
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

#include <string>
#include "RORDGenerator_Random.h"
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/TplConvert.h>
#include <utils/common/RandHelper.h>
#include <utils/options/OptionsCont.h>
#include "RORouteDef.h"
#include "RORouteDef_OrigDest.h"
#include "RONet.h"
#include "ROVehicleBuilder.h"

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
RORDGenerator_Random::RORDGenerator_Random(ROVehicleBuilder &vb, RONet &net,
        SUMOTime begin,
        SUMOTime end,
        bool removeFirst,
        const std::string &)
        : ROAbstractRouteDefLoader(vb, net, begin, end), myIDSupplier("Rand"),
        myCurrentTime(0), myRemoveFirst(removeFirst)
{
    if (OptionsCont::getOptions().isSet("random-route-color")) {
        myColor = RGBColor::parseColor(OptionsCont::getOptions().getString("random-route-color"));
    }
}


RORDGenerator_Random::~RORDGenerator_Random()
{}


void
RORDGenerator_Random::closeReading()
{}


std::string
RORDGenerator_Random::getDataName() const
{
    return "random_trips";
}


bool
RORDGenerator_Random::myReadRoutesAtLeastUntil(SUMOTime time)
{
    // check whether the first route have to be skipped
    if (time==myBegin) {
        myCurrentTime = time + 1;
        myReadNewRoute = true;
        return true;
    }
    myReadNewRoute = false;
    // ... ok, really for route building
    myCurrentProgress += myWishedPerSecond;
    while (myCurrentProgress>0) {
        // get the next trip
        ROEdge *from = myNet.getRandomSource();
        ROEdge *to = myNet.getRandomDestination();
        // chekc whether valid values could be found
        if (from==0||to==0) {
            if (from==0) {
                MsgHandler::getErrorInstance()->inform("The network does not contain any valid starting edges!");
            }
            if (from==0) {
                MsgHandler::getErrorInstance()->inform("The network does not contain any valid end edges!");
            }
            return false;
        }
        // build trip and add
        SUMOVehicleParameter pars;
        pars.id = myIDSupplier.getNext();
        pars.depart = time;
        RORouteDef *route = new RORouteDef_OrigDest(pars.id, myColor, from, to, myRemoveFirst);
        myNet.addVehicle(pars.id, myVehicleBuilder.buildVehicle(pars, route, 0));
        myNet.addRouteDef(route);
        myReadNewRoute = true;
        // decrement counter
        myCurrentProgress -= 1;
    }
    // compute the next emission time
    if (myWishedPerSecond>1) {
        myCurrentTime = time+1;
    } else {
        // compute into the future
        myCurrentTime = time + (size_t)
                        ((1.0-myCurrentProgress) / myWishedPerSecond);
    }
    return true;
}


bool
RORDGenerator_Random::init(OptionsCont &options)
{
    myWishedPerSecond = options.getFloat("random-per-second");
    myCurrentProgress = myWishedPerSecond / (SUMOReal) 2.0;
    if (myWishedPerSecond<0) {
        throw ProcessError("We cannot less than no vehicle!");
    }
    return true;
}


SUMOTime
RORDGenerator_Random::getCurrentTimeStep() const
{
    return myCurrentTime;
}


bool
RORDGenerator_Random::ended() const
{
    return false;
}



/****************************************************************************/


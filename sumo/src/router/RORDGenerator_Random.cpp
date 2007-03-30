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

#include <string>
#include "RORDGenerator_Random.h"
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/TplConvert.h>
#include <utils/common/RandHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsSubSys.h>
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
    if (!OptionsSubSys::getOptions().isSet("random-route-color")) {
        myColor = RGBColor(-1, -1, -1);
        return;
    }
    string color =
        OptionsSubSys::getOptions().getString("random-route-color");
    StringTokenizer st(color, ";");
    try {
        SUMOReal r = TplConvert<char>::_2SUMOReal(st.next().c_str());
        SUMOReal g = TplConvert<char>::_2SUMOReal(st.next().c_str());
        SUMOReal b = TplConvert<char>::_2SUMOReal(st.next().c_str());
        myColor = RGBColor(r, g, b);
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Something is wrong with the color definition for random routes\n Option: 'random-route-color'");
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
        ROEdge *from = _net.getRandomSource();
        ROEdge *to = _net.getRandomDestination();
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
        string id = myIDSupplier.getNext();
        RORouteDef *route =
            new RORouteDef_OrigDest(id, myColor, from, to, myRemoveFirst);
        _net.addVehicle(id,
                        myVehicleBuilder.buildVehicle(
                            id, route, time, 0,
                            RGBColor(randSUMO((SUMOReal) .5, (SUMOReal) 1.), randSUMO((SUMOReal) .5, (SUMOReal) 1.), randSUMO((SUMOReal) .5, (SUMOReal) 1.)),
                            -1, 0));
        _net.addRouteDef(route);
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
        MsgHandler::getErrorInstance()->inform("We cannot less than no vehicle!");
        throw ProcessError();
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


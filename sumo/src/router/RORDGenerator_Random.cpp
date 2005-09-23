//---------------------------------------------------------------------------//
//                        RORDGenerator_Random.cpp -
//  A "trip loader" for random trip generation
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

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
// Revision 1.7  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/05/04 08:50:05  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.4  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.3  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.2  2004/02/16 13:47:07  dkrajzew
// Type-dependent loader/generator-"API" changed
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// ------------------------------------------------
// Revision 1.9  2003/10/31 08:00:32  dkrajzew
// hope to have patched false usage of RAND_MAX when using gcc
//
// Revision 1.8  2003/07/30 09:26:33  dkrajzew
// all vehicles, routes and vehicle types may now have specific colors
//
// Revision 1.7  2003/07/18 12:35:06  dkrajzew
// removed some warnings
//
// Revision 1.6  2003/07/16 15:36:50  dkrajzew
// vehicles and routes may now have colors
//
// Revision 1.5  2003/06/18 11:36:50  dkrajzew
// a new interface which allows to choose whether to stop after a route could not be computed or not; not very sphisticated, in fact
//
// Revision 1.3  2003/05/20 09:48:35  dkrajzew
// debugging
//
// Revision 1.2  2003/04/10 15:47:01  dkrajzew
// random routes are now being prunned to avoid some stress with turning vehicles
//
// Revision 1.1  2003/04/09 15:41:19  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include "RORDGenerator_Random.h"
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/TplConvert.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsSubSys.h>
#include "RORouteDef.h"
#include "RORouteDef_OrigDest.h"
#include "RONet.h"
#include "ROVehicleBuilder.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
RORDGenerator_Random::RORDGenerator_Random(ROVehicleBuilder &vb, RONet &net,
                                           SUMOTime begin,
                                           SUMOTime end,
                                           const std::string &file)
    : ROAbstractRouteDefLoader(vb, net, begin, end), myIDSupplier("Rand"),
    myCurrentTime(0)
{
    if(!OptionsSubSys::getOptions().isSet("random-route-color")) {
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
        MsgHandler::getErrorInstance()->inform(
            string("Something is wrong with the color definition for random routes\n")
            + string(" Option: 'random-route-color'"));
    }
}


RORDGenerator_Random::~RORDGenerator_Random()
{
}


void
RORDGenerator_Random::closeReading()
{
}


std::string
RORDGenerator_Random::getDataName() const
{
    return "random_trips";
}


bool
RORDGenerator_Random::myReadRoutesAtLeastUntil(SUMOTime time)
{
    // check whether the first route have to be skipped
    if(time==myBegin) {
        myCurrentTime = time + 1;
        myReadNewRoute = true;
        return true;
    }
    myReadNewRoute = false;
    // ... ok, really for route building
    myCurrentProgress += myWishedPerSecond;
    while(myCurrentProgress>0) {
        // get the next trip
        ROEdge *from = _net.getRandomSource();
        ROEdge *to = _net.getRandomDestination();
        // chekc whether valid values could be found
        if(from==0||to==0) {
            if(from==0) {
                MsgHandler::getErrorInstance()->inform(
                    "The network does not contain any valid starting edges!");
            }
            if(from==0) {
                MsgHandler::getErrorInstance()->inform(
                    "The network does not contain any valid end edges!");
            }
            return false;
        }
        // build trip and add
        string id = myIDSupplier.getNext();
        RORouteDef *route =
            new RORouteDef_OrigDest(id, myColor, from, to, true);
        _net.addVehicle(id,
            myVehicleBuilder.buildVehicle(
                id, route, time, _net.getDefaultVehicleType(),
                RGBColor(
					(SUMOReal) ((double) rand() / (double) (RAND_MAX + 1) / 2.0 + 0.5),
					(SUMOReal) ((double) rand() / (double) (RAND_MAX + 1) / 2.0 + 0.5),
					(SUMOReal) ((double) rand() / (double) (RAND_MAX + 1) / 2.0 + 0.5)),
                -1, 0));
        _net.addRouteDef(route);
        myReadNewRoute = true;
        // decrement counter
        myCurrentProgress -= 1;
    }
    // compute the next emission time
    if(myWishedPerSecond>1) {
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
    if(myWishedPerSecond<0) {
        MsgHandler::getErrorInstance()->inform(
            "We cannot less than no vehicle!");
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



//---------------------------------------------------------------------------//
//                        RORandomTripGenerator.cpp -
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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include "RORandomTripGenerator.h"
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/options/OptionsCont.h>
#include "RORouteDef.h"
#include "ROOrigDestRouteDef.h"
#include "RONet.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
RORandomTripGenerator::RORandomTripGenerator(RONet &net,
                                             const std::string &file)
    : ROTypedRoutesLoader(net),
    myIDSupplier("Rand")
{
}

RORandomTripGenerator::~RORandomTripGenerator()
{
}



ROTypedRoutesLoader *
RORandomTripGenerator::getAssignedDuplicate(const std::string &file) const
{
    return new RORandomTripGenerator(_net, file);
}


void
RORandomTripGenerator::closeReading()
{
}


std::string
RORandomTripGenerator::getDataName() const
{
    return "random_trips";
}


bool
RORandomTripGenerator::startReadingSteps()
{
    return true;
}


bool
RORandomTripGenerator::readNextRoute(long start)
{
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
            new ROOrigDestRouteDef(id, from, to, true);
        _net.addVehicle(id,
            new ROVehicle(id, route, start, _net.getDefaultVehicleType(),
                RGBColor(
                    double( rand() ) / double( RAND_MAX ) / 2.0 + 0.5,
                    double( rand() ) / double( RAND_MAX ) / 2.0 + 0.5,
                    double( rand() ) / double( RAND_MAX )  / 2.0 + 0.5),
                -1, 0));
        _net.addRouteDef(route);
        _nextRouteRead = true;
        // decrement counter
        myCurrentProgress -= 1;
    }
    // compute the next emission time
    if(myWishedPerSecond>1) {
        _currentTimeStep = start+1;
    } else {
        // compute into the future
        _currentTimeStep = start +
            (1.0-myCurrentProgress) / myWishedPerSecond;
    }
    return true;
}



bool
RORandomTripGenerator::myInit(OptionsCont &options)
{
    myWishedPerSecond = options.getFloat("random-per-second");
    myCurrentProgress = myWishedPerSecond / 2.0;
    if(myWishedPerSecond<0) {
        MsgHandler::getErrorInstance()->inform(
            "We cannot less than no vehicle!");
        throw ProcessError();
    }
    return true;
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "RORandomTripGenerator.icc"
//#endif

// Local Variables:
// mode:C++
// End:



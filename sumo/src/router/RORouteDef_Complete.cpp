/****************************************************************************/
/// @file    RORouteDef_Complete.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A complete route definition (with all passed edges being known)
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
#include <deque>
#include "ROEdge.h"
#include "RORouteDef.h"
#include "RORoute.h"
#include <utils/common/SUMOAbstractRouter.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include "RORouteDef_Complete.h"
#include "ROHelper.h"
#include <utils/iodevices/OutputDevice.h>

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
RORouteDef_Complete::RORouteDef_Complete(const std::string &id,
        const RGBColor * const color,
        const std::vector<const ROEdge*> &edges,
        bool tryRepair) throw()
        : RORouteDef(id, color), myEdges(edges), myTryRepair(tryRepair)
{
}


RORouteDef_Complete::~RORouteDef_Complete() throw()
{}


RORoute *
RORouteDef_Complete::buildCurrentRoute(SUMOAbstractRouter<ROEdge,ROVehicle> &router,
                                       SUMOTime begin, const ROVehicle &veh) const
{
    if (myTryRepair) {
        std::vector<const ROEdge*> newEdges;
        const std::vector<const ROEdge*> &oldEdges = myEdges;
        newEdges.push_back(*(oldEdges.begin()));
        for (std::vector<const ROEdge*>::const_iterator i=oldEdges.begin()+1; i!=oldEdges.end(); ++i) {
            if ((*(i-1))->isConnectedTo(*i)) {
                newEdges.push_back(*i);
            } else {
                std::vector<const ROEdge*> edges;
                router.compute(*(i-1), *i, &veh, begin, edges);
                if (edges.size()==0) {
                    return 0;
                }
                std::copy(edges.begin()+1, edges.end(), back_inserter(newEdges));
            }
        }
        if (myEdges!=newEdges) {
            MsgHandler::getWarningInstance()->inform("Repaired route of vehicle '" + veh.getID() + "'.");
        }
        myEdges = newEdges;
    }
    SUMOReal costs = ROHelper::recomputeCosts(myEdges, &veh, begin);
    if (costs<0) {
        throw ProcessError("Route '" + getID() + "' (vehicle '" + veh.getID() + "') is not valid.");
    }
    return new RORoute(myID, 0, 1, myEdges, copyColorIfGiven());
}


void
RORouteDef_Complete::addAlternative(const ROVehicle *const, RORoute *current, SUMOTime begin)
{
    myStartTime = begin;
    delete current;
}


RORouteDef *
RORouteDef_Complete::copy(const std::string &id) const
{
    return new RORouteDef_Complete(id, copyColorIfGiven(), myEdges, myTryRepair);
}


OutputDevice &
RORouteDef_Complete::writeXMLDefinition(OutputDevice &dev, const ROVehicle * const veh,
                                        bool asAlternatives, bool withExitTimes) const
{
    // (optional) alternatives header
    if (asAlternatives) {
        dev << "<routeDistribution last=\"0\">\n         ";
    }
    // the route
    dev << "<route";
    if (asAlternatives) {
        dev << " cost=\"" << ROHelper::recomputeCosts(myEdges, veh, veh->getDepartureTime());
        dev << "\" probability=\"1.00\"";
    }
    if (myColor!=0) {
        dev << " color=\"" << *myColor << "\"";
    }
    dev << " edges=\"" << myEdges;
    if (withExitTimes) {
        SUMOReal time = veh->getDepartureTime();
        dev << "\" exitTimes=\"";
        std::vector<const ROEdge*>::const_iterator i = myEdges.begin();
        for (; i!=myEdges.end(); ++i) {
            time += (*i)->getEffort(veh, time);
            dev << time << " ";
        }
    }
    dev << "\"/>\n";
    // (optional) alternatives end
    if (asAlternatives) {
        dev << "      </routeDistribution>\n";
    }
    return dev;
}



/****************************************************************************/


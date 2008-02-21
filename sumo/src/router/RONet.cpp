/****************************************************************************/
/// @file    RONet.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The router's network representation
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
#include <iostream>
#include <fstream>
#include <deque>
#include <queue>
#include "RONodeCont.h"
#include "ROEdgeCont.h"
#include "ROVehTypeCont.h"
#include "ROEdge.h"
#include "RONode.h"
#include "RONet.h"
#include "RORoute.h"
#include "RORouteDef.h"
#include "RORouteDefCont.h"
#include "ROVehicle.h"
#include "ROVehicleType.h"
#include "ROVehicleType_ID.h"
#include <utils/common/SUMOAbstractRouter.h>
#include "ROAbstractEdgeBuilder.h"
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
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
RONet::RONet(bool /*multireferencedRoutes*/)
        : myVehicleTypes(),
        myRoutesOutput(0), myRouteAlternativesOutput(0),
        myReadRouteNo(0), myDiscardedRouteNo(0), myWrittenRouteNo(0),
        myHaveRestrictions(false)
{}


RONet::~RONet()
{
    myNodes.clear();
    myEdges.clear();
    myVehicleTypes.clear();
    myRoutes.clear();
    myVehicles.clear();
}


void
RONet::addEdge(ROEdge *edge)
{
    if (!myEdges.add(edge->getID(), edge)) {
        MsgHandler::getErrorInstance()->inform("The edge '" + edge->getID() + "' occures at least twice.");
        delete edge;
    }
}


ROEdge *
RONet::getEdge(const std::string &name) const
{
    return myEdges.get(name);
}


void
RONet::addNode(RONode *node)
{
    myNodes.add(node->getID(), node);
}


RONode *
RONet::getNode(const std::string &name) const
{
    return myNodes.get(name);
}



bool
RONet::isKnownVehicleID(const std::string &id) const
{
    VehIDCont::const_iterator i =
        find(myVehIDs.begin(), myVehIDs.end(), id);
    if (i==myVehIDs.end()) {
        return false;
    }
    return true;
}


void
RONet::addVehicleID(const std::string &id)
{
    myVehIDs.push_back(id);
}


RORouteDef *
RONet::getRouteDef(const std::string &name) const
{
    return myRoutes.get(name);
}


void
RONet::addRouteDef(RORouteDef *def)
{
    myRoutes.add(def);
}


void
RONet::openOutput(const std::string &filename, bool useAlternatives)
{
    myRoutesOutput = &OutputDevice::getDevice(filename);
    myRoutesOutput->writeXMLHeader("routes");
    if (useAlternatives) {
        myRouteAlternativesOutput = &OutputDevice::getDevice(filename+".alt");
        myRouteAlternativesOutput->writeXMLHeader("route-alternatives");
    }
}


void
RONet::closeOutput()
{
    // end writing
    if (myRoutesOutput!= 0) {
        myRoutesOutput->close();
    }
    // only if opened
    if (myRouteAlternativesOutput!=0) {
        myRouteAlternativesOutput->close();
    }
}



ROVehicleType *
RONet::getVehicleTypeSecure(const std::string &id)
{
    // check whether the type was already known
    ROVehicleType *type = myVehicleTypes.get(id);
    if (type!=0) {
        return type;
    }
    if (id=="") {
        // ok, no vehicle type was given within the user input
        //  return the default type
        return 0;
    }
    // Assume, the user will define the type somewhere else
    //  return a type which contains the id only
    type = new ROVehicleType_ID(id);
    addVehicleType(type);
    return type;
}


void
RONet::addVehicleType(ROVehicleType *type)
{
    myVehicleTypes.add(type->getID(), type);
}


void
RONet::addVehicle(const std::string &id, ROVehicle *veh)
{
    myVehicles.add(id, veh);
    myReadRouteNo++;
}

const RORouteDef * const
RONet::computeRoute(OptionsCont &options, SUMOAbstractRouter<ROEdge,ROVehicle> &router,
                    const ROVehicle * const veh)
{
    MsgHandler *mh = MsgHandler::getErrorInstance();
    if (options.getBool("continue-on-unbuild")) {
        mh = MsgHandler::getWarningInstance();
    }
    RORouteDef * const routeDef = veh->getRoute();
    // check if the route definition is valid
    if (routeDef==0) {
        mh->inform("The vehicle '" + veh->getID() + "' has no valid route.");
        return 0;
    }
    // check whether the route was already saved
    if (routeDef->isSaved()) {
        return routeDef;
    }
    //
    RORoute *current =
        routeDef->buildCurrentRoute(router, veh->getDepartureTime(), *veh);
    if (current==0) {
        return 0;
    }
    // check whether we have to evaluate the route for not containing loops
    if (options.getBool("remove-loops")) {
        current->recheckForLoops();
    }
    // check whether the route is valid and does not end on the starting edge
    if (current->size()<2) {
        // check whether the route ends at the starting edge
        //  unbuild routes due to missing connections are reported within the
        //  router
        if (current->size()!=0) {
            mh->inform("The route '" + routeDef->getID() + "' is too short, probably ending at the starting edge.\n Skipping...");
        }
        delete current;
        return 0;
    }
    // check whether the vehicle is able to start at this edge
    //  (the edge must be longer than the vehicle)
    SUMOReal vehLength = veh->getType()!=0 ? veh->getType()->getLength() : (SUMOReal) DEFAULT_VEH_LENGTH;
    while (current->getFirst()->getLength()<=vehLength) {
        if (!options.getBool("move-on-short")||current->size()<3) {
            mh->inform("Vehicle '" + veh->getID() + "' was discarded due being too long to start at edge '" + current->getFirst()->getID() + "'.");
            delete current;
            return 0;
        }
        current->pruneFirst();
        mh->inform("Vehicle '" + veh->getID() + "' is too long to start at edge '" + current->getFirst()->getID() + "'.\n The route was prunned (now starting at '" + current->getFirst()->getID() + "').");
    }
    // add build route
    routeDef->addAlternative(veh, current, veh->getDepartureTime());
    return routeDef;
}


void
RONet::saveAndRemoveRoutesUntil(OptionsCont &options, SUMOAbstractRouter<ROEdge,ROVehicle> &router,
                                SUMOTime time)
{
    // sort the list of route definitions
    SUMOTime lastTime = -1;
    // write all vehicles (and additional structures)
    while (myVehicles.size()!=0) {
        // get the next vehicle
        const ROVehicle * const veh = myVehicles.getTopVehicle();//sortedVehicles.top();
        SUMOTime currentTime = veh->getDepartureTime();
        // check whether it shall not yet be computed
        if (currentTime>time) {
            break;
        }
        // check whether to print the output
        if (lastTime!=currentTime&&lastTime!=-1) {
            // report writing progress
            if (options.getInt("stats-period")>=0 && ((int) currentTime%options.getInt("stats-period"))==0) {
                WRITE_MESSAGE("Read: " + toString<int>(myReadRouteNo) + ",  Discarded: " + toString<int>(myDiscardedRouteNo) + ",  Written: " + toString<int>(myWrittenRouteNo));
            }
        }
        lastTime = currentTime;

        // ok, compute the route (try it)
        const RORouteDef * const route = computeRoute(options, router, veh);
        if (route!=0) {
            // write the route
            veh->saveAllAsXML(*myRoutesOutput, myRouteAlternativesOutput, route);
            myWrittenRouteNo++;
        } else {
            myDiscardedRouteNo++;
        }
        // remove the route if it is not longer used
        removeRouteSecure(veh->getRoute());
        myVehicles.erase(veh->getID());
    }
}


void
RONet::removeRouteSecure(const RORouteDef * const route)
{
    // !!! later, a counter should be used to keep computed routes in the memory
    if (!myRoutes.erase(route->getID())) {
        MsgHandler::getWarningInstance()->inform("Could not remove " + route->getID());
    }
}


bool
RONet::addRouteSnipplet(const ROEdgeVector &item)
{
    return mySnipplets.add(item);
}


const ROEdgeVector &
RONet::getRouteSnipplet(ROEdge *from, ROEdge *to) const
{
    return mySnipplets.get(from, to);
}


bool
RONet::knowsRouteSnipplet(ROEdge *from, ROEdge *to) const
{
    return mySnipplets.knows(from, to);
}


bool
RONet::furtherStored()
{
    return myVehicles.size()>0;
}


ROEdge *
RONet::getRandomSource()
{
    // check whether an edge may be returned
    checkSourceAndDestinations();
    if (mySourceEdges.size()==0) {
        return 0;
    }
    // choose a random edge
    return RandHelper::getRandomFrom(mySourceEdges);
}




ROEdge *
RONet::getRandomDestination()
{
    // check whether an edge may be returned
    checkSourceAndDestinations();
    if (myDestinationEdges.size()==0) {
        return 0;
    }
    // choose a random edge
    return RandHelper::getRandomFrom(myDestinationEdges);
}


void
RONet::checkSourceAndDestinations()
{
    if (myDestinationEdges.size()!=0||mySourceEdges.size()!=0) {
        return;
    }
    std::vector<ROEdge*> edges = myEdges.getTempVector();
    for (std::vector<ROEdge*>::const_iterator i=edges.begin(); i!=edges.end(); i++) {
        ROEdge::EdgeType type = (*i)->getType();
        // !!! add something like "classified edges only" for using only sources or sinks
        if (type!=ROEdge::ET_SOURCE) {
            myDestinationEdges.push_back(*i);
        }
        if (type!=ROEdge::ET_SINK) {
            mySourceEdges.push_back(*i);
        }
    }
}


unsigned int
RONet::getEdgeNo() const
{
    return myEdges.size();
}


ROEdgeCont *
RONet::getMyEdgeCont()
{
    return &myEdges;
}


bool
RONet::hasRestrictions() const
{
    return myHaveRestrictions;
}


void
RONet::setRestrictionFound()
{
    myHaveRestrictions = true;
}



/****************************************************************************/


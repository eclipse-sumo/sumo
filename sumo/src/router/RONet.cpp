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
#include "ROEdge.h"
#include "RONode.h"
#include "RONet.h"
#include "RORoute.h"
#include "RORouteDef.h"
#include "ROVehicle.h"
#include "ROVehicleType.h"
#include "ROVehicleType_ID.h"
#include <utils/common/SUMOAbstractRouter.h>
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
// method definitions
// ===========================================================================
RONet::RONet() throw()
        : myVehicleTypes(),
        myRoutesOutput(0), myRouteAlternativesOutput(0),
        myReadRouteNo(0), myDiscardedRouteNo(0), myWrittenRouteNo(0),
        myHaveRestrictions(false)
{}


RONet::~RONet() throw()
{
    myNodes.clear();
    myEdges.clear();
    myVehicleTypes.clear();
    myRoutes.clear();
    myVehicles.clear();
}


void
RONet::addEdge(ROEdge *edge) throw()
{
    if (!myEdges.add(edge->getID(), edge)) {
        MsgHandler::getErrorInstance()->inform("The edge '" + edge->getID() + "' occures at least twice.");
        delete edge;
    }
}


void
RONet::addNode(RONode *node) throw()
{
    if (!myNodes.add(node->getID(), node)) {
        MsgHandler::getErrorInstance()->inform("The node '" + node->getID() + "' occures at least twice.");
        delete node;
    }
}


bool
RONet::addRouteDef(RORouteDef *def) throw()
{
    return myRoutes.add(def->getID(), def);
}


void
RONet::openOutput(const std::string &filename, bool useAlternatives) throw(IOError)
{
    myRoutesOutput = &OutputDevice::getDevice(filename);
    myRoutesOutput->writeXMLHeader("routes");
    if (useAlternatives) {
        size_t len = filename.length();
        if (len > 4 && filename.substr(len - 4) == ".xml") {
            myRouteAlternativesOutput = &OutputDevice::getDevice(filename.substr(0, len-4)+".alt.xml");
        } else {
            myRouteAlternativesOutput = &OutputDevice::getDevice(filename+".alt");
        }
        myRouteAlternativesOutput->writeXMLHeader("route-alternatives");
    }
}


void
RONet::closeOutput() throw()
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
RONet::getVehicleTypeSecure(const std::string &id) throw()
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


bool
RONet::addVehicleType(ROVehicleType *type) throw()
{
    if (!myVehicleTypes.add(type->getID(), type)) {
        MsgHandler::getErrorInstance()->inform("The vehicle type '" + type->getID() + "' occures at least twice.");
        delete type;
        return false;
    }
    return true;
}


bool
RONet::addVehicle(const std::string &id, ROVehicle *veh) throw()
{
    if (myVehicles.add(id, veh)) {
        myReadRouteNo++;
        return true;
    }
    return false;
}


bool
RONet::computeRoute(OptionsCont &options, SUMOAbstractRouter<ROEdge,ROVehicle> &router,
                    const ROVehicle * const veh)
{
    MsgHandler *mh = MsgHandler::getErrorInstance();
    if (options.getBool("continue-on-unbuild")) {
        mh = MsgHandler::getWarningInstance();
    }
    RORouteDef * const routeDef = veh->getRouteDefinition();
    // check if the route definition is valid
    if (routeDef==0) {
        mh->inform("The vehicle '" + veh->getID() + "' has no valid route.");
        return false;
    }
    // check whether the route was already saved
    if (routeDef->isSaved()) {
        return true;
    }
    //
    RORoute *current =
        routeDef->buildCurrentRoute(router, veh->getDepartureTime(), *veh);
    if (current==0||current->getEdgeVector().size()==0) {
        delete current;
        return false;
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
        return false;
    }
    // add built route
    routeDef->addAlternative(veh, current, veh->getDepartureTime());
    return true;
}


void
RONet::saveAndRemoveRoutesUntil(OptionsCont &options, SUMOAbstractRouter<ROEdge,ROVehicle> &router,
                                SUMOTime time)
{
    SUMOTime lastTime = -1;
    // write all vehicles (and additional structures)
    while (myVehicles.size()!=0) {
        // get the next vehicle
        const ROVehicle * const veh = myVehicles.getTopVehicle();
        SUMOTime currentTime = veh->getDepartureTime();
        // check whether it shall not yet be computed
        if (currentTime>time) {
            break;
        }
        // check whether to print the output
        if (lastTime!=currentTime&&lastTime!=-1) {
            // report writing progress
            if (options.getInt("stats-period")>=0 && ((int) currentTime%options.getInt("stats-period"))==0) {
                WRITE_MESSAGE("Read: " + toString(myReadRouteNo) + ",  Discarded: " + toString(myDiscardedRouteNo) + ",  Written: " + toString(myWrittenRouteNo));
            }
        }
        lastTime = currentTime;

        // ok, compute the route (try it)
        if (computeRoute(options, router, veh)) {
            // write the route
            veh->saveAllAsXML(*myRoutesOutput, myRouteAlternativesOutput, options.getBool("exit-times"));
            myWrittenRouteNo++;
            // remove the route if it is not longer used
            /*
            if (!myRoutes.erase(route->getID())) {
                MsgHandler::getWarningInstance()->inform("Could not remove " + route->getID());
            }
            */
        } else {
            myDiscardedRouteNo++;
        }
        // and the vehicle
        myVehicles.erase(veh->getID());
    }
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
    const std::map<std::string, ROEdge*> &edges = myEdges.getMyMap();
    for (std::map<std::string, ROEdge*>::const_iterator i=edges.begin(); i!=edges.end(); ++i) {
        ROEdge *e = (*i).second;
        ROEdge::EdgeType type = e->getType();
        // !!! add something like "classified edges only" for using only sources or sinks
        if (type!=ROEdge::ET_SOURCE) {
            myDestinationEdges.push_back(e);
        }
        if (type!=ROEdge::ET_SINK) {
            mySourceEdges.push_back(e);
        }
    }
}


unsigned int
RONet::getEdgeNo() const
{
    return (unsigned int) myEdges.size();
}


const std::map<std::string, ROEdge*> &
RONet::getEdgeMap() const
{
    return myEdges.getMyMap();
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


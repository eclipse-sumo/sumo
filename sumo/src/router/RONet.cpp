/****************************************************************************/
/// @file    RONet.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
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
#include "ROVehicleType_Krauss.h"
#include "ROVehicleType_ID.h"
#include "ROAbstractRouter.h"
#include "ROAbstractEdgeBuilder.h"
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
RONet::RONet(bool /*multireferencedRoutes*/)
        : _vehicleTypes(new ROVehicleType_Krauss()),
        myRoutesOutput(0), myRouteAlternativesOutput(0),
        myReadRouteNo(0), myDiscardedRouteNo(0), myWrittenRouteNo(0),
        myHaveRestrictions(false)
{}


RONet::~RONet()
{
    _nodes.clear();
    _edges.clear();
    _vehicleTypes.clear();
    _routes.clear();
    _vehicles.clear();
}


void
RONet::addEdge(ROEdge *edge)
{
    if (!_edges.add(edge->getID(), edge)) {
        MsgHandler::getErrorInstance()->inform("The edge '" + edge->getID() + "' occures at least twice.");
    }
}


ROEdge *
RONet::getEdge(const std::string &name) const
{
    return _edges.get(name);
}


void
RONet::addNode(RONode *node)
{
    _nodes.add(node->getID(), node);
}


RONode *
RONet::getNode(const std::string &name) const
{
    return _nodes.get(name);
}



bool
RONet::isKnownVehicleID(const std::string &id) const
{
    VehIDCont::const_iterator i =
        find(_vehIDs.begin(), _vehIDs.end(), id);
    if (i==_vehIDs.end()) {
        return false;
    }
    return true;
}


void
RONet::addVehicleID(const std::string &id)
{
    _vehIDs.push_back(id);
}


ROVehicleType *
RONet::getVehicleType(const std::string &name) const
{
    return _vehicleTypes.get(name);
}


RORouteDef *
RONet::getRouteDef(const std::string &name) const
{
    return _routes.get(name);
}


void
RONet::addRouteDef(RORouteDef *def)
{
    _routes.add(def);
}


void
RONet::openOutput(const std::string &filename, bool useAlternatives)
{
    myRoutesOutput = buildOutput(filename);
    (*myRoutesOutput) << "<routes>" << endl;
    if (useAlternatives) {
        myRouteAlternativesOutput = buildOutput(filename+".alt");
        (*myRouteAlternativesOutput) << "<route-alternatives>" << endl;
    }
}


void
RONet::closeOutput()
{
    // end writing
    if (myRoutesOutput!= 0) {
        (*myRoutesOutput) << "</routes>" << endl;
        myRoutesOutput->close();
        delete myRoutesOutput;
    }
    // only if opened
    if (myRouteAlternativesOutput!=0) {
        (*myRouteAlternativesOutput) << "</route-alternatives>" << endl;
        myRouteAlternativesOutput->close();
        delete myRouteAlternativesOutput;
    }
}



ROVehicleType *
RONet::getVehicleTypeSecure(const std::string &id)
{
    // check whether the type was already known
    ROVehicleType *type = _vehicleTypes.get(id);
    if (type!=0) {
        return type;
    }
    if (id=="!") { // !!! make this is static const
        // ok, no vehicle type was given within the user input
        //  return the default type
        return getDefaultVehicleType();
    }
    // Assume, the user will define the type somewhere else
    //  return a type which contains the id only
    type = new ROVehicleType_ID(id);
    addVehicleType(type);
    return type;
}


ROVehicleType *
RONet::getDefaultVehicleType() const
{
    return _vehicleTypes.getDefault();
}


void
RONet::addVehicleType(ROVehicleType *type)
{
    _vehicleTypes.add(type->getID(), type);
}


void
RONet::addVehicle(const std::string &id, ROVehicle *veh)
{
    _vehicles.add(id, veh);
    myReadRouteNo++;
}

const RORouteDef * const
RONet::computeRoute(OptionsCont &options, ROAbstractRouter &router,
                    ROVehicle *veh)
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
            mh->inform("The route '" + routeDef->getID() + "' is too short, probably ending at the starting edge.");
            WRITE_WARNING("Skipping...");
        }
        delete current;
        return 0;
    }
    // check whether the vehicle is able to start at this edge
    //  (the edge must be longer than the vehicle)
    while (current->getFirst()->getLength()<=veh->getType()->getLength()) {
        mh->inform("The vehicle '" + veh->getID() + "' is too long to start at edge '" + current->getFirst()->getID() + "'.");
        if (!options.getBool("move-on-short")||current->size()<3) {
            mh->inform(" Discarded.");
            delete current;
            return 0;
        }
        current->pruneFirst();
        mh->inform(" Prunned (now starting at '" + current->getFirst()->getID() + "').");
    }
    // add build route
    routeDef->addAlternative(veh, current, veh->getDepartureTime());
    return routeDef;
}


void
RONet::saveAndRemoveRoutesUntil(OptionsCont &options, ROAbstractRouter &router,
                                SUMOTime time)
{
    // sort the list of route definitions
    priority_queue<ROVehicle*, std::vector<ROVehicle*>, ROHelper::VehicleByDepartureComperator> &sortedVehicles = _vehicles.sort();
    SUMOTime lastTime = -1;
    // write all vehicles (and additional structures)
    while (!sortedVehicles.empty()) {
        // get the next vehicle
        ROVehicle *veh = sortedVehicles.top();
        SUMOTime currentTime = veh->getDepartureTime();
        // check whether it shall not yet be computed
        if (currentTime>time) {
            break;
        }
        // check whether to print the output
        if (lastTime!=currentTime&&lastTime!=-1) {
            // report writing progress
            if (options.getInt("stats-period")>=0 && (currentTime%options.getInt("stats-period"))==0) {
                WRITE_MESSAGE("Read: " + toString<int>(myReadRouteNo) + ",  Discarded: " + toString<int>(myDiscardedRouteNo) + ",  Written: " + toString<int>(myWrittenRouteNo));
            }
        }
        lastTime = currentTime;

        // ok, compute the route (try it)
        sortedVehicles.pop();
        // compute the route
        const RORouteDef * const route = computeRoute(options, router, veh);
        if (route!=0) {
            // write the route
            veh->saveAllAsXML(myRoutesOutput, myRouteAlternativesOutput, *_vehicleTypes.getDefault(), route);
            myWrittenRouteNo++;
        } else {
            myDiscardedRouteNo++;
        }
        // remove the route if it is not longer used
        removeRouteSecure(veh->getRoute());
        _vehicles.erase(veh->getID());
    }
}


void
RONet::removeRouteSecure(const RORouteDef * const route)
{
    // !!! later, a counter should be used to keep computed routes in the memory
    if (!_routes.erase(route->getID())) {
        MsgHandler::getWarningInstance()->inform("Could not remove " + route->getID());
    }
}


bool
RONet::addRouteSnipplet(const ROEdgeVector &item)
{
    return _snipplets.add(item);
}


const ROEdgeVector &
RONet::getRouteSnipplet(ROEdge *from, ROEdge *to) const
{
    return _snipplets.get(from, to);
}


bool
RONet::knowsRouteSnipplet(ROEdge *from, ROEdge *to) const
{
    return _snipplets.knows(from, to);
}


bool
RONet::furtherStored()
{
    return _vehicles.size()>0;
}


ROEdge *
RONet::getRandomSource()
{
    // check whether an edge may be returned
    checkSourceAndDestinations();
    if (mySourceEdges.size()==0) {
        return 0;
    }
    // choose an edge by random
    SUMOReal random = SUMOReal(rand()) /
                      (static_cast<SUMOReal>(RAND_MAX) + 1);
    return mySourceEdges[(size_t)(random*(mySourceEdges.size()-1))];
}


ROEdge *
RONet::getRandomDestination()
{
    // check whether an edge may be returned
    checkSourceAndDestinations();
    if (myDestinationEdges.size()==0) {
        return 0;
    }
    // choose an edge by random
    // choose an edge by random
    SUMOReal random = SUMOReal(rand()) /
                      (static_cast<SUMOReal>(RAND_MAX) + 1);
    return myDestinationEdges[(size_t)(random*(myDestinationEdges.size()-1))];
}


void
RONet::checkSourceAndDestinations()
{
    if (myDestinationEdges.size()!=0||mySourceEdges.size()!=0) {
        return;
    }
    std::vector<ROEdge*> edges = _edges.getTempVector();
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
    return _edges.size();
}


std::ofstream *
RONet::buildOutput(const std::string &name)
{
    std::ofstream *ret = new std::ofstream(name.c_str());
    if (!ret->good()) {
        MsgHandler::getErrorInstance()->inform("The file '" + name + "' could not be opened for writing.");
        throw ProcessError();
    }
    return ret;
}


ROEdgeCont *
RONet::getMyEdgeCont()
{
    return &_edges;
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


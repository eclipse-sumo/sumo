//---------------------------------------------------------------------------//
//                        RONet.cpp -
//  The router's network representation
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
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
// Revision 1.12  2003/10/15 11:55:11  dkrajzew
// false usage of rand() patched
//
// Revision 1.11  2003/07/18 12:35:06  dkrajzew
// removed some warnings
//
// Revision 1.10  2003/07/07 08:36:58  dkrajzew
// Warnings are now reported to the MsgHandler
//
// Revision 1.9  2003/06/18 11:20:54  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.8  2003/05/20 09:48:35  dkrajzew
// debugging
//
// Revision 1.7  2003/04/09 15:39:11  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.6  2003/03/20 16:39:16  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.5  2003/03/17 14:25:28  dkrajzew
// windows eol removed
//
// Revision 1.4  2003/03/03 15:08:21  dkrajzew
// debugging
//
// Revision 1.3  2003/02/07 10:45:04  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

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
#include "RORouter.h"
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>

using namespace std;

RONet::RONet(bool multireferencedRoutes)
    : _vehicleTypes(new ROVehicleType_Krauss())
{
}


RONet::~RONet()
{
    _nodes.clear();
    _edges.clear();
    _vehicleTypes.clear();
    _routes.clear();
    _vehicles.clear();
//    _snipplets.clear();
}


void
RONet::postloadInit()
{
    _edges.postloadInit();
}


void
RONet::addEdge(const std::string &name, ROEdge *edge)
{
    if(!_edges.add(name, edge)) {
        MsgHandler::getErrorInstance()->inform(string("The edge '") + name + string("' occures at least twice."));
    }
}


ROEdge *
RONet::getEdge(const std::string &name) const
{
    return _edges.get(name);
}


void
RONet::addNode(const std::string &name, RONode *node)
{
    _nodes.add(name, node);
}


bool
RONet::isKnownVehicleID(const std::string &id) const
{
    VehIDCont::const_iterator i=_vehIDs.find(id);
    if(i==_vehIDs.end())
        return false;
    return true;
}

void
RONet::addVehicleID(const std::string &id)
{
    _vehIDs.insert(id);
}

/*
void
RONet::computeWeights()
{
    _edges.computeWeights();
}
*/

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

/*
std::string
RONet::addRouteDef(ROEdge *from, ROEdge *to) {
    return _routes.add(from, to);
}
*/
void
RONet::addRouteDef(RORouteDef *def) {
    _routes.add(def->getID(), def);
}

ROVehicleType *
RONet::getVehicleTypeSecure(const std::string &id)
{
    // check whether the type was already known
    ROVehicleType *type = _vehicleTypes.get(id);
    if(type!=0) {
        return type;
    }
    return getDefaultVehicleType();
}


ROVehicleType *
RONet::getDefaultVehicleType() const {
    return _vehicleTypes.getDefault();
}

void
RONet::addVehicleType(ROVehicleType *type)
{
    _vehicleTypes.add(type->getID(), type);
}

void
RONet::addVehicle(const std::string &id, ROVehicle *veh) {
    _vehicles.add(id, veh);
}

/*
void
RONet::computeAndSave(OptionsCont &oc) {
    // start the computation and save the build routes
    ofstream res(oc.getString("o").c_str());
    if(!res.good()) {
        throw ProcessError();
    }
    // build the router
    DijkstraRouter router(&_edges);
    // sort the list of route definitions
    BinaryHeap<ROVehicle*, ROHelper::VehicleByDepartureComperator> &sortedVehicles = _vehicles.sort();
    // begin writing
    res << "<routes>" << endl;
    // write all vehicles (and additional structures)
    while(!sortedVehicles.isEmpty()) {
        // get the next vehicle
        ROVehicle *veh = sortedVehicles.findMin();
        sortedVehicles.deleteMin();
        // write the type if it's new
        saveType(res, veh->getType(), veh->getID());
        // write the route if it's new
        saveRoute(router, res, veh->getRoute(), veh->getID());
        // write the vehicle
        res << "   ";
        veh->xmlOut(res);
    }
    // end writing
    res << "</routes>" << endl;
    res.close();
}
*/

void
RONet::saveType(std::ostream &os, ROVehicleType *type,
                const std::string &vehID) {
    if(type==0) {
        type = _vehicleTypes.getDefault();
        MsgHandler::getWarningInstance()->inform(
            string("The vehicle '") + vehID
            + string("' has no valid type; Using default."));
    }
    os << "   ";
    type->xmlOut(os);
    type->markSaved();
}


bool
RONet::saveRoute(OptionsCont &options, RORouter &router,
                 std::ostream &res,
                 std::ostream &altres,
                 ROVehicle *veh)
{
    RORouteDef *routeDef = veh->getRoute();
    // check if the route definition is valid
    if(routeDef==0) {
        MsgHandler::getErrorInstance()->inform(string("The vehicle '") + veh->getID()
            + string("' has no valid route."));
        return false;
    }
    // check whether the route was already saved
    if(routeDef->isSaved()) {
        return true;
    }
    // build and save the route
    return routeDef->computeAndSave(options,
        router, veh->getDepartureTime(), res, altres,
        veh->periodical());
}

void
RONet::saveAndRemoveRoutesUntil(OptionsCont &options,
                                std::ofstream &res, std::ofstream &altres,
                                long time)
{
    // build the router
    RORouter router(*this, &_edges);
    // sort the list of route definitions
    priority_queue<ROVehicle*,
        std::vector<ROVehicle*>,
        ROHelper::VehicleByDepartureComperator>
        &sortedVehicles = _vehicles.sort();
    // write all vehicles (and additional structures)
    while(!sortedVehicles.empty()) {
        // get the next vehicle
        ROVehicle *veh = sortedVehicles.top();
        if(veh->getDepartureTime()>time) {
            break;
        }
        sortedVehicles.pop();
        // write the route
        if(saveRoute(options, router, res, altres, veh)) {
            // write the type if it's new
            if(!veh->getType()->isSaved()) {
                saveType(res, veh->getType(), veh->getID());
                saveType(altres, veh->getType(), veh->getID());
            }
            // write the vehicle
            res << "   ";
            veh->xmlOut(res);
            altres << "   ";
            veh->xmlOut(altres);
        }
        // remove the route if it is not longer used
        removeRouteSecure(veh->getRoute());
        _vehicles.eraseVehicle(veh);
    }
}


void
RONet::removeRouteSecure(RORouteDef *route)
{
    // !!! later, a counter should be used to keep computed routes in the memory
    _routes.erase(route->getID());
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
    if(mySourceEdges.size()==0) {
        return 0;
    }
    // choose an edge by random
    double random = double( rand() ) / double( (RAND_MAX+1) );
    return mySourceEdges[(size_t) (random*(mySourceEdges.size()-1))];
}


ROEdge *
RONet::getRandomDestination()
{
    // check whether an edge may be returned
    checkSourceAndDestinations();
    if(myDestinationEdges.size()==0) {
        return 0;
    }
    // choose an edge by random
    // choose an edge by random
    double random = double( rand() ) / double( (RAND_MAX+1) );
    return myDestinationEdges[(size_t) (random*(myDestinationEdges.size()-1))];
}


void
RONet::checkSourceAndDestinations()
{
    if(myDestinationEdges.size()!=0||mySourceEdges.size()!=0) {
        return;
    }
    std::vector<ROEdge*> edges = _edges.getAllEdges();
    for(std::vector<ROEdge*>::iterator i=edges.begin(); i!=edges.end(); i++) {
        ROEdge::EdgeType type = (*i)->getType();
        if(type!=ROEdge::ET_SOURCE) {
            myDestinationEdges.push_back(*i);
        }
        if(type!=ROEdge::ET_SINK) {
            mySourceEdges.push_back(*i);
        }
    }
}





/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "RONet.icc"
//#endif

// Local Variables:
// mode:C++
// End:



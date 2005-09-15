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
// Revision 1.23  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.22  2005/05/04 08:48:35  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added; handling of vehicles debugged
//
// Revision 1.21  2004/12/16 12:26:52  dkrajzew
// debugging
//
// Revision 1.20  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.19  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.18  2004/04/23 12:46:41  dkrajzew
// forgetting to set information about periodical emission of vehicles patched
//
// Revision 1.17  2004/04/02 11:26:26  dkrajzew
// moving the vehicle forward if it shall start at a too short edge added; output of the number of loaded, build, and discarded vehicles added
//
// Revision 1.16  2004/01/26 09:54:59  dkrajzew
// error handling corrected
//
// Revision 1.15  2004/01/26 08:01:10  dkrajzew
// loaders and route-def types are now renamed in an senseful way;
//  further changes in order to make both new routers work;
//  documentation added
//
// Revision 1.14  2003/11/11 08:04:46  dkrajzew
// avoiding emissions of vehicles on too short edges
//
// Revision 1.13  2003/10/31 08:00:32  dkrajzew
// hope to have patched false usage of RAND_MAX when using gcc
//
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
// new message and error processing: output to user may be a message, warning
//  or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
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
#include "ROAbstractRouter.h"
#include "ROAbstractEdgeBuilder.h"
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/convert/ToString.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG




/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

//ofstream blaa("bla.txt");

RONet *RONet::myInstance;
/* =========================================================================
 * method definitions
 * ======================================================================= */
RONet::RONet(bool multireferencedRoutes)
    : _vehicleTypes(new ROVehicleType_Krauss()),
    myRoutesOutput(0), myRouteAlternativesOutput(0),
    myReadRouteNo(0), myDiscardedRouteNo(0), myWrittenRouteNo(0)
{
}


RONet::~RONet()
{
    _nodes.clear();
    _edges.clear();
    _vehicleTypes.clear();
    _routes.clear();
    _vehicles.clear();
}

/*
void
RONet::postloadInit()
{
    _edges.postloadInit();
}
*/
/*
void
RONet::preInitRONet()
{
	myInstance = new RONet(false);
}
*/
void
RONet::addEdge(ROEdge *edge)
{
    if(!_edges.add(edge->getID(), edge)) {
        MsgHandler::getErrorInstance()->inform(
            string("The edge '") + edge->getID()
            + string("' occures at least twice."));
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
    VehIDCont::const_iterator i =
        find(_vehIDs.begin(), _vehIDs.end(), id);
    if(i==_vehIDs.end()) {
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
    if(useAlternatives) {
        myRouteAlternativesOutput = buildOutput(filename+string(".alt"));
        (*myRouteAlternativesOutput) << "<route-alternatives>" << endl;
    }
}


void
RONet::closeOutput()
{
    // end writing
	if(myRoutesOutput!= 0) {
		(*myRoutesOutput) << "</routes>" << endl;
		myRoutesOutput->close();
		delete myRoutesOutput;
	}
    // only if opened
    if(myRouteAlternativesOutput!=0) {
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
    if(type!=0) {
        return type;
    }
    return getDefaultVehicleType();
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

void
RONet::setNetInstance(RONet *net)
{
	myInstance = net;

}

RONet *
RONet::getNetInstance()
{
	if(myInstance == 0){
		return 0;
	}
	return myInstance;
}

RORouteDef *
RONet::computeRoute(OptionsCont &options, ROAbstractRouter &router,
                    ROVehicle *veh)
{
    MsgHandler *mh = MsgHandler::getErrorInstance();
    if(options.getBool("continue-on-unbuild")) {
        mh = MsgHandler::getWarningInstance();
    }
    RORouteDef *routeDef = veh->getRoute();
    // check if the route definition is valid
    if(routeDef==0) {
        mh->inform(string("The vehicle '") + veh->getID()
            + string("' has no valid route."));
        return 0;
    }
    // check whether the route was already saved
    if(routeDef->isSaved()) {
        return routeDef;
    }
    //
    RORoute *current = routeDef->buildCurrentRoute(router,
        veh->getDepartureTime(), options.getBool("continue-on-unbuild"), *veh, 0);
    if(current==0) {
        return 0;
    }
    // check whether the route is valid and does not end on the starting edge
    if(current->size()<2) {
        // check whether the route ends at the starting edge
        //  unbuild routes due to missing connections are reported within the
        //  router
        if(current->size()!=0) {
            cout << endl;
            mh->inform(
                string("The route '") + routeDef->getID()
                + string("' is too short, propably ending at the starting edge."));
            WRITE_WARNING("Skipping...");
        }
        delete current;
        return 0;
    }
    // check whether the vehicle is able to start at this edge
    //  (the edge must be longer than the vehicle)
    while(current->getFirst()->getLength()<=veh->getType()->getLength()) {
        mh->inform(string("The vehicle '")
            + veh->getID()
            + string("' is too long to start at edge '")
            + current->getFirst()->getID() + string("'."));
        if(!options.getBool("move-on-short")||current->size()<3) {
            mh->inform(" Discarded.");
            delete current;
            return 0;
        }
        current->pruneFirst();
        mh->inform(string(" Prunned (now starting at '")
            + current->getFirst()->getID() + string("')."));
    }
    // add build route
    routeDef->addAlternative(current, veh->getDepartureTime());
    return routeDef;
}


bool
RONet::saveRoute(RORouteDef *route, ROVehicle *veh)
{
    if(route->isSaved()) {
        return true;
    }
    // save route
    route->xmlOutCurrent(*myRoutesOutput, veh->periodical());
    if(myRouteAlternativesOutput!=0) {
        route->xmlOutAlternatives(*myRouteAlternativesOutput);
    }
    return true;
}

void
RONet::saveAndRemoveRoutesUntil(OptionsCont &options, ROAbstractRouter &router,
                                SUMOTime time)
{
    // sort the list of route definitions
    priority_queue<ROVehicle*,
        std::vector<ROVehicle*>,
        ROHelper::VehicleByDepartureComperator>
        &sortedVehicles = _vehicles.sort();
    SUMOTime lastTime = -1;
    // write all vehicles (and additional structures)
    while(!sortedVehicles.empty()) {
        // get the next vehicle
        ROVehicle *veh = sortedVehicles.top();
        SUMOTime currentTime = veh->getDepartureTime();
        // check whether it shall not yet be computed
        if(currentTime>time) {
            break;
        }
        // check whether to print the output
        if(lastTime!=currentTime&&lastTime!=-1) {
            // report writing progress
            if( options.getInt("stats-period")>=0
                &&
                (currentTime%options.getInt("stats-period"))==0) {

                WRITE_MESSAGE(\
                    string("Read: ") + toString<int>(myReadRouteNo)\
                    + string(",  Discarded: ") + toString<int>(myDiscardedRouteNo)\
                    + string(",  Written: ") + toString<int>(myWrittenRouteNo));
            }
        }
        lastTime = currentTime;

        // ok, compute the route (try it)
        sortedVehicles.pop();
        // compute the route
        RORouteDef *route = computeRoute(options, router, veh);
        if(route!=0) {
            // write the route
            saveRoute(route, veh);
            if(myRouteAlternativesOutput==0) {
                veh->saveTypeAndSelf(*myRoutesOutput,
                    *_vehicleTypes.getDefault());
            } else {
                veh->saveTypeAndSelf(*myRoutesOutput,
                    *myRouteAlternativesOutput, *_vehicleTypes.getDefault());
            }
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
RONet::removeRouteSecure(RORouteDef *route)
{
    // !!! later, a counter should be used to keep computed routes in the memory
    if(!_routes.erase(route->getID())) {
        cout << "Could not remove " << route->getID() << endl;
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
    if(mySourceEdges.size()==0) {
        return 0;
    }
    // choose an edge by random
    double random = double( rand() ) /
        ( static_cast<double>(RAND_MAX) + 1);
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
    double random = double( rand() ) /
        ( static_cast<double>(RAND_MAX) + 1);
    return myDestinationEdges[(size_t) (random*(myDestinationEdges.size()-1))];
}


void
RONet::checkSourceAndDestinations()
{
    if(myDestinationEdges.size()!=0||mySourceEdges.size()!=0) {
        return;
    }
    std::vector<ROEdge*> edges = _edges.getTempVector();
    for(std::vector<ROEdge*>::const_iterator i=edges.begin(); i!=edges.end(); i++) {
        ROEdge::EdgeType type = (*i)->getType();
        // !!! add something like "classified edges only" for using only sources or sinks
        if(type!=ROEdge::ET_SOURCE) {
            myDestinationEdges.push_back(*i);
        }
        if(type!=ROEdge::ET_SINK) {
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
    if(!ret->good()) {
        MsgHandler::getErrorInstance()->inform(
            string("The file '") + name +
            string("' could not be opened for writing."));
        throw ProcessError();
    }
    return ret;
}

ROEdgeCont *
RONet::getMyEdgeCont()
{
	return &_edges;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



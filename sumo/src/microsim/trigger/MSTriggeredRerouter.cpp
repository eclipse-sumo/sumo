/****************************************************************************/
/// @file    MSTriggeredRerouter.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 25 July 2005
/// @version $Id: $
///
// Allows the triggered rerouting of vehicles within the simulation
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
#include <algorithm>
#include <utils/common/MsgHandler.h>
#include <utils/helpers/Command.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSGlobals.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/common/UtilExceptions.h>
#include "MSTriggeredRerouter.h"
#include <utils/common/XMLHelpers.h>
#include <utils/common/TplConvert.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/helpers/SUMODijkstraRouter.h>

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
/* -------------------------------------------------------------------------
 * MSTriggeredRerouter::Setter - methods
 * ----------------------------------------------------------------------- */
MSTriggeredRerouter::Setter::Setter(MSTriggeredRerouter *parent,
                                    MSLane *lane)
        : MSMoveReminder(lane), myParent(parent)
{}


MSTriggeredRerouter::Setter::~Setter()
{}


bool
MSTriggeredRerouter::Setter::isStillActive(MSVehicle& veh, SUMOReal /*oldPos*/,
        SUMOReal /*newPos*/, SUMOReal /*newSpeed*/)
{
    myParent->reroute(veh, laneM->getEdge());
    return false;
}


void
MSTriggeredRerouter::Setter::dismissByLaneChange(MSVehicle&)
{}


bool
MSTriggeredRerouter::Setter::isActivatedByEmitOrLaneChange(MSVehicle& veh)
{
    myParent->reroute(veh, laneM->getEdge());
    return false;
}


/* -------------------------------------------------------------------------
 * MSTriggeredRerouter - methods
 * ----------------------------------------------------------------------- */
MSTriggeredRerouter::MSTriggeredRerouter(const std::string &id,
        const std::vector<MSEdge*> &edges,
        SUMOReal prob,
        const std::string &aXMLFilename)
        : MSTrigger(id), SUMOSAXHandler("reroutings", aXMLFilename),
        myProbability(prob), myUserProbability(prob), myAmInUserMode(false)
{
    // read in the trigger description
    SAX2XMLReader* triggerParser = 0;
    try {
        triggerParser = XMLHelpers::getSAXReader(*this);
        triggerParser->parse(aXMLFilename.c_str());
    } catch (SAXException &e) {
        MsgHandler::getErrorInstance()->inform(TplConvert<XMLCh>::_2str(e.getMessage()));
        throw ProcessError();
    } catch (XMLException &e) {
        MsgHandler::getErrorInstance()->inform(TplConvert<XMLCh>::_2str(e.getMessage()));
        throw ProcessError();
    }
    delete triggerParser;
    // build actors
    for (std::vector<MSEdge*>::const_iterator j=edges.begin(); j!=edges.end(); ++j) {
        const std::vector<MSLane*> * const destLanes = (*j)->getLanes();
        std::vector<MSLane*>::const_iterator i;
        for (i=destLanes->begin(); i!=destLanes->end(); ++i) {
            mySetter.push_back(new Setter(this, (*i)));
        }
    }
}


MSTriggeredRerouter::~MSTriggeredRerouter()
{
    {
        std::vector<Setter*>::iterator i;
        for (i=mySetter.begin(); i!=mySetter.end(); ++i) {
            delete *i;
        }
    }
}

// ------------ loading begin
void
MSTriggeredRerouter::myStartElement(int /*element*/,
                                    const std::string &name,
                                    const Attributes &attrs)
{
    if (name=="interval") {
        myCurrentIntervalBegin = getIntSecure(attrs, SUMO_ATTR_BEGIN, -1);
        myCurrentIntervalEnd = getIntSecure(attrs, SUMO_ATTR_END, -1);
    }
    // maybe by giving probabilities of new destinations
    if (name=="dest_prob_reroute") {
        string dest = getStringSecure(attrs, SUMO_ATTR_ID, "");
        MSEdge *to = MSEdge::dictionary(dest);
        if (to==0) {
            MsgHandler::getErrorInstance()->inform("Could not find edge '" + dest + "' to reroute in '" + _file + "'.");
            return;
        }
        SUMOReal prob = -1;
        try {
            prob = getFloatSecure(attrs, SUMO_ATTR_PROB, -1);
        } catch (EmptyData &) {
            MsgHandler::getErrorInstance()->inform("Missing probability in '" + _file + "'.");
            return;
        } catch (NumberFormatException &) {
            MsgHandler::getErrorInstance()->inform("No numeric probability '" + getStringSecure(attrs, SUMO_ATTR_PROB, "") + "' " + _file + "'.");
            return;
        }
        if (prob<0) {
            MsgHandler::getErrorInstance()->inform("Negative probability '" + getStringSecure(attrs, SUMO_ATTR_PROB, "") + "' " + _file + "'.");
            return;
        }
        myCurrentEdgeProb.add(prob, to);
    }
    // maybe by closing
    if (name=="closing_reroute") {
        string closed_id = getStringSecure(attrs, SUMO_ATTR_ID, "");
        MSEdge *closed = MSEdge::dictionary(closed_id);
        if (closed==0) {
            MsgHandler::getErrorInstance()->inform("Could not find edge '" + closed_id + "' to reroute in '" + _file + "'.");
            return;
        }
        myCurrentClosed.push_back(closed);
        /*
        string destid = getStringSecure(attrs, SUMO_ATTR_TO, "");
        MSEdge *dest = MSEdge::dictionary(destid);
        */
    }
    // maybe by giving probabilities of new routes
    if (name=="route_prob_reroute") {
        string routeID = getStringSecure(attrs, SUMO_ATTR_ID, "");
        MSRoute *route = MSRoute::dictionary(routeID);
        if (route==0) {
            MsgHandler::getErrorInstance()->inform("Could not find route '" + routeID + "' to reroute in '" + _file + "'.");
            return;
        }
        SUMOReal prob = -1;
        try {
            prob = getFloatSecure(attrs, SUMO_ATTR_PROB, -1);
        } catch (EmptyData &) {
            MsgHandler::getErrorInstance()->inform("Missing probability in '" + _file + "'.");
            return;
        } catch (NumberFormatException &) {
            MsgHandler::getErrorInstance()->inform("No numeric probability '" + getStringSecure(attrs, SUMO_ATTR_PROB, "") + "' " + _file + "'.");
            return;
        }
        if (prob<0) {
            MsgHandler::getErrorInstance()->inform("Negative probability '" + getStringSecure(attrs, SUMO_ATTR_PROB, "") + "' " + _file + "'.");
            return;
        }
        myCurrentRouteProb.add(prob, route);
    }
}


void
MSTriggeredRerouter::myCharacters(int , const std::string &,
                                  const std::string &)
{}


void
MSTriggeredRerouter::myEndElement(int , const std::string &name)
{
    if (name=="interval") {
        RerouteInterval ri;
        ri.begin = myCurrentIntervalBegin;
        ri.end = myCurrentIntervalEnd;
        ri.closed = myCurrentClosed;
        ri.edgeProbs = myCurrentEdgeProb;
        ri.routeProbs = myCurrentRouteProb;
        myCurrentClosed.clear();
        myCurrentEdgeProb.clear();
        myCurrentRouteProb.clear();
        myIntervals.push_back(ri);
    }
}


// ------------ loading end


bool
MSTriggeredRerouter::hasCurrentReroute(SUMOTime time, MSVehicle &veh) const
{
    std::vector<RerouteInterval>::const_iterator i = myIntervals.begin();
    const MSRoute &route = veh.getRoute();
    while (i!=myIntervals.end()) {
        if ((*i).begin<=time && (*i).end>=time) {
            if ((*i).edgeProbs.getOverallProb()!=0||(*i).routeProbs.getOverallProb()!=0||route.containsAnyOf((*i).closed)) {
                return true;
            }
        }
        i++;
    }
    return false;
}


bool
MSTriggeredRerouter::hasCurrentReroute(SUMOTime time) const
{
    std::vector<RerouteInterval>::const_iterator i = myIntervals.begin();
    while (i!=myIntervals.end()) {
        if ((*i).begin<=time && (*i).end>=time) {
            if ((*i).edgeProbs.getOverallProb()!=0||(*i).routeProbs.getOverallProb()!=0) {
                return true;
            }
        }
        i++;
    }
    return false;
}


const MSTriggeredRerouter::RerouteInterval &
MSTriggeredRerouter::getCurrentReroute(SUMOTime time, MSVehicle &veh) const
{
    std::vector<RerouteInterval>::const_iterator i = myIntervals.begin();
    const MSRoute &route = veh.getRoute();
    while (i!=myIntervals.end()) {
        if ((*i).begin<=time && (*i).end>=time) {
            if ((*i).edgeProbs.getOverallProb()!=0||(*i).routeProbs.getOverallProb()!=0||route.containsAnyOf((*i).closed)) {
                return *i;
            }
        }
        i++;
    }
    throw 1;
}


const MSTriggeredRerouter::RerouteInterval &
MSTriggeredRerouter::getCurrentReroute(SUMOTime) const
{
    std::vector<RerouteInterval>::const_iterator i = myIntervals.begin();
    while (i!=myIntervals.end()) {
        if ((*i).edgeProbs.getOverallProb()!=0||(*i).routeProbs.getOverallProb()!=0) {
            return *i;
        }
        i++;
    }
    throw 1;
}



void
MSTriggeredRerouter::reroute(MSVehicle &veh, const MSEdge *src)
{
    // check whether the vehicle shall be rerouted
    SUMOTime time = MSNet::getInstance()->getCurrentTimeStep();
    if (!hasCurrentReroute(time, veh)) {
        return;
    }

    SUMOReal prob = myAmInUserMode ? myUserProbability : myProbability;
    if ((double) rand()/(double)(RAND_MAX) > prob) {
        return;
    }

    // get vehicle params
    const MSRoute &route = veh.getRoute();
    const MSEdge *lastEdge = route.getLastEdge();
    // get rerouting params
    const MSTriggeredRerouter::RerouteInterval &rerouteDef = getCurrentReroute(time, veh);
    MSRoute *newRoute = rerouteDef.routeProbs.getOverallProb()>0 ? rerouteDef.routeProbs.get() : 0;
    // we will use the route if given rather than calling our own dijsktra...
    if (newRoute!=0) {
        veh.replaceRoute(newRoute, time);
        return;
    }
    // ok, try using a new destination
    const MSEdge *newEdge = rerouteDef.edgeProbs.getOverallProb()>0 ? rerouteDef.edgeProbs.get() : route.getLastEdge();
    if (newEdge==0) {
        newEdge = lastEdge;
    }

    // we have a new destination, let's replace the vehicle route
    if (route.inFurtherUse()) {
        string nid = _id + "_re_" + src->getID() + "_" + route.getID();
        if (MSRoute::dictionary(nid)!=0) {
            MSRoute *rep = MSRoute::dictionary(nid);
            veh.replaceRoute(rep, MSNet::getInstance()->getCurrentTimeStep());
        } else {
            SUMODijkstraRouter<MSEdge, MSVehicle, prohibited_withRestrictions<MSEdge, MSVehicle>, MSEdge> router(MSEdge::dictSize(), true, &MSEdge::getEffort);
            router.prohibit(rerouteDef.closed);
            std::vector<const MSEdge*> edges;
            router.compute(src, newEdge, &veh, MSNet::getInstance()->getCurrentTimeStep(), edges);
            MSRoute *rep = new MSRoute(nid, edges, true);
            if (!MSRoute::dictionary(nid, rep)) {
                cout << "Error: Could not insert route ''" << endl;
                return;
            }
            veh.replaceRoute(rep, MSNet::getInstance()->getCurrentTimeStep());
        }
    } else {
        // we can simply replace the vehicle route
        SUMODijkstraRouter<MSEdge, MSVehicle, prohibited_withRestrictions<MSEdge, MSVehicle>, MSEdge> router(MSEdge::dictSize(), true, &MSEdge::getEffort);
        router.prohibit(rerouteDef.closed);
        MSEdgeVector edges;
        router.compute(src, newEdge, &veh, MSNet::getInstance()->getCurrentTimeStep(), edges);
        veh.replaceRoute(edges, time);
    }
}


void
MSTriggeredRerouter::setUserMode(bool val)
{
    myAmInUserMode = val;
}


void
MSTriggeredRerouter::setUserUsageProbability(SUMOReal prob)
{
    myUserProbability = prob;
}


bool
MSTriggeredRerouter::inUserMode() const
{
    return myAmInUserMode;
}


SUMOReal
MSTriggeredRerouter::getProbability() const
{
    return myAmInUserMode ? myUserProbability : myProbability;
}


SUMOReal
MSTriggeredRerouter::getUserProbability() const
{
    return myUserProbability;
}



/****************************************************************************/


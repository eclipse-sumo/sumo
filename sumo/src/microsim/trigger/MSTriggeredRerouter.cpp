/****************************************************************************/
/// @file    MSTriggeredRerouter.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 25 July 2005
/// @version $Id$
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/UtilExceptions.h>
#include "MSTriggeredRerouter.h"
#include <utils/xml/XMLSubSys.h>
#include <utils/common/TplConvert.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/helpers/SUMODijkstraRouter.h>
#include <utils/common/RandHelper.h>

#ifdef HAVE_MESOSIM
#include <mesosim/MELoop.h>
#endif

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
    if(!XMLSubSys::runParser(*this, aXMLFilename)) {
        throw ProcessError();
    }
    // build actors
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        for (std::vector<MSEdge*>::const_iterator j=edges.begin(); j!=edges.end(); ++j) {
            MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(*j);
            s->addRerouter(this);
        }
    } else {
#endif
        for (std::vector<MSEdge*>::const_iterator j=edges.begin(); j!=edges.end(); ++j) {
            const std::vector<MSLane*> * const destLanes = (*j)->getLanes();
            std::vector<MSLane*>::const_iterator i;
            for (i=destLanes->begin(); i!=destLanes->end(); ++i) {
                mySetter.push_back(new Setter(this, (*i)));
            }
        }
#ifdef HAVE_MESOSIM
    }
#endif
}


MSTriggeredRerouter::~MSTriggeredRerouter() throw()
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
MSTriggeredRerouter::myStartElement(SumoXMLTag /*element*/,
                                    const std::string &name,
                                    const Attributes &attrs) throw()
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
            MsgHandler::getErrorInstance()->inform("Could not find edge '" + dest + "' to reroute in '" + getFileName() + "'.");
            return;
        }
        SUMOReal prob = -1;
        try {
            prob = getFloatSecure(attrs, SUMO_ATTR_PROB, -1);
        } catch (EmptyData &) {
            MsgHandler::getErrorInstance()->inform("Missing probability in '" + getFileName() + "'.");
            return;
        } catch (NumberFormatException &) {
            MsgHandler::getErrorInstance()->inform("No numeric probability '" + getStringSecure(attrs, SUMO_ATTR_PROB, "") + "' " + getFileName() + "'.");
            return;
        }
        if (prob<0) {
            MsgHandler::getErrorInstance()->inform("Negative probability '" + getStringSecure(attrs, SUMO_ATTR_PROB, "") + "' " + getFileName() + "'.");
            return;
        }
        myCurrentEdgeProb.add(prob, to);
    }
    // maybe by closing
    if (name=="closing_reroute") {
        string closed_id = getStringSecure(attrs, SUMO_ATTR_ID, "");
        MSEdge *closed = MSEdge::dictionary(closed_id);
        if (closed==0) {
            MsgHandler::getErrorInstance()->inform("Could not find edge '" + closed_id + "' to reroute in '" + getFileName() + "'.");
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
            MsgHandler::getErrorInstance()->inform("Could not find route '" + routeID + "' to reroute in '" + getFileName() + "'.");
            return;
        }
        SUMOReal prob = -1;
        try {
            prob = getFloatSecure(attrs, SUMO_ATTR_PROB, -1);
        } catch (EmptyData &) {
            MsgHandler::getErrorInstance()->inform("Missing probability in '" + getFileName() + "'.");
            return;
        } catch (NumberFormatException &) {
            MsgHandler::getErrorInstance()->inform("No numeric probability '" + getStringSecure(attrs, SUMO_ATTR_PROB, "") + "' " + getFileName() + "'.");
            return;
        }
        if (prob<0) {
            MsgHandler::getErrorInstance()->inform("Negative probability '" + getStringSecure(attrs, SUMO_ATTR_PROB, "") + "' " + getFileName() + "'.");
            return;
        }
        myCurrentRouteProb.add(prob, route);
    }
}


void
MSTriggeredRerouter::myCharacters(SumoXMLTag , const std::string &,
                                  const std::string &) throw()
{}


void
MSTriggeredRerouter::myEndElement(SumoXMLTag , const std::string &name) throw()
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
    if (randSUMO() > prob) {
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


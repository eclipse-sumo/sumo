//---------------------------------------------------------------------------//
//                        MSTriggeredRerouter.cpp -
//  Allows the triggered rerouting of vehicles within the simulation
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 25 July 2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.5  2006/01/17 14:10:56  dkrajzew
// debugging
//
// Revision 1.4  2005/10/07 11:37:47  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.3  2005/09/23 13:16:41  dkrajzew
// debugging the building process
//
// Revision 1.2  2005/09/22 13:45:52  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.1  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

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


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSTriggeredRerouter::Setter::Setter(MSTriggeredRerouter *parent,
                                    MSLane *lane,
                                    const std::string &id)
    : MSMoveReminder( lane, id ), myParent(parent)
{
}


MSTriggeredRerouter::Setter::~Setter()
{
}


bool
MSTriggeredRerouter::Setter::isStillActive(MSVehicle& veh, SUMOReal oldPos,
                                           SUMOReal newPos, SUMOReal newSpeed )
{
    myParent->reroute(veh, &(laneM->edge()));
    return false;
}


void
MSTriggeredRerouter::Setter::dismissByLaneChange( MSVehicle& veh )
{
}


bool
MSTriggeredRerouter::Setter::isActivatedByEmitOrLaneChange( MSVehicle& veh )
{
    myParent->reroute(veh, &(laneM->edge()));
    return false;
}



MSTriggeredRerouter::MSTriggeredRerouter(const std::string &id,
                                         MSNet &net,
                                         const std::vector<MSEdge*> &edges,
                                         SUMOReal prob,
                                         const std::string &aXMLFilename)
    : MSTrigger(id), SUMOSAXHandler("reroutings", aXMLFilename),
    myProbability(prob), myUserProbability(prob), myAmInUserMode(false)

    /*,
    myEdge(edge)*/
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
    for(std::vector<MSEdge*>::const_iterator j=edges.begin(); j!=edges.end(); ++j) {
        std::vector<MSLane*> *destLanes = (*j)->getLanes();
        std::vector<MSLane*>::const_iterator i;
        for(i=destLanes->begin(); i!=destLanes->end(); ++i) {
            string sid = id + "_at_" + (*i)->id();
            mySetter.push_back(new Setter(this, (*i), sid));
        }
    }
}


MSTriggeredRerouter::~MSTriggeredRerouter()
{
    {
        std::vector<Setter*>::iterator i;
        for(i=mySetter.begin(); i!=mySetter.end(); ++i) {
            delete *i;
        }
    }
}


void
MSTriggeredRerouter::myStartElement(int element, const std::string &name,
                                   const Attributes &attrs)
{
    if(name=="interval") {
        myCurrentIntervalBegin = getIntSecure(attrs, SUMO_ATTR_BEGIN, -1);
        myCurrentIntervalEnd = getIntSecure(attrs, SUMO_ATTR_END, -1);
    }
    // maybe by percentage
    if(name=="prob_reroute") {
        string dest = getStringSecure(attrs, SUMO_ATTR_DEST, "");
        MSEdge *to = MSEdge::dictionary(dest);
        if(to==0) {
            MsgHandler::getErrorInstance()->inform(string("Could not find edge '") + dest + string("' to reroute in '") + _file + "'.");
            return;
        }
        SUMOReal prob = -1;
        try {
            prob = getFloatSecure(attrs, SUMO_ATTR_PROB, -1);
        } catch(EmptyData &) {
            MsgHandler::getErrorInstance()->inform(string("Missing probability in '") + _file + "'.");
            return;
        } catch(NumberFormatException &) {
            MsgHandler::getErrorInstance()->inform(string("No numeric probability '") + getStringSecure(attrs, SUMO_ATTR_PROB, "") + "' " + _file + "'.");
            return;
        }
        if(prob<0) {
            MsgHandler::getErrorInstance()->inform(string("Negative probability '") + getStringSecure(attrs, SUMO_ATTR_PROB, "") + "' " + _file + "'.");
            return;
        }
        myCurrentProb.push_back(std::pair<SUMOReal, MSEdge*>(prob, to));
    }
    // maybe by closing
    if(name=="closing_reroute") {
        string toid = getStringSecure(attrs, SUMO_ATTR_ID, "");
        MSEdge *to = MSEdge::dictionary(toid);
        if(to==0) {
            MsgHandler::getErrorInstance()->inform("Could not find edge '" + toid + "' to reroute in '" + _file + "'.");
            return;
        }
        myCurrentClosed.push_back(to);
        string destid = getStringSecure(attrs, SUMO_ATTR_TO, "");
        MSEdge *dest = MSEdge::dictionary(destid);
        if(dest!=0) {
            myCurrentDests.push_back(dest);
        }
    }
    /*
    // maybe by explicite
    if(name=="explicite_reroute") {
        string dest = getStringSecure(attrs, SUMO_ATTR_DESTINATION, "");
        MSEdge *to = MSEdge::dictionary(dest);
        if(to==0) {
            MsgHandler::getErrorInstance()->inform(string("Could not find edge '") + dest + string("' to reroute in '") + _file "'.");
            return;
        }
        myCurrentExplicite.push_back(to);
    }
    */
}


void
MSTriggeredRerouter::myCharacters(int , const std::string &,
                                 const std::string &)
{
}


void
MSTriggeredRerouter::myEndElement(int , const std::string &name)
{
    if(name=="interval") {
        RerouteInterval ri;
        ri.begin = myCurrentIntervalBegin;
        ri.end = myCurrentIntervalEnd;
        ri.closed = myCurrentClosed;
        ri.dests = myCurrentDests;
        ri.prob = myCurrentProb;
        myCurrentClosed.clear();
        myCurrentProb.clear();
        myCurrentDests.clear();
        myIntervals.push_back(ri);
    }
}


bool
MSTriggeredRerouter::hasCurrentReroute(SUMOTime time, MSVehicle &veh) const
{
    std::vector<RerouteInterval>::const_iterator i =
        myIntervals.begin();
    const MSRoute &route = veh.getRoute();
    while(i!=myIntervals.end()) {
        if((*i).begin<=time && (*i).end>=time) {
            if(route.containsAnyOf((*i).closed)) {
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
    std::vector<RerouteInterval>::const_iterator i =
        myIntervals.begin();
    while(i!=myIntervals.end()) {
        if((*i).begin<=time && (*i).end>=time) {
            return true;
        }
        i++;
    }
    return false;
}


const MSTriggeredRerouter::RerouteInterval &
MSTriggeredRerouter::getCurrentReroute(SUMOTime time, MSVehicle &veh) const
{
    std::vector<RerouteInterval>::const_iterator i =
        myIntervals.begin();
    const MSRoute &route = veh.getRoute();
    while(i!=myIntervals.end()) {
        if((*i).begin<=time && (*i).end>=time) {
            if(route.containsAnyOf((*i).closed)) {
                return *i;
            }
        }
        i++;
    }
    throw 1;
}


const MSTriggeredRerouter::RerouteInterval &
MSTriggeredRerouter::getCurrentReroute(SUMOTime time) const
{
    std::vector<RerouteInterval>::const_iterator i =
        myIntervals.begin();
    while(i!=myIntervals.end()) {
        if((*i).begin<=time && (*i).end>=time) {
            return *i;
        }
        i++;
    }
    throw 1;
}



void
MSTriggeredRerouter::reroute(MSVehicle &veh, const MSEdge *src)
{
    SUMOTime time = MSNet::getInstance()->getCurrentTimeStep();
    if(hasCurrentReroute(time, veh)) {
        SUMOReal prob =
            myAmInUserMode ? myUserProbability : myProbability;
        if((SUMOReal) rand()/(SUMOReal) (RAND_MAX-1) > prob) {
            return;
        }
        const MSTriggeredRerouter::RerouteInterval &rerouteDef =
            getCurrentReroute(time, veh);
        const MSRoute &route = veh.getRoute();
        const MSEdge *lastEdge = route.getLastEdge();
        if(rerouteDef.dests.size()!=0) {
            lastEdge = rerouteDef.dests[0]; // !!!!
        }
        if(route.inFurtherUse()) {
            string nid = _id + "_re_"
                + src->id()
                + "_" + route.getID();
            if(MSRoute::dictionary(nid)!=0) {
                MSRoute *rep = MSRoute::dictionary(nid);
                veh.replaceRoute(rep,
                    MSNet::getInstance()->getCurrentTimeStep());
            } else {
                SUMODijkstraRouter<MSEdge> router(MSEdge::dictSize());
                router.prohibit(rerouteDef.closed);
                std::deque<const MSEdge*> newRoute =
                    router.compute(src, lastEdge,
                        MSNet::getInstance()->getCurrentTimeStep(), true);
                MSEdgeVector edges;
                copy(newRoute.begin(), newRoute.end(), back_inserter(edges));
                MSRoute *rep = new MSRoute(nid, edges, true);
                if(!MSRoute::dictionary(nid, rep)) {
                    cout << "Error: Could not insert route ''" << endl;
                    return;
                }
                veh.replaceRoute(rep,
                    MSNet::getInstance()->getCurrentTimeStep());
            }
        } else {
            SUMODijkstraRouter<MSEdge> router(MSEdge::dictSize());
            router.prohibit(rerouteDef.closed);
            std::deque<const MSEdge*> newRoute =
                router.compute(src, lastEdge,
                    MSNet::getInstance()->getCurrentTimeStep(), true);
            MSEdgeVector edges;
            copy(newRoute.begin(), newRoute.end(), back_inserter(edges));
            veh.replaceRoute(edges, time);
        }
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




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



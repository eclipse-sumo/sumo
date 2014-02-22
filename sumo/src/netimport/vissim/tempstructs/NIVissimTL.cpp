/****************************************************************************/
/// @file    NIVissimTL.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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


#include <map>
#include <string>
#include <cassert>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Boundary.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "NIVissimConnection.h"
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netbuild/NBLoadedTLDef.h>
#include "NIVissimDisturbance.h"
#include "NIVissimNodeDef.h"
#include "NIVissimEdge.h"
#include "NIVissimTL.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
NIVissimTL::SignalDictType NIVissimTL::NIVissimTLSignal::myDict;


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimTL::NIVissimTLSignal::NIVissimTLSignal(int lsaid, int id,
        const std::string& name,
        const std::vector<int>& groupids,
        int edgeid,
        int laneno,
        SUMOReal position,
        const std::vector<int>& vehicleTypes)
    : myLSA(lsaid), myID(id), myName(name), myGroupIDs(groupids),
      myEdgeID(edgeid), myLane(laneno), myPosition(position),
      myVehicleTypes(vehicleTypes) {}


NIVissimTL::NIVissimTLSignal::~NIVissimTLSignal() {}

bool
NIVissimTL::NIVissimTLSignal::isWithin(const PositionVector& poly) const {
    return poly.around(getPosition());
}


Position
NIVissimTL::NIVissimTLSignal::getPosition() const {
    return NIVissimAbstractEdge::dictionary(myEdgeID)->getGeomPosition(myPosition);
}


bool
NIVissimTL::NIVissimTLSignal::dictionary(int lsaid, int id,
        NIVissimTL::NIVissimTLSignal* o) {
    SignalDictType::iterator i = myDict.find(lsaid);
    if (i == myDict.end()) {
        myDict[lsaid] = SSignalDictType();
        i = myDict.find(lsaid);
    }
    SSignalDictType::iterator j = (*i).second.find(id);
    if (j == (*i).second.end()) {
        myDict[lsaid][id] = o;
        return true;
    }
    return false;
}


NIVissimTL::NIVissimTLSignal*
NIVissimTL::NIVissimTLSignal::dictionary(int lsaid, int id) {
    SignalDictType::iterator i = myDict.find(lsaid);
    if (i == myDict.end()) {
        return 0;
    }
    SSignalDictType::iterator j = (*i).second.find(id);
    if (j == (*i).second.end()) {
        return 0;
    }
    return (*j).second;
}


void
NIVissimTL::NIVissimTLSignal::clearDict() {
    for (SignalDictType::iterator i = myDict.begin(); i != myDict.end(); i++) {
        for (SSignalDictType::iterator j = (*i).second.begin(); j != (*i).second.end(); j++) {
            delete(*j).second;
        }
    }
    myDict.clear();
}


NIVissimTL::SSignalDictType
NIVissimTL::NIVissimTLSignal::getSignalsFor(int tlid) {
    SignalDictType::iterator i = myDict.find(tlid);
    if (i == myDict.end()) {
        return SSignalDictType();
    }
    return (*i).second;
}


bool
NIVissimTL::NIVissimTLSignal::addTo(NBEdgeCont& ec, NBLoadedTLDef* tl) const {
    NIVissimConnection* c = NIVissimConnection::dictionary(myEdgeID);
    NBConnectionVector assignedConnections;
    if (c == 0) {
        // What to do if on an edge? -> close all outgoing connections
        NBEdge* edge = ec.retrievePossiblySplit(toString<int>(myEdgeID), myPosition);
        if (edge == 0) {
            WRITE_WARNING("Could not set tls signal at edge '" + toString(myEdgeID) + "' - the edge was not built.");
            return false;
        }
        // Check whether it is already known, which edges are approached
        //  by which lanes
        // check whether to use the original lanes only
        if (edge->lanesWereAssigned()) {
            std::vector<NBEdge::Connection> connections = edge->getConnectionsFromLane(myLane - 1);
            for (std::vector<NBEdge::Connection>::iterator i = connections.begin(); i != connections.end(); i++) {
                const NBEdge::Connection& conn = *i;
                assert(myLane - 1 < (int)edge->getNumLanes());
                assignedConnections.push_back(NBConnection(edge, myLane - 1, conn.toEdge, conn.toLane));
            }
        } else {
            WRITE_WARNING("Edge : Lanes were not assigned(!)");
            for (unsigned int j = 0; j < edge->getNumLanes(); j++) {
                std::vector<NBEdge::Connection> connections = edge->getConnectionsFromLane(j);
                for (std::vector<NBEdge::Connection>::iterator i = connections.begin(); i != connections.end(); i++) {
                    const NBEdge::Connection& conn = *i;
                    assignedConnections.push_back(NBConnection(edge, j, conn.toEdge, conn.toLane));
                }
            }
        }
    } else {
        // get the edges
        NBEdge* tmpFrom = ec.retrievePossiblySplit(toString<int>(c->getFromEdgeID()), toString<int>(c->getToEdgeID()), true);
        NBEdge* tmpTo = ec.retrievePossiblySplit(toString<int>(c->getToEdgeID()), toString<int>(c->getFromEdgeID()), false);
        // check whether the edges are known
        if (tmpFrom != 0 && tmpTo != 0) {
            // add connections this signal is responsible for
            assignedConnections.push_back(NBConnection(tmpFrom, -1, tmpTo, -1));
        } else {
            return false;
            // !!! one of the edges could not be build
        }
    }
    // add to the group
    assert(myGroupIDs.size() != 0);
    // @todo just another hack?!
    /*
    if (myGroupIDs.size() == 1) {
        return tl->addToSignalGroup(toString<int>(*(myGroupIDs.begin())),
                                    assignedConnections);
    } else {
        // !!!
        return tl->addToSignalGroup(toString<int>(*(myGroupIDs.begin())),
                                    assignedConnections);
    }
    */
    return tl->addToSignalGroup(toString<int>(myGroupIDs.front()), assignedConnections);
}








NIVissimTL::GroupDictType NIVissimTL::NIVissimTLSignalGroup::myDict;

NIVissimTL::NIVissimTLSignalGroup::NIVissimTLSignalGroup(
    int lsaid, int id,
    const std::string& name,
    bool isGreenBegin, const std::vector<SUMOReal>& times,
    SUMOTime tredyellow, SUMOTime tyellow)
    : myLSA(lsaid), myID(id), myName(name), myTimes(times),
      myFirstIsRed(!isGreenBegin), myTRedYellow(tredyellow),
      myTYellow(tyellow) {}


NIVissimTL::NIVissimTLSignalGroup::~NIVissimTLSignalGroup() {}


bool
NIVissimTL::NIVissimTLSignalGroup::dictionary(int lsaid, int id,
        NIVissimTL::NIVissimTLSignalGroup* o) {
    GroupDictType::iterator i = myDict.find(lsaid);
    if (i == myDict.end()) {
        myDict[lsaid] = SGroupDictType();
        i = myDict.find(lsaid);
    }
    SGroupDictType::iterator j = (*i).second.find(id);
    if (j == (*i).second.end()) {
        myDict[lsaid][id] = o;
        return true;
    }
    return false;
    /*
        GroupDictType::iterator i=myDict.find(id);
        if(i==myDict.end()) {
            myDict[id] = o;
            return true;
        }
        return false;
        */
}


NIVissimTL::NIVissimTLSignalGroup*
NIVissimTL::NIVissimTLSignalGroup::dictionary(int lsaid, int id) {
    GroupDictType::iterator i = myDict.find(lsaid);
    if (i == myDict.end()) {
        return 0;
    }
    SGroupDictType::iterator j = (*i).second.find(id);
    if (j == (*i).second.end()) {
        return 0;
    }
    return (*j).second;
}

void
NIVissimTL::NIVissimTLSignalGroup::clearDict() {
    for (GroupDictType::iterator i = myDict.begin(); i != myDict.end(); i++) {
        for (SGroupDictType::iterator j = (*i).second.begin(); j != (*i).second.end(); j++) {
            delete(*j).second;
        }
    }
    myDict.clear();
}


NIVissimTL::SGroupDictType
NIVissimTL::NIVissimTLSignalGroup::getGroupsFor(int tlid) {
    GroupDictType::iterator i = myDict.find(tlid);
    if (i == myDict.end()) {
        return SGroupDictType();
    }
    return (*i).second;
}


bool
NIVissimTL::NIVissimTLSignalGroup::addTo(NBLoadedTLDef* tl) const {
    // get the color at the begin
    NBTrafficLightDefinition::TLColor color = myFirstIsRed
            ? NBTrafficLightDefinition::TLCOLOR_RED : NBTrafficLightDefinition::TLCOLOR_GREEN;
    std::string id = toString<int>(myID);
    tl->addSignalGroup(id); // !!! myTimes als SUMOTime
    for (std::vector<SUMOReal>::const_iterator i = myTimes.begin(); i != myTimes.end(); i++) {
        tl->addSignalGroupPhaseBegin(id, (SUMOTime) *i, color);
        color = color == NBTrafficLightDefinition::TLCOLOR_RED
                ? NBTrafficLightDefinition::TLCOLOR_GREEN : NBTrafficLightDefinition::TLCOLOR_RED;
    }
    if (myTimes.size() == 0) {
        if (myFirstIsRed) {
            tl->addSignalGroupPhaseBegin(id, 0, NBTrafficLightDefinition::TLCOLOR_RED);
        } else {
            tl->addSignalGroupPhaseBegin(id, 0, NBTrafficLightDefinition::TLCOLOR_GREEN);
        }
    }
    tl->setSignalYellowTimes(id, myTRedYellow, myTYellow);
    return true;
}








NIVissimTL::DictType NIVissimTL::myDict;

NIVissimTL::NIVissimTL(int id, const std::string& type,
                       const std::string& name, SUMOTime absdur,
                       SUMOTime offset)
    : myID(id), myName(name), myAbsDuration(absdur), myOffset(offset),
      myCurrentGroup(0), myType(type)

{}


NIVissimTL::~NIVissimTL() {}





bool
NIVissimTL::dictionary(int id, const std::string& type,
                       const std::string& name, SUMOTime absdur,
                       SUMOTime offset) {
    NIVissimTL* o = new NIVissimTL(id, type, name, absdur, offset);
    if (!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}

bool
NIVissimTL::dictionary(int id, NIVissimTL* o) {
    DictType::iterator i = myDict.find(id);
    if (i == myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimTL*
NIVissimTL::dictionary(int id) {
    DictType::iterator i = myDict.find(id);
    if (i == myDict.end()) {
        return 0;
    }
    return (*i).second;
}


void
NIVissimTL::clearDict() {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}





bool
NIVissimTL::dict_SetSignals(NBTrafficLightLogicCont& tlc,
                            NBEdgeCont& ec) {
    size_t ref = 0;
    size_t ref_groups = 0;
    size_t ref_signals = 0;
    size_t no_signals = 0;
    size_t no_groups = 0;
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); i++) {
        NIVissimTL* tl = (*i).second;
        /*		if(tl->myType!="festzeit") {
        			cout << " Warning: The traffic light '" << tl->myID
        				<< "' could not be assigned to a node." << endl;
        			ref++;
        			continue;
        		}*/
        std::string id = toString<int>(tl->myID);
        TrafficLightType type = ((tl->getType() == "festzeit" || tl->getType() == "festzeit_fake") ?
                                 TLTYPE_STATIC : TLTYPE_ACTUATED);
        NBLoadedTLDef* def = new NBLoadedTLDef(id, 0, type);
        if (!tlc.insert(def)) {
            WRITE_ERROR("Error on adding a traffic light\n Must be a multiple id ('" + id + "')");
            continue;
        }
        def->setCycleDuration((unsigned int) tl->myAbsDuration);
        // add each group to the node's container
        SGroupDictType sgs = NIVissimTLSignalGroup::getGroupsFor(tl->getID());
        for (SGroupDictType::const_iterator j = sgs.begin(); j != sgs.end(); j++) {
            if (!(*j).second->addTo(def)) {
                WRITE_WARNING("The signal group '" + toString<int>((*j).first) + "' could not be assigned to tl '" + toString<int>(tl->myID) + "'.");
                ref_groups++;
            }
            no_groups++;
        }
        // add the signal group signals to the node
        SSignalDictType signals = NIVissimTLSignal::getSignalsFor(tl->getID());
        for (SSignalDictType::const_iterator k = signals.begin(); k != signals.end(); k++) {
            if (!(*k).second->addTo(ec, def)) {
                WRITE_WARNING("The signal '" + toString<int>((*k).first) + "' could not be assigned to tl '" + toString<int>(tl->myID) + "'.");
                ref_signals++;
            }
            no_signals++;
        }
    }
    if (ref != 0) {
        WRITE_WARNING("Could not set " + toString<size_t>(ref) + " of " + toString<size_t>(myDict.size()) + " traffic lights.");
    }
    if (ref_groups != 0) {
        WRITE_WARNING("Could not set " + toString<size_t>(ref_groups) + " of " + toString<size_t>(no_groups) + " groups.");
    }
    if (ref_signals != 0) {
        WRITE_WARNING("Could not set " + toString<size_t>(ref_signals) + " of " + toString<size_t>(no_signals) + " signals.");
    }
    return true;

}


std::string
NIVissimTL::getType() const {
    return myType;
}


int
NIVissimTL::getID() const {
    return myID;
}



/****************************************************************************/


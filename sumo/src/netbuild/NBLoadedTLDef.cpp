/****************************************************************************/
/// @file    NBLoadedTLDef.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id$
///
// A lodeded (complete) traffic light logic
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <vector>
#include <set>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include "NBTrafficLightLogic.h"
#include "NBTrafficLightDefinition.h"
#include "NBTrafficLightLogicVector.h"
#include "NBLoadedTLDef.h"
#include "NBNode.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// used namespaces
// ===========================================================================

using namespace std;

/* -------------------------------------------------------------------------
 * NBLoadedTLDef::SignalGroup-methods
 * ----------------------------------------------------------------------- */
NBLoadedTLDef::SignalGroup::SignalGroup(const std::string &id) throw()
        : Named(id) {}

NBLoadedTLDef::SignalGroup::~SignalGroup() throw() {}

void
NBLoadedTLDef::SignalGroup::addConnection(const NBConnection &c) {
    assert(c.getFromLane()<0||c.getFrom()->getNoLanes()>(size_t)c.getFromLane());
    myConnections.push_back(c);
}


void
NBLoadedTLDef::SignalGroup::addPhaseBegin(SUMOTime time, TLColor color) {
    myPhases.push_back(PhaseDef(time, color));
}


void
NBLoadedTLDef::SignalGroup::setYellowTimes(SUMOTime tRedYellow,
        SUMOTime tYellow) {
    myTRedYellow = tRedYellow;
    myTYellow = tYellow;
}


void
NBLoadedTLDef::SignalGroup::sortPhases() {
    sort(myPhases.begin(), myPhases.end(),
         phase_by_time_sorter());
}


void
NBLoadedTLDef::SignalGroup::patchTYellow(SUMOTime tyellow) {
    if (myTYellow<tyellow) {
        WRITE_WARNING("TYellow of signal group '" + getID()+ "' was less than the computed one; patched (was:" + toString<SUMOTime>(myTYellow) + ", is:" + toString<int>(tyellow) + ")");
        myTYellow = tyellow;
    }
}

/*
void
NBLoadedTLDef::SignalGroup::patchFalseGreenPhases(SUMOReal cycleDuration)
{
    for(GroupsPhases::iterator i=myPhases.begin(); i!=myPhases.end(); i++) {
        SUMOReal duration;
        if(i==myPhases.end()-1) {
            duration = cycleDuration - (*i).myTime + (*myPhases.begin()).myTime;
        } else {
            duration = (*(i+1)).myTime - (*i).myTime;
        }
        if((*i).myColor==TLCOLOR_GREEN&&duration-myTYellow<1.0) {
            WRITE_WARNING(\
                string("Replacing a non-existing (length<1s)")\
                + string(" green phase by red in signal group '")\
                + getID() + string("' by red (removing)."));
            i = myPhases.erase(i);
        }
    }
}
*/

DoubleVector
NBLoadedTLDef::SignalGroup::getTimes(SUMOTime cycleDuration) const {
    // within the phase container, we should have the green and red phases
    //  add their times
    DoubleVector ret; // !!! time vector
    for (GroupsPhases::const_iterator i=myPhases.begin(); i!=myPhases.end(); i++) {
        ret.push_back((SUMOReal)(*i).myTime);
    }
    // further, we possibly should set the yellow phases
    if (myTYellow>0) {
        for (GroupsPhases::const_iterator i=myPhases.begin(); i!=myPhases.end(); i++) {
            if ((*i).myColor==TLCOLOR_RED) {
                SUMOTime time = (SUMOTime)(*i).myTime + myTYellow;
                if (time>cycleDuration) {
                    time = time - cycleDuration ;
                }
                ret.push_back((SUMOReal) time);
            } /*else {
                                                                // verify whether the green phases are long enough
                                                                if((*i).myTime-myTYellow<5) {
                                                                    WRITE_WARNING(
                                                                        string("The signal group '") + getID()\
                                                                        + string("' holds a green phase with a duration below 5s (")\
                                                                        + toString<int>((*i).myTime-myTYellow)\
                                                                        + string(")."));
                                                                }
                                                            }*/
        }
    }
    return ret;
}


size_t
NBLoadedTLDef::SignalGroup::getLinkNo() const {
    return myConnections.size();
}


bool
NBLoadedTLDef::SignalGroup::mayDrive(SUMOTime time) const {
    assert(myPhases.size()!=0);
    for (GroupsPhases::const_reverse_iterator i=myPhases.rbegin(); i!=myPhases.rend(); i++) {
        SUMOTime nextTime = (*i).myTime;
        if (time>=nextTime) {
            /*            if(i==myPhases.rbegin()) {
                            return (*(myPhases.end()-1)).myColor==TLCOLOR_GREEN;
                        } else {*/
            return (*i).myColor==TLCOLOR_GREEN;
//            }
        }
    }
    return (*(myPhases.end()-1)).myColor==TLCOLOR_GREEN;
}


bool
NBLoadedTLDef::SignalGroup::hasYellow(SUMOTime time) const {
    bool has_red_now = !mayDrive(time);
    bool had_green = mayDrive(time-myTYellow);
    return has_red_now&&had_green;
}

/*
bool
NBLoadedTLDef::SignalGroup::mustBrake(SUMOReal time) const
{
	assert(myPhases.size()!=0);
    for(GroupsPhases::const_iterator i=myPhases.begin(); i!=myPhases.end(); i++) {
        SUMOReal nextTime = (*i).myTime;
        if(nextTime>time) {
            if(i==myPhases.begin()) {
                return (*(myPhases.end()-1)).myColor==TLCOLOR_RED;
            } else {
                return (*(i-1)).myColor==TLCOLOR_RED;
            }
        }
    }
    return (*(myPhases.end()-1)).myColor==TLCOLOR_RED;
}
*/

bool
NBLoadedTLDef::SignalGroup::containsConnection(NBEdge *from, NBEdge *to) const {
    for (NBConnectionVector::const_iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        if ((*i).getFrom()==from&&(*i).getTo()==to) {
            return true;
        }
    }
    return false;

}


const NBConnection &
NBLoadedTLDef::SignalGroup::getConnection(size_t pos) const {
    assert(pos<myConnections.size());
    return myConnections[pos];
}


bool
NBLoadedTLDef::SignalGroup::containsIncoming(NBEdge *from) const {
    for (NBConnectionVector::const_iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        if ((*i).getFrom()==from) {
            return true;
        }
    }
    return false;
}


void
NBLoadedTLDef::SignalGroup::remapIncoming(NBEdge *which, const EdgeVector &by) {
    NBConnectionVector newConns;
    for (NBConnectionVector::iterator i=myConnections.begin(); i!=myConnections.end();) {
        if ((*i).getFrom()==which) {
            NBConnection conn((*i).getFrom(), (*i).getTo());
            i = myConnections.erase(i);
            for (EdgeVector::const_iterator j=by.begin(); j!=by.end(); j++) {
                NBConnection curr(conn);
                if (!curr.replaceFrom(which, *j)) {
                    throw 1;
                }
                newConns.push_back(curr);
            }
        } else {
            i++;
        }
    }
    copy(newConns.begin(), newConns.end(),
         back_inserter(myConnections));
}


bool
NBLoadedTLDef::SignalGroup::containsOutgoing(NBEdge *to) const {
    for (NBConnectionVector::const_iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        if ((*i).getTo()==to) {
            return true;
        }
    }
    return false;
}


void
NBLoadedTLDef::SignalGroup::remapOutgoing(NBEdge *which, const EdgeVector &by) {
    NBConnectionVector newConns;
    for (NBConnectionVector::iterator i=myConnections.begin(); i!=myConnections.end();) {
        if ((*i).getTo()==which) {
            NBConnection conn((*i).getFrom(), (*i).getTo());
            i = myConnections.erase(i);
            for (EdgeVector::const_iterator j=by.begin(); j!=by.end(); j++) {
                NBConnection curr(conn);
                if (!curr.replaceTo(which, *j)) {
                    throw 1;
                }
                newConns.push_back(curr);
            }
        } else {
            i++;
        }
    }
    copy(newConns.begin(), newConns.end(),
         back_inserter(myConnections));
}


void
NBLoadedTLDef::SignalGroup::remap(NBEdge *removed, int removedLane,
                                  NBEdge *by, int byLane) {
    for (NBConnectionVector::iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        if ((*i).getTo()==removed
                &&
                ((*i).getToLane()==removedLane
                 ||
                 (*i).getToLane()==-1)) {
            (*i).replaceTo(removed, removedLane, by, byLane);

        } else if ((*i).getTo()==removed && removedLane==-1) {
            (*i).replaceTo(removed, by);
        }

        if ((*i).getFrom()==removed
                &&
                ((*i).getFromLane()==removedLane
                 ||
                 (*i).getFromLane()==-1)) {
            (*i).replaceFrom(removed, removedLane, by, byLane);

        } else if ((*i).getFrom()==removed && removedLane==-1) {
            (*i).replaceFrom(removed, by);
        }
    }
}


/* -------------------------------------------------------------------------
 * NBLoadedTLDef::Phase-methods
 * ----------------------------------------------------------------------- */
NBLoadedTLDef::Phase::Phase(const std::string &id, SUMOTime begin, SUMOTime end) throw()
        : Named(id), myBegin(begin), myEnd(end) {}


NBLoadedTLDef::Phase::~Phase() throw() {}


/* -------------------------------------------------------------------------
 * NBLoadedTLDef::Phase-methods
 * ----------------------------------------------------------------------- */
NBLoadedTLDef::NBLoadedTLDef(const std::string &id,
                             const std::set<NBNode*> &junctions) throw()
        : NBTrafficLightDefinition(id, junctions) {}


NBLoadedTLDef::NBLoadedTLDef(const std::string &id, NBNode *junction) throw()
        : NBTrafficLightDefinition(id, junction) {}


NBLoadedTLDef::NBLoadedTLDef(const std::string &id) throw()
        : NBTrafficLightDefinition(id) {}


NBLoadedTLDef::~NBLoadedTLDef() throw() {
    for (SignalGroupCont::iterator i=mySignalGroups.begin(); i!=mySignalGroups.end(); ++i) {
        delete(*i).second;
    }
}


NBTrafficLightLogicVector *
NBLoadedTLDef::myCompute(const NBEdgeCont &ec, size_t breakingTime, std::string type) {
    MsgHandler::getWarningInstance()->clear(); // !!!
    NBLoadedTLDef::SignalGroupCont::const_iterator i;
    // compute the switching times
    std::set<SUMOReal> tmpSwitchTimes;
    for (i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        NBLoadedTLDef::SignalGroup *group = (*i).second;
        // needed later
        group->sortPhases();
        // patch the yellow time for this group
        if (OptionsCont::getOptions().getBool("patch-small-tyellow")) {
            group->patchTYellow(breakingTime);
        }
        // check for too short green lights to be patched
//        group->patchFalseGreenPhases(myCycleDuration);
        // copy the now valid times into the container
        //  both the given red and green phases are added and also the
        //  yellow times
        DoubleVector gtimes = group->getTimes(myCycleDuration);
        for (DoubleVector::const_iterator k=gtimes.begin(); k!=gtimes.end(); k++) {
            tmpSwitchTimes.insert(*k);
        }
    }
    std::vector<SUMOReal> switchTimes;
    copy(tmpSwitchTimes.begin(), tmpSwitchTimes.end(),
         back_inserter(switchTimes));
    sort(switchTimes.begin(), switchTimes.end());

    // count the signals
    size_t noSignals = 0;
    for (i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        noSignals += (*i).second->getLinkNo();
    }
    // build the phases
    NBTrafficLightLogic *logic =
        new NBTrafficLightLogic(getID(), "0", type, noSignals);
    for (std::vector<SUMOReal>::iterator l=switchTimes.begin(); l!=switchTimes.end(); l++) {
        // compute the duration of the current phase
        size_t duration;
        if (l!=switchTimes.end()-1) {
            // get from the difference to the next switching time
            duration = (size_t)((*(l+1)) - (*l));
        } else {
            // get from the differenc to the first switching time
            duration = (size_t)(myCycleDuration - (*l) + *(switchTimes.begin())) ;
        }
        // no information about yellow times will be generated
        assert((*l)>=0);
        Masks masks = buildPhaseMasks(ec, (size_t)(*l));
        logic->addStep(duration,
                       masks.driveMask, masks.brakeMask, masks.yellowMask);
    }
    // check whether any warnings were printed
    if (MsgHandler::getWarningInstance()->wasInformed()) {
        WRITE_WARNING("During computation of traffic light '" + getID() + "'.");
    }
    logic->closeBuilding();
    // returns the build logic
    NBTrafficLightLogicVector *ret =
        new NBTrafficLightLogicVector(myControlledLinks, type);
    ret->add(logic);
    return ret;
}


void
NBLoadedTLDef::setTLControllingInformation(const NBEdgeCont &ec) const {
    // assign the links to the connections
    size_t pos = 0;
    for (SignalGroupCont::const_iterator m=mySignalGroups.begin(); m!=mySignalGroups.end(); m++) {
        SignalGroup *group = (*m).second;
        size_t linkNo = group->getLinkNo();
        for (size_t j=0; j<linkNo; j++) {
            const NBConnection &conn = group->getConnection(j);
            assert(conn.getFromLane()<0||(int) conn.getFrom()->getNoLanes()>conn.getFromLane());
            NBConnection tst(conn);
            if (tst.check(ec)) {
                NBEdge *edge = conn.getFrom();
                if (edge->setControllingTLInformation(
                            conn.getFromLane(), conn.getTo(), conn.getToLane(),
                            getID(), pos)) {
                    pos++;
                }
            } else {
                WRITE_WARNING("Could not set signal on connection (signal: " + getID() + ", group: " + group->getID()+ ")");
            }
        }
    }
}

NBLoadedTLDef::Masks
NBLoadedTLDef::buildPhaseMasks(const NBEdgeCont &ec, size_t time) const {
    // set the masks
    Masks masks;
    size_t pos = 0;
    SignalGroupCont::const_iterator i;
    // set the green and yellow information first;
    //  the information whether other have to break needs those masks
    //  completely filled
    for (i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        SignalGroup *group = (*i).second;
        size_t linkNo = group->getLinkNo();
        bool mayDrive = group->mayDrive(time);
        bool hasYellow = group->hasYellow(time);
        for (size_t j=0; j<linkNo; j++) {
            masks.driveMask[pos] = mayDrive;
            masks.yellowMask[pos] = hasYellow;
            const NBConnection &conn = group->getConnection(j);
            NBConnection assConn(conn);
            // assert that the connection really exists
            if (assConn.check(ec)) {
                /*                masks.brakeMask[pos] =
                                    mustBrake(conn.getFrom(), conn.getTo()) | !mayDrive;*/
//                masks.brakeMask = mustBrake(
                pos++;
            }
        }
    }
    // set the breaking mask
    pos = 0;
    for (i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        SignalGroup *group = (*i).second;
        size_t linkNo = group->getLinkNo();
        for (size_t j=0; j<linkNo; j++) {
            const NBConnection &conn = group->getConnection(j);
            NBConnection assConn(conn);
            if (assConn.check(ec)) {
                masks.brakeMask[pos] =
                    mustBrake(ec, assConn,
                              masks.driveMask, masks.yellowMask, pos);
                pos++;
            }
        }
    }
    return masks;
}


bool
NBLoadedTLDef::mustBrake(const NBEdgeCont &ec,
                         const NBConnection &possProhibited,
                         const std::bitset<64> &green,
                         const std::bitset<64> &/*yellow*/,
                         size_t strmpos) const {
    // check whether the stream has red
    if (!green.test(strmpos)) {
        return true;
    }

    // check whether another stream which has green is a higher
    //  priorised foe to the given
    size_t pos = 0;
    for (SignalGroupCont::const_iterator i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        SignalGroup *group = (*i).second;
        // get otherlinks that have green
        size_t linkNo = group->getLinkNo();
        for (size_t j=0; j<linkNo; j++) {
            // get the current connection (possible foe)
            const NBConnection &other = group->getConnection(j);
            NBConnection possProhibitor(other);
            // if the connction ist still valid ...
            if (possProhibitor.check(ec)) {
                /*                assert(possProhibited.getToLane()>=0);
                                assert(possProhibitor.getToLane()>=0);
                                // does not have to break du to this connection as
                                //  different destination lanes are used
                                if(possProhibited.getToLane()!=possProhibitor.getToLane()) {
                                    continue;
                                }*/
                // ... do nothing if it starts at the same edge
                if (possProhibited.getFrom()==possProhibitor.getFrom()) {
                    pos++;
                    continue;
                }
                if (green.test(pos)) {
                    if (NBTrafficLightDefinition::mustBrake(possProhibited, possProhibitor, true)) {
                        return true;
                    }
                }
                /*                if( yellow.test(pos) ) {
                                    if(NBTrafficLightDefinition::mustBrake(possProhibited, possProhibitor, true)) {
                                        return true;
                                    }
                                }*/
                pos++;
            }
        }
    }
    return false;
}


void
NBLoadedTLDef::collectNodes() {
    SignalGroupCont::const_iterator m;
    for (m=mySignalGroups.begin(); m!=mySignalGroups.end(); m++) {
        SignalGroup *group = (*m).second;
        size_t linkNo = group->getLinkNo();
        for (size_t j=0; j<linkNo; j++) {
            const NBConnection &conn = group->getConnection(j);
            NBEdge *edge = conn.getFrom();
            NBNode *node = edge->getToNode();
            myControlledNodes.insert(node);
        }
    }
}


void
NBLoadedTLDef::collectLinks() {
    // build the list of links which are controled by the traffic light
    for (EdgeVector::iterator i=myIncomingEdges.begin(); i!=myIncomingEdges.end(); i++) {
        NBEdge *incoming = *i;
        unsigned int noLanes = incoming->getNoLanes();
        for (unsigned int j=0; j<noLanes; j++) {
            vector<NBEdge::Connection> elv = incoming->getConnectionsFromLane(j);
            for (vector<NBEdge::Connection>::iterator k=elv.begin(); k!=elv.end(); k++) {
                NBEdge::Connection el = *k;
                if (el.toEdge!=0) {
                    myControlledLinks.push_back(NBConnection(incoming, j, el.toEdge, el.toLane));
                }
            }
        }
    }
}


NBLoadedTLDef::SignalGroup *
NBLoadedTLDef::findGroup(NBEdge *from, NBEdge *to) const {
    for (SignalGroupCont::const_iterator i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        if ((*i).second->containsConnection(from, to)) {
            return (*i).second;
        }
    }
    return 0;
}



bool
NBLoadedTLDef::addToSignalGroup(const std::string &groupid,
                                const NBConnection &connection) {
    if (mySignalGroups.find(groupid)==mySignalGroups.end()) {
        return false;
    }
    mySignalGroups[groupid]->addConnection(connection);
    NBNode *n1 = connection.getFrom()->getToNode();
    if (n1!=0) {
        addNode(n1);
        n1->addTrafficLight(this);
    }
    NBNode *n2 = connection.getTo()->getFromNode();
    if (n2!=0) {
        addNode(n2);
        n2->addTrafficLight(this);
    }
    return true;
}


bool
NBLoadedTLDef::addToSignalGroup(const std::string &groupid,
                                const NBConnectionVector &connections) {
    bool ok = true;
    for (NBConnectionVector::const_iterator i=connections.begin(); i!=connections.end(); i++) {
        ok &= addToSignalGroup(groupid, *i);
    }
    return ok;
}


void
NBLoadedTLDef::addSignalGroup(const std::string &id) {
    assert(mySignalGroups.find(id)==mySignalGroups.end());
    mySignalGroups[id] = new SignalGroup(id);
}


void
NBLoadedTLDef::addSignalGroupPhaseBegin(const std::string &groupid, SUMOTime time,
                                        TLColor color) {
    assert(mySignalGroups.find(groupid)!=mySignalGroups.end());
    mySignalGroups[groupid]->addPhaseBegin(time, color);
}

void
NBLoadedTLDef::setSignalYellowTimes(const std::string &groupid,
                                    SUMOTime myTRedYellow, SUMOTime myTYellow) {
    assert(mySignalGroups.find(groupid)!=mySignalGroups.end());
    mySignalGroups[groupid]->setYellowTimes(myTRedYellow, myTYellow);
}


void
NBLoadedTLDef::setCycleDuration(size_t cycleDur) {
    myCycleDuration = cycleDur;
}


void
NBLoadedTLDef::remapRemoved(NBEdge *removed,
                            const EdgeVector &incoming,
                            const EdgeVector &outgoing) {
    for (SignalGroupCont::const_iterator i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        SignalGroup *group = (*i).second;
        if (group->containsIncoming(removed)) {
            group->remapIncoming(removed, incoming);
        }
        if (group->containsOutgoing(removed)) {
            group->remapOutgoing(removed, outgoing);
        }
    }
}


void
NBLoadedTLDef::replaceRemoved(NBEdge *removed, int removedLane,
                              NBEdge *by, int byLane) {
    for (SignalGroupCont::const_iterator i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        SignalGroup *group = (*i).second;
        if (group->containsIncoming(removed)||group->containsOutgoing(removed)) {
            group->remap(removed, removedLane, by, byLane);
        }
    }
}



/****************************************************************************/


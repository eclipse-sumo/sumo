/****************************************************************************/
/// @file    NBLoadedTLDef.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id$
///
// A loaded (complete) traffic light logic
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
#include "NBLoadedTLDef.h"
#include "NBNode.h"

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
 * NBLoadedTLDef::SignalGroup-methods
 * ----------------------------------------------------------------------- */
NBLoadedTLDef::SignalGroup::SignalGroup(const std::string &id) throw()
        : Named(id) {}

NBLoadedTLDef::SignalGroup::~SignalGroup() throw() {}

void
NBLoadedTLDef::SignalGroup::addConnection(const NBConnection &c) throw() {
    assert(c.getFromLane()<0||c.getFrom()->getNoLanes()>(unsigned int)c.getFromLane());
    myConnections.push_back(c);
}


void
NBLoadedTLDef::SignalGroup::addPhaseBegin(SUMOTime time, TLColor color) throw() {
    myPhases.push_back(PhaseDef(time, color));
}


void
NBLoadedTLDef::SignalGroup::setYellowTimes(SUMOTime tRedYellow,
        SUMOTime tYellow) throw() {
    myTRedYellow = tRedYellow;
    myTYellow = tYellow;
}


void
NBLoadedTLDef::SignalGroup::sortPhases() throw() {
    sort(myPhases.begin(), myPhases.end(),
         phase_by_time_sorter());
}


void
NBLoadedTLDef::SignalGroup::patchTYellow(SUMOTime tyellow) throw() {
    if (myTYellow<tyellow) {
        WRITE_WARNING("TYellow of signal group '" + getID()+ "' was less than the computed one; patched (was:" + toString<SUMOTime>(myTYellow) + ", is:" + toString<int>(tyellow) + ")");
        myTYellow = tyellow;
    }
}


DoubleVector
NBLoadedTLDef::SignalGroup::getTimes(SUMOTime cycleDuration) const throw() {
    // within the phase container, we should have the green and red phases
    //  add their times
    DoubleVector ret; // !!! time vector
    for (std::vector<PhaseDef>::const_iterator i=myPhases.begin(); i!=myPhases.end(); i++) {
        ret.push_back((SUMOReal)(*i).myTime);
    }
    // further, we possibly should set the yellow phases
    if (myTYellow>0) {
        for (std::vector<PhaseDef>::const_iterator i=myPhases.begin(); i!=myPhases.end(); i++) {
            if ((*i).myColor==TLCOLOR_RED) {
                SUMOTime time = (SUMOTime)(*i).myTime + myTYellow;
                if (time>cycleDuration) {
                    time = time - cycleDuration ;
                }
                ret.push_back((SUMOReal) time);
            }
        }
    }
    return ret;
}


unsigned int
NBLoadedTLDef::SignalGroup::getLinkNo() const throw() {
    return (unsigned int) myConnections.size();
}


bool
NBLoadedTLDef::SignalGroup::mayDrive(SUMOTime time) const throw() {
    assert(myPhases.size()!=0);
    for (std::vector<PhaseDef>::const_reverse_iterator i=myPhases.rbegin(); i!=myPhases.rend(); i++) {
        SUMOTime nextTime = (*i).myTime;
        if (time>=nextTime) {
            return (*i).myColor==TLCOLOR_GREEN;
        }
    }
    return (*(myPhases.end()-1)).myColor==TLCOLOR_GREEN;
}


bool
NBLoadedTLDef::SignalGroup::hasYellow(SUMOTime time) const throw() {
    bool has_red_now = !mayDrive(time);
    bool had_green = mayDrive(time-myTYellow);
    return has_red_now&&had_green;
}


bool
NBLoadedTLDef::SignalGroup::containsConnection(NBEdge *from, NBEdge *to) const throw() {
    for (NBConnectionVector::const_iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        if ((*i).getFrom()==from&&(*i).getTo()==to) {
            return true;
        }
    }
    return false;

}


const NBConnection &
NBLoadedTLDef::SignalGroup::getConnection(unsigned int pos) const throw() {
    assert(pos<myConnections.size());
    return myConnections[pos];
}


bool
NBLoadedTLDef::SignalGroup::containsIncoming(NBEdge *from) const throw() {
    for (NBConnectionVector::const_iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        if ((*i).getFrom()==from) {
            return true;
        }
    }
    return false;
}


void
NBLoadedTLDef::SignalGroup::remapIncoming(NBEdge *which, const EdgeVector &by) throw(ProcessError) {
    NBConnectionVector newConns;
    for (NBConnectionVector::iterator i=myConnections.begin(); i!=myConnections.end();) {
        if ((*i).getFrom()==which) {
            NBConnection conn((*i).getFrom(), (*i).getTo());
            i = myConnections.erase(i);
            for (EdgeVector::const_iterator j=by.begin(); j!=by.end(); j++) {
                NBConnection curr(conn);
                if (!curr.replaceFrom(which, *j)) {
                    throw ProcessError("Could not replace edge '" + which->getID() + "' by '" + (*j)->getID() + "'.\nUndefined...");
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
NBLoadedTLDef::SignalGroup::containsOutgoing(NBEdge *to) const throw() {
    for (NBConnectionVector::const_iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        if ((*i).getTo()==to) {
            return true;
        }
    }
    return false;
}


void
NBLoadedTLDef::SignalGroup::remapOutgoing(NBEdge *which, const EdgeVector &by) throw(ProcessError) {
    NBConnectionVector newConns;
    for (NBConnectionVector::iterator i=myConnections.begin(); i!=myConnections.end();) {
        if ((*i).getTo()==which) {
            NBConnection conn((*i).getFrom(), (*i).getTo());
            i = myConnections.erase(i);
            for (EdgeVector::const_iterator j=by.begin(); j!=by.end(); j++) {
                NBConnection curr(conn);
                if (!curr.replaceTo(which, *j)) {
                    throw ProcessError("Could not replace edge '" + which->getID() + "' by '" + (*j)->getID() + "'.\nUndefined...");
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
                                  NBEdge *by, int byLane) throw() {
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


NBTrafficLightLogic *
NBLoadedTLDef::myCompute(const NBEdgeCont &ec, unsigned int brakingTime) throw() {
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
            group->patchTYellow(brakingTime);
        }
        // copy the now valid times into the container
        //  both the given red and green phases are added and also the
        //  yellow times
        DoubleVector gtimes = group->getTimes(myCycleDuration);
        for (DoubleVector::const_iterator k=gtimes.begin(); k!=gtimes.end(); k++) {
            tmpSwitchTimes.insert(*k);
        }
    }
    std::vector<SUMOReal> switchTimes;
    copy(tmpSwitchTimes.begin(), tmpSwitchTimes.end(), back_inserter(switchTimes));
    sort(switchTimes.begin(), switchTimes.end());

    // count the signals
    unsigned int noSignals = 0;
    for (i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        noSignals += (*i).second->getLinkNo();
    }
    // build the phases
    NBTrafficLightLogic *logic = new NBTrafficLightLogic(getID(), "0", noSignals);
    for (std::vector<SUMOReal>::iterator l=switchTimes.begin(); l!=switchTimes.end(); l++) {
        // compute the duration of the current phase
        unsigned int duration;
        if (l!=switchTimes.end()-1) {
            // get from the difference to the next switching time
            duration = (unsigned int)((*(l+1)) - (*l));
        } else {
            // get from the differenc to the first switching time
            duration = (unsigned int)(myCycleDuration - (*l) + *(switchTimes.begin())) ;
        }
        // no information about yellow times will be generated
        assert((*l)>=0);
        logic->addStep(duration, buildPhaseState(ec, (unsigned int)(*l)));
    }
    // check whether any warnings were printed
    if (MsgHandler::getWarningInstance()->wasInformed()) {
        WRITE_WARNING("During computation of traffic light '" + getID() + "'.");
    }
    logic->closeBuilding();
    return logic;
}


void
NBLoadedTLDef::setTLControllingInformation(const NBEdgeCont &ec) const throw() {
    // assign the links to the connections
    unsigned int pos = 0;
    for (SignalGroupCont::const_iterator m=mySignalGroups.begin(); m!=mySignalGroups.end(); m++) {
        SignalGroup *group = (*m).second;
        unsigned int linkNo = group->getLinkNo();
        for (unsigned int j=0; j<linkNo; j++) {
            const NBConnection &conn = group->getConnection(j);
            assert(conn.getFromLane()<0||(int) conn.getFrom()->getNoLanes()>conn.getFromLane());
            NBConnection tst(conn);
            if (tst.check(ec)) {
                NBEdge *edge = conn.getFrom();
                if (edge->setControllingTLInformation(conn.getFromLane(), conn.getTo(), conn.getToLane(), getID(), pos)) {
                    pos++;
                }
            } else {
                WRITE_WARNING("Could not set signal on connection (signal: " + getID() + ", group: " + group->getID()+ ")");
            }
        }
    }
}


std::string
NBLoadedTLDef::buildPhaseState(const NBEdgeCont &ec, unsigned int time) const throw() {
    unsigned int pos = 0;
    std::string state;
    // set the green and yellow information first;
    //  the information whether other have to break needs those masks
    //  completely filled
    for (SignalGroupCont::const_iterator i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        SignalGroup *group = (*i).second;
        unsigned int linkNo = group->getLinkNo();
        bool mayDrive = group->mayDrive(time);
        bool hasYellow = group->hasYellow(time);
        char c = 'r';
        if (mayDrive) {
            c = 'g';
        }
        if (hasYellow) {
            c = 'y';
        }
        for (unsigned int j=0; j<linkNo; j++) {
            const NBConnection &conn = group->getConnection(j);
            NBConnection assConn(conn);
            // assert that the connection really exists
            if (assConn.check(ec)) {
                state = state + c;
                ++pos;
            }
        }
    }
    // set the braking mask
    pos = 0;
    for (SignalGroupCont::const_iterator i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        SignalGroup *group = (*i).second;
        unsigned int linkNo = group->getLinkNo();
        for (unsigned int j=0; j<linkNo; j++) {
            const NBConnection &conn = group->getConnection(j);
            NBConnection assConn(conn);
            if (assConn.check(ec)) {
                if (!mustBrake(ec, assConn, state, pos)) {
                    if (state[pos]=='g') {
                        state[pos] = 'G';
                    }
                    if (state[pos]=='y') {
                        state[pos] = 'Y';
                    }
                }
                pos++;
            }
        }
    }
    return state;
}


bool
NBLoadedTLDef::mustBrake(const NBEdgeCont &ec,
                         const NBConnection &possProhibited,
                         const std::string &state,
                         unsigned int strmpos) const throw() {
    // check whether the stream has red
    if (state[strmpos]!='g'&&state[strmpos]!='G') {
        return true;
    }

    // check whether another stream which has green is a higher
    //  priorised foe to the given
    unsigned int pos = 0;
    for (SignalGroupCont::const_iterator i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        SignalGroup *group = (*i).second;
        // get otherlinks that have green
        unsigned int linkNo = group->getLinkNo();
        for (unsigned int j=0; j<linkNo; j++) {
            // get the current connection (possible foe)
            const NBConnection &other = group->getConnection(j);
            NBConnection possProhibitor(other);
            // if the connction ist still valid ...
            if (possProhibitor.check(ec)) {
                // ... do nothing if it starts at the same edge
                if (possProhibited.getFrom()==possProhibitor.getFrom()) {
                    pos++;
                    continue;
                }
                if (state[pos]=='g'||state[pos]=='G') {
                    if (NBTrafficLightDefinition::mustBrake(possProhibited, possProhibitor, true)) {
                        return true;
                    }
                }
                pos++;
            }
        }
    }
    return false;
}


void
NBLoadedTLDef::collectNodes() throw() {
    SignalGroupCont::const_iterator m;
    for (m=mySignalGroups.begin(); m!=mySignalGroups.end(); m++) {
        SignalGroup *group = (*m).second;
        unsigned int linkNo = group->getLinkNo();
        for (unsigned int j=0; j<linkNo; j++) {
            const NBConnection &conn = group->getConnection(j);
            NBEdge *edge = conn.getFrom();
            NBNode *node = edge->getToNode();
            myControlledNodes.insert(node);
        }
    }
}


void
NBLoadedTLDef::collectLinks() throw(ProcessError) {
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
NBLoadedTLDef::findGroup(NBEdge *from, NBEdge *to) const throw() {
    for (SignalGroupCont::const_iterator i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        if ((*i).second->containsConnection(from, to)) {
            return (*i).second;
        }
    }
    return 0;
}


bool
NBLoadedTLDef::addToSignalGroup(const std::string &groupid,
                                const NBConnection &connection) throw() {
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
                                const NBConnectionVector &connections) throw() {
    bool ok = true;
    for (NBConnectionVector::const_iterator i=connections.begin(); i!=connections.end(); i++) {
        ok &= addToSignalGroup(groupid, *i);
    }
    return ok;
}


void
NBLoadedTLDef::addSignalGroup(const std::string &id) throw() {
    assert(mySignalGroups.find(id)==mySignalGroups.end());
    mySignalGroups[id] = new SignalGroup(id);
}


void
NBLoadedTLDef::addSignalGroupPhaseBegin(const std::string &groupid, SUMOTime time,
                                        TLColor color) throw() {
    assert(mySignalGroups.find(groupid)!=mySignalGroups.end());
    mySignalGroups[groupid]->addPhaseBegin(time, color);
}

void
NBLoadedTLDef::setSignalYellowTimes(const std::string &groupid,
                                    SUMOTime myTRedYellow, SUMOTime myTYellow) throw() {
    assert(mySignalGroups.find(groupid)!=mySignalGroups.end());
    mySignalGroups[groupid]->setYellowTimes(myTRedYellow, myTYellow);
}


void
NBLoadedTLDef::setCycleDuration(unsigned int cycleDur) throw() {
    myCycleDuration = cycleDur;
}


void
NBLoadedTLDef::remapRemoved(NBEdge *removed,
                            const EdgeVector &incoming,
                            const EdgeVector &outgoing) throw() {
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
                              NBEdge *by, int byLane) throw() {
    for (SignalGroupCont::const_iterator i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        SignalGroup *group = (*i).second;
        if (group->containsIncoming(removed)||group->containsOutgoing(removed)) {
            group->remap(removed, removedLane, by, byLane);
        }
    }
}



/****************************************************************************/


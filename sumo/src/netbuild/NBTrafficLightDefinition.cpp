//---------------------------------------------------------------------------//
//                        NBTrafficLightDefinition.cpp -
//  The definition of a traffic light logic
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
// Revision 1.4  2003/06/24 08:21:01  dkrajzew
// some further work on importing traffic lights
//
// Revision 1.3  2003/06/18 11:13:13  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/06/16 08:02:44  dkrajzew
// further work on Vissim-import
//
// Revision 1.1  2003/06/05 11:43:20  dkrajzew
// definition class for traffic lights added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include <utils/convert/ToString.h>
#include "NBTrafficLightDefinition.h"
#include <utils/options/OptionsCont.h>
#include "NBTrafficLightLogicVector.h"
#include "NBTrafficLightDefinition.h"
#include "NBTrafficLightPhases.h"
#include "NBLinkPossibilityMatrix.h"
#include "NBTrafficLightLogic.h"
#include "NBContHelper.h"
#include "NBRequestEdgeLinkIterator.h"
#include "NBLinkCliqueContainer.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * some definitions (debugging only)
 * ======================================================================= */
#define DEBUG_OUT cout


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * NBTrafficLightDefinition::SignalGroup-methods
 * ----------------------------------------------------------------------- */
NBTrafficLightDefinition::SignalGroup::SignalGroup(const std::string &id)
    : Named(id)
{
}

NBTrafficLightDefinition::SignalGroup::~SignalGroup()
{
}

void
NBTrafficLightDefinition::SignalGroup::addConnection(const NBConnection &c)
{
    assert(c.getFromLane()<0||c.getFrom()->getNoLanes()>c.getFromLane());
    myConnections.push_back(c);
}


void
NBTrafficLightDefinition::SignalGroup::addPhaseBegin(double time, TLColor color)
{
    myPhases.push_back(PhaseDef(time, color));
}


void
NBTrafficLightDefinition::SignalGroup::setYellowTimes(double tRedYellow,
                                    double tYellow)
{
    myTRedYellow = tRedYellow;
    myTYellow = tYellow;
}


void
NBTrafficLightDefinition::SignalGroup::sortPhases()
{
    sort(myPhases.begin(), myPhases.end(),
        phase_by_time_sorter());
}


DoubleVector
NBTrafficLightDefinition::SignalGroup::getTimes() const
{
    DoubleVector ret;
    for(GroupsPhases::const_iterator i=myPhases.begin(); i!=myPhases.end(); i++) {
        ret.push_back((*i).myTime);
    }
    return ret;
}


size_t
NBTrafficLightDefinition::SignalGroup::getLinkNo() const
{
    return myConnections.size();
}


bool
NBTrafficLightDefinition::SignalGroup::mayDrive(double time) const
{
	assert(myPhases.size()!=0);
    for(GroupsPhases::const_iterator i2=myPhases.begin(); i2!=myPhases.end(); i2++) {
        cout
            << ((*(i2)).myColor==TLCOLOR_GREEN)
            << endl;
    }
    if(time==60) {
        int bla = 0;
    }
    for(GroupsPhases::const_reverse_iterator i=myPhases.rbegin(); i!=myPhases.rend(); i++) {
        double nextTime = (*i).myTime;
        if(time>=nextTime) {
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
NBTrafficLightDefinition::SignalGroup::mustBrake(double time) const
{
	assert(myPhases.size()!=0);
    for(GroupsPhases::const_iterator i=myPhases.begin(); i!=myPhases.end(); i++) {
        double nextTime = (*i).myTime;
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


bool
NBTrafficLightDefinition::SignalGroup::containsConnection(NBEdge *from, NBEdge *to) const
{
    for(NBConnectionVector::const_iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        if((*i).getFrom()==from&&(*i).getTo()==to) {
            return true;
        }
    }
    return false;

}


const NBConnection &
NBTrafficLightDefinition::SignalGroup::getConnection(size_t pos) const
{
    assert(pos<myConnections.size());
    return myConnections[pos];
}


bool
NBTrafficLightDefinition::SignalGroup::containsIncoming(NBEdge *from) const
{
    for(NBConnectionVector::const_iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        if((*i).getFrom()==from) {
            return true;
        }
    }
    return false;
}


void
NBTrafficLightDefinition::SignalGroup::remapIncoming(NBEdge *which, const EdgeVector &by)
{
    assert(by.size()>0);
    bool changed = true;
    while(changed) {
        changed = false;
        for(NBConnectionVector::iterator i=myConnections.begin(); !changed&&i!=myConnections.end(); i++) {
            if((*i).getFrom()==which) {
                NBConnection conn((*i).getFrom(), (*i).getTo());
                changed = true;
                myConnections.erase(i);
                if(by.size()==0) {
                    return; // !!! (?)
                }
                for(EdgeVector::const_iterator j=by.begin(); j!=by.end(); j++) {
                    NBConnection curr(conn);
                    if(!curr.replaceFrom(which, *j)) {
                        throw 1;
                    }
                    myConnections.push_back(curr);
                }
            }
        }
    }
}


bool
NBTrafficLightDefinition::SignalGroup::containsOutgoing(NBEdge *to) const
{
    for(NBConnectionVector::const_iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        if((*i).getTo()==to) {
            return true;
        }
    }
    return false;
}


void
NBTrafficLightDefinition::SignalGroup::remapOutgoing(NBEdge *which, const EdgeVector &by)
{
    bool changed = true;
    while(changed) {
        changed = false;
        for(NBConnectionVector::iterator i=myConnections.begin(); !changed&&i!=myConnections.end(); i++) {
            if((*i).getTo()==which) {
                NBConnection conn((*i).getFrom(), (*i).getTo());
                changed = true;
                myConnections.erase(i);
                if(by.size()==0) {
                    return; // !!! (?)
                }
                for(EdgeVector::const_iterator j=by.begin(); j!=by.end(); j++) {
                    NBConnection curr(conn);
                    if(!curr.replaceTo(which, *j)) {
                        throw 1;
                    }
                    myConnections.push_back(curr);
                }
            }
        }
    }
}




/* -------------------------------------------------------------------------
 * NBTrafficLightDefinition::Phase-methods
 * ----------------------------------------------------------------------- */
NBTrafficLightDefinition::Phase::Phase(const std::string &id, size_t begin, size_t end)
    : Named(id), myBegin(begin), myEnd(end)
{
}


NBTrafficLightDefinition::Phase::~Phase()
{
}

/*
void
NBTrafficLightDefinition::Phase::addSignalGroupColor(const std::string &signalgroup, TLColor color)
{
    assert(_groupColors.find(signalgroup)==_groupColors.end());
    _groupColors[signalgroup] = color;
}
*/




/* -------------------------------------------------------------------------
 * NBTrafficLightDefinition::Phase-methods
 * ----------------------------------------------------------------------- */
NBTrafficLightDefinition::NBTrafficLightDefinition(const std::string &id,
                                                   const std::vector<NBNode*> &junctions)
    : Named(id), _nodes(junctions)
{
    for(NodeCont::const_iterator i=junctions.begin(); i!=junctions.end(); i++) {
        (*i)->addTrafficLight(this);
    }
}


NBTrafficLightDefinition::NBTrafficLightDefinition(const std::string &id,
                                                   NBNode *junction)
    : Named(id)
{
    addNode(junction);
}


NBTrafficLightDefinition::NBTrafficLightDefinition(const std::string &id)
    : Named(id)
{
}


NBTrafficLightDefinition::~NBTrafficLightDefinition()
{
}


NBTrafficLightLogicVector *
NBTrafficLightDefinition::compute(OptionsCont &oc)
{
    // assign participating nodes to the request
    size_t pos = 0;
    SignalGroupCont::const_iterator m;
    for(m=mySignalGroups.begin(); m!=mySignalGroups.end(); m++) {
        SignalGroup *group = (*m).second;
        size_t linkNo = group->getLinkNo();
        for(size_t j=0; j<linkNo; j++) {
            const NBConnection &conn = group->getConnection(j);
            NBEdge *edge = conn.getFrom();
            NBNode *node = edge->getToNode();
            if(find(_nodes.begin(), _nodes.end(), node)==_nodes.end()) {
                _nodes.push_back(node);
            }
        }
    }

    collectEdges();
    collectLinks();
    if(_incoming.size()==0) {
        return 0;
    }
    size_t breakingTime = computeBrakingTime(oc.getFloat("min-decel"));
    NBTrafficLightLogicVector *logics = mySignalGroups.size()!=0
        ? buildLoadedTrafficLights(breakingTime)
        : buildOwnTrafficLights(breakingTime,
            oc.getBool("all-logics"));
    return logics;
}


size_t
NBTrafficLightDefinition::computeBrakingTime(double minDecel) const
{
    double vmax = NBContHelper::maxSpeed(_incoming);
    return (size_t) (vmax / minDecel);
}



NBTrafficLightLogicVector *
NBTrafficLightDefinition::buildLoadedTrafficLights(size_t breakingTime)
{
    NBTrafficLightDefinition::SignalGroupCont::const_iterator i;
    // sort the phases
    // compute the switching times
    std::set<double> tmpSwitchTimes;
    for(i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        NBTrafficLightDefinition::SignalGroup *group = (*i).second;
        DoubleVector gtimes = group->getTimes();
        for(DoubleVector::const_iterator k=gtimes.begin(); k!=gtimes.end(); k++) {
            tmpSwitchTimes.insert(*k);
        }
        // needed later
        group->sortPhases();
    }
    std::vector<double> switchTimes;
    copy(tmpSwitchTimes.begin(), tmpSwitchTimes.end(),
        back_inserter(switchTimes));
    sort(switchTimes.begin(), switchTimes.end());

/*
    // assign participating nodes to the request
    size_t pos = 0;
    SignalGroupCont::const_iterator m;
    for(m=mySignalGroups.begin(); m!=mySignalGroups.end(); m++) {
        SignalGroup *group = (*m).second;
        size_t linkNo = group->getLinkNo();
        for(size_t j=0; j<linkNo; j++) {
            const NBConnection &conn = group->getConnection(j);
            NBEdge *edge = conn.getFrom();
            NBNode *node = edge->getToNode();
            if(find(_nodes.begin(), _nodes.end(), node)==_nodes.end()) {
                _nodes.push_back(node);
            }
        }
    }
*/
    // count the signals
    size_t noSignals = 0;
    for(i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        noSignals += (*i).second->getLinkNo();
    }

    // build the phases
    NBTrafficLightLogic *logic =
        new NBTrafficLightLogic(getID(), noSignals);
    for(std::vector<double>::iterator l=switchTimes.begin(); l!=switchTimes.end(); l++) {
        // compute the duration of the current phase
        size_t duration;
        if(l!=switchTimes.end()-1) {
            // get from the difference to the next switching time
            duration = (size_t) ((*(l+1)) - (*l));
        } else {
            // get from the differenc to the first switching time
            duration = (size_t) (myCycleDuration - (*l) + *(switchTimes.begin())) ;
        }
        // no information about yellow times will be generated
        std::pair<std::bitset<64>, std::bitset<64> > masks =
            buildPhaseMasks(*l);
        // compute the yellow times first
            // all vehicle which have red, must have yellow first
            // get the drive mask
        std::bitset<64> yellowMask1 = masks.first;
            // invert
        yellowMask1.flip();
            // no one may drive, all have to break
        std::bitset<64> tmpBrake;
        tmpBrake.flip();
        // !!! possibly, not breakingTime should be used
        if(yellowMask1.any()) {
            duration -= breakingTime;
        }
            // add the step itself
        assert(duration>0);
        logic->addStep(duration, masks.first, masks.second, std::bitset<64>());
        if(yellowMask1.any()) {
            logic->addStep(breakingTime, std::bitset<64>(), tmpBrake, yellowMask1);
        }
    }
    // assign the links to the connections
    size_t pos = 0;
    for(SignalGroupCont::const_iterator m=mySignalGroups.begin(); m!=mySignalGroups.end(); m++) {
        SignalGroup *group = (*m).second;
        size_t linkNo = group->getLinkNo();
        for(size_t j=0; j<linkNo; j++) {
            const NBConnection &conn = group->getConnection(j);
            assert(conn.getFromLane()<0||conn.getFrom()->getNoLanes()>conn.getFromLane());
            NBConnection tst(conn);
            if(tst.check()) {
                NBEdge *edge = conn.getFrom();
                edge->setControllingTLInformation(
                    conn.getFromLane(), conn.getTo(), conn.getToLane(),
                    getID(), pos++);
            } else {
                MsgHandler::getWarningInstance()->inform(
                    string("Could not set signal on connection (signal: ")
                    + getID() + string(", group: ") + group->getID()
                    + string(")"));
            }
        }
    }
    // returns the build logic
    NBTrafficLightLogicVector *ret =
        new NBTrafficLightLogicVector(_links);
    ret->add(logic);
    return ret;
}


std::pair<std::bitset<64>, std::bitset<64> >
NBTrafficLightDefinition::buildPhaseMasks(size_t time) const
{
    cout << time << endl;// bla
    // set the masks
    std::bitset<64> driveMask;
    std::bitset<64> brakeMask;
    size_t pos = 0;
    size_t bla = mySignalGroups.size();
    for(SignalGroupCont::const_iterator i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        SignalGroup *group = (*i).second;
        size_t linkNo = group->getLinkNo();
        bool mayDrive = group->mayDrive(time);
        for(size_t j=0; j<linkNo; j++) {
            driveMask[pos] = mayDrive;
            const NBConnection &conn = group->getConnection(j);
            NBConnection assConn(conn);
            if(assConn.check()) {
                brakeMask[pos] = mustBrake(conn.getFrom(), conn.getTo());
                pos++;
            }
        }
    }
    return std::pair<std::bitset<64>, std::bitset<64> >(driveMask, brakeMask);
}


NBTrafficLightLogicVector *
NBTrafficLightDefinition::buildOwnTrafficLights(size_t breakingTime,
                                                bool buildAll) const
{
    bool appendSmallestOnly = true;
    bool skipLarger = true;

    bool joinLaneLinks = false;
    bool removeTurnArounds = true;
    LinkRemovalType removal = LRT_REMOVE_WHEN_NOT_OWN;
    NBTrafficLightLogicVector *logics1 =
        computeTrafficLightLogics(getID(),
            joinLaneLinks, removeTurnArounds, removal,
            appendSmallestOnly, skipLarger, breakingTime);

    if(buildAll) {
        joinLaneLinks = false;
        removeTurnArounds = true;
        removal = LRT_NO_REMOVAL;
        NBTrafficLightLogicVector *logics2 =
            computeTrafficLightLogics(getID(),
                joinLaneLinks, removeTurnArounds, removal,
                appendSmallestOnly, skipLarger, breakingTime);

        joinLaneLinks = false;
        removeTurnArounds = true;
        removal = LRT_REMOVE_ALL_LEFT;
        NBTrafficLightLogicVector *logics3 =
            computeTrafficLightLogics(getID(),
                joinLaneLinks, removeTurnArounds, removal,
                appendSmallestOnly, skipLarger, breakingTime);

        // join build logics
        logics1->add(*logics2);
        logics1->add(*logics3);
        delete logics2;
        delete logics3;
    }
    return logics1;
}


NBTrafficLightLogicVector *
NBTrafficLightDefinition::computeTrafficLightLogics(const std::string &key,
                                     bool joinLaneLinks,
                                     bool removeTurnArounds,
                                     LinkRemovalType removal,
                                     bool appendSmallestOnly,
                                     bool skipLarger,
                                     size_t breakingTime) const
{
    // compute the matrix of possible links x links
    //  (links allowing each other the parallel execution)
    NBLinkPossibilityMatrix *v = getPossibilityMatrix(joinLaneLinks,
        removeTurnArounds, removal);
    // get the number of regarded links
    NBRequestEdgeLinkIterator cei1(this,
        joinLaneLinks, removeTurnArounds, removal);
    size_t maxStromAnz = cei1.getNoValidLinks();

#ifdef TL_DEBUG
    if(maxStromAnz>=10) {
        DEBUG_OUT << _junction->getID() << ":" << maxStromAnz << endl;
    }
#endif

    // compute the link cliquen
    NBLinkCliqueContainer cliquen(v, maxStromAnz);
    // compute the phases
    NBTrafficLightPhases *phases = cliquen.computePhases(v,
        maxStromAnz, appendSmallestOnly, skipLarger);
    // compute the possible logics
    NBTrafficLightLogicVector *logics =
        phases->computeLogics(key, getSizes().second, cei1,
        _links, breakingTime);
    // clean everything
    delete v;
    delete phases;
    return logics;
}


std::vector<std::bitset<64> > *
NBTrafficLightDefinition::getPossibilityMatrix(bool joinLaneLinks,
                                bool removeTurnArounds,
                                LinkRemovalType removalType) const
{
    size_t noEdges = _incoming.size();
    // go through all links
    NBRequestEdgeLinkIterator cei1(this, joinLaneLinks, removeTurnArounds,
        removalType);
    std::vector<std::bitset<64> > *ret =
        new std::vector<std::bitset<64> >(cei1.getNoValidLinks(),
        std::bitset<64>());
    do {
        assert(ret!=0 && cei1.getLinkNumber()<ret->size());
        (*ret)[cei1.getLinkNumber()].set(cei1.getLinkNumber(), 1);
        NBRequestEdgeLinkIterator cei2(cei1);
        if(cei2.pp()) {
            do {
                if(cei1.forbids(cei2)) {
                    assert(ret!=0 && cei1.getLinkNumber()<ret->size());
                    assert(ret!=0 && cei2.getLinkNumber()<ret->size());
                    (*ret)[cei1.getLinkNumber()].set(cei2.getLinkNumber(), 0);
                    (*ret)[cei2.getLinkNumber()].set(cei1.getLinkNumber(), 0);
                } else {
                    (*ret)[cei1.getLinkNumber()].set(cei2.getLinkNumber(), 1);
                    (*ret)[cei2.getLinkNumber()].set(cei1.getLinkNumber(), 1);
                }
            } while(cei2.pp());
        }
    } while(cei1.pp());
    return ret;
}



NBTrafficLightDefinition::SignalGroup *
NBTrafficLightDefinition::findGroup(NBEdge *from, NBEdge *to) const
{
    for(SignalGroupCont::const_iterator i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        if((*i).second->containsConnection(from, to)) {
            return (*i).second;
        }
    }
    return 0;
}



bool
NBTrafficLightDefinition::addToSignalGroup(const std::string &groupid,
                         const NBConnection &connection)
{
    if(mySignalGroups.find(groupid)==mySignalGroups.end()) {
        return false;
    }
    mySignalGroups[groupid]->addConnection(connection);
    return true;
}


bool
NBTrafficLightDefinition::addToSignalGroup(const std::string &groupid,
                         const NBConnectionVector &connections)
{
    bool ok = true;
    for(NBConnectionVector::const_iterator i=connections.begin(); i!=connections.end(); i++) {
        ok &= addToSignalGroup(groupid, *i);
    }
    return ok;
}


void
NBTrafficLightDefinition::addSignalGroup(const std::string &id)
{
    assert(mySignalGroups.find(id)==mySignalGroups.end());
    mySignalGroups[id] = new SignalGroup(id);
}


void
NBTrafficLightDefinition::addSignalGroupPhaseBegin(const std::string &groupid, double time,
                                 TLColor color)
{
    assert(mySignalGroups.find(groupid)!=mySignalGroups.end());
    mySignalGroups[groupid]->addPhaseBegin(time, color);
}

void
NBTrafficLightDefinition::setSignalYellowTimes(const std::string &groupid,
                             double myTRedYellow, double myTYellow)
{
    assert(mySignalGroups.find(groupid)!=mySignalGroups.end());
    mySignalGroups[groupid]->setYellowTimes(myTRedYellow, myTYellow);
}


void
NBTrafficLightDefinition::setCycleDuration(size_t cycleDur)
{
    myCycleDuration = cycleDur;
}


void
NBTrafficLightDefinition::collectEdges()
{
    EdgeVector myOutgoing;
    // collect the edges from the participating nodes
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        const EdgeVector &incoming = (*i)->getIncomingEdges();
        copy(incoming.begin(), incoming.end(), back_inserter(_incoming));
        const EdgeVector &outgoing = (*i)->getOutgoingEdges();
        copy(outgoing.begin(), outgoing.end(), back_inserter(myOutgoing));
    }
    // check which of the edges are completely within the junction
    //  remove these edges from the list of incoming edges
    //  add them to the list of edges lying within the node
    size_t pos = 0;
    while(pos<_incoming.size()) {
        NBEdge *edge = *(_incoming.begin() + pos);
        // an edge lies within the logic if it outgoing as well as incoming
        EdgeVector::iterator j = find(myOutgoing.begin(), myOutgoing.end(), edge);
        if(j!=myOutgoing.end()) {
            _within.push_back(edge);
            _incoming.erase(_incoming.begin() + pos);
//            _outgoing.erase(j);
        } else {
            pos++;
        }
    }
}


void
NBTrafficLightDefinition::collectLinks()
{
    // build the list of links which are controled by the traffic light
    for(EdgeVector::iterator i=_incoming.begin(); i!=_incoming.end(); i++) {
        NBEdge *incoming = *i;
        size_t noLanes = incoming->getNoLanes();
        for(size_t j=0; j<noLanes; j++) {
            const EdgeLaneVector *connected = incoming->getEdgeLanesFromLane(j);
            for(EdgeLaneVector::const_iterator k=connected->begin(); k!=connected->end(); k++) {
                const EdgeLane &el = *k;
                if(el.edge!=0) {
                    _links.push_back(
                            NBConnection(incoming, j, el.edge, el.lane));
                }
            }
        }
    }
    // set the information about the link's positions within the tl into the
    //  edges the links are starting at, respectively
    size_t pos = 0;
    for(NBConnectionVector::iterator j=_links.begin(); j!=_links.end(); j++) {
        const NBConnection &conn = *j;
        NBEdge *edge = conn.getFrom();
        edge->setControllingTLInformation(
            conn.getFromLane(), conn.getTo(), conn.getToLane(),
            getID(), pos++);
    }
}


pair<size_t, size_t>
NBTrafficLightDefinition::getSizes() const
{
    size_t noLanes = 0;
    size_t noLinks = 0;
    for(EdgeVector::const_iterator i=_incoming.begin(); i!=_incoming.end(); i++) {
        size_t noLanesEdge = (*i)->getNoLanes();
        for(size_t j=0; j<noLanesEdge; j++) {
			assert((*i)->getEdgeLanesFromLane(j)->size()!=0);
            noLinks += (*i)->getEdgeLanesFromLane(j)->size();
        }
        noLanes += noLanesEdge;
    }
    return pair<size_t, size_t>(noLanes, noLinks);
}


bool
NBTrafficLightDefinition::isLeftMover(NBEdge *from, NBEdge *to) const
{
    // the destination edge may be unused
    if(to==0) {
        return false;
    }
    // get the node which is holding this connection
    NodeCont::const_iterator i =
        find_if(_nodes.begin(), _nodes.end(),
            NBContHelper::node_with_incoming_finder(from));
    assert(i!=_nodes.end());
    NBNode *node = *i;
    return node->isLeftMover(from, to);
}


bool
NBTrafficLightDefinition::mustBrake(NBEdge *from, NBEdge *to) const
{
    NodeCont::const_iterator i =
        find_if(_nodes.begin(), _nodes.end(),
            NBContHelper::node_with_incoming_finder(from));
    assert(i!=_nodes.end());
    NBNode *node = *i;
    if(!node->hasOutgoing(to)) {
        return true; // !!!
    }
    return node->mustBrake(from, to);
}


bool
NBTrafficLightDefinition::forbidden(NBEdge *from1, NBEdge *to1,
                                    NBEdge *from2, NBEdge *to2) const
{
    NodeCont::const_iterator i =
        find_if(_nodes.begin(), _nodes.end(),
            NBContHelper::node_with_incoming_finder(from1));
    assert(i!=_nodes.end());
    NBNode *node = *i;
    return node->forbidden(from1, to1, from2, to2);
}


void
NBTrafficLightDefinition::addNode(NBNode *node)
{
    _nodes.push_back(node);
    node->addTrafficLight(this);
}



void
NBTrafficLightDefinition::remapRemoved(NBEdge *removed,
                                       const EdgeVector &incoming,
                                       const EdgeVector &outgoing)
{

    for(SignalGroupCont::const_iterator i=mySignalGroups.begin(); i!=mySignalGroups.end(); i++) {
        SignalGroup *group = (*i).second;
        if(group->getID()=="8"&&removed->getID()=="12") {
            int bla = 0;
        }
        if(group->containsIncoming(removed)) {
            group->remapIncoming(removed, incoming);
        }
        if(group->containsOutgoing(removed)) {
            group->remapOutgoing(removed, outgoing);
        }
    }
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBTrafficLightDefinition.icc"
//#endif

// Local Variables:
// mode:C++
// End:



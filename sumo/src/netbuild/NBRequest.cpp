/****************************************************************************/
/// @file    NBRequest.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// This class computes the logic of a junction
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include <vector>
#include <set>
#include <algorithm>
#include <bitset>
#include <sstream>
#include <map>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "NBEdge.h"
#include "NBJunctionLogicCont.h"
#include "NBContHelper.h"
#include "NBTrafficLightLogic.h"
#include "NBTrafficLightLogicCont.h"
#include "NBNode.h"
#include "NBRequest.h"
#include <utils/options/OptionsCont.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
size_t NBRequest::myGoodBuilds = 0;
size_t NBRequest::myNotBuild = 0;


// ===========================================================================
// method definitions
// ===========================================================================
NBRequest::NBRequest(const NBEdgeCont &ec,
                     NBNode *junction, const EdgeVector * const all,
                     const EdgeVector * const incoming,
                     const EdgeVector * const outgoing,
                     const NBConnectionProhibits &loadedProhibits)
        : myJunction(junction),
        myAll(all), myIncoming(incoming), myOutgoing(outgoing) {
    size_t variations = myIncoming->size() * myOutgoing->size();
    // build maps with information which forbidding connection were
    //  computed and what's in there
    myForbids.reserve(variations);
    myDone.reserve(variations);
    for (size_t i=0; i<variations; i++) {
        myForbids.push_back(LinkInfoCont(variations, false));
        myDone.push_back(LinkInfoCont(variations, false));
    }
    // insert loaded prohibits
    for (NBConnectionProhibits::const_iterator j=loadedProhibits.begin(); j!=loadedProhibits.end(); j++) {
        NBConnection prohibited = (*j).first;
        bool ok1 = prohibited.check(ec);
        if (find(myIncoming->begin(), myIncoming->end(), prohibited.getFrom())==myIncoming->end()) {
            ok1 = false;
        }
        if (find(myOutgoing->begin(), myOutgoing->end(), prohibited.getTo())==myOutgoing->end()) {
            ok1 = false;
        }
        int idx1 = 0;
        if (ok1) {
            idx1 = getIndex(prohibited.getFrom(), prohibited.getTo());
            if (idx1<0) {
                ok1 = false;
            }
        }
        const NBConnectionVector &prohibiting = (*j).second;
        for (NBConnectionVector::const_iterator k=prohibiting.begin(); k!=prohibiting.end(); k++) {
            NBConnection sprohibiting = *k;
            bool ok2 = sprohibiting.check(ec);
            if (find(myIncoming->begin(), myIncoming->end(), sprohibiting.getFrom())==myIncoming->end()) {
                ok2 = false;
            }
            if (find(myOutgoing->begin(), myOutgoing->end(), sprohibiting.getTo())==myOutgoing->end()) {
                ok2 = false;
            }
            if (ok1&&ok2) {
                int idx2 = getIndex(sprohibiting.getFrom(), sprohibiting.getTo());
                if (idx2<0) {
                    ok2 = false;
                } else {
                    myForbids[idx2][idx1] = true;
                    myDone[idx2][idx1] = true;
                    myDone[idx1][idx2] = true;
                    myGoodBuilds++;
                }
            } else {
                std::string pfID = prohibited.getFrom()!=0 ? prohibited.getFrom()->getID() : "UNKNOWN";
                std::string ptID = prohibited.getTo()!=0 ? prohibited.getTo()->getID() : "UNKNOWN";
                std::string bfID = sprohibiting.getFrom()!=0 ? sprohibiting.getFrom()->getID() : "UNKNOWN";
                std::string btID = sprohibiting.getTo()!=0 ? sprohibiting.getTo()->getID() : "UNKNOWN";
                WRITE_WARNING("could not prohibit " + pfID + "->" + ptID+ " by "+ bfID + "->" + btID);
                myNotBuild++;
            }
        }
    }
    // ok, check whether someone has prohibited two links vice versa
    //  (this happens also in some Vissim-networks, when edges are joined)
    size_t no = myIncoming->size()*myOutgoing->size();
    for (size_t s1=0; s1<no; s1++) {
        for (size_t s2=s1+1; s2<no; s2++) {
            // not set, yet
            if (!myDone[s1][s2]) {
                continue;
            }
            // check whether both prohibit vice versa
            if (myForbids[s1][s2]&&myForbids[s2][s1]) {
                // mark unset - let our algorithm fix it later
                myDone[s1][s2] = false;
                myDone[s2][s1] = false;
            }
        }
    }
}


NBRequest::~NBRequest() {}


void
NBRequest::buildBitfieldLogic(bool leftHanded, NBJunctionLogicCont &jc,
                              const std::string &key) {
    EdgeVector::const_iterator i, j;
    for (i=myIncoming->begin(); i!=myIncoming->end(); i++) {
        for (j=myOutgoing->begin(); j!=myOutgoing->end(); j++) {
            computeRightOutgoingLinkCrossings(leftHanded, *i, *j);
            computeLeftOutgoingLinkCrossings(leftHanded, *i, *j);
        }
    }
    jc.add(key, bitsetToXML(key));
}


void
NBRequest::computeRightOutgoingLinkCrossings(bool leftHanded, NBEdge *from, NBEdge *to) {
    EdgeVector::const_iterator pfrom = find(myAll->begin(), myAll->end(), from);
    while (*pfrom!=to) {
        NBContHelper::nextCCW(myAll, pfrom);
        if ((*pfrom)->getToNode()==myJunction) {
            EdgeVector::const_iterator pto = find(myAll->begin(), myAll->end(), to);
            while (*pto!=from) {
                if (!((*pto)->getToNode()==myJunction)) {
                    setBlocking(leftHanded, from, to, *pfrom, *pto);
                }
                NBContHelper::nextCCW(myAll, pto);
            }
        }
    }
}


void
NBRequest::computeLeftOutgoingLinkCrossings(bool leftHanded, NBEdge *from, NBEdge *to) {
    EdgeVector::const_iterator pfrom = find(myAll->begin(), myAll->end(), from);
    while (*pfrom!=to) {
        NBContHelper::nextCW(myAll, pfrom);
        if ((*pfrom)->getToNode()==myJunction) {
            EdgeVector::const_iterator pto = find(myAll->begin(), myAll->end(), to);
            while (*pto!=from) {
                if (!((*pto)->getToNode()==myJunction)) {
                    setBlocking(leftHanded, from, to, *pfrom, *pto);
                }
                NBContHelper::nextCW(myAll, pto);
            }
        }
    }
}


void
NBRequest::setBlocking(bool leftHanded,
                       NBEdge *from1, NBEdge *to1,
                       NBEdge *from2, NBEdge *to2) {
    // check whether one of the links has a dead end
    if (to1==0||to2==0) {
        return;
    }
    // get the indices of both links
    int idx1 = getIndex(from1, to1);
    int idx2 = getIndex(from2, to2);
    if (idx1<0||idx2<0) {
        return; // !!! error output? did not happend, yet
    }
    // check whether the link crossing has already been checked
    assert((size_t) idx1<myIncoming->size()*myOutgoing->size());
    if (myDone[idx1][idx2]) {
        return;
    }
    // mark the crossings as done
    myDone[idx1][idx2] = true;
    myDone[idx2][idx1] = true;
    // check if one of the links is a turn; this link is always not priorised
    //  true for right-before-left and priority
    if (from1->isTurningDirectionAt(myJunction, to1)) {
        myForbids[idx2][idx1] = true;
        return;
    }
    if (from2->isTurningDirectionAt(myJunction, to2)) {
        myForbids[idx1][idx2] = true;
        return;
    }

    // check the priorities
    int from1p = from1->getJunctionPriority(myJunction);
    int from2p = from2->getJunctionPriority(myJunction);
    // check if one of the connections is higher priorised when incoming into
    //  the junction, the connection road will yield
    // should be valid for priority junctions only
    if (from1p>from2p) {
        assert(myJunction->getType()!=NODETYPE_RIGHT_BEFORE_LEFT);
        myForbids[idx1][idx2] = true;
        return;
    }
    if (from2p>from1p) {
        assert(myJunction->getType()!=NODETYPE_RIGHT_BEFORE_LEFT);
        myForbids[idx2][idx1] = true;
        return;
    }

    // check whether one of the connections is higher priorised on
    //  the outgoing edge when both roads are high priorised
    //  the connection with the lower priorised outgoing edge will lead
    // should be valid for priority junctions only
    if (from1p>0&&from2p>0) {
        assert(myJunction->getType()!=NODETYPE_RIGHT_BEFORE_LEFT);
        int to1p = to1->getJunctionPriority(myJunction);
        int to2p = to2->getJunctionPriority(myJunction);
        if (to1p>to2p) {
            myForbids[idx1][idx2] = true;
            return;
        }
        if (to2p>to1p) {
            myForbids[idx2][idx1] = true;
            return;
        }
    }

    // compute the yielding due to the right-before-left rule
    // get the position of the incoming lanes in the junction-wheel
    EdgeVector::const_iterator c1 = find(myAll->begin(), myAll->end(), from1);
    NBContHelper::nextCW(myAll, c1);
    // go through next edges clockwise...
    while (*c1!=from1&&*c1!=from2) {
        if (*c1==to2) {
            // if we encounter to2 the second one prohibits the first
            if (!leftHanded) {
                myForbids[idx2][idx1] = true;
            } else {
                myForbids[idx1][idx2] = true;
            }
            return;
        }
        NBContHelper::nextCW(myAll, c1);
    }
    // get the position of the incoming lanes in the junction-wheel
    EdgeVector::const_iterator c2 = find(myAll->begin(), myAll->end(), from2);
    NBContHelper::nextCW(myAll, c2);
    // go through next edges clockwise...
    while (*c2!=from2&&*c2!=from1) {
        if (*c2==to1) {
            // if we encounter to1 the second one prohibits the first
            if (!leftHanded) {
                myForbids[idx1][idx2] = true;
            } else {
                myForbids[idx2][idx1] = true;
            }
            return;
        }
        NBContHelper::nextCW(myAll, c2);
    }
}


size_t
NBRequest::distanceCounterClockwise(NBEdge *from, NBEdge *to) {
    EdgeVector::const_iterator p = find(myAll->begin(), myAll->end(), from);
    size_t ret = 0;
    while (true) {
        ret++;
        if (p==myAll->begin()) {
            p = myAll->end();
        }
        p--;
        if ((*p)==to) {
            return ret;
        }
    }
}


std::string
NBRequest::bitsetToXML(std::string key) {
    std::ostringstream os;
    // reset signalised/non-signalised dependencies
    resetSignalised();
    // init
    std::pair<size_t, size_t> sizes = getSizes();
    size_t absNoLinks = sizes.second;
    size_t absNoLanes = sizes.first;
    assert(absNoLinks>=absNoLanes);
    os << "   <" << SUMOXMLDefinitions::Tags.getString(SUMO_TAG_ROWLOGIC) << " " 
        << SUMOXMLDefinitions::Attrs.getString(SUMO_ATTR_ID) << "=\"" << key << "\" "
        << SUMOXMLDefinitions::Attrs.getString(SUMO_ATTR_REQUESTSIZE) << "=\"" << absNoLinks << "\">" 
        << std::endl;
    int pos = 0;
    // save the logic
    EdgeVector::const_iterator i;
    for (i=myIncoming->begin(); i!=myIncoming->end(); i++) {
        unsigned int noLanes = (*i)->getNoLanes();
        for (unsigned int k=0; k<noLanes; k++) {
            pos = writeLaneResponse(os, *i, k, pos);
        }
    }
    os << "   </" << SUMOXMLDefinitions::Tags.getString(SUMO_TAG_ROWLOGIC) << ">" << std::endl;
    return os.str();
}


void
NBRequest::resetSignalised() {
    // go through possible prohibitions
    for (EdgeVector::const_iterator i11=myIncoming->begin(); i11!=myIncoming->end(); i11++) {
        unsigned int noLanesEdge1 = (*i11)->getNoLanes();
        for (unsigned int j1=0; j1<noLanesEdge1; j1++) {
            std::vector<NBEdge::Connection> el1 = (*i11)->getConnectionsFromLane(j1);
            for (std::vector<NBEdge::Connection>::iterator i12=el1.begin(); i12!=el1.end(); ++i12) {
                int idx1 = getIndex((*i11), (*i12).toEdge);
                if (idx1<0) {
                    continue;
                }
                // go through possibly prohibited
                for (EdgeVector::const_iterator i21=myIncoming->begin(); i21!=myIncoming->end(); i21++) {
                    unsigned int noLanesEdge2 = (*i21)->getNoLanes();
                    for (unsigned int j2=0; j2<noLanesEdge2; j2++) {
                        std::vector<NBEdge::Connection> el2 = (*i21)->getConnectionsFromLane(j2);
                        for (std::vector<NBEdge::Connection>::iterator i22=el2.begin(); i22!=el2.end(); i22++) {
                            int idx2 = getIndex((*i21), (*i22).toEdge);
                            if (idx2<0) {
                                continue;
                            }
                            // check
                            // same incoming connections do not prohibit each other
                            if ((*i11)==(*i21)) {
                                myForbids[idx1][idx2] = false;
                                myForbids[idx2][idx1] = false;
                                continue;
                            }
                            // check other
                            // if both are non-signalised or both are signalised
                            if (((*i12).tlID==""&&(*i22).tlID=="")
                                    ||
                                    ((*i12).tlID!=""&&(*i22).tlID!="")) {
                                // do nothing
                                continue;
                            }
                            // supposing, we don not have to
                            //  brake if we are no foes
                            if (!foes(*i11, (*i12).toEdge, *i21, (*i22).toEdge)) {
                                continue;
                            }
                            // otherwise:
                            //  the non-signalised must break
                            if ((*i12).tlID!="") {
                                myForbids[idx1][idx2] = true;
                                myForbids[idx2][idx1] = false;
                            } else {
                                myForbids[idx1][idx2] = false;
                                myForbids[idx2][idx1] = true;
                            }
                        }
                    }
                }
            }
        }
    }
}


std::pair<unsigned int, unsigned int>
NBRequest::getSizes() const {
    unsigned int noLanes = 0;
    unsigned int noLinks = 0;
    for (EdgeVector::const_iterator i=myIncoming->begin();
            i!=myIncoming->end(); i++) {
        unsigned int noLanesEdge = (*i)->getNoLanes();
        for (unsigned int j=0; j<noLanesEdge; j++) {
            // assert that at least one edge is approached from this lane
            assert((*i)->getConnectionsFromLane(j).size()!=0);
            noLinks += (unsigned int)(*i)->getConnectionsFromLane(j).size();
        }
        noLanes += noLanesEdge;
    }
    return std::make_pair(noLanes, noLinks);
}


bool
NBRequest::foes(const NBEdge * const from1, const NBEdge * const to1,
                const NBEdge * const from2, const NBEdge * const to2) const throw() {
    // unconnected edges do not forbid other edges
    if (to1==0 || to2==0) {
        return false;
    }
    // get the indices
    int idx1 = getIndex(from1, to1);
    int idx2 = getIndex(from2, to2);
    if (idx1<0||idx2<0) {
        return false; // sure? (The connection does not exist within this junction)
    }
    assert((size_t) idx1<myIncoming->size()*myOutgoing->size());
    assert((size_t) idx2<myIncoming->size()*myOutgoing->size());
    return myForbids[idx1][idx2] || myForbids[idx2][idx1];
}


bool
NBRequest::forbids(const NBEdge * const possProhibitorFrom, const NBEdge * const possProhibitorTo,
                   const NBEdge * const possProhibitedFrom, const NBEdge * const possProhibitedTo,
                   bool regardNonSignalisedLowerPriority) const throw() {
    // unconnected edges do not forbid other edges
    if (possProhibitorTo==0 || possProhibitedTo==0) {
        return false;
    }
    // get the indices
    int possProhibitorIdx = getIndex(possProhibitorFrom, possProhibitorTo);
    int possProhibitedIdx = getIndex(possProhibitedFrom, possProhibitedTo);
    if (possProhibitorIdx<0||possProhibitedIdx<0) {
        return false; // sure? (The connection does not exist within this junction)
    }
    assert((size_t) possProhibitorIdx<myIncoming->size()*myOutgoing->size());
    assert((size_t) possProhibitedIdx<myIncoming->size()*myOutgoing->size());
    // check simple right-of-way-rules
    if (!regardNonSignalisedLowerPriority) {
        return myForbids[possProhibitorIdx][possProhibitedIdx];
    }
    // if its not forbidden, report
    if (!myForbids[possProhibitorIdx][possProhibitedIdx]) {
        return false;
    }
    // do not forbid a signalised stream by a non-signalised
    if (!possProhibitorFrom->hasSignalisedConnectionTo(possProhibitorTo)) {
        return false;
    }
    return true;
}


int
NBRequest::writeLaneResponse(std::ostream &os, NBEdge *from,
                             int fromLane, int pos) {
    std::vector<NBEdge::Connection> connected = from->getConnectionsFromLane(fromLane);
    for (std::vector<NBEdge::Connection>::iterator j=connected.begin(); j!=connected.end(); j++) {
        os << "      <logicitem request=\"" << pos++ << "\" response=\"";
        writeResponse(os, from, (*j).toEdge, fromLane, (*j).toLane, (*j).mayDefinitelyPass);
        os << "\" foes=\"";
        writeAreFoes(os, from, (*j).toEdge, myJunction->getCrossingPosition(from, fromLane, (*j).toEdge, (*j).toLane).first>=0);
        os << "\"";
        if (!OptionsCont::getOptions().getBool("no-internal-links")) {
            if (myJunction->getCrossingPosition(from, fromLane, (*j).toEdge, (*j).toLane).first>=0) {
                os << " cont=\"1\"";
            } else {
                os << " cont=\"0\"";
            }
        }
        os << "/>" << std::endl;
    }
    return pos;
}


void
NBRequest::writeResponse(std::ostream &os, const NBEdge * const from, const NBEdge * const to,
                         int fromLane, int toLane, bool mayDefinitelyPass) const throw(IOError) {
    UNUSED_PARAMETER(toLane);
    int idx = 0;
    if (to!=0) {
        idx = getIndex(from, to);
    }
    for (EdgeVector::const_reverse_iterator i=myIncoming->rbegin(); i!=myIncoming->rend(); i++) {
        //const std::vector<NBEdge::Connection> &allConnections = (*i)->getConnections();
        unsigned int noLanes = (*i)->getNoLanes();
        for (int j=noLanes; j-->0;) {
            std::vector<NBEdge::Connection> connected = (*i)->getConnectionsFromLane(j);
            int size = (int) connected.size();
            for (int k=size; k-->0;) {
                if (mayDefinitelyPass) {
                    os << '0';
                } else if (to==0) {
                    // should wait if no further connection!?
                    os << '1';
                } else if ((*i)==from&&fromLane==j) {
                    // do not prohibit a connection by others from same lane
                    os << '0';
                } else {
                    assert(k<(int) connected.size());
                    assert((size_t) idx<myIncoming->size()*myOutgoing->size());
                    assert(connected[k].toEdge==0 || (size_t) getIndex(*i, connected[k].toEdge)<myIncoming->size()*myOutgoing->size());
                    // check whether the connection is prohibited by another one
                    if (connected[k].toEdge!=0 && myForbids[getIndex(*i, connected[k].toEdge)][idx]) {
                        os << '1';
                        continue;
                    }
                    os << '0';
                }
            }
        }
    }
}


void
NBRequest::writeAreFoes(std::ostream &os, NBEdge *from, NBEdge *to, bool isInnerEnd) {
    // remember the case when the lane is a "dead end" in the meaning that
    // vehicles must choose another lane to move over the following
    // junction
    UNUSED_PARAMETER(isInnerEnd);
    int idx = 0;
    if (to!=0) {
        idx = getIndex(from, to);
    }
    // !!! move to forbidden
    for (EdgeVector::const_reverse_iterator i=myIncoming->rbegin();
            i!=myIncoming->rend(); i++) {

        unsigned int noLanes = (*i)->getNoLanes();
        for (unsigned int j=noLanes; j-->0;) {
            std::vector<NBEdge::Connection> connected = (*i)->getConnectionsFromLane(j);
            int size = (int) connected.size();
            for (int k=size; k-->0;) {
                if (to==0) {
                    os << '0';
                } else {
//                    if (foes(from, to, (*i), connected[k].edge) && !isInnerEnd) {
                    if (foes(from, to, (*i), connected[k].toEdge)) {
                        os << '1';
                    } else {
                        os << '0';
                    }
                }
            }
        }
    }
}


int
NBRequest::getIndex(const NBEdge * const from, const NBEdge * const to) const throw() {
    EdgeVector::const_iterator fp = find(myIncoming->begin(), myIncoming->end(), from);
    EdgeVector::const_iterator tp = find(myOutgoing->begin(), myOutgoing->end(), to);
    if (fp==myIncoming->end()||tp==myOutgoing->end()) {
        return -1;
    }
    // compute the index
    return (int)(distance(myIncoming->begin(), fp) * myOutgoing->size() + distance(myOutgoing->begin(), tp));
}


std::ostream &
operator<<(std::ostream &os, const NBRequest &r) {
    size_t variations = r.myIncoming->size() * r.myOutgoing->size();
    for (size_t i=0; i<variations; i++) {
        os << i << ' ';
        for (size_t j=0; j<variations; j++) {
            if (r.myForbids[i][j])
                os << '1';
            else
                os << '0';
        }
        os << std::endl;
    }
    os << std::endl;
    return os;
}


bool
NBRequest::mustBrake(const NBEdge * const from, const NBEdge * const to) const throw() {
    // vehicles which do not have a following lane must always decelerate to the end
    if (to==0) {
        return true;
    }
    // get the indices
    int idx2 = getIndex(from, to);
    if (idx2==-1) {
        return false;
    }
    // go through all (existing) connections;
    //  check whether any of these forbids the one to determine
    assert((size_t) idx2<myIncoming->size()*myOutgoing->size());
    for (size_t idx1=0; idx1<myIncoming->size()*myOutgoing->size(); idx1++) {
        //assert(myDone[idx1][idx2]);
        if (myDone[idx1][idx2]&&myForbids[idx1][idx2]) {
            return true;
        }
    }
    return false;
}


bool
NBRequest::mustBrake(const NBEdge * const possProhibitorFrom, const NBEdge * const possProhibitorTo,
                     const NBEdge * const possProhibitedFrom, const NBEdge * const possProhibitedTo) const throw() {
    // get the indices
    int idx1 = getIndex(possProhibitorFrom, possProhibitorTo);
    int idx2 = getIndex(possProhibitedFrom, possProhibitedTo);
    return (myForbids[idx2][idx1]);
}


void
NBRequest::reportWarnings() {
    // check if any errors occured on build the link prohibitions
    if (myNotBuild!=0) {
        WRITE_WARNING(toString(myNotBuild) + " of " + toString(myNotBuild+myGoodBuilds)+ " prohibitions were not build.");
    }
}



/****************************************************************************/


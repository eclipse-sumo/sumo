/***************************************************************************
                          NBRequest.cpp
			  This class computes the logic of a junction
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.20  2003/10/06 07:46:12  dkrajzew
// further work on vissim import (unsignalised vs. signalised streams modality cleared & lane2lane instead of edge2edge-prohibitions implemented
//
// Revision 1.18  2003/09/30 14:48:52  dkrajzew
// debug work on vissim-junctions
//
// Revision 1.17  2003/09/05 15:16:57  dkrajzew
// umlaute conversion; node geometry computation; internal links computation
//
// Revision 1.16  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of geometry information
//
// Revision 1.15  2003/06/18 11:13:13  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.14  2003/06/05 11:43:35  dkrajzew
// class templates applied; documentation added
//
// Revision 1.13  2003/05/21 15:18:19  dkrajzew
// yellow traffic lights implemented
//
// Revision 1.12  2003/05/20 09:33:48  dkrajzew
// false computation of yielding on lane ends debugged; some debugging on tl-import; further work on vissim-import
//
// Revision 1.11  2003/04/16 10:03:48  dkrajzew
// further work on Vissim-import
//
// Revision 1.10  2003/04/14 08:34:59  dkrajzew
// some further bugs removed
//
// Revision 1.9  2003/04/10 15:45:20  dkrajzew
// some lost changes reapplied
//
// Revision 1.8  2003/04/07 12:15:43  dkrajzew
// first steps towards a junctions geometry; tyellow removed again, traffic lights have yellow times given explicitely, now
//
// Revision 1.7  2003/04/04 07:43:04  dkrajzew
// Yellow phases must be now explicetely given; comments added; order of edge sorting (false lane connections) debugged
//
// Revision 1.6  2003/04/01 15:15:54  dkrajzew
// further work on vissim-import
//
// Revision 1.5  2003/03/17 14:22:33  dkrajzew
// further debug and windows eol removed
//
// Revision 1.4  2003/03/06 17:18:44  dkrajzew
// debugging during vissim implementation
//
// Revision 1.3  2003/03/03 14:59:15  dkrajzew
// debugging; handling of imported traffic light definitions
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.6  2002/06/11 16:00:42  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.5  2002/06/07 14:58:45  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed; Comments improved
//
// Revision 1.4  2002/05/14 04:42:56  dkrajzew
// new computation flow
//
// Revision 1.3  2002/04/26 10:07:12  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.2  2002/04/10 04:52:25  dkrajzew
// False priority of joining lanes removed
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.3  2002/03/22 10:50:03  dkrajzew
// Memory leaks debugging added (MSVC++)
//
// Revision 1.2  2002/03/15 09:19:01  traffic
// False data output patched; Number of lanes added to output; Warnings (conversion, unused variables) patched
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <bitset>
#include <sstream>
#include <map>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include <utils/convert/ToString.h>
#include "NBEdge.h"
#include "NBJunctionLogicCont.h"
#include "NBContHelper.h"
#include "NBTrafficLightLogic.h"
#include "NBTrafficLightLogicCont.h"
#include "NBTrafficLightLogicVector.h"
#include "NBNode.h"
#include "NBRequest.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif




size_t NBRequest::myGoodBuilds = 0;
size_t NBRequest::myNotBuild = 0;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NBRequest::NBRequest(NBNode *junction, const EdgeVector * const all,
                     const EdgeVector * const incoming,
                     const EdgeVector * const outgoing,
                     const NBConnectionProhibits &loadedProhibits)
	: _junction(junction),
    _all(all), _incoming(incoming), _outgoing(outgoing)
{
    size_t variations = _incoming->size() * _outgoing->size();
    _forbids.reserve(variations);
    _done.reserve(variations);
    for(size_t i=0; i<variations; i++) {
        _forbids.push_back(LinkInfoCont(variations, false));
        _done.push_back(LinkInfoCont(variations, false));
    }
    // insert loaded prohibits
    for(NBConnectionProhibits::const_iterator j=loadedProhibits.begin(); j!=loadedProhibits.end(); j++) {
        NBConnection prohibited = (*j).first;
        bool ok1 = prohibited.check();
        if(find(_incoming->begin(), _incoming->end(), prohibited.getFrom())==_incoming->end()) {
            ok1 = false;
        }
        if(find(_outgoing->begin(), _outgoing->end(), prohibited.getTo())==_outgoing->end()) {
            ok1 = false;
        }
        int idx1 = 0;
        if(ok1) {
            idx1 = getIndex(prohibited.getFrom(), prohibited.getTo());
            if(idx1<0) {
                ok1 = false;
            }
        }
        const NBConnectionVector &prohibiting = (*j).second;
        for(NBConnectionVector::const_iterator k=prohibiting.begin(); k!=prohibiting.end(); k++) {
            NBConnection sprohibiting = *k;
	if( (sprohibiting.getFrom()->getID()=="476+434[1]"||prohibited.getFrom()->getID()=="476+434[1]")
		&&
		(sprohibiting.getFrom()->getID()=="4[1]+477"||prohibited.getFrom()->getID()=="4[1]+477")
		&&
		(sprohibiting.getTo()->getID()=="610"&&prohibited.getTo()->getID()=="610") ) {

		int bla = 0;
	}
            bool ok2 = sprohibiting.check();
            if(find(_incoming->begin(), _incoming->end(), sprohibiting.getFrom())==_incoming->end()) {
                ok2 = false;
            }
            if(find(_outgoing->begin(), _outgoing->end(), sprohibiting.getTo())==_outgoing->end()) {
                ok2 = false;
            }
            if(ok1&&ok2) {
                int idx2 = getIndex(sprohibiting.getFrom(), sprohibiting.getTo());
                if(idx2<0) {
                    ok2 = false;
                } else {
                    _forbids[idx2][idx1] = true;
                    _done[idx2][idx1] = true;
                    _done[idx1][idx2] = true;
                    myGoodBuilds++;
                }
            } else {
                string pfID = prohibited.getFrom()!=0 ? prohibited.getFrom()->getID() : "UNKNOWN";
                string ptID = prohibited.getTo()!=0 ? prohibited.getTo()->getID() : "UNKNOWN";
                string bfID = sprohibiting.getFrom()!=0 ? sprohibiting.getFrom()->getID() : "UNKNOWN";
                string btID = sprohibiting.getTo()!=0 ? sprohibiting.getTo()->getID() : "UNKNOWN";
                MsgHandler::getWarningInstance()->inform(
				    string("could not prohibit ")
					+ pfID + string("->") + ptID
					+ string(" by ")
					+ bfID + string("->") + ptID);
                myNotBuild++;
            }
        }
    }
    // ok, check whether someone has prohibited two links vice versa
    //  (this happens also in some Vissim-networks, when edges are joined)
    size_t no = _incoming->size()*_outgoing->size();
    for(size_t s1=0; s1<no; s1++) {
        for(size_t s2=s1+1; s2<no; s2++) {
            // not set, yet
            if(!_done[s1][s2]) {
                continue;
            }
            // check whether both prohibit vice versa
            if(_forbids[s1][s2]&&_forbids[s2][s1]) {
                // mark unset - let our algorithm fix it later
                _done[s1][s2] = false;
                _done[s2][s1] = false;
            }
        }
    }
}


NBRequest::~NBRequest()
{
}


void
NBRequest::buildBitfieldLogic(const std::string &key)
{
    EdgeVector::const_iterator i, j;
    for(i=_incoming->begin(); i!=_incoming->end(); i++) {
        for(j=_outgoing->begin(); j!=_outgoing->end(); j++) {
            computeRightOutgoingLinkCrossings(*i, *j);
            computeLeftOutgoingLinkCrossings(*i, *j);
        }
    }
    NBJunctionLogicCont::add(key, bitsetToXML(key));
}


void
NBRequest::computeRightOutgoingLinkCrossings(NBEdge *from, NBEdge *to)
{
    EdgeVector::const_iterator pfrom = find(_all->begin(), _all->end(), from);
    while(*pfrom!=to) {
        NBContHelper::nextCCW(_all, pfrom);
        if((*pfrom)->getToNode()==_junction) {
            EdgeVector::const_iterator pto =
                find(_all->begin(), _all->end(), to);
            while(*pto!=from) {
                if(!((*pto)->getToNode()==_junction)) {
                    setBlocking(from, to, *pfrom, *pto);
                }
                NBContHelper::nextCCW(_all, pto);
            }
        }
    }
}


void
NBRequest::computeLeftOutgoingLinkCrossings(NBEdge *from, NBEdge *to)
{
    EdgeVector::const_iterator pfrom = find(_all->begin(), _all->end(), from);
    while(*pfrom!=to) {
        NBContHelper::nextCW(_all, pfrom);
        if((*pfrom)->getToNode()==_junction) {
            EdgeVector::const_iterator pto =
                find(_all->begin(), _all->end(), to);
            while(*pto!=from) {
                if(!((*pto)->getToNode()==_junction)) {
                    setBlocking(from, to, *pfrom, *pto);
                }
                NBContHelper::nextCW(_all, pto);
            }
        }
    }
}


void
NBRequest::setBlocking(NBEdge *from1, NBEdge *to1,
                       NBEdge *from2, NBEdge *to2)
{
	if( (from1->getID()=="476+434[1]"||from2->getID()=="476+434[1]")
		&&
		(from1->getID()=="4[1]+477"||from2->getID()=="4[1]+477")
		&&
		(to1->getID()=="610"&&to2->getID()=="610") ) {

		int bla = 0;
	}
	if( (from1->getID()=="231"||from2->getID()=="231")
		&&
		(from1->getID()=="68"||from2->getID()=="68")
		&&
		(to1->getID()=="658"&&to2->getID()=="658") ) {

		int bla = 0;
	}
    // check whether one of the links has a dead end
    if(to1==0||to2==0) {
        return;
    }
    // get the indices of both links
    int idx1 = getIndex(from1, to1);
    int idx2 = getIndex(from2, to2);
    if(idx1<0||idx2<0) {
        return; // !!! error output? did not happend, yet
    }
    // check whether the link crossing has already been checked
    assert(idx1<_incoming->size()*_outgoing->size());
    if(_done[idx1][idx2]) {
        return;
    }
    // mark the crossings as done
    _done[idx1][idx2] = true;
    _done[idx2][idx1] = true;
    // check if one of the links is a turn; this link is always not priorised
    if(from1->isTurningDirection(to1)) {
        _forbids[idx2][idx1] = true;
        return;
    }
    if(from2->isTurningDirection(to2)) {
        _forbids[idx1][idx2] = true;
        return;
    }

    // check the priorities
    int from1p = from1->getJunctionPriority(_junction);
    int from2p = from2->getJunctionPriority(_junction);
    // check if one of the connections is higher priorised when incoming into
    // the junction
    // the connection road will yield
    if(from1p>from2p) {
        _forbids[idx1][idx2] = true;
        return;
    }
    if(from2p>from1p) {
        _forbids[idx2][idx1] = true;
        return;
    }
    // check whether one of the connections is higher priorised on
    // the outgoing edge when both roads are high priorised
    // the connection with the lower priorised outgoing edge will lead
    if(from1p>0&&from2p>0) {
        int to1p = to1->getJunctionPriority(_junction);
        int to2p = to2->getJunctionPriority(_junction);
        if(to1p>to2p) {
            _forbids[idx1][idx2] = true;
            return;
        }
        if(to2p>to1p) {
            _forbids[idx2][idx1] = true;
            return;
        }
    }
    // compute the yielding due to the right-before-left rule
    EdgeVector::const_iterator inIncoming1 =
        find(_incoming->begin(), _incoming->end(), from1);
    EdgeVector::const_iterator inIncoming2 =
        find(_incoming->begin(), _incoming->end(), from2);
        // get the position of the incoming lanes in the junction-wheel
    size_t d1 = distance(_incoming->begin(), inIncoming1);
    size_t d2 = distance(_incoming->begin(), inIncoming2);
        // compute the information whether one of the lanes is right of
        // the other (this will then be priorised)
    size_t du, dg;
    if(d1>d2) {
        du = (_incoming->size() - d1) + d2;
        dg = d1 - d2;
    } else {
        du = d2 - d1;
        dg = d1 + (_incoming->size() - d2);
    }
        // the incoming lanes are opposite
        // check which of them will cross the other due to moving to the left
        // this will be the yielding lane
    if(du==dg) {
        size_t dist1 = distanceCounterClockwise(from1, to1);
        size_t dist2 = distanceCounterClockwise(from2, to2);
        if(dist1<dist2)
            _forbids[idx1][idx2] = true;
        if(dist2<dist1)
            _forbids[idx2][idx1] = true;
        return;
    }
    // connection2 forbids proceeding on connection1
    if(dg<du) {
        _forbids[idx2][idx1] = true;
    }
    // connection1 forbids proceeding on connection2
    if(dg>du) {
        _forbids[idx1][idx2] = true;
    }
}


size_t
NBRequest::distanceCounterClockwise(NBEdge *from, NBEdge *to)
{
    EdgeVector::const_iterator p = find(_all->begin(), _all->end(), from);
    size_t ret = 0;
    while(true) {
        ret++;
        if(p==_all->begin()) {
            p = _all->end();
        }
        p--;
        if((*p)==to) {
            return ret;
        }
    }
}


string
NBRequest::bitsetToXML(string key)
{
    ostringstream os;
    // reset signalised/non-signalised dependencies
    resetSignalised();
    // init
    pair<size_t, size_t> sizes = getSizes();
    size_t absNoLinks = sizes.second;
    size_t absNoLanes = sizes.first;
    assert(absNoLinks>=absNoLanes);
    os << "   <row-logic>" << endl;
    os << "      <key>" << key << "</key>" << endl;
    os << "      <requestsize>" << absNoLinks << "</requestsize>" << endl;
    os << "      <lanenumber>" << absNoLanes << "</lanenumber>" << endl;
    int pos = 0;
    // save the logic
    os << "      <logic>" << endl;
    EdgeVector::const_iterator i;
    for(i=_incoming->begin(); i!=_incoming->end(); i++) {
        size_t noLanes = (*i)->getNoLanes();
        for(size_t k=0; k<noLanes; k++) {
            pos = writeLaneResponse(os, *i, k, pos);
        }
    }
    os << "      </logic>" << endl;
    os << "   </row-logic>" << endl;
    return string(os.str());
}


void
NBRequest::resetSignalised()
{
    // go through possible prohibitions
    for( EdgeVector::const_iterator i11=_incoming->begin(); i11!=_incoming->end(); i11++) {
        size_t noLanesEdge1 = (*i11)->getNoLanes();
        for(size_t j1=0; j1<noLanesEdge1; j1++) {
            const EdgeLaneVector *el1 = (*i11)->getEdgeLanesFromLane(j1);
            for(EdgeLaneVector::const_iterator i12=el1->begin(); i12!=el1->end(); i12++) {
                int idx1 = getIndex((*i11), (*i12).edge);
                if(idx1<0) {
                    continue;
                }
                // go through possibly prohibited
                for( EdgeVector::const_iterator i21=_incoming->begin(); i21!=_incoming->end(); i21++) {
                    int noLanesEdge2 = (*i21)->getNoLanes();
                    for(size_t j2=0; j2<noLanesEdge2; j2++) {
                        const EdgeLaneVector *el2 = (*i21)->getEdgeLanesFromLane(j2);
                        for(EdgeLaneVector::const_iterator i22=el2->begin(); i22!=el2->end(); i22++) {
                            int idx2 = getIndex((*i21), (*i22).edge);
if((*i11)->getID()=="219"&&(*i12).edge->getID()=="10000764"&&(*i21)->getID()=="791"&&(*i22).edge->getID()=="10000764") {
    const EdgeLane &el1 = (*i12);
    const EdgeLane &el2 = (*i22);
    int bla = 0;
}
                            if(idx2<0) {
                                continue;
                            }
                            // check
                            // same incoming connections do not prohibit each other
                            if((*i11)==(*i21)) {
//                                assert(!_forbids[idx1][idx2]&&!_forbids[idx2][idx1]);
                                _forbids[idx1][idx2] = false;
                                _forbids[idx2][idx1] = false;
                                continue;
                            }
                            // check other
                                // if both are non-signalised or both are signalised
                            if( ((*i12).tlID==""&&(*i22).tlID=="")
                                ||
                                ((*i12).tlID!=""&&(*i22).tlID!="") ) {
                                // do nothing
                                continue;
                            }
                            // otherwise:
                            //  the non-signalised must break
                            if((*i12).tlID!="") {
                                _forbids[idx1][idx2] = true;
                                _forbids[idx2][idx1] = false;
                            } else {
                                _forbids[idx1][idx2] = false;
                                _forbids[idx2][idx1] = true;
                            }
                        }
                    }
                }
            }
        }
    }
}


pair<size_t, size_t>
NBRequest::getSizes() const
{
    size_t noLanes = 0;
    size_t noLinks = 0;
    for( EdgeVector::const_iterator i=_incoming->begin();
         i!=_incoming->end(); i++) {
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
NBRequest::foes(NBEdge *from1, NBEdge *to1,
                NBEdge *from2, NBEdge *to2) const
{
    // unconnected edges do not forbid other edges
    if(to1==0 || to2==0) {
        return false;
    }
    // get the indices
    int idx1 = getIndex(from1, to1);
    int idx2 = getIndex(from2, to2);
    if(idx1<0||idx2<0) {
        return false; // sure? (The connection does not exist within this junction)
    }
    assert(idx1<_incoming->size()*_outgoing->size());
    assert(idx2<_incoming->size()*_outgoing->size());
    return _forbids[idx1][idx2] || _forbids[idx2][idx1];
}


bool
NBRequest::forbids(NBEdge *possProhibitorFrom, NBEdge *possProhibitorTo,
				   NBEdge *possProhibitedFrom, NBEdge *possProhibitedTo,
                   bool regardNonSignalisedLowerPriority) const
{
    // unconnected edges do not forbid other edges
    if(possProhibitorTo==0 || possProhibitedTo==0) {
        return false;
    }
    // get the indices
    int possProhibitorIdx = getIndex(possProhibitorFrom, possProhibitorTo);
    int possProhibitedIdx = getIndex(possProhibitedFrom, possProhibitedTo);
    if(possProhibitorIdx<0||possProhibitedIdx<0) {
        return false; // sure? (The connection does not exist within this junction)
    }
    assert(possProhibitorIdx<_incoming->size()*_outgoing->size());
    assert(possProhibitedIdx<_incoming->size()*_outgoing->size());
    // check simple right-of-way-rules
    if(!regardNonSignalisedLowerPriority) {
        return _forbids[possProhibitorIdx][possProhibitedIdx];
    }
    // if its not forbidden, report
    if(!_forbids[possProhibitorIdx][possProhibitedIdx]) {
        return false;
    }
    // do not forbid a signalised stream by a non-signalised
    if(!possProhibitorFrom->hasSignalisedConnectionTo(possProhibitorTo)) {
        return false;
    }
    return true;
}


int
NBRequest::writeLaneResponse(std::ostream &os, NBEdge *from,
                             int fromLane, int pos)
{
    const EdgeLaneVector * const connected =
        from->getEdgeLanesFromLane(fromLane);
    for( EdgeLaneVector::const_iterator j=connected->begin();
                j!=connected->end(); j++) {
        os << "         <logicitem request=\"" << pos++
            << "\" response=\"";
        writeResponse(os, from, (*j).edge, fromLane, (*j).lane);
        os << "\"/>" << endl;
    }
    return pos;
}


void
NBRequest::writeResponse(std::ostream &os, NBEdge *from, NBEdge *to,
                         int fromLane, int toLane)
{
    if(from!=0&&from->getID()=="10001326"&&to!=0&&to->getID()=="13000028") {
        int bla = 0;
    }
    // remember the case when the lane is a "dead end" in the meaning that
    // vehicles must choose another lane to move over the following
    // junction
    int idx = 0;
    if(to!=0) {
        idx = getIndex(from, to);
    }
    // !!! move to forbidden
    for( EdgeVector::const_reverse_iterator i=_incoming->rbegin();
         i!=_incoming->rend(); i++) {
        unsigned int noLanes = (*i)->getNoLanes();
        for(unsigned int j=noLanes; j-->0; ) {
            const EdgeLaneVector *connected = (*i)->getEdgeLanesFromLane(j);
            size_t size = connected->size();
            for(int k=size; k-->0; ) {
                if(to==0) {
                    os << '1';
                } else if((*i)==from&&fromLane==j) {
                    // do not prohibit a connection by others from same lane
                    os << '0';
                } else {
                    assert(connected!=0&&k<connected->size());
                    assert(idx<_incoming->size()*_outgoing->size());
                    assert((*connected)[k].edge==0 || getIndex(*i, (*connected)[k].edge)<_incoming->size()*_outgoing->size());
					// check whether the connection is prohibited by another one
                    if( (*connected)[k].edge!=0 &&
                        _forbids[getIndex(*i, (*connected)[k].edge)][idx] &&
                        toLane == (*connected)[k].lane ) {
                        os << '1';
						continue;
                    }
                    os << '0';
/*	                // if this node is controlled by a traffic light
                    // but this connection is not, this connection may not drive
                    // if the other is a foe
                    const EdgeLane &el = (*connected)[k];
                    if(_junction->getID()=="858") {
                        string fromID = (*i)->getID();
                        string toID = (*connected)[k].edge->getID();
                        if(from->getID()=="1000045+1000043[1]"&&to->getID()=="1000046") {
                            if((*i)->getID()=="1000041+1000040"&&(*connected)[k].edge->getID()=="1000046") {
                                    int bla = 0;
                            }
                        }
                    }
                    if( _junction->isTLControlled() && (*i)!=from) { // !!! (the best way to check this?)
                        if(from->getToNode()->foes(from, to, *i, (*connected)[k].edge)) {
                            os << '1';
                            continue;
                        }
                    }
                    // ok, seems to be priorised
                    os << '0';*/
                }
            }
        }
    }
}


int
NBRequest::getIndex(NBEdge *from, NBEdge *to) const
{
    EdgeVector::const_iterator fp = find(_incoming->begin(),
        _incoming->end(), from);
    EdgeVector::const_iterator tp = find(_outgoing->begin(),
        _outgoing->end(), to);
    if(fp==_incoming->end()||tp==_outgoing->end()) {
        return -1;
    }
    // compute the index
    return distance(
        _incoming->begin(), fp) * _outgoing->size()
        + distance(_outgoing->begin(), tp);
}


std::ostream &operator<<(std::ostream &os, const NBRequest &r) {
    size_t variations = r._incoming->size() * r._outgoing->size();
    for(size_t i=0; i<variations; i++) {
        os << i << ' ';
        for(size_t j=0; j<variations; j++) {
            if(r._forbids[i][j])
                os << '1';
            else
                os << '0';
        }
        os << endl;
    }
    os << endl;
    return os;
}


bool
NBRequest::mustBrake(NBEdge *from, NBEdge *to) const
{
	// vehicles which do not have a following lane must always decelerate to the end
	if(to==0) {
		return true;
	}
    // get the indices
    int idx2 = getIndex(from, to);
    if(idx2==-1) {
        return false;
    }
    assert(idx2<_incoming->size()*_outgoing->size());
	for(size_t idx1=0; idx1<_incoming->size()*_outgoing->size(); idx1++) {
		if(_forbids[idx1][idx2]) {
			return true;
		}
	}
	return false;
}


bool
NBRequest::mustBrake(NBEdge *from1, NBEdge *to1, NBEdge *from2, NBEdge *to2) const
{
    // get the indices
    int idx1 = getIndex(from1, to1);
    int idx2 = getIndex(from2, to2);
    return (_forbids[idx2][idx1]);
}


void
NBRequest::reportWarnings()
{
    // check if any errors occured on build the link prohibitions
    if(myNotBuild!=0) {
        MsgHandler::getWarningInstance()->inform(
            toString<int>(myNotBuild) + string(" of ")
            + toString<int>(myNotBuild+myGoodBuilds)
            + string(" prohibitions were not build."));
    }
}

/*
bool
NBRequest::isLeftMover(const NBTrafficLightDefinition * const request,
                       NBEdge *from, NBEdge *to) const
{
    // the destination edge may be unused
    if(to==0) {
        return false;
    }
    // when the junction has only one incoming edge, there are no
    //  problems caused by left blockings
    if(request->_incoming.size()==1) {
        return false;
    }
    // now again some heuristics...
    //  how to compute whether an edge is going to the left in the meaning,
    //  that it crosses the opposite straight direction?!
    vector<NBEdge*> incoming(request->_incoming);
    sort(incoming.begin(), incoming.end(),
        NBContHelper::edge_opposite_direction_sorter(from));
    NBEdge *opposite = *(incoming.begin());
    assert(opposite!=from);
    EdgeVector::const_iterator i =
        find(request->_all->begin(), request->_all->end(), from);
    i = NBContHelper::nextCW(request->_all, i);
    while(true) {
        if((*i)==opposite) {
            return false;
        }
        if((*i)==to) {
            return true;
        }
        i = NBContHelper::nextCW(request->_all, i);
    }
    return false;
}


*/
/*

NBMMLDirection
NBRequest::getMMLDirection(NBEdge *incoming, NBEdge *outgoing) const
{
    // get the indices of both links
    int idx1 = getIndex(incoming, outgoing);
    int idx2 = getIndex(from2, to2);
    if(idx1<0||idx2<0) {
        return; // !!! error output? did not happend, yet
    }
    // check whether the link crossing has already been checked
    assert(idx1<_incoming->size()*_outgoing->size());
    if(_done[idx1][idx2]) {
        return;
    }
    // mark the crossings as done
    _done[idx1][idx2] = true;
    _done[idx2][idx1] = true;
    // check if one of the links is a turn; this link is always not priorised
    if(from1->isTurningDirection(to1)) {
        _forbids[idx2][idx1] = true;
        return;
    }
    if(from2->isTurningDirection(to2)) {
        _forbids[idx1][idx2] = true;
        return;
    }

    // check the priorities
    int from1p = from1->getJunctionPriority(_junction);
    int from2p = from2->getJunctionPriority(_junction);
    // check if one of the connections is higher priorised when incoming into
    // the junction
    // the connection road will yield
    if(from1p>from2p) {
        _forbids[idx1][idx2] = true;
        return;
    }
    if(from2p>from1p) {
        _forbids[idx2][idx1] = true;
        return;
    }
    // check whether one of the connections is higher priorised on
    // the outgoing edge when both roads are high priorised
    // the connection with the lower priorised outgoing edge will lead
    if(from1p>0&&from2p>0) {
        int to1p = to1->getJunctionPriority(_junction);
        int to2p = to2->getJunctionPriority(_junction);
        if(to1p>to2p) {
            _forbids[idx1][idx2] = true;
            return;
        }
        if(to2p>to1p) {
            _forbids[idx2][idx1] = true;
            return;
        }
    }
    // compute the yielding due to the right-before-left rule
    EdgeVector::const_iterator inIncoming1 =
        find(_incoming->begin(), _incoming->end(), from1);
    EdgeVector::const_iterator inIncoming2 =
        find(_incoming->begin(), _incoming->end(), from2);
        // get the position of the incoming lanes in the junction-wheel
    size_t d1 = distance(_incoming->begin(), inIncoming1);
    size_t d2 = distance(_incoming->begin(), inIncoming2);
        // compute the information whether one of the lanes is right of
        // the other (this will then be priorised)
    size_t du, dg;
    if(d1>d2) {
        du = (_incoming->size() - d1) + d2;
        dg = d1 - d2;
    } else {
        du = d2 - d1;
        dg = d1 + (_incoming->size() - d2);
    }
        // the incoming lanes are opposite
        // check which of them will cross the other due to moving to the left
        // this will be the yielding lane
    if(du==dg) {
        size_t dist1 = distanceCounterClockwise(from1, to1);
        size_t dist2 = distanceCounterClockwise(from2, to2);
        if(dist1<dist2)
            _forbids[idx1][idx2] = true;
        if(dist2<dist1)
            _forbids[idx2][idx1] = true;
        return;
    }
    // connection2 forbids proceeding on connection1
    if(dg<du) {
        _forbids[idx2][idx1] = true;
    }
    // connection1 forbids proceeding on connection2
    if(dg>du) {
        _forbids[idx1][idx2] = true;
    }


}
*/



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBRequest.icc"
//#endif

// Local Variables:
// mode:C++
// End:


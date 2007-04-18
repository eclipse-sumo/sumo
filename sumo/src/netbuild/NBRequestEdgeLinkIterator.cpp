/****************************************************************************/
/// @file    NBRequestEdgeLinkIterator.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// An iterator over all possible links of a junction regarding movement
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

#include <bitset>
#include <vector>
#include <cassert>
#include "NBRequest.h"
#include "NBRequestEdgeLinkIterator.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
NBRequestEdgeLinkIterator::NBRequestEdgeLinkIterator(
    const NBTrafficLightDefinition * const request, bool joinLaneLinks,
    bool removeTurnArounds, NBOwnTLDef::LinkRemovalType removalType)
        :
        _request(request), _linkNumber(0), _validLinks(0), _position(0),
        _joinLaneLinks(joinLaneLinks)
{
    init(request, joinLaneLinks, removeTurnArounds, removalType);
    setValidNonLeft(removeTurnArounds, removalType);
    joinLaneLinksFunc(request->getIncomingEdges(), joinLaneLinks);
    computeValidLinks();
}


NBRequestEdgeLinkIterator::~NBRequestEdgeLinkIterator()
{}


void
NBRequestEdgeLinkIterator::init(
    const NBTrafficLightDefinition * const request, bool /*joinLaneLinks*/,
    bool /*removeTurnArounds*/, NBOwnTLDef::LinkRemovalType /*removalType*/)
{
    // build complete lists first
    const EdgeVector &incoming = request->getIncomingEdges();
    size_t i1;
    for (i1=0; i1<incoming.size(); i1++) {
        assert(i1<incoming.size());
        size_t noLanes = incoming[i1]->getNoLanes();
        for (size_t i2=0; i2<noLanes; i2++) {
            NBEdge *fromEdge = incoming[i1];
            const EdgeLaneVector &approached = fromEdge->getEdgeLanesFromLane(i2);
            for (size_t i3=0; i3<approached.size(); i3++) {
                assert(i3<approached.size());
                NBEdge *toEdge = approached[i3].edge;
                _fromEdges.push_back(fromEdge);
                _fromLanes.push_back(i2);
                _toEdges.push_back(toEdge);
                if (toEdge!=0) {
                    _isLeftMover.push_back(
                        request->isLeftMover(fromEdge, toEdge)
                        ||
                        fromEdge->isTurningDirectionAt(fromEdge->getToNode(), toEdge));

                    _isTurnaround.push_back(
                        fromEdge->isTurningDirectionAt(
                            fromEdge->getToNode(), toEdge));
                } else {
                    _isLeftMover.push_back(true);
                    _isTurnaround.push_back(true);
                }
            }
        }
    }
}


void
NBRequestEdgeLinkIterator::setValidNonLeft(
    bool removeTurnArounds, NBOwnTLDef::LinkRemovalType removalType)
{
    // reparse lists and remove unwanted items
    NBEdge *currentEdge = 0;
    int currentLane = -1;
    for (size_t i1=0; i1<_fromEdges.size()&&i1<64; i1++) { // !!! hell happens when i1>=64
        assert(i1<_fromEdges.size());
        if (currentEdge!=_fromEdges[i1] ||
                valid(i1, removeTurnArounds, removalType)) {

            _validNonLeft.set(i1, 1);
        } else {
            _validNonLeft.set(i1, 0);
        }
        assert(i1<_fromEdges.size());
        currentEdge = _fromEdges[i1];
        assert(i1<_fromEdges.size());
        currentLane = _fromLanes[i1];
    }
}


void
NBRequestEdgeLinkIterator::joinLaneLinksFunc(
    const EdgeVector &incoming, bool joinLaneLinks)
{
    // the set of links to view from the outside stays the same
    //  when the links of a lane shall not be merged
    if (!joinLaneLinks) {
        for (size_t i=0; i<_fromEdges.size()&&i<64; i++) { // !!! hell happens when i>=64
            _valid.set(i, _validNonLeft.test(i));
        }
        return;
    }
    // the set of links to view from the outside will be the
    //  number of lanes
    size_t pos = 0;
    for (size_t i1=0; i1<incoming.size(); i1++) {
        assert(i1<incoming.size());
        size_t noLanes = incoming[i1]->getNoLanes();
        for (size_t i2=0; i2<noLanes; i2++) {
            NBEdge *fromEdge = incoming[i1];
            const EdgeLaneVector &approached = fromEdge->getEdgeLanesFromLane(i2);
            _valid.set(pos++, 1);
            for (size_t i3=1; i3<approached.size(); i3++) {
                _valid.set(pos++, 0);
            }
        }
    }
}


void
NBRequestEdgeLinkIterator::computeValidLinks()
{
    for (size_t i=0; i<_fromEdges.size()&&i<64; i++) { // !!! hell happens when i>=64
        if (_valid.test(i)==1) {
            _positions.push_back(i);
            _validLinks++;
        }
    }
}


bool
NBRequestEdgeLinkIterator::valid(size_t pos,
                                 bool removeTurnArounds,
                                 NBOwnTLDef::LinkRemovalType removalType)
{
    // if only turnaround are not wished
    if (removeTurnArounds && _isTurnaround[pos]) {
        return false;
    }

    // leftmovers shall be kept -> keep
    if (removalType==NBOwnTLDef::LRT_NO_REMOVAL) {
        return true;
    }

    // check if is a potentially unwanted leftmover -> keep if not
    if (!_isLeftMover[pos]) {
        return true;
    }

    // when only left-moving edges shall be removed that do not have
    //  an own lane, check whether the current left-mover has a
    //  non-left mover on the same lane -> refuse if not
    if (removalType==NBOwnTLDef::LRT_REMOVE_WHEN_NOT_OWN) {
        int tmpPos = int(pos) - 1;
        assert(pos<_fromEdges.size());
        assert(pos<_fromLanes.size());
        while (//!!! (tmppos): was : pos>=0 &&
            _fromEdges[pos]==_fromEdges[tmpPos] &&
            _fromLanes[pos]==_fromLanes[tmpPos]) {
            if (!_isLeftMover[tmpPos--]) {
                return false;
            }
        }
    }
    // all other are left-movers (no turnings) with no
    return false;
}


size_t
NBRequestEdgeLinkIterator::getLinkNumber() const
{
    return _linkNumber;
}


NBEdge *
NBRequestEdgeLinkIterator::getFromEdge() const
{
    assert(_position<_fromEdges.size());
    return _fromEdges[_position];
}


NBEdge *
NBRequestEdgeLinkIterator::getToEdge() const
{
    assert(_position<_toEdges.size());
    return _toEdges[_position];
}


bool
NBRequestEdgeLinkIterator::pp()
{
    if (_position>=63) {
        // !!! hell happens when _position >= 64
        return false;
    }
    _position++;
    while (_position<_fromEdges.size() && !_valid.test(_position)) {
        _position++;
    }
    _linkNumber++;
    return _linkNumber < _validLinks;
}


size_t
NBRequestEdgeLinkIterator::getNoValidLinks() const
{
    return _validLinks;
}


size_t
NBRequestEdgeLinkIterator::getNumberOfAssignedLinks(size_t pos) const
{
    // count the number of assigned links
    assert(pos<_positions.size());
    size_t current_pointer = _positions[pos];
    size_t count = 1;
    assert(current_pointer<_fromEdges.size());
    NBEdge *srcEdge = _fromEdges[current_pointer];
    current_pointer++;
    while (current_pointer<_fromEdges.size() &&
            !_valid.test(current_pointer) &&
            _fromEdges[current_pointer] == srcEdge) {
        current_pointer++;
        count++;
    }
    return count;
}

size_t
NBRequestEdgeLinkIterator::getNumberOfAssignedLinks(size_t pos, int /*dummy*/) const
{
    // count the number of assigned links
    assert(pos<_positions.size());
    size_t current_pointer = _positions[pos];
    size_t count = 1;
    assert(current_pointer<_fromEdges.size());
    NBEdge *srcEdge = _fromEdges[current_pointer];
    current_pointer++;
    while (current_pointer<_fromEdges.size() &&
            !_valid.test(current_pointer) &&
            _fromEdges[current_pointer] == srcEdge) {
        current_pointer++;
        count++;
    }
    return count;
}

void
NBRequestEdgeLinkIterator::resetNonLeftMovers(std::bitset<64> &driveMask,
        std::bitset<64> &brakeMask,
        std::bitset<64> &yellowM) const
{
    // get bitset showing left-movers
    std::bitset<64> tmp = _validNonLeft;
    tmp.flip();
    // combine with drive mask
    //  -> only left-movers are set which had to wait
    driveMask &= tmp;

    // everyone has to brake..
    brakeMask.reset();
    brakeMask.flip();
    tmp = driveMask;
    tmp.flip();
    // ...beside the left movers
    brakeMask &= tmp;

    // the left-movers do not have yellow
    yellowM &= tmp;
}


bool
NBRequestEdgeLinkIterator::forbids(
    const NBRequestEdgeLinkIterator &other) const
{
    if (!_joinLaneLinks) {
        assert(_position<_fromEdges.size());
        assert(_position<_toEdges.size());
        return _request->foes(
                   _fromEdges[_position], _toEdges[_position],
                   other.getFromEdge(), other.getToEdge());
    }
    bool forbids = other.internJoinLaneForbids(_fromEdges[_position],
                   _toEdges[_position]);
    size_t position = _position+1;
    while (position<_fromEdges.size() &&
            !_valid.test(position) && _validNonLeft.test(position)) {
        forbids |=
            other.internJoinLaneForbids(_fromEdges[position],
                                        _toEdges[position]);
        position++;
    }
    return forbids;
}


bool
NBRequestEdgeLinkIterator::internJoinLaneForbids(NBEdge *fromEdge,
        NBEdge *toEdge) const
{
    assert(_position<_toEdges.size());
    assert(_position<_fromEdges.size());
    bool forbids = _request->foes(fromEdge, toEdge,
                                  _fromEdges[_position], _toEdges[_position]);
    size_t position = _position + 1;
    while (position<_fromEdges.size() &&
            !_valid.test(position) && _validNonLeft.test(position)) {
        assert(position<_toEdges.size());
        assert(position<_fromEdges.size());
        forbids |= _request->foes(
                       fromEdge, toEdge,
                       _fromEdges[position], _toEdges[position]);
        position++;
    }
    return forbids;
}


bool
NBRequestEdgeLinkIterator::testBrakeMask(bool hasGreen, size_t pos) const
{
    return
        _request->mustBrake(_fromEdges[pos], _toEdges[pos])
        ||
        !hasGreen;
}


bool
NBRequestEdgeLinkIterator::testBrakeMask(size_t pos,
        const std::bitset<64> &driveMask) const
{
    NBEdge *from1 = _fromEdges[pos];
    NBEdge *to1 = _toEdges[pos];
    size_t run = 0;
    for (size_t j=0; j<getNoValidLinks(); j++) {
        size_t noEdges = getNumberOfAssignedLinks(j);
        for (size_t k=0; k<noEdges; k++, run++) {
            if (!driveMask.test(run)) {
                continue;
            }
            NBEdge *from2 = _fromEdges[run];
            if (from1==from2) {
                continue;
            }
            NBEdge *to2 = _toEdges[run];
            if (_request->mustBrake(from1, to1, from2, to2, false)) {
                return true;
            }
        }
    }
    return false;
}


std::ostream &
operator<<(std::ostream &os, const NBRequestEdgeLinkIterator &o)
{
    os << "ValidNonLeft: " << o._validNonLeft << endl;
    os << "Valid: " << o._valid << endl;
    return os;
}



/****************************************************************************/


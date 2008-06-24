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
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
        myRequest(request), myLinkNumber(0), myValidLinks(0), myPosition(0),
        myJoinLaneLinks(joinLaneLinks)
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
            EdgeLaneVector approached = fromEdge->getEdgeLanesFromLane(i2);
            for (size_t i3=0; i3<approached.size(); i3++) {
                assert(i3<approached.size());
                NBEdge *toEdge = approached[i3].edge;
                myFromEdges.push_back(fromEdge);
                myFromLanes.push_back(i2);
                myToEdges.push_back(toEdge);
                if (toEdge!=0) {
                    myIsLeftMover.push_back(
                        request->isLeftMover(fromEdge, toEdge)
                        ||
                        fromEdge->isTurningDirectionAt(fromEdge->getToNode(), toEdge));

                    myIsTurnaround.push_back(
                        fromEdge->isTurningDirectionAt(
                            fromEdge->getToNode(), toEdge));
                } else {
                    myIsLeftMover.push_back(true);
                    myIsTurnaround.push_back(true);
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
    for (size_t i1=0; i1<myFromEdges.size()&&i1<64; i1++) { // !!! hell happens when i1>=64
        assert(i1<myFromEdges.size());
        if (currentEdge!=myFromEdges[i1] ||
                valid(i1, removeTurnArounds, removalType)) {

            myValidNonLeft.set(i1, 1);
        } else {
            myValidNonLeft.set(i1, 0);
        }
        assert(i1<myFromEdges.size());
        currentEdge = myFromEdges[i1];
        assert(i1<myFromEdges.size());
        currentLane = myFromLanes[i1];
    }
}


void
NBRequestEdgeLinkIterator::joinLaneLinksFunc(
    const EdgeVector &incoming, bool joinLaneLinks)
{
    // the set of links to view from the outside stays the same
    //  when the links of a lane shall not be merged
    if (!joinLaneLinks) {
        for (size_t i=0; i<myFromEdges.size()&&i<64; i++) { // !!! hell happens when i>=64
            myValid.set(i, myValidNonLeft.test(i));
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
            EdgeLaneVector approached = fromEdge->getEdgeLanesFromLane(i2);
            myValid.set(pos++, 1);
            for (size_t i3=1; i3<approached.size(); i3++) {
                myValid.set(pos++, 0);
            }
        }
    }
}


void
NBRequestEdgeLinkIterator::computeValidLinks()
{
    for (size_t i=0; i<myFromEdges.size()&&i<64; i++) { // !!! hell happens when i>=64
        if (myValid.test(i)==1) {
            myPositions.push_back(i);
            myValidLinks++;
        }
    }
}


bool
NBRequestEdgeLinkIterator::valid(size_t pos,
                                 bool removeTurnArounds,
                                 NBOwnTLDef::LinkRemovalType removalType)
{
    // if only turnaround are not wished
    if (removeTurnArounds && myIsTurnaround[pos]) {
        return false;
    }

    // leftmovers shall be kept -> keep
    if (removalType==NBOwnTLDef::LRT_NO_REMOVAL) {
        return true;
    }

    // check if is a potentially unwanted leftmover -> keep if not
    if (!myIsLeftMover[pos]) {
        return true;
    }

    // when only left-moving edges shall be removed that do not have
    //  an own lane, check whether the current left-mover has a
    //  non-left mover on the same lane -> refuse if not
    if (removalType==NBOwnTLDef::LRT_REMOVE_WHEN_NOT_OWN) {
        int tmpPos = int(pos) - 1;
        assert(pos<myFromEdges.size());
        assert(pos<myFromLanes.size());
        while (tmpPos>=0 &&//!!! (tmppos): was : pos>=0 &&
                myFromEdges[pos]==myFromEdges[tmpPos] &&
                myFromLanes[pos]==myFromLanes[tmpPos]) {
            if (!myIsLeftMover[tmpPos--]) {
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
    return myLinkNumber;
}


NBEdge *
NBRequestEdgeLinkIterator::getFromEdge() const
{
    assert(myPosition<myFromEdges.size());
    return myFromEdges[myPosition];
}


NBEdge *
NBRequestEdgeLinkIterator::getToEdge() const
{
    assert(myPosition<myToEdges.size());
    return myToEdges[myPosition];
}


bool
NBRequestEdgeLinkIterator::pp()
{
    if (myPosition>=63) {
        // !!! hell happens when myPosition >= 64
        return false;
    }
    myPosition++;
    while (myPosition<myFromEdges.size() && !myValid.test(myPosition)) {
        myPosition++;
    }
    myLinkNumber++;
    return myLinkNumber < myValidLinks;
}


size_t
NBRequestEdgeLinkIterator::getNoValidLinks() const
{
    return myValidLinks;
}


size_t
NBRequestEdgeLinkIterator::getNumberOfAssignedLinks(size_t pos) const
{
    // count the number of assigned links
    assert(pos<myPositions.size());
    size_t current_pointer = myPositions[pos];
    size_t count = 1;
    assert(current_pointer<myFromEdges.size());
    NBEdge *srcEdge = myFromEdges[current_pointer];
    current_pointer++;
    while (current_pointer<myFromEdges.size() &&
            !myValid.test(current_pointer) &&
            myFromEdges[current_pointer] == srcEdge) {
        current_pointer++;
        count++;
    }
    return count;
}

size_t
NBRequestEdgeLinkIterator::getNumberOfAssignedLinks(size_t pos, int /*dummy*/) const
{
    // count the number of assigned links
    assert(pos<myPositions.size());
    size_t current_pointer = myPositions[pos];
    size_t count = 1;
    assert(current_pointer<myFromEdges.size());
    NBEdge *srcEdge = myFromEdges[current_pointer];
    current_pointer++;
    while (current_pointer<myFromEdges.size() &&
            !myValid.test(current_pointer) &&
            myFromEdges[current_pointer] == srcEdge) {
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
    std::bitset<64> tmp = myValidNonLeft;
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
    if (!myJoinLaneLinks) {
        assert(myPosition<myFromEdges.size());
        assert(myPosition<myToEdges.size());
        return myRequest->foes(
                   myFromEdges[myPosition], myToEdges[myPosition],
                   other.getFromEdge(), other.getToEdge());
    }
    bool forbids = other.internJoinLaneForbids(myFromEdges[myPosition],
                   myToEdges[myPosition]);
    size_t position = myPosition+1;
    while (position<myFromEdges.size() &&
            !myValid.test(position) && myValidNonLeft.test(position)) {
        forbids |=
            other.internJoinLaneForbids(myFromEdges[position],
                                        myToEdges[position]);
        position++;
    }
    return forbids;
}


bool
NBRequestEdgeLinkIterator::internJoinLaneForbids(NBEdge *fromEdge,
        NBEdge *toEdge) const
{
    assert(myPosition<myToEdges.size());
    assert(myPosition<myFromEdges.size());
    bool forbids = myRequest->foes(fromEdge, toEdge,
                                   myFromEdges[myPosition], myToEdges[myPosition]);
    size_t position = myPosition + 1;
    while (position<myFromEdges.size() &&
            !myValid.test(position) && myValidNonLeft.test(position)) {
        assert(position<myToEdges.size());
        assert(position<myFromEdges.size());
        forbids |= myRequest->foes(
                       fromEdge, toEdge,
                       myFromEdges[position], myToEdges[position]);
        position++;
    }
    return forbids;
}


bool
NBRequestEdgeLinkIterator::testBrakeMask(bool hasGreen, size_t pos) const
{
    return
        myRequest->mustBrake(myFromEdges[pos], myToEdges[pos])
        ||
        !hasGreen;
}


bool
NBRequestEdgeLinkIterator::testBrakeMask(size_t pos,
        const std::bitset<64> &driveMask) const
{
    NBEdge *from1 = myFromEdges[pos];
    NBEdge *to1 = myToEdges[pos];
    size_t run = 0;
    for (size_t j=0; j<getNoValidLinks(); j++) {
        size_t noEdges = getNumberOfAssignedLinks(j);
        for (size_t k=0; k<noEdges; k++, run++) {
            if (!driveMask.test(run)) {
                continue;
            }
            NBEdge *from2 = myFromEdges[run];
            if (from1==from2) {
                continue;
            }
            NBEdge *to2 = myToEdges[run];
            if (myRequest->mustBrake(from1, to1, from2, to2, false)) {
                return true;
            }
        }
    }
    return false;
}


std::ostream &
operator<<(std::ostream &os, const NBRequestEdgeLinkIterator &o)
{
    os << "ValidNonLeft: " << o.myValidNonLeft << endl;
    os << "Valid: " << o.myValid << endl;
    return os;
}



/****************************************************************************/


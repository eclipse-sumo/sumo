#include <bitset>
#include <vector>
#include <cassert>
#include "NBRequest.h"
#include "NBRequestEdgeLinkIterator.h"



using namespace std;

NBRequestEdgeLinkIterator::NBRequestEdgeLinkIterator(
    const NBRequest * const request, bool joinLaneLinks,
    bool removeTurnArounds, NBRequest::LinkRemovalType removalType)
    : 
    _request(request), _linkNumber(0), _validLinks(0), _position(0), 
    _joinLaneLinks(joinLaneLinks), _outerValidLinks(0)
{
    init(request, joinLaneLinks, removeTurnArounds, removalType);
    setValidNonLeft(removeTurnArounds, removalType);
    joinLaneLinksFunc(request->_incoming, joinLaneLinks);
    computeValidLinks();
}


NBRequestEdgeLinkIterator::~NBRequestEdgeLinkIterator()
{
}


void
NBRequestEdgeLinkIterator::init(
    const NBRequest * const request, bool joinLaneLinks,
    bool removeTurnArounds, NBRequest::LinkRemovalType removalType)
{
    // build complete lists first
    const EdgeCont * const incoming = request->_incoming;
    size_t i1;
    for(i1=0; i1<incoming->size(); i1++) {
        size_t noLanes = (*incoming)[i1]->getNoLanes();
        for(size_t i2=0; i2<noLanes; i2++) {
            NBEdge *fromEdge = (*incoming)[i1];
            const EdgeLaneCont * const approached = 
                fromEdge->getEdgeLanesFromLane(i2);
            for(size_t i3=0; i3<approached->size(); i3++) {
                NBEdge *toEdge = (*approached)[i3].edge;
                _fromEdges.push_back(fromEdge);
                _fromLanes.push_back(i2);
                _toEdges.push_back(toEdge);
                _isLeftMover.push_back(
                    isLeftMover(request, fromEdge, toEdge)
                    ||
                    fromEdge->isTurningDirection(toEdge) );
                _isTurnaround.push_back(fromEdge->isTurningDirection(toEdge));
            }
        }
    }
}


void
NBRequestEdgeLinkIterator::setValidNonLeft(
    bool removeTurnArounds, NBRequest::LinkRemovalType removalType)
{
    // reparse lists and remove unwanted items
    NBEdge *currentEdge = 0;
    int currentLane = -1;
    for(size_t i1=0; i1<_fromEdges.size(); i1++) {
        if( currentEdge!=_fromEdges[i1] || 
//            currentLane!=_fromLanes[i1] ||
            valid(i1, removeTurnArounds, removalType) ) {
            _validNonLeft.set(i1, 1);
        } else {
            _validNonLeft.set(i1, 0);
        }
        currentEdge = _fromEdges[i1];
        currentLane = _fromLanes[i1];
    }
}


void
NBRequestEdgeLinkIterator::joinLaneLinksFunc(
    const EdgeCont * const incoming, bool joinLaneLinks)
{
    // the set of links to view from the outside stays the same 
    //  when the links of a lane shall not be merged
    if(!joinLaneLinks) {
        for(size_t i=0; i<_fromEdges.size(); i++) {
            _valid.set(i, _validNonLeft.test(i));
        }
        return;
    }
    // the set of links to view from the outside will be the 
    //  number of lanes 
    size_t pos = 0;
    for(size_t i1=0; i1<(*incoming).size(); i1++) {
        size_t noLanes = (*incoming)[i1]->getNoLanes();
        for(size_t i2=0; i2<noLanes; i2++) {
            NBEdge *fromEdge = (*incoming)[i1];
            const EdgeLaneCont * const approached = 
                fromEdge->getEdgeLanesFromLane(i2);
            _valid.set(pos++, 1);
            for(size_t i3=1; i3<approached->size(); i3++) {
                _valid.set(pos++, 0);
            }
        }
    }
}


void
NBRequestEdgeLinkIterator::computeValidLinks() {
    for(size_t i=0; i<_fromEdges.size(); i++) {
        if(_valid.test(i)==1) {
            _positions.push_back(i);
            _validLinks++;
        }
    }
}


bool
NBRequestEdgeLinkIterator::valid(size_t pos,
                                 bool removeTurnArounds, 
                                 NBRequest::LinkRemovalType removalType) 
{
    // if only turnaround are not wished
    if(removeTurnArounds && _isTurnaround[pos] ) {
        return false;
    }

    // leftmovers shall be kept -> keep
    if( removalType==NBRequest::LRT_NO_REMOVAL ) {
        return true;
    }

    // check if is a potentially unwanted leftmover -> keep if not
    if( !_isLeftMover[pos] ) {
        return true;
    }

    // when only left-moving edges shall be removed that do not have
    //  an own lane, check whether the current left-mover has a 
    //  non-left mover on the same lane -> refuse if not
    if( removalType==NBRequest::LRT_REMOVE_WHEN_NOT_OWN ) {
        int tmpPos = int(pos) - 1;
        while( pos>=0 &&
               _fromEdges[pos]==_fromEdges[tmpPos] && 
               _fromLanes[pos]==_fromLanes[tmpPos] ) {
            if(!_isLeftMover[tmpPos--]) {
                return false;
            }
        }
    }

    // all other are left-movers (no turnings) with no 
    return false;

/*
    // check whether the next is a left mover from the same edge
        // no further from same edge as the list is over -> unwanted
    if(pos+1==_fromEdges.size()) {
        return false;
    }

        // no further from the same edge -> unwanted
    if(_fromEdges[pos+1]!=_fromEdges[pos]) {
        return false;
    }

    // check whether is left mover and not a turning
    if(_isTurnaround[pos+1]) {
        return false;
    }
    return !_isLeftMover[pos+1];
*/
}


size_t
NBRequestEdgeLinkIterator::getLinkNumber() const
{
    return _linkNumber;
}


NBEdge *
NBRequestEdgeLinkIterator::getFromEdge() const
{
    return _fromEdges[_position];
}


NBEdge *
NBRequestEdgeLinkIterator::getToEdge() const
{
    return _toEdges[_position];
}


bool
NBRequestEdgeLinkIterator::pp()
{
    _position++;
    while( _position<_fromEdges.size() && !_valid.test(_position) ) {
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
    size_t current_pointer = _positions[pos];
    size_t count = 1;
    NBEdge *srcEdge = _fromEdges[current_pointer];
    current_pointer++;
    while( !_valid.test(current_pointer) &&
           _fromEdges[current_pointer] == srcEdge) {
        current_pointer++;
        count++;
    }
    return count;
}


bool
NBRequestEdgeLinkIterator::isLeftMover(const NBRequest * const request,
                                       NBEdge *from, NBEdge *to) const
{
    // the destination edge may be unused
    if(to==0) {
        return false;
    }
    // when the junction has only one incoming edge, there are no
    //  problems caused by left blockings
    if(request->_incoming->size()==1) {
        return false;
    }
    // now again some heuristics...
    //  how to compute whether an edge is goin to the left in the meaning,
    //  that it crosses the opposite straight direction?!
    vector<NBEdge*> incoming(*(request->_incoming));
    sort(incoming.begin(), incoming.end(),
        NBContHelper::edge_opposite_direction_sorter(from));
    NBEdge *opposite = *(incoming.begin());
    assert(opposite!=from);
    EdgeCont::const_iterator i =
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


void
NBRequestEdgeLinkIterator::resetNonLeftMovers(
    std::bitset<64> &driveMask, std::bitset<64> &brakeMask) const
{
    std::bitset<64> tmp = _validNonLeft;
    tmp.flip();
    driveMask &= tmp;
    brakeMask.reset();
    brakeMask.flip();
}


bool
NBRequestEdgeLinkIterator::forbids(
    const NBRequestEdgeLinkIterator &other) const
{
    if(!_joinLaneLinks) {
        return _request->forbidden(
            _fromEdges[_position], _toEdges[_position],
                other.getFromEdge(), other.getToEdge());
    }
    bool forbids = other.internJoinLaneForbids(_fromEdges[_position], 
        _toEdges[_position]);
    size_t position = _position+1;
    while(position<_fromEdges.size() && 
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
    bool forbids = _request->forbidden(fromEdge, toEdge,
        _fromEdges[_position], _toEdges[_position]);
    size_t position = _position + 1;
    while(position<_fromEdges.size() && 
        !_valid.test(position) && _validNonLeft.test(position)) {
        forbids |= _request->forbidden(
            fromEdge, toEdge,
            _fromEdges[position], _toEdges[position]);
        position++;
    }
    return forbids;
}


bool 
NBRequestEdgeLinkIterator::testBrakeMask(int set, size_t pos) const
{
    return set==0 || !_validNonLeft.test(pos);
}


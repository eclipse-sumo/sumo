#ifndef NBRequestEdgeLinkIterator_h
#define NBRequestEdgeLinkIterator_h
//---------------------------------------------------------------------------//
//                        NBRequestEdgeLinkIterator.h -
//  An iterator over all possible links of a junction regarding movement
//      directions (turn-around and left-movers may be left of)
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
// $Log$
// Revision 1.4  2003/03/20 16:23:10  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.3  2003/03/03 14:59:17  dkrajzew
// debugging; handling of imported traffic light definitions
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <bitset>
#include <vector>
#include "NBNode.h"
#include "NBRequest.h"
#include "NBContHelper.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NBRequestEdgeLinkIterator
 * As iteration over all links of a junction isn't that trivial, it is
 * encapsulated within this class.
 * The iteration may be bound to certain types of links, e.g. only links
 * which do not move to the left.
 */
class NBRequestEdgeLinkIterator {
public:
    /// constructor
    NBRequestEdgeLinkIterator(const NBRequest * const request,
        bool joinLanes, bool removeTurnArounds,
        NBRequest::LinkRemovalType removalType);

    /// destructor
    ~NBRequestEdgeLinkIterator();

    /// returns the absolute number of links
    size_t getLinkNumber() const;

    /** returns the junctions incoming edge that is regarded at the current
        position */
    NBEdge *getFromEdge() const;

    /** returns the junctions outgoing edge that is regarded at the current
        position */
    NBEdge *getToEdge() const;

    /// returns the number of relevant links
    size_t getNoValidLinks() const;

    /// returns the number of links that are stored under the given valid link
    size_t getNumberOfAssignedLinks(size_t pos) const;
    size_t getNumberOfAssignedLinks(size_t pos, int dummy) const;

    /// move forward
    bool pp();

    /// sets the non-left movers within the given bitset to zero
    void resetNonLeftMovers(std::bitset<64> &driveMask,
        std::bitset<64> &brakeMask) const;

    /** returns the information whether the link at the current position
        is a foe to the link represented by the given iterator */
    bool forbids(const NBRequestEdgeLinkIterator &other) const;

    /** returns the information whether the vehicle that uses the link at
        the defined position must break */
    bool testBrakeMask(int set, size_t pos) const;

    bool getDriveAllowed(const NBNode::SignalGroupCont &defs,
        double time);
    bool getBrakeNeeded(const NBNode::SignalGroupCont &defs,
        double time);

    friend std::ostream &operator<<(std::ostream os, const NBRequestEdgeLinkIterator &o);

private:
    /// returns the information whether the given link is a left-mover
    bool isLeftMover(const NBRequest * const request,
        NBEdge *from, NBEdge *to) const;

    /// initialises the iterator
    void init(const NBRequest * const request, bool joinLanes,
        bool removeTurnArounds, NBRequest::LinkRemovalType removalType);

    /// joins the links of the lane
    void joinLaneLinksFunc(const EdgeVector * const incoming,
        bool joinLaneLinks);

    /// sets the information whether the link is a valid non-left mover
    void setValidNonLeft(bool removeTurnArounds,
        NBRequest::LinkRemovalType removalType);

    /// computes the information about validity of all links
    void computeValidLinks();

    /// returns the information whether the given link is valid
    bool valid(size_t pos, bool removeTurnArounds,
        NBRequest::LinkRemovalType removalType);

    /// !!!
    bool internJoinLaneForbids(NBEdge *fromEdge, NBEdge *toEdge) const;


private:
    /// the request to use
    const NBRequest * const _request;

    /// the list of edges incoming into the regarded junction
    EdgeVector _fromEdges;

    /// the list of edges outgoing from the regarded junction
    EdgeVector _toEdges;

    /// the number of links
    size_t _linkNumber;

    /// the number of valid links
    size_t _validLinks;

    /// the current position
    size_t _position;

    /// information which links are valid non-left movers
    std::bitset<64> _validNonLeft;

    /// information which links are valid
    std::bitset<64> _valid;

    /// information about valid positions
    std::vector<size_t> _positions;

    /// information which link is a left-mover
    std::vector<bool> _isLeftMover;

    /// information which link is a turnaround
    std::vector<bool> _isTurnaround;

    /// list of lanes to use for incoming edges
    std::vector<int> _fromLanes;

    /// information whether links shall be joined
    bool _joinLaneLinks;

    /// !!!
    size_t _outerValidLinks;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NBRequestEdgeLinkIterator.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


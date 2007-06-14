/****************************************************************************/
/// @file    NBRequestEdgeLinkIterator.h
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
#ifndef NBRequestEdgeLinkIterator_h
#define NBRequestEdgeLinkIterator_h


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
#include "nodes/NBNode.h"
#include "NBOwnTLDef.h"
#include "NBContHelper.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBRequestEdgeLinkIterator
 *
 * NBRequestEdgeLinkIterator
 * As iteration over all links of a junction isn't that trivial, it is
 * encapsulated within this class.
 * The iteration may be bound to certain types of links, e.g. only links
 * which do not move to the left.
 */
class NBRequestEdgeLinkIterator
{
public:
    /// constructor
    NBRequestEdgeLinkIterator(const NBTrafficLightDefinition * const request,
                              bool joinLanes, bool removeTurnArounds,
                              NBOwnTLDef::LinkRemovalType removalType);

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
                            std::bitset<64> &brakeMask, std::bitset<64> &yellowMask) const;

    /** returns the information whether the link at the current position
        is a foe to the link represented by the given iterator */
    bool forbids(const NBRequestEdgeLinkIterator &other) const;

    /** returns the information whether the vehicle that uses the link at
        the defined position must break */
    bool testBrakeMask(bool hasGreen, size_t pos) const;

    bool testBrakeMask(size_t pos, const std::bitset<64> &driveMask) const;

    /*
    bool getDriveAllowed(const NBNode::SignalGroupCont &defs,
        SUMOReal time);
    bool getBrakeNeeded(const NBNode::SignalGroupCont &defs,
        SUMOReal time);
    */
    friend std::ostream &operator<<(std::ostream &os, const NBRequestEdgeLinkIterator &o);

private:
    /// initialises the iterator
    void init(const NBTrafficLightDefinition * const request, bool joinLanes,
              bool removeTurnArounds, NBOwnTLDef::LinkRemovalType removalType);

    /// joins the links of the lane
    void joinLaneLinksFunc(const EdgeVector &incoming,
                           bool joinLaneLinks);

    /// sets the information whether the link is a valid non-left mover
    void setValidNonLeft(bool removeTurnArounds,
                         NBOwnTLDef::LinkRemovalType removalType);

    /// computes the information about validity of all links
    void computeValidLinks();

    /// returns the information whether the given link is valid
    bool valid(size_t pos, bool removeTurnArounds,
               NBOwnTLDef::LinkRemovalType removalType);

    /// !!!
    bool internJoinLaneForbids(NBEdge *fromEdge, NBEdge *toEdge) const;


private:
    /// the request to use
    const NBTrafficLightDefinition * const _request;

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

};


#endif

/****************************************************************************/


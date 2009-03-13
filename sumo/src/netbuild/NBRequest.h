/****************************************************************************/
/// @file    NBRequest.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// This class computes the logic of a junction
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
#ifndef NBRequest_h
#define NBRequest_h


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
#include <map>
#include <bitset>
#include "NBTrafficLightLogicVector.h"
#include "NBConnectionDefs.h"
#include "NBContHelper.h"
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdge;
class NBJunctionTypeIO;
class NBTrafficLightLogic;
class OptionsCont;
class NBTrafficLightDefinition;
class NBEdgeCont;
class NBJunctionLogicCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBRequest
 * Given a special node, this class builds the logic of this (junction)
 * regarding the relationships between the incoming and outgoing edges and
 * their priorities. The junction's logic is saved when it does not yet exist.
 */
class NBRequest {
public:
    /** constructor
        The parameter are the logic's lists of edges (all, incoming only and
        outgoing only edges). By now no further informations are needed to
        describe the junctions. These parameter must not be changed during the
        logic's building */
    NBRequest(const NBEdgeCont &ec,
              NBNode *junction, const EdgeVector * const all,
              const EdgeVector * const incoming,
              const EdgeVector * const outgoing,
              const NBConnectionProhibits &loadedProhibits);

    /** destructor */
    ~NBRequest();

    /** builds the bitset-representation of the logic */
    void buildBitfieldLogic(NBJunctionLogicCont &jc,
                            const std::string &key);

    /** returns the number of the junction's lanes and the number
        of the junction's links in respect */
    std::pair<unsigned int, unsigned int> getSizes() const;

    bool mustBrake(NBEdge *from1, NBEdge *to1,
                   NBEdge *from2, NBEdge *to2) const;

    bool mustBrake(NBEdge *from, NBEdge *to) const; // !!!

    /** returns the information whether the connections from1->to1 and
        from2->to2 are foes */
    bool foes(NBEdge *from1, NBEdge *to1,
              NBEdge *from2, NBEdge *to2) const;

    bool forbids(NBEdge *possProhibitorFrom, NBEdge *possProhibitorTo,
                 NBEdge *possProhibitedFrom, NBEdge *possProhibitedTo,
                 bool regardNonSignalisedLowerPriority) const;

    /// prints the request
    friend std::ostream &operator<<(std::ostream &os, const NBRequest &r);

    /// reports warnings if any occured
    static void reportWarnings();


private:
    /** sets the information that the edge from1->to1 blocks the edge
        from2->to2 (is higher priorised than this) */
    void setBlocking(NBEdge *from1, NBEdge *to1, NBEdge *from2, NBEdge *to2);

    /** returns the XML-representation of the logic as a bitset-logic
        XML representation */
    std::string bitsetToXML(std::string key);

    /** @brief writes the response of a certain lane
        Returns the next link index within the junction */
    int writeLaneResponse(std::ostream &os, NBEdge *from, int lane,
                          int pos);

    /** @brief Writes the response of a certain link
     *
     * For the link (described by the connected edges and lanes), the response in dependence
     *  to all other links of this junction is computed. Herefor, the method
     *  goes through all links of this junction and writes a '0' if the link
     *  is not blocked by the currently investigated one, or '1' if it is.
     *
     * In the case "mayDefinitelyPass" is true, the link will not be disturbed by
     *  any other (special case for on-ramps).
     *
     * @param[in] os The stream to write the information to
     * @param[in] from The link's starting edge
     * @param[in] to The link's destination edge
     * @param[in] fromLane The link's starting lane
     * @param[in] toLane The link's destination lane
     * @param[in] mayDefinitelyPass Whether this link is definitely not disturbed
     * @exception IOError not yet implemented
     */
    void writeResponse(std::ostream &os, const NBEdge * const from, const NBEdge * const to,
                       int fromLane, int toLane, bool mayDefinitelyPass) const throw(IOError);


    /** writes which participating links are foes to the given */
    void writeAreFoes(std::ostream &os, NBEdge *from, NBEdge *to,
                      bool isInnerEnd);


    /** @brief Returns the index to the internal combination container for the given edge combination
     *
     * If one of the edges is not known, -1 is returned.
     *
     * @param[in] from The starting edge (incoming to this logic)
     * @param[in] to The destination edge (outgoing from this logic)
     * @return The index within the internal container
     */
    int getIndex(const NBEdge * const from, const NBEdge * const to) const throw();


    /** returns the distance between the incoming (from) and the outgoing (to)
        edge clockwise in edges */
    size_t distanceCounterClockwise(NBEdge *from, NBEdge *to);

    /** computes the relationships between links outgoing right of the given
        link */
    void computeRightOutgoingLinkCrossings(NBEdge *from, NBEdge *to);

    /** computes the relationships between links outgoing left of the given
        link */
    void computeLeftOutgoingLinkCrossings(NBEdge *from, NBEdge *to);


    void resetSignalised();

private:
    /// the node the request is assigned to
    NBNode *myJunction;

    /** all (icoming and outgoing) of the junctions edges */
    const EdgeVector * const myAll;

    /** edges incoming to the junction */
    const EdgeVector * const myIncoming;

    /** edges outgoing from the junction */
    const EdgeVector * const myOutgoing;

    /** definition of a container to store boolean informations about a link
        into */
    typedef std::vector<bool> LinkInfoCont;

    /** definition of a container for link(edge->edge) X link(edge->edge)
        combinations (size = |myIncoming|*|myOutgoing|) */
    typedef std::vector<LinkInfoCont> CombinationsCont;

    /** a container for approached lanes of a certain edge */
    typedef std::map<NBEdge*, LaneVector> OccupiedLanes;

    /** the link X link blockings */
    CombinationsCont  myForbids;

    /** the link X link is done-checks */
    CombinationsCont  myDone;

private:
    static size_t myGoodBuilds, myNotBuild;

};


#endif

/****************************************************************************/


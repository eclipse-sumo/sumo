#ifndef NBRequest_h
#define NBRequest_h
/***************************************************************************
                          NBRequest.h
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
// $Log$
// Revision 1.14  2003/10/06 07:46:12  dkrajzew
// further work on vissim import (unsignalised vs. signalised streams modality cleared & lane2lane instead of edge2edge-prohibitions implemented
//
// Revision 1.12  2003/09/05 15:16:57  dkrajzew
// umlaute conversion; node geometry computation; internal links computation
//
// Revision 1.11  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of geometry information
//
// Revision 1.10  2003/06/05 11:43:35  dkrajzew
// class templates applied; documentation added
//
// Revision 1.9  2003/05/20 09:33:48  dkrajzew
// false computation of yielding on lane ends debugged; some debugging on tl-import; further work on vissim-import
//
// Revision 1.8  2003/04/16 10:03:48  dkrajzew
// further work on Vissim-import
//
// Revision 1.7  2003/04/14 08:35:00  dkrajzew
// some further bugs removed
//
// Revision 1.5  2003/04/04 07:43:04  dkrajzew
// Yellow phases must be now explicetely given; comments added; order of edge sorting (false lane connections) debugged
//
// Revision 1.4  2003/03/17 14:22:33  dkrajzew
// further debug and windows eol removed
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
// Revision 1.4  2002/06/11 16:00:40  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.3  2002/05/14 04:42:56  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:12  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.3  2002/04/09 12:23:09  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.2  2002/03/15 09:19:44  traffic
// Handling of map logics removed
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
#include <map>
#include <bitset>
#include "NBTrafficLightPhases.h"
#include "NBLinkCliqueContainer.h"
#include "NBTrafficLightLogicVector.h"
#include "NBConnectionDefs.h"
#include "NBContHelper.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBEdge;
class NBJunctionTypeIO;
class NBTrafficLightLogic;
class NBTrafficLightPhases;
class OptionsCont;
class NBTrafficLightDefinition;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NBRequest
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
    NBRequest(NBNode *junction, const EdgeVector * const all,
        const EdgeVector * const incoming,
        const EdgeVector * const outgoing,
        const NBConnectionProhibits &loadedProhibits);

    /** destructor */
    ~NBRequest();

    /** builds the bitset-representation of the logic */
    void buildBitfieldLogic(const std::string &key);

    /** returns the number of the junction's lanes and the number
        of the junction's links in respect */
    std::pair<size_t, size_t> getSizes() const;

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

//    NBMMLDirection getMMLDirection(NBEdge *incoming, NBEdge *outgoing) const;


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

    /** writes the response of a certain link */
    void writeResponse(std::ostream &os, NBEdge *from, NBEdge *to,
        int fromLane, int toLane);

    /** returns the index to the internal combination container */
    int getIndex(NBEdge *from, NBEdge *to) const;

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
    NBNode *_junction;

    /** all (icoming and outgoing) of the junctions edges */
    const EdgeVector * const _all;

    /** edges incoming to the junction */
    const EdgeVector * const _incoming;

    /** edges outgoing from the junction */
    const EdgeVector * const _outgoing;

    /** definition of a container to store boolean informations about a link
        into */
    typedef std::vector<bool> LinkInfoCont;

    /** definition of a container for link(edge->edge) X link(edge->edge)
        combinations (size = |_incoming|*|_outgoing|) */
    typedef std::vector<LinkInfoCont> CombinationsCont;

    /** a container for approached lanes of a certain edge */
    typedef std::map<NBEdge*, LaneVector> OccupiedLanes;

    /** the link X link blockings */
    CombinationsCont  _forbids;

    /** the link X link is done-checks */
    CombinationsCont  _done;

private:
    static size_t myGoodBuilds, myNotBuild;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NBRequest.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

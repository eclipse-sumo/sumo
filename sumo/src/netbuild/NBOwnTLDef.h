/****************************************************************************/
/// @file    NBOwnTLDef.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id$
///
// A traffic light logics which must be computed (only nodes/edges are given)
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
#ifndef NBOwnTLDef_h
#define NBOwnTLDef_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <set>
#include "NBTrafficLightDefinition.h"

class NBNode;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBOwnTLDef
 * @brief A traffic light logics which must be computed (only nodes/edges are given)
 */
class NBOwnTLDef : public NBTrafficLightDefinition
{
public:
    /// Constructor
    NBOwnTLDef(const std::string &id,
               const std::set<NBNode*> &junctions) throw();

    /// Constructor
    NBOwnTLDef(const std::string &id, NBNode *junction) throw();

    /// Constructor
    NBOwnTLDef(const std::string &id, std::string type,
               NBNode *junction) throw();

    /// Constructor
    NBOwnTLDef(const std::string &id) throw();

    /// Destructor
    ~NBOwnTLDef() throw();

    void setParticipantsInformation();

public:
    void remapRemoved(NBEdge *removed,
                      const EdgeVector &incoming, const EdgeVector &outgoing);

protected:
    /// Computes the traffic light logic
    NBTrafficLightLogicVector *myCompute(const NBEdgeCont &ec,
                                         size_t breakingTime,
                                         std::string type);

    /// Collects the nodes participating in this traffic light
    void collectNodes();

    void collectLinks();

    void replaceRemoved(NBEdge *removed, int removedLane,
                        NBEdge *by, int byLane);

    void setTLControllingInformation(const NBEdgeCont &ec) const;


protected:
    int getToPrio(NBEdge *e);
    SUMOReal computeUnblockedWeightedStreamNumber(NBEdge* e1, NBEdge *e2);
    std::pair<NBEdge*, NBEdge*> getBestCombination(const std::vector<NBEdge*> &edges);
    std::pair<NBEdge*, NBEdge*> getBestPair(std::vector<NBEdge*> &incoming);

    /**
     * edge_by_incoming_priority_sorter
     */
    class edge_by_incoming_priority_sorter
    {
    public:
        /// comparing operator
        int operator()(NBEdge *e1, NBEdge *e2) const {
            if (e1->getJunctionPriority(e1->getToNode())!=e2->getJunctionPriority(e2->getToNode())) {
                return e1->getJunctionPriority(e1->getToNode())> e2->getJunctionPriority(e2->getToNode());
            }
            return e1->getID() > e2->getID();
        }
    };

};


#endif

/****************************************************************************/


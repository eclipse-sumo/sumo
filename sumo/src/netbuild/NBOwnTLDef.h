/****************************************************************************/
/// @file    NBOwnTLDef.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id$
///
//	»missingDescription«
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

    /** possible types of removeing a link from regardation during the
        building of the traffic light logic */
    enum LinkRemovalType {
        /// all links will be regarded
        LRT_NO_REMOVAL,
        /** all left-movers which are together with other direction on the same
            lane will be removed */
        LRT_REMOVE_WHEN_NOT_OWN,
        /// remove all left-movers
        LRT_REMOVE_ALL_LEFT
    };

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


private:
    /** compute the traffic light logics for the current node and the
        given settings */
    NBTrafficLightLogicVector *computeTrafficLightLogics(
        const std::string &key, std::string type,
        bool joinLaneLinks, bool removeTurnArounds, LinkRemovalType removal,
        bool appendSmallestOnly, bool skipLarger,
        size_t breakingTime) const;

    /** compute the pases for the current node and the given settings */
    /*    NBTrafficLightPhases * computePhases(bool joinLaneLinks,
            bool removeTurnArounds, LinkRemovalType removal,
            bool appendSmallestOnly, bool skipLarger) const;*/

    /** build the matrix of links that may be used simultaneously */
    NBLinkPossibilityMatrix *getPossibilityMatrix(bool joinLanes,
            bool removeTurnArounds, LinkRemovalType removalType) const;

};


#endif

/****************************************************************************/


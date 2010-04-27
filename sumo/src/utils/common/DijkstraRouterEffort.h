/****************************************************************************/
/// @file    DijkstraRouterEffort.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 25 July 2005
/// @version $Id$
///
// Dijkstra shortest path algorithm using other values
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef DijkstraRouterEffort_h
#define DijkstraRouterEffort_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <functional>
#include <vector>
#include <set>
#include <limits>
#include <algorithm>
#include <utils/common/InstancePool.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include "SUMOAbstractRouter.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class DijkstraRouterEffort
 * @brief Computes the shortest path through a network using the Dijkstra algorithm.
 *
 * The template parameters are:
 * @param E The edge class to use (MSEdge/ROEdge)
 * @param V The vehicle class to use (MSVehicle/ROVehicle)
 * @param PF The prohibition function to use (prohibited_withRestrictions/prohibited_noRestrictions)
 * @param EC The class to retrieve the effort for an edge from
 *
 * The router is edge-based. It must know the number of edges for internal reasons
 *  and whether a missing connection between two given edges (unbuild route) shall
 *  be reported as an error or as a warning.
 *
 */
template<class E, class V, class PF>
class DijkstraRouterEffortBase : public SUMOAbstractRouter<E, V>, public PF {
public:
    /// Constructor
    DijkstraRouterEffortBase(size_t noE, bool unbuildIsWarningOnly)
            : myUnbuildIsWarningOnly(unbuildIsWarningOnly) {
        for (size_t i = 0; i < noE; i++) {
            myEdgeInfos.push_back(EdgeInfo(i));
        }
    }

    /// Destructor
    virtual ~DijkstraRouterEffortBase() { }

    /**
     * @struct EdgeInfo
     * A definition about a route's edge with the effort needed to reach it and
     *  the information about the previous edge.
     */
    class EdgeInfo {
    public:
        /// Constructor
        EdgeInfo() : edge(0), effort(0), leaveTime(0), prev(0) {}

        /// Constructor
        EdgeInfo(size_t id)
                : edge(E::dictionary(id)), effort(0), leaveTime(0), prev(0), visited(false) {}

        /// The current edge
        const E *edge;

        /// Effort to reach the edge
        SUMOReal effort;

        /// The previous edge
        EdgeInfo *prev;

        /// The time the vehicle leaves the edge
        SUMOReal leaveTime;

        /// The previous edge
        bool visited;

    };

    /**
     * @class EdgeInfoByEffortComparator
     * Class to compare (and so sort) nodes by their effort
     */
    class EdgeInfoByEffortComparator {
    public:
        /// Comparing method
        bool operator()(EdgeInfo *nod1, EdgeInfo *nod2) const {
            if (nod1->effort == nod2->effort) {
                return nod1->edge->getNumericalID() > nod2->edge->getNumericalID();
            }
            return nod1->effort>nod2->effort;
        }
    };

    virtual SUMOReal getEffort(const E * const e, const V * const v, SUMOTime t) = 0;
    virtual SUMOReal getTravelTime(const E * const e, const V * const v, SUMOTime t) = 0;


    /** @brief Builds the route between the given edges using the minimum afford at the given time
        The definition of the afford depends on the wished routing scheme */
    virtual void compute(const E *from, const E *to, const V * const vehicle,
                         SUMOTime msTime, std::vector<const E*> &into) {

	    SUMOReal time = (SUMOReal) msTime / 1000.;
        for (typename std::vector<EdgeInfo>::iterator i=myEdgeInfos.begin(); i!=myEdgeInfos.end(); i++) {
            (*i).effort = std::numeric_limits<SUMOReal>::max();
            (*i).visited = false;
        }
        assert(from!=0&&to!=0);
        myFrontierList.clear();
        // add begin node
        EdgeInfo* const fromInfo = &(myEdgeInfos[from->getNumericalID()]);
        fromInfo->effort = 0;
        fromInfo->prev = 0;
        fromInfo->leaveTime = (SUMOReal) time;
        myFrontierList.push_back(fromInfo);
        // loop
        while (!myFrontierList.empty()) {
            // use the node with the minimal length
            EdgeInfo * const minimumInfo = myFrontierList.front();
            const E * const minEdge = minimumInfo->edge;
            pop_heap(myFrontierList.begin(), myFrontierList.end(), myComparator);
            myFrontierList.pop_back();
            // check whether the destination node was already reached
            if (minEdge == to) {
                buildPathFrom(minimumInfo, into);
                return;
            }
            minimumInfo->visited = true;
            const SUMOReal effort = minimumInfo->effort + getEffort(minEdge, vehicle, (SUMOTime) minimumInfo->leaveTime);
            const SUMOReal leaveTime = minimumInfo->leaveTime + getTravelTime(minEdge, vehicle, (SUMOTime)minimumInfo->leaveTime);
            // check all ways from the node with the minimal length
            unsigned int i = 0;
            unsigned int length_size = minEdge->getNoFollowing();
            for (i=0; i<length_size; i++) {
                const E* const follower = minEdge->getFollower(i);
                EdgeInfo* const followerInfo = &(myEdgeInfos[follower->getNumericalID()]);
                // check whether it can be used
                if (PF::operator()(follower, vehicle)) {
                    continue;
                }
                const SUMOReal oldEffort = followerInfo->effort;
                if (!followerInfo->visited && effort < oldEffort) {
                    followerInfo->effort = effort;
                    followerInfo->leaveTime = leaveTime;
                    followerInfo->prev = minimumInfo;
                    if (oldEffort == std::numeric_limits<SUMOReal>::max()) {
                        myFrontierList.push_back(followerInfo);
                        push_heap(myFrontierList.begin(), myFrontierList.end(), myComparator);
                    } else {
                        push_heap(myFrontierList.begin(),
                                  find(myFrontierList.begin(), myFrontierList.end(), followerInfo) + 1,
                                  myComparator);
                    }
                }
            }
        }
        if (!myUnbuildIsWarningOnly) {
            MsgHandler::getErrorInstance()->inform("No connection between '" + from->getID() + "' and '" + to->getID() + "' found.");
        } else {
            WRITE_WARNING("No connection between '" + from->getID() + "' and '" + to->getID() + "' found.");
        }
    }


    SUMOReal recomputeCosts(const std::vector<const E*> &edges, const V * const v, SUMOTime msTime) throw() {
		SUMOReal time = (SUMOReal) msTime / 1000.;
        SUMOReal costs = 0;
        SUMOReal t = (SUMOReal) time;
        for (typename std::vector<const E*>::const_iterator i=edges.begin(); i!=edges.end(); ++i) {
            if (PF::operator()(*i, v)) {
                return -1;
            }
            costs += getEffort(*i, v, (SUMOTime) t);
            t += getTravelTime(*i, v, (SUMOTime) t);
        }
        return costs;
    }

public:
    /// Builds the path from marked edges
    void buildPathFrom(EdgeInfo *rbegin, std::vector<const E *> &edges) {
        std::deque<const E*> tmp;
        while (rbegin!=0) {
            tmp.push_front((E *) rbegin->edge); // !!!
            rbegin = rbegin->prev;
        }
        std::copy(tmp.begin(), tmp.end(), std::back_inserter(edges));
    }

protected:
    /// The container of edge information
    std::vector<EdgeInfo> myEdgeInfos;

    /// A container for reusage of the min edge heap
    std::vector<EdgeInfo*> myFrontierList;

    EdgeInfoByEffortComparator myComparator;

    bool myUnbuildIsWarningOnly;

};


template<class E, class V, class PF, class EC>
class DijkstraRouterEffort_ByProxi : public DijkstraRouterEffortBase<E, V, PF> {
public:
    /// Type of the function that is used to retrieve the edge effort.
    typedef SUMOReal(EC::* Operation)(const E * const, const V * const, SUMOTime) const;

    DijkstraRouterEffort_ByProxi(size_t noE, bool unbuildIsWarningOnly, EC* receiver, Operation effortOperation, Operation ttOperation)
            : DijkstraRouterEffortBase<E, V, PF>(noE, unbuildIsWarningOnly),
            myReceiver(receiver), myEffortOperation(effortOperation), myTTOperation(ttOperation) {}

    inline SUMOReal getEffort(const E * const e, const V * const v, SUMOTime t) {
        return (myReceiver->*myEffortOperation)(e, v, t);
    }

    inline SUMOReal getTravelTime(const E * const e, const V * const v, SUMOTime t) {
        return (myReceiver->*myTTOperation)(e, v, t);
    }

private:
    /// @brief The object the action is directed to.
    EC* myReceiver;

    /// @brief The object's operation to perform for obtaining the effort
    Operation myEffortOperation;

    /// @brief The object's operation to perform for obtaining the travel time
    Operation myTTOperation;

};


template<class E, class V, class PF>
class DijkstraRouterEffort_Direct : public DijkstraRouterEffortBase<E, V, PF> {
public:
    /// Type of the function that is used to retrieve the edge effort.
    typedef SUMOReal(E::* Operation)(const V * const, SUMOTime) const;

    DijkstraRouterEffort_Direct(size_t noE, bool unbuildIsWarningOnly, Operation effortOperation, Operation ttOperation)
            : DijkstraRouterEffortBase<E, V, PF>(noE, unbuildIsWarningOnly),
            myEffortOperation(effortOperation), myTTOperation(ttOperation) {}

    inline SUMOReal getEffort(const E * const e, const V * const v, SUMOTime t) {
        return (e->*myEffortOperation)(v, t);
    }

    inline SUMOReal getTravelTime(const E * const e, const V * const v, SUMOTime t) {
        return (e->*myTTOperation)(v, t);
    }

private:
    /// @brief The object's operation to perform for obtaining the effort
    Operation myEffortOperation;

    /// @brief The object's operation to perform for obtaining the travel time
    Operation myTTOperation;


};


#endif

/****************************************************************************/


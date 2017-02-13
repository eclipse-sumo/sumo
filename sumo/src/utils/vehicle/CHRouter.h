/****************************************************************************/
/// @file    CHRouter.h
/// @author  Jakob Erdmann
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    February 2012
/// @version $Id$
///
// Shortest Path search using a Contraction Hierarchy
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef CHRouter_h
#define CHRouter_h


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
#include <iterator>
#include <utils/common/SysUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include <utils/vehicle/SUMOAbstractRouter.h>
#include "CHBuilder.h"

//#define CHRouter_DEBUG_QUERY
//#define CHRouter_DEBUG_QUERY_PERF

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CHRouter
 * @brief Computes the shortest path through a contracted network
 *
 * The template parameters are:
 * @param E The edge class to use (MSEdge/ROEdge)
 * @param V The vehicle class to use (MSVehicle/ROVehicle)
 * @param PF The prohibition function to use (prohibited_withPermissions/noProhibitions)
 *
 * The router is edge-based. It must know the number of edges for internal reasons
 *  and whether a missing connection between two given edges (unbuild route) shall
 *  be reported as an error or as a warning.
 *
 */
template<class E, class V, class PF>
class CHRouter: public SUMOAbstractRouter<E, V>, public PF {

public:
    /// Type of the function that is used to retrieve the edge effort.
    typedef SUMOReal(* Operation)(const E* const, const V* const, SUMOReal);

    /**
     * @struct EdgeInfo
     * A definition about a route's edge with the effort needed to reach it and
     *  the information about the previous edge.
     */
    class EdgeInfo {
    public:
        /// Constructor
        EdgeInfo(const E* e) :
            edge(e),
            traveltime(std::numeric_limits<SUMOReal>::max()),
            prev(0),
            visited(false) {
        }

        /// The current edge
        const E* edge;

        /// Effort to reach the edge
        SUMOReal traveltime;

        /// The previous edge
        EdgeInfo* prev;

        /// Whether the shortest path to this edge is already found
        bool visited;

        inline void reset() {
            traveltime = std::numeric_limits<SUMOReal>::max();
            visited = false;
        }
    };


    /// A meeting point of the two search scopes
    typedef std::pair<const EdgeInfo*, const EdgeInfo*> Meeting;

    /**
     * @class Unidirectional
     * class for searching in one direction
     */
    class Unidirectional: public PF {
    public:
        /// @brief Constructor
        Unidirectional(const std::vector<E*>& edges, bool forward):
            myAmForward(forward),
            myVehicle(0) {
            for (typename std::vector<E*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
                myEdgeInfos.push_back(EdgeInfo(*i));
            }
        }

        inline bool found(const E* edge) const {
            return myFound.count(edge) > 0;
        }

        inline EdgeInfo* getEdgeInfo(const E* const edge) {
            return &(myEdgeInfos[edge->getNumericalID()]);
        }

        inline const EdgeInfo* getEdgeInfo(const E* const edge) const {
            return &(myEdgeInfos[edge->getNumericalID()]);
        }

        /**
         * @class EdgeInfoByEffortComparator
         * Class to compare (and so sort) nodes by their effort
         */
        class EdgeInfoByTTComparator {
        public:
            /// Comparing method
            bool operator()(const EdgeInfo* nod1, const EdgeInfo* nod2) const {
                if (nod1->traveltime == nod2->traveltime) {
                    return nod1->edge->getNumericalID() > nod2->edge->getNumericalID();
                }
                return nod1->traveltime > nod2->traveltime;
            }
        };


        void init(const E* const start, const V* const vehicle) {
            assert(vehicle != 0);
            // all EdgeInfos touched in the previous query are either in myFrontier or myFound: clean those up
            for (typename std::vector<EdgeInfo*>::iterator i = myFrontier.begin(); i != myFrontier.end(); i++) {
                (*i)->reset();
            }
            myFrontier.clear();
            for (typename std::set<const E*>::const_iterator i = myFound.begin(); i != myFound.end(); i++) {
                getEdgeInfo(*i)->reset();
            }
            myFound.clear();
            myVehicle = vehicle;
            EdgeInfo* startInfo = getEdgeInfo(start);
            startInfo->traveltime = 0;
            startInfo->prev = 0;
            myFrontier.push_back(startInfo);
        }


        typedef std::vector<typename CHBuilder<E, V>::Connection> ConnectionVector;
        /** @brief explore on element from the frontier,update minTTSeen and meeting
         * if an EdgeInfo found by the otherSearch is encountered
         * returns whether stepping should continue
         */
        bool step(const std::vector<ConnectionVector>& uplinks, const Unidirectional& otherSearch, SUMOReal& minTTSeen, Meeting& meeting) {
            // pop the node with the minimal length
            EdgeInfo* const minimumInfo = myFrontier.front();
            pop_heap(myFrontier.begin(), myFrontier.end(), myComparator);
            myFrontier.pop_back();
            // check for a meeting with the other search
            const E* const minEdge = minimumInfo->edge;
#ifdef CHRouter_DEBUG_QUERY
            std::cout << "DEBUG: " << (myAmForward ? "Forward" : "Backward") << " hit '" << minEdge->getID() << "' Q: ";
            for (typename std::vector<EdgeInfo*>::iterator it = myFrontier.begin(); it != myFrontier.end(); it++) {
                std::cout << (*it)->traveltime << "," << (*it)->edge->getID() << " ";
            }
            std::cout << "\n";
#endif
            if (otherSearch.found(minEdge)) {
                const EdgeInfo* const otherInfo = otherSearch.getEdgeInfo(minEdge);
                const SUMOReal ttSeen = minimumInfo->traveltime + otherInfo->traveltime;
#ifdef CHRouter_DEBUG_QUERY
                std::cout << "DEBUG: " << (myAmForward ? "Forward" : "Backward") << "-Search hit other search at '" << minEdge->getID() << "', tt: " << ttSeen << " \n";
#endif
                if (ttSeen < minTTSeen) {
                    minTTSeen = ttSeen;
                    if (myAmForward) {
                        meeting.first = minimumInfo;
                        meeting.second = otherInfo;
                    } else {
                        meeting.first = otherInfo;
                        meeting.second = minimumInfo;
                    }
                }
            }
            // prepare next steps
            minimumInfo->visited = true;
            // XXX we only need to keep found elements if they have a higher rank than the lowest rank in the other search queue
            myFound.insert(minimumInfo->edge);
            const ConnectionVector& upward = uplinks[minEdge->getNumericalID()];
            for (typename ConnectionVector::const_iterator it = upward.begin(); it != upward.end(); it++) {
                EdgeInfo* upwardInfo = &myEdgeInfos[it->target];
                const SUMOReal traveltime = minimumInfo->traveltime + it->cost;
                const SUMOVehicleClass svc = myVehicle->getVClass();
                // check whether it can be used
                if ((it->permissions & svc) != svc) {
                    continue;
                }
                const SUMOReal oldTraveltime = upwardInfo->traveltime;
                if (!upwardInfo->visited && traveltime < oldTraveltime) {
                    upwardInfo->traveltime = traveltime;
                    upwardInfo->prev = minimumInfo;
                    if (oldTraveltime == std::numeric_limits<SUMOReal>::max()) {
                        myFrontier.push_back(upwardInfo);
                        push_heap(myFrontier.begin(), myFrontier.end(), myComparator);
                    } else {
                        push_heap(myFrontier.begin(),
                                  find(myFrontier.begin(), myFrontier.end(), upwardInfo) + 1,
                                  myComparator);
                    }
                }
            }
            // @note: this effectively does a full dijkstra search.
            // the effort compared to the naive stopping criterion is thus
            // quadrupled. We could implement a better stopping criterion (Holte)
            // However since the search shall take place in a contracted graph
            // it probably does not matter
            return !myFrontier.empty() && myFrontier.front()->traveltime < minTTSeen;
        }

    private:
        /// @brief the role of this search
        bool myAmForward;
        /// @brief the min edge heap
        std::vector<EdgeInfo*> myFrontier;
        /// @brief the set of visited (settled) Edges
        std::set<const E*> myFound;
        /// @brief The container of edge information
        std::vector<EdgeInfo> myEdgeInfos;

        EdgeInfoByTTComparator myComparator;

        const V* myVehicle;

    };

    /** @brief Constructor
     * @param[in] validatePermissions Whether a multi-permission hierarchy shall be built
     *            If set to false, the net is pruned in synchronize() and the
     *            hierarchy is tailored to the svc
     */
    CHRouter(const std::vector<E*>& edges, bool unbuildIsWarning, Operation operation,
             const SUMOVehicleClass svc,
             SUMOTime weightPeriod,
             bool validatePermissions):
        SUMOAbstractRouter<E, V>(operation, "CHRouter"),
        myEdges(edges),
        myErrorMsgHandler(unbuildIsWarning ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()),
        myForwardSearch(edges, true),
        myBackwardSearch(edges, false),
        myHierarchyBuilder(new CHBuilder<E, V>(edges, unbuildIsWarning, svc, validatePermissions)),
        myHierarchy(0),
        myWeightPeriod(weightPeriod),
        myValidUntil(0),
        mySVC(svc) {
    }

    /** @brief Cloning constructor
     */
    CHRouter(const std::vector<E*>& edges, bool unbuildIsWarning, Operation operation,
             const SUMOVehicleClass svc,
             SUMOTime weightPeriod,
             const typename CHBuilder<E, V>::Hierarchy* hierarchy) :
        SUMOAbstractRouter<E, V>(operation, "CHRouter"),
        myEdges(edges),
        myErrorMsgHandler(unbuildIsWarning ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()),
        myForwardSearch(edges, true),
        myBackwardSearch(edges, false),
        myHierarchyBuilder(0),
        myHierarchy(hierarchy),
        myWeightPeriod(weightPeriod),
        myValidUntil(0),
        mySVC(svc) {
    }

    /// Destructor
    virtual ~CHRouter() {
        if (myHierarchyBuilder != 0) {
            delete myHierarchy;
            delete myHierarchyBuilder;
        }
    }


    virtual SUMOAbstractRouter<E, V>* clone() {
        WRITE_MESSAGE("Cloning Contraction Hierarchy for " + SumoVehicleClassStrings.getString(mySVC) + " and time " + time2string(myValidUntil) + ".");
        CHRouter<E, V, PF>* clone = new CHRouter<E, V, PF>(myEdges, myErrorMsgHandler == MsgHandler::getWarningInstance(), this->myOperation,
                mySVC, myWeightPeriod, myHierarchy);
        clone->myValidUntil = myValidUntil;
        return clone;
    }

    /** @brief Builds the route between the given edges using the minimum traveltime in the contracted graph
     * @note: since the contracted graph is static (weights averaged over time)
     * the computed routes only approximated shortest paths in the real graph
     * */
    virtual bool compute(const E* from, const E* to, const V* const vehicle,
                         SUMOTime msTime, std::vector<const E*>& into) {
        assert(from != 0 && to != 0);
        // assert(myHierarchyBuilder.mySPTree->validatePermissions() || vehicle->getVClass() == mySVC || mySVC == SVC_IGNORING);
        // do we need to rebuild the hierarchy?
        if (msTime >= myValidUntil) {
            while (msTime >= myValidUntil) {
                myValidUntil += myWeightPeriod;
            }
            buildContractionHierarchy(myValidUntil - myWeightPeriod, vehicle);
        }
        // ready for routing
        this->startQuery();
        myForwardSearch.init(from, vehicle);
        myBackwardSearch.init(to, vehicle);
        SUMOReal minTTSeen = std::numeric_limits<SUMOReal>::max();
        Meeting meeting(static_cast<EdgeInfo*>(0), static_cast<EdgeInfo*>(0));
        bool continueForward = true;
        bool continueBackward = true;
        int num_visited_fw = 0;
        int num_visited_bw = 0;
        bool result = true;
        while (continueForward || continueBackward) {
            if (continueForward) {
                continueForward = myForwardSearch.step(myHierarchy->forwardUplinks, myBackwardSearch, minTTSeen, meeting);
                num_visited_fw += 1;
            }
            if (continueBackward) {
                continueBackward = myBackwardSearch.step(myHierarchy->backwardUplinks, myForwardSearch, minTTSeen, meeting);
                num_visited_bw += 1;
            }
        }
        if (minTTSeen < std::numeric_limits<SUMOReal>::max()) {
            buildPathFromMeeting(meeting, into);
        } else {
            myErrorMsgHandler->inform("No connection between edge '" + from->getID() + "' and edge '" + to->getID() + "' found.");
            result = false;
        }
#ifdef CHRouter_DEBUG_QUERY_PERF
        std::cout << "visited " << num_visited_fw + num_visited_bw << " edges (" << num_visited_fw << "," << num_visited_bw << ") ,final path length: " + toString(into.size()) + ")\n";
#endif
        this->endQuery(num_visited_bw + num_visited_fw);
        return result;
    }


    SUMOReal recomputeCosts(const std::vector<const E*>& edges, const V* const v, SUMOTime msTime) const {
        const SUMOReal time = STEPS2TIME(msTime);
        SUMOReal costs = 0;
        for (typename std::vector<const E*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            if (PF::operator()(*i, v)) {
                return -1;
            }
            costs += this->getEffort(*i, v, time + costs);
        }
        return costs;
    }

    /// normal routing methods

    /// Builds the path from marked edges
    void buildPathFromMeeting(Meeting meeting, std::vector<const E*>& into) const {
        std::deque<const E*> tmp;
        const EdgeInfo* backtrack = meeting.first;
        while (backtrack != 0) {
            tmp.push_front((E*) backtrack->edge);  // !!!
            backtrack = backtrack->prev;
        }
        backtrack = meeting.second->prev; // don't use central edge twice
        while (backtrack != 0) {
            tmp.push_back((E*) backtrack->edge);  // !!!
            backtrack = backtrack->prev;
        }
        // expand shortcuts
        const E* prev = 0;
        while (!tmp.empty()) {
            const E* cur = tmp.front();
            tmp.pop_front();
            if (prev == 0) {
                into.push_back(cur);
                prev = cur;
            } else {
                const E* via = getVia(prev, cur);
                if (via == 0) {
                    into.push_back(cur);
                    prev = cur;
                } else {
                    tmp.push_front(cur);
                    tmp.push_front(via);
                }
            }
        }
    }

    void buildContractionHierarchy(SUMOTime time, const V* const vehicle) {
        if (myHierarchyBuilder != 0) {
            delete myHierarchy;
            myHierarchy = myHierarchyBuilder->buildContractionHierarchy(time, vehicle, this);
        }
        // declare new validUntil (prevent overflow)
        if (myWeightPeriod < std::numeric_limits<int>::max()) {
            myValidUntil = time + myWeightPeriod;
        } else {
            myValidUntil = myWeightPeriod;
        }
    }

private:
    // retrieve the via edge for a shortcut
    const E* getVia(const E* forwardFrom, const E* forwardTo) const {
        typename CHBuilder<E, V>::ConstEdgePair forward(forwardFrom, forwardTo);
        typename CHBuilder<E, V>::ShortcutVia::const_iterator it = myHierarchy->shortcuts.find(forward);
        if (it != myHierarchy->shortcuts.end()) {
            return it->second;
        } else {
            return 0;
        }
    }


private:
    /// @brief all edges with numerical ids
    const std::vector<E*>& myEdges;

    /// @brief the handler for routing errors
    MsgHandler* const myErrorMsgHandler;

    /// @brief the unidirectional search queues
    Unidirectional myForwardSearch;
    Unidirectional myBackwardSearch;

    CHBuilder<E, V>* myHierarchyBuilder;
    const typename CHBuilder<E, V>::Hierarchy* myHierarchy;

    /// @brief the validity duration of one weight interval
    const SUMOTime myWeightPeriod;

    /// @brief the validity duration of the current hierarchy (exclusive)
    SUMOTime myValidUntil;

    /// @brief the permissions for which the hierarchy was constructed
    const SUMOVehicleClass mySVC;
};


#endif

/****************************************************************************/


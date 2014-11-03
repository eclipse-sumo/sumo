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
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include "SPTree.h"

//#define CHRouter_DEBUG_QUERY
//#define CHRouter_DEBUG_QUERY_PERF
//#define CHRouter_DEBUG_CONTRACTION
//#define CHRouter_DEBUG_CONTRACTION_WITNESSES
//#define CHRouter_DEBUG_CONTRACTION_QUEUE
//#define CHRouter_DEBUG_CONTRACTION_DEGREE
//#define CHRouter_DEBUG_WEIGHTS

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
 * @param PF The prohibition function to use (prohibited_withRestrictions/prohibited_noRestrictions)
 *
 * The router is edge-based. It must know the number of edges for internal reasons
 *  and whether a missing connection between two given edges (unbuild route) shall
 *  be reported as an error or as a warning.
 *
 */
template<class E, class V, class PF>
class CHRouter: public SUMOAbstractRouter<E, V>, public PF {

public:
    class EdgeInfo;

    /// Type of the function that is used to retrieve the edge effort.
    typedef SUMOReal(* Operation)(const E* const, const V* const, SUMOReal);

    /// A meeting point of the two search scopes
    typedef std::pair<const EdgeInfo*, const EdgeInfo*> Meeting;

    /// A set of (found) Edges
    typedef std::set<const E*> EdgeSet;

    /// The found route (used as output parameter)
    typedef std::vector<const E*> Result;

    /// @brief Forward/backward connection with associated forward/backward cost
    // forward connections are used only in forward search
    // backward connections are used only in backwards search
    class Connection {
    public:
        Connection(EdgeInfo* t, SUMOReal c, SVCPermissions p): target(t), cost(c), permissions(p) {}
        EdgeInfo* target;
        SUMOReal cost;
        SVCPermissions permissions;
    };

    /**
     * @struct EdgeInfo
     * A definition about a route's edge with the effort needed to reach it and
     *  the information about the previous edge.
     */
    class EdgeInfo {
    public:
        /// Constructor
        EdgeInfo(size_t id) :
            edge(E::dictionary(id)),
            traveltime(std::numeric_limits<SUMOReal>::max()),
            prev(0),
            visited(false)
        {}

        /// The current edge
        const E* edge;

        /// Effort to reach the edge
        SUMOReal traveltime;

        /// The previous edge
        EdgeInfo* prev;

        /// Whether the shortest path to this edge is already found
        bool visited;

        /// Connections to higher ranked nodes
        std::vector<Connection> upward;

        /// the contraction rank (higher means more important)
        int rank;

        inline void reset() {
            traveltime = std::numeric_limits<SUMOReal>::max();
            visited = false;
        }
    };


    /**
     * @class Unidirectional
     * class for searching in one direction
     */
    class Unidirectional: public PF {
    public:
        /// @brief Constructor
        Unidirectional(size_t numEdges, bool forward):
            myAmForward(forward),
            myVehicle(0) {
            for (size_t i = 0; i < numEdges; i++) {
                myEdgeInfos.push_back(EdgeInfo(i));
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
            for (typename EdgeSet::iterator i = myFound.begin(); i != myFound.end(); i++) {
                getEdgeInfo(*i)->reset();
            }
            myFound.clear();
            myVehicle = vehicle;
            EdgeInfo* startInfo = getEdgeInfo(start);
            startInfo->traveltime = 0;
            startInfo->prev = 0;
            myFrontier.push_back(startInfo);
        }


        /** @brief explore on element from the frontier,update minTTSeen and meeting
         * if an EdgeInfo found by the otherSearch is encountered
         * returns whether stepping should continue
         */
        bool step(const Unidirectional& otherSearch, SUMOReal& minTTSeen, Meeting& meeting) {
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
            for (typename std::vector<Connection>::iterator it = minimumInfo->upward.begin(); it != minimumInfo->upward.end(); it++) {
                EdgeInfo* upwardInfo = it->target;
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


        // reset state before rebuilding the contraction hierarchy
        void reset() {
            for (typename std::vector<EdgeInfo>::iterator it = myEdgeInfos.begin(); it != myEdgeInfos.end(); ++it) {
                it->upward.clear();
            }
        }

    private:
        /// @brief the role of this search
        bool myAmForward;
        /// @brief the min edge heap
        std::vector<EdgeInfo*> myFrontier;
        /// @brief the set of visited (settled) Edges
        EdgeSet myFound;
        /// @brief The container of edge information
        std::vector<EdgeInfo> myEdgeInfos;

        EdgeInfoByTTComparator myComparator;

        const V* myVehicle;

    };

    class CHInfo;

    /// @brief Forward/backward connection with associated FORWARD cost
    class CHConnection {
    public:
        CHConnection(CHInfo* t, SUMOReal c, SVCPermissions p, int u):
            target(t), cost(c), permissions(p), underlying(u) {}
        CHInfo* target;
        SUMOReal cost;
        SVCPermissions permissions;
        /// the number of connections underlying this connection
        int underlying;
    };

    typedef std::vector<CHConnection> CHConnections;
    typedef std::pair<const CHConnection*, const CHConnection*> CHConnectionPair;
    typedef std::vector<CHConnectionPair> CHConnectionPairs;

    /** @brief Constructor
     * @param[in] validatePermissions Whether a multi-permission hierarchy shall be built
     *            If set to false, the net is pruned in synchronize() and the
     *            hierarchy is tailored to the vClass of the defaultVehicle
     * @note: defaultVehicle is not transient and must be kept after constructor finishes
     */
    CHRouter(size_t numEdges, bool unbuildIsWarning, Operation operation,
             const SUMOVehicleClass svc,
             SUMOTime weightPeriod,
             bool validatePermissions):
        SUMOAbstractRouter<E, V>(operation, "CHRouter"),
        myErrorMsgHandler(unbuildIsWarning ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()),
        myForwardSearch(numEdges, true),
        myBackwardSearch(numEdges, false),
        mySPTree(new SPTree<CHInfo, CHConnection>(4, validatePermissions)),
        myWeightPeriod(weightPeriod),
        myValidUntil(0),
        mySVC(svc),
        myUpdateCount(0) {
        for (size_t i = 0; i < numEdges; i++) {
            myCHInfos.push_back(CHInfo(i));
        }
    }

    /// Destructor
    virtual ~CHRouter() {
        delete mySPTree;
    }


    virtual SUMOAbstractRouter<E, V>* clone() const {
        return new CHRouter<E, V, PF>(myCHInfos.size(), myErrorMsgHandler == MsgHandler::getWarningInstance(), this->myOperation,
                                      mySVC, myWeightPeriod, mySPTree->validatePermissions());
    }

    /** @brief Builds the route between the given edges using the minimum traveltime in the contracted graph
     * @note: since the contracted graph is static (weights averaged over time)
     * the computed routes only approximated shortest paths in the real graph
     * */
    virtual void compute(const E* from, const E* to, const V* const vehicle,
                         SUMOTime msTime, Result& into) {
        assert(from != 0 && to != 0);
        assert(mySPTree->validatePermissions() || vehicle->getVClass() == mySVC || mySVC == SVC_IGNORING);
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
        while (continueForward || continueBackward) {
            if (continueForward) {
                continueForward = myForwardSearch.step(myBackwardSearch, minTTSeen, meeting);
                num_visited_fw += 1;
            }
            if (continueBackward) {
                continueBackward = myBackwardSearch.step(myForwardSearch, minTTSeen, meeting);
                num_visited_bw += 1;
            }
        }
        if (minTTSeen < std::numeric_limits<SUMOReal>::max()) {
            buildPathFromMeeting(meeting, into);
        } else {
            myErrorMsgHandler->inform("No connection between '" + from->getID() + "' and '" + to->getID() + "' found.");
        }
#ifdef CHRouter_DEBUG_QUERY_PERF
        std::cout << "visited " << num_visited_fw + num_visited_bw << " edges (" << num_visited_fw << "," << num_visited_bw << ") ,final path length: " + toString(into.size()) + ")\n";
#endif
        this->endQuery(num_visited_bw + num_visited_fw);
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
    void buildPathFromMeeting(Meeting meeting, Result& into) {
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

    /// contraction related members
    typedef std::pair<const E*, const E*> ConstEdgePair;
    typedef std::pair<E*, E*> EdgePair;

    struct Shortcut {
        Shortcut(EdgePair e, SUMOReal c, int u, SVCPermissions p):
            edgePair(e), cost(c), underlying(u), permissions(p) {}
        EdgePair edgePair;
        SUMOReal cost;
        int underlying;
        SVCPermissions permissions;
    };

    typedef std::vector<Shortcut> Shortcuts;
    typedef std::map<ConstEdgePair, const E*> ShortcutVia;

    /* @brief container class to use when building the contraction hierarchy.
     * instances are reused every time the hierarchy is rebuilt (new time slice)
     * but they must be synchronized first */
    class CHInfo {
    public:
        /// @brief Constructor
        CHInfo(size_t id) :
            edge(E::dictionary(id)),
            contractedNeighbors(0),
            rank(-1),
            level(0),
            underlyingTotal(0),
            visited(false),
            traveltime(std::numeric_limits<SUMOReal>::max())
        {}

        /// @brief recompute the contraction priority and report whether it changed
        bool updatePriority(SPTree<CHInfo, CHConnection>* spTree) {
            if (spTree != 0) {
                updateShortcuts(spTree);
                updateLevel();
            } else {
                contractedNeighbors += 1; // called when a connected edge was contracted
            }
            const SUMOReal oldPriority = priority;
            // priority term as used by abraham []
            const int edge_difference = (int)followers.size() + (int)approaching.size() - 2 * (int)shortcuts.size();
            priority = (SUMOReal)(2 * edge_difference - contractedNeighbors - underlyingTotal - 5 * level);
            return priority != oldPriority;
        }

        /// compute needed shortcuts when contracting this edge
        void updateShortcuts(SPTree<CHInfo, CHConnection>* spTree) {
            const bool validatePermissions = spTree->validatePermissions();
#ifdef CHRouter_DEBUG_CONTRACTION_DEGREE
            const int degree = approaching.size() + followers.size();
            std::cout << "computing shortcuts for '" + edge->getID() + "' with degree " + toString(degree) + "\n";
#endif
            shortcuts.clear();
            underlyingTotal = 0;
            for (typename CHConnections::iterator it_a = approaching.begin(); it_a != approaching.end(); it_a++) {
                CHConnection& aInfo = *it_a;
                // build shortest path tree in a fixed neighborhood
                spTree->rebuildFrom(aInfo.target, this);
                for (typename CHConnections::iterator it_f = followers.begin(); it_f != followers.end(); it_f++) {
                    CHConnection& fInfo = *it_f;
                    const SUMOReal viaCost = aInfo.cost + fInfo.cost;
                    const SVCPermissions viaPermissions = (aInfo.permissions & fInfo.permissions);
                    if (fInfo.target->traveltime > viaCost) {
                        // found no faster path -> we need a shortcut via edge
#ifdef CHRouter_DEBUG_CONTRACTION_WITNESSES
                        debugNoWitness(aInfo, fInfo);
#endif
                        const int underlying = aInfo.underlying + fInfo.underlying;
                        underlyingTotal += underlying;
                        shortcuts.push_back(Shortcut(EdgePair(aInfo.target->edge, fInfo.target->edge),
                                                     viaCost, underlying, viaPermissions));

                    } else if (validatePermissions) {
                        if ((fInfo.target->permissions & viaPermissions) != viaPermissions) {
                            // witness has weaker restrictions. try to find another witness
                            spTree->registerForValidation(&aInfo, &fInfo);
                        } else {
#ifdef CHRouter_DEBUG_CONTRACTION_WITNESSES
                            debugNoWitness(aInfo, fInfo);
#endif
                        }
                    } else {
#ifdef CHRouter_DEBUG_CONTRACTION_WITNESSES
                        debugNoWitness(aInfo, fInfo);
#endif
                    }
                }
            }
            // insert shortcuts needed due to unmet permissions
            if (validatePermissions) {
                const CHConnectionPairs& pairs = spTree->getNeededShortcuts(this);
                for (typename CHConnectionPairs::const_iterator it = pairs.begin(); it != pairs.end(); ++it) {
                    const CHConnection* aInfo = it->first;
                    const CHConnection* fInfo = it->second;
                    const SUMOReal viaCost = aInfo->cost + fInfo->cost;
                    const SVCPermissions viaPermissions = (aInfo->permissions & fInfo->permissions);
                    const int underlying = aInfo->underlying + fInfo->underlying;
                    underlyingTotal += underlying;
                    shortcuts.push_back(Shortcut(EdgePair(aInfo->target->edge, fInfo->target->edge),
                                                 viaCost, underlying, viaPermissions));
                }
            }
        }


        // update level as defined by Abraham
        void updateLevel() {
            int maxLower = std::numeric_limits<int>::min();
            int otherRank;
            for (typename CHConnections::iterator it = approaching.begin(); it != approaching.end(); it++) {
                otherRank = it->target->rank;
                if (otherRank < rank) {
                    maxLower = MAX2(rank, maxLower);
                }
            }
            for (typename CHConnections::iterator it = followers.begin(); it != followers.end(); it++) {
                otherRank = it->target->rank;
                if (otherRank < rank) {
                    maxLower = MAX2(rank, maxLower);
                }
            }
            if (maxLower == std::numeric_limits<int>::min()) {
                level = 0;
            } else {
                level = maxLower + 1;
            }
        }

        // resets state before rebuilding the hierarchy
        void resetContractionState() {
            contractedNeighbors = 0;
            rank = -1;
            level = 0;
            underlyingTotal = 0;
            shortcuts.clear();
            followers.clear();
            approaching.clear();
        }


        /// @brief The current edge - not const since it may receive shortcut edges
        E* edge;
        /// @brief The contraction priority
        SUMOReal priority;
        /// @brief The needed shortcuts
        Shortcuts shortcuts;
        /// @brief priority subterms
        int contractedNeighbors;
        int rank;
        int level;
        int underlyingTotal;

        /// @brief connections (only valid after synchronization)
        CHConnections followers;
        CHConnections approaching;


        /// members used in SPTree
        bool visited;
        /// Effort to reach the edge
        SUMOReal traveltime;
        /// number of edges from start
        int depth;
        /// the permissions when reaching this edge on the fastest path
        // @note: we may miss some witness paths by making traveltime the only
        // criteria durinng search
        SVCPermissions permissions;

        inline void reset() {
            traveltime = std::numeric_limits<SUMOReal>::max();
            visited = false;
        }


        /// debugging methods
        inline void debugNoWitness(const CHConnection& aInfo, const CHConnection& fInfo) {
            std::cout << "adding shortcut between " << aInfo.target->edge->getID() << ", " << fInfo.target->edge->getID() << " via " << edge->getID() << "\n";
        }

        inline void debugWitness(const CHConnection& aInfo, const CHConnection& fInfo) {
            const SUMOReal viaCost = aInfo.cost + fInfo.cost;
            std::cout << "found witness with lenght " << fInfo.target->traveltime << " against via " << edge->getID() << " (length " << viaCost << ") for " << aInfo.target->edge->getID() << ", " << fInfo.target->edge->getID() << "\n";
        }

    };

private:

    /**
     * @class EdgeInfoByRankComparator
     * Class to compare (and so sort) nodes by their contraction priority
     */
    class CHInfoComparator {
    public:
        /// Comparing method
        bool operator()(const CHInfo* a, const CHInfo* b) const {
            if (a->priority == b->priority) {
                return a->edge->getNumericalID() > b->edge->getNumericalID();
            } else {
                return a->priority < b->priority;
            };
        }
    };


    inline CHInfo* getCHInfo(const E* const edge) {
        return &(myCHInfos[edge->getNumericalID()]);
    }


    /// @brief copy connections from the original net (modified destructively during contraction)
    void synchronize(CHInfo& info, SUMOReal time, const V* const vehicle) {
        // forward and backward connections are used only in forward search,
        // thus approaching costs are those of the approaching edge and not of the edge itself
        const bool prune = !mySPTree->validatePermissions();
        const E* const edge = info.edge;
        if (prune && ((edge->getPermissions() & mySVC) != mySVC)) {
            return;
        }
        const SUMOReal cost = this->getEffort(edge, vehicle, time);
        const unsigned int numFollowers = edge->getNumSuccessors();
        for (unsigned int i = 0; i < numFollowers; i++) {
            const E* fEdge = edge->getSuccessor(i);
            if (prune && ((fEdge->getPermissions() & mySVC) != mySVC)) {
                continue;
            }
            CHInfo* follower = getCHInfo(fEdge);
            SVCPermissions permissions = (edge->getPermissions() & follower->edge->getPermissions());
            info.followers.push_back(CHConnection(follower, cost, permissions, 1));
            follower->approaching.push_back(CHConnection(&info, cost, permissions, 1));
        }
#ifdef CHRouter_DEBUG_WEIGHTS
        std::cout << time << ": " << edge->getID() << " cost: " << cost << "\n";
#endif
        // @todo: check whether we even need to save approaching in ROEdge;
    }


    /// @brief remove all connections to/from the given edge (assume it exists only once)
    void disconnect(CHConnections& connections, CHInfo* other) {
        for (typename CHConnections::iterator it = connections.begin(); it != connections.end(); it++) {
            if (it->target == other) {
                connections.erase(it);
                return;
            }
        }
        assert(false);
    }


    void buildContractionHierarchy(SUMOTime time, const V* const vehicle) {
        const size_t numEdges = myCHInfos.size();
        const std::string vClass = (mySPTree->validatePermissions() ?
                                    "all vehicle classes " : "vClass='" + SumoVehicleClassStrings.getString(mySVC) + "' ");
        PROGRESS_BEGIN_MESSAGE("Building Contraction Hierarchy for " + vClass
                               + "and time=" + time2string(time) + " (" + toString(numEdges) + " edges)\n");
        const long startMillis = SysUtils::getCurrentMillis();
        // init queue
        std::vector<CHInfo*> queue; // max heap: edge to be contracted is front
        myShortcuts.clear();
        // reset previous connections etc
        myForwardSearch.reset();
        myBackwardSearch.reset();
        for (size_t i = 0; i < numEdges; i++) {
            myCHInfos[i].resetContractionState();
        }
        // copy connections from the original net
        const SUMOReal time_seconds = STEPS2TIME(time); // timelines store seconds!
        for (size_t i = 0; i < numEdges; i++) {
            synchronize(myCHInfos[i], time_seconds, vehicle);
        }
        // synchronization is finished. now we can compute priorities for the first time
        for (size_t i = 0; i < numEdges; i++) {
            myCHInfos[i].updatePriority(mySPTree);
            queue.push_back(&(myCHInfos[i]));
        }
        make_heap(queue.begin(), queue.end(), myCmp);
        int contractionRank = 0;
        // contraction loop
        while (!queue.empty()) {
            while (tryUpdateFront(queue)) {}
            CHInfo* max = queue.front();
            max->rank = contractionRank;
#ifdef CHRouter_DEBUG_CONTRACTION
            std::cout << "contracting '" << max->edge->getID() << "' with prio: " << max->priority << " (rank " << contractionRank << ")\n";
#endif
            E* edge = max->edge;
            // add outgoing connections to the forward search
            EdgeInfo* edgeInfoFW = myForwardSearch.getEdgeInfo(edge);
            edgeInfoFW->rank = contractionRank;
            for (typename CHConnections::iterator it = max->followers.begin(); it != max->followers.end(); it++) {
                CHConnection& con = *it;
                EdgeInfo* followerInfoFW = myForwardSearch.getEdgeInfo(con.target->edge);
                edgeInfoFW->upward.push_back(Connection(followerInfoFW, con.cost, con.permissions));
                disconnect(con.target->approaching, max);
                con.target->updatePriority(0);
            }
            // add incoming connections to the backward search
            EdgeInfo* edgeInfoBW = myBackwardSearch.getEdgeInfo(edge);
            edgeInfoBW->rank = contractionRank;
            for (typename CHConnections::iterator it = max->approaching.begin(); it != max->approaching.end(); it++) {
                CHConnection& con = *it;
                EdgeInfo* approachingInfoBW = myBackwardSearch.getEdgeInfo(con.target->edge);
                edgeInfoBW->upward.push_back(Connection(approachingInfoBW, con.cost, con.permissions));
                disconnect(con.target->followers, max);
                con.target->updatePriority(0);
            }
            // add shortcuts to the net
            for (typename Shortcuts::iterator it = max->shortcuts.begin(); it != max->shortcuts.end(); it++) {
                EdgePair& edgePair = it->edgePair;
                myShortcuts[edgePair] = edge;
                CHInfo* from = getCHInfo(edgePair.first);
                CHInfo* to = getCHInfo(edgePair.second);
                from->followers.push_back(CHConnection(to, it->cost, it->permissions, it->underlying));
                to->approaching.push_back(CHConnection(from, it->cost, it->permissions, it->underlying));
            }
            // remove from queue
            pop_heap(queue.begin(), queue.end(), myCmp);
            queue.pop_back();
            /*
            if (contractionRank % 10000 == 0) {
                // update all and rebuild queue
                for (typename std::vector<CHInfo*>::iterator it = queue.begin(); it != queue.end(); ++it) {
                    (*it)->updatePriority(mySPTree);
                }
                make_heap(queue.begin(), queue.end(), myCmp);
            }
            */
            contractionRank++;
        }
        // reporting
        const long duration = SysUtils::getCurrentMillis() - startMillis;
        WRITE_MESSAGE("Created " + toString(myShortcuts.size()) + " shortcuts.");
        WRITE_MESSAGE("Recomputed priority " + toString(myUpdateCount) + " times.");
        MsgHandler::getMessageInstance()->endProcessMsg("done (" + toString(duration) + "ms).");
        PROGRESS_DONE_MESSAGE();
        // declare new validUntil (prevent overflow)
        if (myWeightPeriod < std::numeric_limits<int>::max()) {
            myValidUntil = time + myWeightPeriod;
        } else {
            myValidUntil = myWeightPeriod;
        }
        myUpdateCount = 0;
    }

    // retrieve the via edge for a shortcut
    const E* getVia(const E* forwardFrom, const E* forwardTo) {
        ConstEdgePair forward(forwardFrom, forwardTo);
        typename ShortcutVia::iterator it = myShortcuts.find(forward);
        if (it != myShortcuts.end()) {
            return it->second;
        } else {
            return 0;
        }
    }


    /** @brief tries to update the priority of the first edge
     * @return wether updating changed the first edge
     */
    bool tryUpdateFront(std::vector<CHInfo*>& queue) {
        myUpdateCount++;
        CHInfo* max = queue.front();
#ifdef CHRouter_DEBUG_CONTRACTION_QUEUE
        std::cout << "updating '" << max->edge->getID() << "'\n";
        debugPrintQueue(queue);
#endif
        if (max->updatePriority(mySPTree)) {
            pop_heap(queue.begin(), queue.end(), myCmp);
            push_heap(queue.begin(), queue.end(), myCmp);
            return true;
        } else {
            return false;
        }
    }

    // helper method for debugging
    void debugPrintQueue(std::vector<CHInfo*>& queue) {
        for (typename std::vector<CHInfo*>::iterator it = queue.begin(); it != queue.end(); it++) {
            CHInfo* chInfo = *it;
            std::cout << "(" << chInfo->edge->getID() << "," << chInfo->priority << ") ";
        }
        std::cout << "\n";
    }

private:
    /// @brief the handler for routing errors
    MsgHandler* const myErrorMsgHandler;

    /// @brief the unidirectional search queues
    Unidirectional myForwardSearch;
    Unidirectional myBackwardSearch;

    /// @brief map from (forward) shortcut to via-Edge
    ShortcutVia myShortcuts;

    /// @brief static vector for lookup
    std::vector<CHInfo> myCHInfos;

    /// @brief Comparator for contraction priority
    CHInfoComparator myCmp;

    /// @brief the shortest path tree to use when searching for shortcuts
    SPTree<CHInfo, CHConnection>* mySPTree;

    /// @brief the validity duration of one weight interval
    const SUMOTime myWeightPeriod;

    /// @brief the validity duration of the current hierarchy (exclusive)
    SUMOTime myValidUntil;

    /// @brief the permissions for which the hierarchy was constructed
    SUMOVehicleClass mySVC;

    /// @brief counters for performance logging
    int myUpdateCount;
};


#endif

/****************************************************************************/


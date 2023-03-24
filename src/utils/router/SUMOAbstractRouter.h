/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    SUMOAbstractRouter.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    25.Jan 2006
///
// An abstract router base class
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <assert.h>
#include <utils/common/SysUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
//#define ROUTER_DEBUG_HINT
//#define ROUTER_DEBUG_COND (true)


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOAbstractRouter
 * The interface for routing the vehicles over the network.
 */
template<class E, class V>
class SUMOAbstractRouter {
public:
    /**
    * @class EdgeInfo
    * A definition about a route's edge with the effort needed to reach it and
    *  the information about the previous edge.
    */
    class EdgeInfo {
    public:
        /// Constructor
        EdgeInfo(const E* const e)
            : edge(e), effort(std::numeric_limits<double>::max()),
              heuristicEffort(std::numeric_limits<double>::max()),
              leaveTime(0.), prev(nullptr), visited(false), prohibited(false) {}

        /// The current edge
        const E* const edge;

        /// Effort to reach the edge
        double effort;

        /// Estimated effort to reach the edge (effort + lower bound on remaining effort)
        // only used by A*
        double heuristicEffort;

        /// The time the vehicle leaves the edge
        double leaveTime;

        /// The previous edge
        const EdgeInfo* prev;

        /// whether the edge was already evaluated
        bool visited;

        /// whether the edge is currently not allowed
        bool prohibited;

        inline void reset() {
            effort = std::numeric_limits<double>::max();
            heuristicEffort = std::numeric_limits<double>::max();
            visited = false;
        }

    };

    /// Type of the function that is used to retrieve the edge effort.
    typedef double(* Operation)(const E* const, const V* const, double);

    /// Constructor
    SUMOAbstractRouter(const std::string& type, bool unbuildIsWarning, Operation operation, Operation ttOperation,
                       const bool havePermissions, const bool haveRestrictions) :
        myErrorMsgHandler(unbuildIsWarning ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()),
        myOperation(operation), myTTOperation(ttOperation),
        myBulkMode(false),
        myAutoBulkMode(false),
        myHavePermissions(havePermissions),
        myHaveRestrictions(haveRestrictions),
        myType(type),
        myQueryVisits(0),
        myNumQueries(0),
        myQueryStartTime(0),
        myQueryTimeSum(0) {
    }

    /// Copy Constructor
    SUMOAbstractRouter(SUMOAbstractRouter* other) :
        myErrorMsgHandler(other->myErrorMsgHandler),
        myOperation(other->myOperation), myTTOperation(other->myTTOperation),
        myBulkMode(false),
        myAutoBulkMode(false),
        myHavePermissions(other->myHavePermissions),
        myHaveRestrictions(other->myHaveRestrictions),
        myType(other->myType),
        myQueryVisits(0),
        myNumQueries(0),
        myQueryStartTime(0),
        myQueryTimeSum(0) { }



    /// Destructor
    virtual ~SUMOAbstractRouter() {
        if (myNumQueries > 0) {
            WRITE_MESSAGE(myType + " answered " + toString(myNumQueries) + " queries and explored " + toString((double)myQueryVisits / (double)myNumQueries) +  " edges on average.");
            WRITE_MESSAGE(myType + " spent " + elapsedMs2string(myQueryTimeSum) + " answering queries (" + toString((double)myQueryTimeSum / (double)myNumQueries) +  "ms on average).");
        }
    }

    virtual SUMOAbstractRouter* clone() = 0;

    inline void init(const int edgeID, const SUMOTime msTime) {
//        if (!myAmClean) {
        // all EdgeInfos touched in the previous query are either in myFrontierList or myFound: clean those up
        for (auto& edgeInfo : myFrontierList) {
            edgeInfo->reset();
        }
        myFrontierList.clear();
        for (auto& edgeInfo : myFound) {
            edgeInfo->reset();
        }
        myFound.clear();
        if (edgeID > -1) {
            // add begin node
            auto& fromInfo = myEdgeInfos[edgeID];
            fromInfo.effort = 0.;
            fromInfo.heuristicEffort = 0.;
            fromInfo.prev = nullptr;
            fromInfo.leaveTime = STEPS2TIME(msTime);
            myFrontierList.push_back(&fromInfo);
        }
        myAmClean = true;
//        }
    }

    /// reset internal caches, used by CHRouter
    virtual void reset(const V* const vehicle) {
        UNUSED_PARAMETER(vehicle);
    }

    const std::string& getType() const {
        return myType;
    }

    inline const typename SUMOAbstractRouter<E, V>::EdgeInfo& getEdgeInfo(int index) const {
        return myEdgeInfos[index];
    }

    /** @brief Builds the route between the given edges using the minimum effort at the given time
        The definition of the effort depends on the wished routing scheme */
    virtual bool compute(const E* from, const E* to, const V* const vehicle,
                         SUMOTime msTime, std::vector<const E*>& into, bool silent = false) = 0;


    /** @brief Builds the route between the given edges using the minimum effort at the given time,
     * also taking into account position along the edges to ensure currect
     * handling of looped routes
     * The definition of the effort depends on the wished routing scheme */
    inline bool compute(
        const E* from, double fromPos,
        const E* to, double toPos,
        const V* const vehicle,
        SUMOTime msTime, std::vector<const E*>& into, bool silent = false) {
        if (from != to || fromPos <= toPos) {
            return compute(from, to, vehicle, msTime, into, silent);
        } else {
            return computeLooped(from, to, vehicle, msTime, into, silent);
        }
    }


    /** @brief Builds the route between the given edges using the minimum effort at the given time
     * if from == to, return the shortest looped route */
    inline bool computeLooped(const E* from, const E* to, const V* const vehicle,
                              SUMOTime msTime, std::vector<const E*>& into, bool silent = false) {
        if (from != to) {
            return compute(from, to, vehicle, msTime, into, silent);
        }
        double minEffort = std::numeric_limits<double>::max();
        std::vector<const E*> best;
        const SUMOVehicleClass vClass = vehicle == 0 ? SVC_IGNORING : vehicle->getVClass();
        for (const std::pair<const E*, const E*>& follower : from->getViaSuccessors(vClass)) {
            std::vector<const E*> tmp;
            compute(follower.first, to, vehicle, msTime, tmp, true);
            if (tmp.size() > 0) {
                double effort = recomputeCosts(tmp, vehicle, msTime);
                if (effort < minEffort) {
                    minEffort = effort;
                    best = tmp;
                }
            }
        }
        if (minEffort != std::numeric_limits<double>::max()) {
            into.push_back(from);
            std::copy(best.begin(), best.end(), std::back_inserter(into));
            return true;
        } else if (!silent && myErrorMsgHandler != nullptr) {
            myErrorMsgHandler->informf("No connection between edge '%' and edge '%' found.", from->getID(), to->getID());
        }
        return false;
    }

    inline bool isProhibited(const E* const edge, const V* const vehicle) const {
        return (myHavePermissions && edge->prohibits(vehicle)) || (myHaveRestrictions && edge->restricts(vehicle));
    }

    inline double getTravelTime(const E* const e, const V* const v, const double t, const double effort) const {
        return myTTOperation == nullptr ? effort : (*myTTOperation)(e, v, t);
    }

    inline void updateViaEdgeCost(const E* viaEdge, const V* const v, double& time, double& effort, double& length) const {
        while (viaEdge != nullptr && viaEdge->isInternal()) {
            const double viaEffortDelta = this->getEffort(viaEdge, v, time);
            time += getTravelTime(viaEdge, v, time, viaEffortDelta);
            effort += viaEffortDelta;
            length += viaEdge->getLength();
            viaEdge = viaEdge->getViaSuccessors().front().second;
        }
    }

    inline void updateViaCost(const E* const prev, const E* const e, const V* const v, double& time, double& effort, double& length) const {
        if (prev != nullptr) {
            for (const std::pair<const E*, const E*>& follower : prev->getViaSuccessors()) {
                if (follower.first == e) {
                    updateViaEdgeCost(follower.second, v, time, effort, length);
                    break;
                }
            }
        }
        const double effortDelta = this->getEffort(e, v, time);
        effort += effortDelta;
        time += getTravelTime(e, v, time, effortDelta);
        length += e->getLength();
    }


    inline double recomputeCosts(const std::vector<const E*>& edges, const V* const v, SUMOTime msTime, double* lengthp = nullptr) const {
        double time = STEPS2TIME(msTime);
        double effort = 0.;
        double length = 0.;
        if (lengthp == nullptr) {
            lengthp = &length;
        } else {
            *lengthp = 0.;
        }
        const E* prev = nullptr;
        for (const E* const e : edges) {
            if (isProhibited(e, v)) {
                return -1;
            }
            updateViaCost(prev, e, v, time, effort, *lengthp);
            prev = e;
        }
        return effort;
    }


    inline double recomputeCosts(const std::vector<const E*>& edges, const V* const v, double fromPos, double toPos, SUMOTime msTime, double* lengthp = nullptr) const {
        double effort = recomputeCosts(edges, v, msTime, lengthp);
        if (!edges.empty()) {
            double firstEffort = this->getEffort(edges.front(), v, STEPS2TIME(msTime));
            double lastEffort = this->getEffort(edges.back(), v, STEPS2TIME(msTime));
            effort -= firstEffort * fromPos / edges.front()->getLength();
            effort -= lastEffort * (edges.back()->getLength() - toPos) / edges.back()->getLength();
        }
        return effort;
    }


    inline double setHint(const typename std::vector<const E*>::const_iterator routeBegin, const typename std::vector<const E*>::const_iterator routeEnd, const V* const v, SUMOTime msTime) {
        double time = STEPS2TIME(msTime);
        double effort = 0.;
        double length = 0.;
        const EdgeInfo* prev = &myEdgeInfos[(*routeBegin)->getNumericalID()];
        init((*routeBegin)->getNumericalID(), msTime);
        for (auto e = routeBegin + 1; e != routeEnd; ++e) {
            if (isProhibited(*e, v)) {
                return -1;
            }
            auto& edgeInfo = myEdgeInfos[(*e)->getNumericalID()];
            edgeInfo.heuristicEffort = effort;
            edgeInfo.prev = prev;
            updateViaCost(prev->edge, *e, v, time, effort, length);
            edgeInfo.effort = effort;
            edgeInfo.leaveTime = time;
            myFound.push_back(&edgeInfo);
            prev = &edgeInfo;
#ifdef ROUTER_DEBUG_HINT
            if (ROUTER_DEBUG_COND) {
                std::cout << "DEBUG: hit=" << (*e)->getID()
                          << " TT=" << edgeInfo.effort
                          << " EF=" << this->getEffort(*e, v, edgeInfo.leaveTime)
                          << " HT=" << edgeInfo.heuristicEffort << "\n";
            }
#endif
        }
        return effort;
    }


    inline double getEffort(const E* const e, const V* const v, double t) const {
        return (*myOperation)(e, v, t);
    }

    inline void startQuery() {
        myNumQueries++;
        myQueryStartTime = SysUtils::getCurrentMillis();
    }

    inline void endQuery(int visits) {
        myQueryVisits += visits;
        myQueryTimeSum += (SysUtils::getCurrentMillis() - myQueryStartTime);
    }

    virtual void setBulkMode(const bool mode) {
        myBulkMode = mode;
    }

    inline void setAutoBulkMode(const bool mode) {
        myAutoBulkMode = mode;
    }

    virtual void prohibit(const std::vector<E*>& toProhibit) {
        for (E* const edge : this->myProhibited) {
            myEdgeInfos[edge->getNumericalID()].prohibited = false;
        }
        for (E* const edge : toProhibit) {
            myEdgeInfos[edge->getNumericalID()].prohibited = true;
        }
        this->myProhibited = toProhibit;
    }


    /// Builds the path from marked edges
    void buildPathFrom(const typename SUMOAbstractRouter<E, V>::EdgeInfo* rbegin, std::vector<const E*>& edges) {
        std::vector<const E*> tmp;
        while (rbegin != nullptr) {
            tmp.push_back(rbegin->edge);
            rbegin = rbegin->prev;
        }
        std::copy(tmp.rbegin(), tmp.rend(), std::back_inserter(edges));
    }

protected:
    /// @brief the handler for routing errors
    MsgHandler* const myErrorMsgHandler;

    /// @brief The object's operation to perform.
    Operation myOperation;

    /// @brief The object's operation to perform for travel times
    Operation myTTOperation;

    /// @brief whether we are currently operating several route queries in a bulk
    bool myBulkMode;

    /// @brief whether we are currently trying to detect bulk mode automatically
    bool myAutoBulkMode;

    /// @brief whether we are already initialized
    bool myAmClean;

    /// @brief whether edge permissions need to be considered
    const bool myHavePermissions;

    /// @brief whether edge restrictions need to be considered
    const bool myHaveRestrictions;

    /// The list of explicitly prohibited edges
    std::vector<E*> myProhibited;

    /// The container of edge information
    std::vector<typename SUMOAbstractRouter<E, V>::EdgeInfo> myEdgeInfos;

    /// A container for reusage of the min edge heap
    std::vector<typename SUMOAbstractRouter<E, V>::EdgeInfo*> myFrontierList;
    /// @brief list of visited Edges (for resetting)
    std::vector<typename SUMOAbstractRouter<E, V>::EdgeInfo*> myFound;

private:
    /// @brief the type of this router
    const std::string myType;

    /// @brief counters for performance logging
    long long int myQueryVisits;
    long long int myNumQueries;
    /// @brief the time spent querying in milliseconds
    long long int myQueryStartTime;
    long long int myQueryTimeSum;

private:
    /// @brief Invalidated assignment operator
    SUMOAbstractRouter& operator=(const SUMOAbstractRouter& s) = delete;
};

/****************************************************************************/
/// @file    DijkstraRouterTT.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 25 July 2005
/// @version $Id$
///
// The dijkstra-router
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
#ifndef DijkstraRouterTT_h
#define DijkstraRouterTT_h


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
#include <queue>
#include <string>
#include <utils/common/InstancePool.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include "SUMOAbstractRouter.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class DijkstraRouterTT
 * @brief Computes the shortest path through a network using the dijkstra algorithm.
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
class DijkstraRouterTTBase : public SUMOAbstractRouter<E, V>, public PF {
public:
    /// Constructor
    DijkstraRouterTTBase(size_t noE, bool unbuildIsWarningOnly)
            : myNoE(noE), myReusableEdgeLists(true), myReusableEdgeInfoLists(true),
            myUnbuildIsWarningOnly(unbuildIsWarningOnly) { }

    /// Destructor
    virtual ~DijkstraRouterTTBase() { }

    /**
     * @struct EdgeInfo
     * A definition about a route's edge with the effort needed to reach it and
     *  the information about the previous edge.
     */
    class EdgeInfo {
    public:
        /// Constructor
        EdgeInfo()
                : edge(0), traveltime(0), prev(0) {}

        /// The current edge
        const E *edge;

        /// Effort to reach the edge
        SUMOReal traveltime;

        /// The previous edge
        EdgeInfo *prev;

    };

    /**
     * @class EdgeInfoByEffortComperator
     * Class to compare (and so sort) nodes by their effort
     */
    class EdgeInfoByTTComperator {
    public:
        /// Constructor
        explicit EdgeInfoByTTComperator() { }

        /// Destructor
        ~EdgeInfoByTTComperator() { }

        /// Comparing method
        bool operator()(EdgeInfo *nod1, EdgeInfo *nod2) const {
            return nod1->traveltime>nod2->traveltime;
        }
    };

    virtual SUMOReal getEffort(const E * const e, const V * const v, SUMOTime t) = 0;


    /** @brief Builds the route between the given edges using the minimum afford at the given time
        The definition of the afford depends on the wished routing scheme */
    virtual void compute(const E *from, const E *to, const V * const vehicle,
                         SUMOTime time, std::vector<const E*> &into) {

        // get structures to reuse
        std::vector<bool> *visited = myReusableEdgeLists.getFreeInstance();
        if (visited==0) {
            visited = new std::vector<bool>(myNoE, false);
        } else {
            std::fill(visited->begin(), visited->end(), false);
        }
        EdgeInfoCont *storage = myReusableEdgeInfoLists.getFreeInstance();
        if (storage==0) {
            storage = new EdgeInfoCont(myNoE);
        }
        storage->reset();
        assert(from!=0&&to!=0);
        // begin computation
        std::priority_queue<EdgeInfo*, std::vector<EdgeInfo*>, EdgeInfoByTTComperator> frontierList;
        // add begin node
        frontierList.push(storage->add(from, 0, 0));

        // loop
        while (!frontierList.empty()) {
            // use the node with the minimal length
            EdgeInfo * const minimumKnot = frontierList.top();
            const E * const minEdge = minimumKnot->edge;
            frontierList.pop();
            // check whether the destination node was already reached
            if (minEdge == to) {
                buildPathFrom(minimumKnot, into);
                clearTemporaryStorages(visited, storage);
                return;
            }
            (*visited)[minEdge->getNumericalID()] = true;
            const SUMOReal traveltime = minimumKnot->traveltime + getEffort(minEdge, vehicle, time + (SUMOTime)minimumKnot->traveltime);
            // check all ways from the node with the minimal length
            unsigned int i = 0;
            const unsigned int length_size = minEdge->getNoFollowing();
            for (i=0; i<length_size; i++) {
                const E* const help = minEdge->getFollower(i);
                // check whether it can be used
                if (PF::operator()(help, vehicle)) {
                    continue;
                }
                //
                if (!(*visited)[help->getNumericalID()] && traveltime < storage->getEffort(help)) {
                    if (help!=from) {
                        frontierList.push(storage->add(help, traveltime, minimumKnot));
                    }
                }
            }
        }
        if (!myUnbuildIsWarningOnly) {
            MsgHandler::getErrorInstance()->inform("No connection between '" + from->getID() + "' and '" + to->getID() + "' found.");
        } else {
            WRITE_WARNING("No connection between '" + from->getID() + "' and '" + to->getID() + "' found.");
        }
        clearTemporaryStorages(visited, storage);
    }


    SUMOReal recomputeCosts(const std::vector<const E*> &edges, const V * const v, SUMOTime time) throw() {
        SUMOReal costs = 0;
        for (typename std::vector<const E*>::const_iterator i=edges.begin(); i!=edges.end(); ++i) {
            if (PF::operator()(*i, v)) {
                return -1;
            }
            costs += getEffort(*i, v, (SUMOTime)(time + costs));
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

public:
    /**
     * @class EdgeInfoCont
     * A class holding the instances of effort-to-reach and predeccessor
     *  information for every ede within the network.
     * This class is used instead of the former saving of these values within
     *  the edges to allow parallel route computation in multithreading mode.
     */
    class EdgeInfoCont {
    public:
        /// Constructor
        EdgeInfoCont(size_t toAlloc)
                : myEdgeInfos(toAlloc+1, EdgeInfo()) { }

        /// Destructor
        ~EdgeInfoCont() { }

        /// Adds the information about the effort to get to an edge and its predeccessing edge
        EdgeInfo *add(const E *edgeArg, const SUMOReal traveltimeArg, EdgeInfo *prevArg) {
            EdgeInfo *ret = &(myEdgeInfos[edgeArg->getNumericalID()]);
            ret->edge = edgeArg; // !!! may be set within the constructor
            ret->traveltime = traveltimeArg;
            ret->prev = prevArg;
            return ret;
        }

        /// Resets all effort-information
        void reset() {
            for (typename std::vector<EdgeInfo>::iterator i=myEdgeInfos.begin(); i!=myEdgeInfos.end(); i++) {
                (*i).traveltime = std::numeric_limits<SUMOReal>::max();
            }
        }


        /** @brief Returns the effort to get to the specify edge
            The value is valid if the edge was already visited */
        SUMOReal getEffort(const E *to) const {
            return myEdgeInfos[to->getNumericalID()].traveltime;
        }

    private:
        /// The container of edge information
        std::vector<EdgeInfo> myEdgeInfos;

    };

protected:
    /// Saves the temporary storages for further usage
    void clearTemporaryStorages(std::vector<bool> *edgeList,
                                EdgeInfoCont *consecutionList) {
        myReusableEdgeLists.addFreeInstance(edgeList);
        myReusableEdgeInfoLists.addFreeInstance(consecutionList);
    }


protected:
    /// The network to use
    size_t myNoE;

    /// A container for reusage of examined edges lists
    InstancePool<std::vector<bool> > myReusableEdgeLists;

    /// A container for reusage of edge consecution lists
    InstancePool<EdgeInfoCont> myReusableEdgeInfoLists;

    bool myUnbuildIsWarningOnly;

};


template<class E, class V, class PF, class EC>
class DijkstraRouterTT_ByProxi : public DijkstraRouterTTBase<E, V, PF> {
public:
    /// Type of the function that is used to retrieve the edge effort.
    typedef SUMOReal(EC::* Operation)(const E * const, const V * const, SUMOTime) const;

    DijkstraRouterTT_ByProxi(size_t noE, bool unbuildIsWarningOnly, EC* receiver, Operation operation)
            : DijkstraRouterTTBase<E, V, PF>(noE, unbuildIsWarningOnly),
            myReceiver(receiver), myOperation(operation) {}

    inline SUMOReal getEffort(const E * const e, const V * const v, SUMOTime t) {
        return (myReceiver->*myOperation)(e, v, t);
    }

private:
    /// @brief The object the action is directed to.
    EC* myReceiver;

    /// @brief The object's operation to perform.
    Operation myOperation;


};


template<class E, class V, class PF>
class DijkstraRouterTT_Direct : public DijkstraRouterTTBase<E, V, PF> {
public:
    /// Type of the function that is used to retrieve the edge effort.
    typedef SUMOReal(E::* Operation)(const V * const, SUMOTime) const;

    DijkstraRouterTT_Direct(size_t noE, bool unbuildIsWarningOnly, Operation operation)
            : DijkstraRouterTTBase<E, V, PF>(noE, unbuildIsWarningOnly), myOperation(operation) {}

    inline SUMOReal getEffort(const E * const e, const V * const v, SUMOTime t) {
        return (e->*myOperation)(v, t);
    }

private:
    Operation myOperation;

};


#endif

/****************************************************************************/


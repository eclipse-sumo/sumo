/****************************************************************************/
/// @file    TraCIDijkstraRouter.h
/// @author  Friedemann Wesner <wesner@itm.uni-luebeck.de>
/// @date    2008/03/29
/// @version $Id$
///
/// Dijkstra Router for use by TraCI
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
#ifndef TRACIDIJKSTRAROUTER_H
#define TRACIDIJKSTRAROUTER_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifndef NO_TRACI

#include "utils/common/SUMODijkstraRouter.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCIDijkstraRouter
 * @brief Computes the shortest path through a network using the dijkstra algorithm.
 *
 * The template parameters are:
 * @param E The edge class to use (MSEdge/ROEdge)
 *
 * This router is basically the same as the SUMODijkstraRouter, except for the following:
 * If start and destination edge are the same, the computed route does not consist of just the
 * starting edge. Instead, if there is a path from the starting edge through the network back 
 * to itself, the route will consist of this path, containing the same edge both at the 
 * beginning and at the end.
 * Furthermore, no vehicle is regarded to determine the efforts of the edges,
 * therefore no prohibition function is used.
 *
 */
template<class E>
class TraCIDijkstraRouter : public SUMOAbstractRouter<E, MSVehicle>
{
public:
    /// Constructor
    TraCIDijkstraRouter(size_t noE/*, bool unbuildIsWarningOnly*/)
            : myNoE(noE), myReusableEdgeLists(true), myReusableEdgeInfoLists(true){ }

    /// Destructor
    virtual ~TraCIDijkstraRouter() { }

    /**
     * @struct EdgeInfo
     * A definition about a route's edge with the effort needed to reach it and
     *  the information about the previous edge.
     */
    class EdgeInfo
    {
    public:
        /// Constructor
        EdgeInfo()
                : edge(0), effort(0), prev(0) {}


        /// Constructor
        EdgeInfo(const E *edgeArg, SUMOReal effortArg, EdgeInfo *prevArg)
                : edge(edgeArg), effort(effortArg), prev(prevArg) {}

        /// Constructor
        EdgeInfo(const E *edgeArg, SUMOReal effortArg, EdgeInfo *prevArg, SUMOReal distArg)
                : edge(edgeArg), effort(effortArg), prev(prevArg), dist(distArg) {}

        /// The current edge
        const E *edge;

        /// Effort to reach the edge
        SUMOReal effort;

        /// The previous edge
        EdgeInfo *prev;

        /// Distance from the begin
        SUMOReal dist;

    };

    /**
     * @class EdgeInfoByEffortComperator
     * Class to compare (and so sort) nodes by their effort
     */
    class EdgeInfoByEffortComperator
    {
    public:
        /// Constructor
        explicit EdgeInfoByEffortComperator() { }

        /// Destructor
        ~EdgeInfoByEffortComperator() { }

        /// Comparing method
        bool operator()(EdgeInfo *nod1, EdgeInfo *nod2) const {
            return nod1->effort>nod2->effort;
        }
    };

	virtual SUMOReal getEffort(const E * const e, SUMOReal t) {
		return e->getEffort(t);	
	}


    /** @brief Builds the route between the given edges using the minimum effort at the given time
        The definition of the effort depends on the wished routing scheme */
    virtual void compute(const E *from, const E *to, const MSVehicle * const vehicle,
                         SUMOTime time, std::vector<const E*> &into) {

        // get structures to reuse
        std::vector<bool> *visited = myReusableEdgeLists.getFreeInstance();
        if (visited==0) {
            visited = new std::vector<bool>(myNoE, false);
        } else {
            for (size_t i=0; i<myNoE; i++) {
                (*visited)[i] = false; // too slow? !!!
            }
        }
        EdgeInfoCont *storage = myReusableEdgeInfoLists.getFreeInstance();
        if (storage==0) {
            storage = new EdgeInfoCont(myNoE);
        }
        storage->reset();

        // check the nodes
        if (from==0||to==0) {
            throw std::exception();
        }

        // begin computation
        std::priority_queue<EdgeInfo*,
        std::vector<EdgeInfo*>,
        EdgeInfoByEffortComperator> frontierList;
        // add begin node
        const E *actualKnot = from;
        if (from != 0) {
            EdgeInfo *ei = storage->add(actualKnot, 0, 0);
            frontierList.push(ei);
        }
		bool isFirstIteration = true;

        // loop
        while (!frontierList.empty()) {
            // use the node with the minimal length
            EdgeInfo *minimumKnot = frontierList.top();
            const E *minEdge = minimumKnot->edge;
            frontierList.pop();
            // check whether the destination node was already reached
            if ((minEdge == to) && (!isFirstIteration)) {
                buildPathFrom(minimumKnot, into);
                clearTemporaryStorages(visited, storage);
                return;
            }
			(*visited)[minEdge->getNumericalID()] = true;
            SUMOReal effort = (SUMOReal)(minimumKnot->effort + getEffort(minEdge, time + minimumKnot->effort));
            // check all ways from the node with the minimal length
            unsigned int i = 0;
            unsigned int length_size = minEdge->getNoFollowing();
            for (i=0; i<length_size; i++) {
                const E* help = minEdge->getFollower(i);

                if ( (!(*visited)[help->getNumericalID()] && effort < storage->getEffort(help))
						|| (help == to)) {
//                    if (help!=from) {
                        frontierList.push(storage->add(help, effort, minimumKnot));
//                    }
                }
            }

			isFirstIteration = false;
        }
        clearTemporaryStorages(visited, storage);
    }

public:
    /// Builds the path from marked edges
    void buildPathFrom(EdgeInfo *rbegin, std::vector<const E *> &edges) {
        std::deque<const E*> tmp;
		EdgeInfo* last = rbegin;
        while (rbegin!=0) {
            tmp.push_front((E *) rbegin->edge); // !!!
            rbegin = rbegin->prev;
			if (rbegin == last) {
				tmp.push_front((E *) rbegin->edge);
				break;
			}	
        }
        std::copy(tmp.begin(), tmp.end(), std::back_inserter(edges));
    }

public:
    /**
     * @class EdgeInfoCont
     * A class holding the instances of effort-to-reach and predeccessor
     *  information for every edge within the network.
     * This class is used instead of the former saving of these values within
     *  the edges to allow parallel route computation in multithreading mode.
     */
    class EdgeInfoCont
    {
    public:
        /// Constructor
        EdgeInfoCont(size_t toAlloc)
                : myEdgeInfos(toAlloc+1, EdgeInfo()) { }

        /// Destructor
        ~EdgeInfoCont() { }

        /// Adds the information about the effort to get to an edge and its predeccessing edge
        EdgeInfo *add(const E *edgeArg, SUMOReal effortArg, EdgeInfo *prevArg) {
            EdgeInfo *ret = &(myEdgeInfos[edgeArg->getNumericalID()]);
            ret->edge = edgeArg; // !!! may be set within the constructor
            ret->effort = effortArg;
            ret->prev = prevArg;
            ret->dist = 0;
            return ret;
        }

        /// Adds the information about the effort to get to an edge and its predeccessing edge
        EdgeInfo *add(const E *edgeArg, SUMOReal effortArg, EdgeInfo *prevArg,
                      SUMOReal distArg) {
            EdgeInfo *ret = &(myEdgeInfos[edgeArg->getNumericalID()]);
            ret->edge = edgeArg; // !!! may be set within the constructor
            ret->effort = effortArg;
            ret->prev = prevArg;
            ret->dist = distArg;
            return ret;
        }

        /// Resets all effort-information
        void reset() {
            for (typename std::vector<EdgeInfo>::iterator i=myEdgeInfos.begin(); i!=myEdgeInfos.end(); i++) {
                (*i).effort = std::numeric_limits<SUMOReal>::max();
            }
        }


        /** @brief Returns the effort to get to the specify edge
            The value is valid if the edge was already visited */
        SUMOReal getEffort(const E *to) const {
            return myEdgeInfos[to->getNumericalID()].effort;
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
};

#endif

#endif


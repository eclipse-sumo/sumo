#ifndef SUMODijkstraRouter_h
#define SUMODijkstraRouter_h
//---------------------------------------------------------------------------//
//                        SUMODijkstraRouter.h -
//  The dijkstra-router
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 25 July 2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.1  2005/09/15 12:20:44  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2005/09/09 12:56:09  dksumo
// helpers added
//
// Revision 1.1  2005/08/01 13:45:17  dksumo
// rerouting within the simulation added
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <functional>
#include <vector>
#include <set>
#include <limits>
#include <queue>
#include <string>
#include <utils/common/InstancePool.h>
#include <utils/common/MsgHandler.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class RONet;
class ROEdgeCont;
class ROEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class SUMODijkstraRouter
 * Lays the given route over the edges using the dijkstra algorithm
 */
template<class E>
class SUMODijkstraRouter {
public:
    /// Constructor
    SUMODijkstraRouter(size_t noE)
        : myNoE(noE), myReusableEdgeLists(true), myReusableEdgeInfoLists(true)
    { }

    /// Destructor
    virtual ~SUMODijkstraRouter() { }

    /**
     * @struct EdgeInfo
     * A definition about a route's edge with the effort needed to reach it and
     *  the information about the previous edge.
     */
    class EdgeInfo {
    public:
        /// Constructor
        EdgeInfo()
            : edge(0), effort(0), prev(0)
        {
        }


        /// Constructor
        EdgeInfo(const E *edgeArg, double effortArg, EdgeInfo *prevArg)
            : edge(edgeArg), effort(effortArg), prev(prevArg)
        {
        }

        /// Constructor
        EdgeInfo(const E *edgeArg, double effortArg, EdgeInfo *prevArg, double distArg)
            : edge(edgeArg), effort(effortArg), prev(prevArg), dist(distArg)
        {
        }

        /// The current edge
        const E *edge;

        /// Effort to reach the edge
        double effort;

        /// The previous edge
        EdgeInfo *prev;

		/// Distance from the begin
		double dist;

    };

    /**
     * @class EdgeInfoByEffortComperator
     * Class to compare (and so sort) nodes by their effort
     */
    class EdgeInfoByEffortComperator /*: public std::less<ROEdge*>*/ {
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


    /** @brief Builds the route between the given edges using the minimum afford at the given time
        The definition of the afford depends on the wished routing scheme */
    virtual std::deque<const E*> compute(const E *from, const E *to,
        SUMOTime time, bool continueOnUnbuild) {

        std::vector<bool> *visited = myReusableEdgeLists.getFreeInstance();
        if(visited==0) {
            visited = new std::vector<bool>(myNoE, false);
        } else {
            for(size_t i=0; i<myNoE; i++) {
                (*visited)[i] = false; // !!!
            }
        }
        EdgeInfoCont *storage = myReusableEdgeInfoLists.getFreeInstance();
        if(storage==0) {
            storage = new EdgeInfoCont(myNoE);
        }
        storage->reset();
        // check the nodes
        if(from==0||to==0) {
	  throw std::exception();
        }
        std::priority_queue<EdgeInfo*,
	  std::vector<EdgeInfo*>,
            EdgeInfoByEffortComperator> frontierList;
        // add begin node
	    const E *actualKnot = from;
	    if(from != 0) {
            EdgeInfo *ei = storage->add(actualKnot, 0, 0);
            frontierList.push(ei);
	    }
        // loop
	    while(!frontierList.empty()) {
            // use the node with the minimal length
            EdgeInfo *minimumKnot = frontierList.top();
            const E *minEdge = minimumKnot->edge;
            frontierList.pop();
                // check whether the destination node was already reached
            if(minEdge == to) {
                std::deque<const E*> ret = buildPathFrom(minimumKnot);
                clearTemporaryStorages(visited, storage);
                return ret;
            }
            (*visited)[minEdge->getNumericalID()] = true; //minimumKnot->setExplored(true);
            float effort = (float) (minimumKnot->effort
		    	+ minEdge->getEffort(time + minimumKnot->effort));
    		// check all ways from the node with the minimal length
            size_t i = 0;
            size_t length_size = minEdge->getNoFollowing();
            for(i=0; i<length_size; i++) {
                const E* help = minEdge->getFollower(i);
                // check whether it can be used
                if(std::find(myProhibited.begin(), myProhibited.end(), help)!=myProhibited.end()) {
                    continue;
                }
                //
                if( !(*visited)[help->getNumericalID()] //&&//!help->isExplored() &&
                    && effort < storage->getEffort(help) ) {
                    if(help!=from) {
                        frontierList.push(storage->add(help, effort, minimumKnot));
                    }
                }
		    }
	    }
        if(!continueOnUnbuild) {
            MsgHandler::getErrorInstance()->inform(
                "No connection between '" + from->id()
                + "' and '" + to->id() + "' found.");
        } else {
            WRITE_WARNING("No connection between '" + from->id() + "' and '" + to->id() + "' found.");
        }
        clearTemporaryStorages(visited, storage);
        return std::deque<const E*>();
    }

    void prohibit(const std::vector<E*> &toProhibit) {
        myProhibited = toProhibit;
    }

public:
    /// Builds the path from marked edges
    std::deque<const E*> buildPathFrom(EdgeInfo *rbegin) {
        std::deque<const E*> ret;
        while(rbegin!=0) {
            ret.push_front(rbegin->edge);
            rbegin = rbegin->prev;
        }
        return ret;
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
            : myEdgeInfos(toAlloc+1, EdgeInfo())
        { }

        /// Destructor
        ~EdgeInfoCont() { }

        /// Adds the information about the effort to get to an edge and its predeccessing edge
        EdgeInfo *add(const E *edgeArg, double effortArg, EdgeInfo *prevArg)
        {
            EdgeInfo *ret = &(myEdgeInfos[edgeArg->getNumericalID()]);
            ret->edge = edgeArg; // !!! may be set within the constructor
            ret->effort = effortArg;
            ret->prev = prevArg;
            ret->dist = 0;
            return ret;
        }

        /// Adds the information about the effort to get to an edge and its predeccessing edge
        EdgeInfo *add(const E *edgeArg, double effortArg, EdgeInfo *prevArg,
			double distArg)
        {
            EdgeInfo *ret = &(myEdgeInfos[edgeArg->getNumericalID()]);
            ret->edge = edgeArg; // !!! may be set within the constructor
            ret->effort = effortArg;
            ret->prev = prevArg;
            ret->dist = distArg;
            return ret;
        }

        /// Resets all effort-information
        void reset()
        {
            for(typename std::vector<EdgeInfo>::iterator i=myEdgeInfos.begin(); i!=myEdgeInfos.end(); i++) {
                (*i).effort = std::numeric_limits<double>::max();
            }
        }


        /** @brief Returns the effort to get to the specify edge
            The value is valid if the edge was already visited */
        double getEffort(const E *to) const
        {
            return myEdgeInfos[to->getNumericalID()].effort;
        }

    private:
        /// The container of edge information
        std::vector<EdgeInfo> myEdgeInfos;

    };

protected:
    /// Saves the temporary storages for further usage
    void clearTemporaryStorages(std::vector<bool> *edgeList,
        EdgeInfoCont *consecutionList)
    {
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

    std::vector<E*> myProhibited;


};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


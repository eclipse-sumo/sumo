#ifndef RODijkstraRouter_h
#define RODijkstraRouter_h
//---------------------------------------------------------------------------//
//                        RODijkstraRouter.h -
//  The dijkstra-router
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.1  2005/12/08 12:59:41  ericnicolay
// *** empty log message ***
//
// Revision 1.6  2005/10/07 11:42:28  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/23 06:04:48  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.4  2005/09/15 12:05:23  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/05/04 08:57:12  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.2  2004/11/23 10:26:27  dkrajzew
// debugging
//
// Revision 1.1  2004/01/26 06:08:38  dkrajzew
// initial commit for dua-classes
//
// --------------------------------------------
// Revision 1.4  2003/06/18 11:36:50  dkrajzew
// a new interface which allows to choose whether to stop after a route could
//  not be computed or not; not very sphisticated, in fact
//
// Revision 1.3  2003/02/07 10:45:07  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <functional>
#include <vector>
#include <set>
#include <utils/common/InstancePool.h>
#include <router/ROEdgeVector.h>
#include <router/ROAbstractRouter.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class RONet;
class ROEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RODijkstraRouter
 * Lays the given route over the edges using the dijkstra algorithm
 */
class RODijkstraRouter : public ROAbstractRouter {
public:
    /// Constructor
    RODijkstraRouter(RONet &net);

    /// Destructor
    virtual ~RODijkstraRouter();

    /** @brief Builds the route between the given edges using the minimum afford at the given time
        The definition of the afford depends on the wished routing scheme */
    virtual ROEdgeVector compute(ROEdge *from, ROEdge *to,
        SUMOTime time, bool continueOnUnbuild,
		ROAbstractEdgeEffortRetriever * const retriever=0);

public:
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
        EdgeInfo(ROEdge *edgeArg, SUMOReal effortArg, EdgeInfo *prevArg)
            : edge(edgeArg), effort(effortArg), prev(prevArg)
        {
        }

        /// Constructor
        EdgeInfo(ROEdge *edgeArg, SUMOReal effortArg, EdgeInfo *prevArg, SUMOReal distArg)
            : edge(edgeArg), effort(effortArg), prev(prevArg), dist(distArg)
        {
        }

        /// The current edge
        ROEdge *edge;

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

    /// Builds the path from marked edges
    ROEdgeVector buildPathFrom(EdgeInfo *rbegin);

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
        EdgeInfoCont(size_t toAlloc);

        /// Destructor
        ~EdgeInfoCont();

        /// Adds the information about the effort to get to an edge and its predeccessing edge
        EdgeInfo *add(ROEdge *edgeArg, SUMOReal effortArg, EdgeInfo *prevArg);

        /// Adds the information about the effort to get to an edge and its predeccessing edge
        EdgeInfo *add(ROEdge *edgeArg, SUMOReal effortArg, EdgeInfo *prevArg,
            SUMOReal distArg);

        /// Resets all effort-information
        void reset();

        /** @brief Returns the effort to get to the specify edge
            The value is valid if the edge was already visited */
        SUMOReal getEffort(ROEdge *to) const;

    private:
        /// The container of edge information
        std::vector<EdgeInfo> myEdgeInfos;

    };

protected:
    /// Saves the temporary storages for further usage
    void clearTemporaryStorages(std::vector<bool> *edgeList,
        EdgeInfoCont *consecutionList);

protected:
    /// The network to use
    RONet &_net;

    /// A container for reusage of examined edges lists
    InstancePool<std::vector<bool> > myReusableEdgeLists;

    /// A container for reusage of edge consecution lists
    InstancePool<EdgeInfoCont> myReusableEdgeInfoLists;


};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


//---------------------------------------------------------------------------//
//                        RODijkstraRouter.cpp -
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.7  2005/10/07 11:42:28  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:04:48  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:05:23  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/07/12 12:39:24  dkrajzew
// forgetting to take time with patched
//
// Revision 1.3  2005/05/04 08:57:11  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.2  2004/11/23 10:26:27  dkrajzew
// debugging
//
// Revision 1.1  2004/01/26 06:08:38  dkrajzew
// initial commit for dua-classes
//
// ---------------------------------------------------------------------------
// Revision 1.10  2003/12/09 11:31:18  dkrajzew
// documentation added
//
// Revision 1.9  2003/08/21 12:59:35  dkrajzew
// some bugs patched
//
// Revision 1.8  2003/06/18 11:36:50  dkrajzew
// a new interface which allows to choose whether to stop after a route could not be computed or not; not very sphisticated, in fact
//
// Revision 1.7  2003/05/20 09:48:35  dkrajzew
// debugging
//
// Revision 1.6  2003/04/09 15:39:11  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.5  2003/04/02 11:48:44  dkrajzew
// debug statements removed
//
// Revision 1.4  2003/04/01 15:19:51  dkrajzew
// behaviour on broken nets patched
//
// Revision 1.3  2003/02/07 10:45:06  dkrajzew
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
#include <deque>
#include <algorithm>
#include <utils/common/MsgHandler.h>
#include <router/RONet.h>
#include <router/ROEdge.h>
#include "RODijkstraRouter.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * RODijkstraRouter::EdgeInfoCont-methods
 * ----------------------------------------------------------------------- */
RODijkstraRouter::EdgeInfoCont::EdgeInfoCont(size_t toAlloc)
    : myEdgeInfos(toAlloc+1, EdgeInfo())
{
}


RODijkstraRouter::EdgeInfoCont::~EdgeInfoCont()
{
}


RODijkstraRouter::EdgeInfo *
RODijkstraRouter::EdgeInfoCont::add(ROEdge *edgeArg, SUMOReal effortArg, EdgeInfo *prevArg)
{
    EdgeInfo *ret = &(myEdgeInfos[edgeArg->getIndex()]);
    ret->edge = edgeArg; // !!! may be set within the constructor
    ret->effort = effortArg;
    ret->prev = prevArg;
	ret->dist = 0;
    return ret;
}


RODijkstraRouter::EdgeInfo *
RODijkstraRouter::EdgeInfoCont::add(ROEdge *edgeArg, SUMOReal effortArg,
									EdgeInfo *prevArg, SUMOReal distArg)
{
    EdgeInfo *ret = &(myEdgeInfos[edgeArg->getIndex()]);
    ret->edge = edgeArg; // !!! may be set within the constructor
    ret->effort = effortArg;
    ret->prev = prevArg;
	ret->dist = distArg;
    return ret;
}


#ifdef MSVC
#include <limits>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG
#else
#define DBL_MAX 10000000000.0 // !!!
#endif

void
RODijkstraRouter::EdgeInfoCont::reset()
{
    for(std::vector<EdgeInfo>::iterator i=myEdgeInfos.begin(); i!=myEdgeInfos.end(); i++) {
        (*i).effort = DBL_MAX;
    }
}


SUMOReal
RODijkstraRouter::EdgeInfoCont::getEffort(ROEdge *to) const
{
    return myEdgeInfos[to->getIndex()].effort;
}


/* -------------------------------------------------------------------------
 * RODijkstraRouter-methods
 * ----------------------------------------------------------------------- */
RODijkstraRouter::RODijkstraRouter(RONet &net)
    : _net(net), myReusableEdgeLists(true),
    myReusableEdgeInfoLists(true)
{
}


RODijkstraRouter::~RODijkstraRouter()
{
}


ROEdgeVector
RODijkstraRouter::compute(ROEdge *from, ROEdge *to, SUMOTime time,
						  bool continueOnUnbuild,
						  ROAbstractEdgeEffortRetriever * const retriever)
{
    std::vector<bool> *visited = myReusableEdgeLists.getFreeInstance();
    if(visited==0) {
        visited = new std::vector<bool>(_net.getEdgeNo(), false);
    } else {
        for(size_t i=0; i<_net.getEdgeNo(); i++) {
            (*visited)[i] = false; // !!!
        }
    }
    EdgeInfoCont *storage = myReusableEdgeInfoLists.getFreeInstance();
    if(storage==0) {
        storage = new EdgeInfoCont(_net.getEdgeNo());
    }
    storage->reset();
    // check the nodes
    if(from==0||to==0) {
        throw exception();
    }
    priority_queue<EdgeInfo*,
        vector<EdgeInfo*>,
        EdgeInfoByEffortComperator> frontierList;
    // add begin node
	ROEdge *actualKnot = from;
	if(from != 0) {
        EdgeInfo *ei = storage->add(actualKnot, 0, 0);
        frontierList.push(ei);
	}
    // loop
	while(!frontierList.empty()) {
        // use the node with the minimal length
        EdgeInfo *minimumKnot = frontierList.top();
        ROEdge *minEdge = minimumKnot->edge;
        frontierList.pop();
            // check whether the destination node was already reached
        if(minEdge == to) {
			ROEdgeVector ret = buildPathFrom(minimumKnot);
            clearTemporaryStorages(visited, storage);
            return ret;
        }
        (*visited)[minEdge->getIndex()] = true; //minimumKnot->setExplored(true);
        SUMOReal effort = (SUMOReal) (minimumKnot->effort
			+ minEdge->getEffort((SUMOTime) (time + minimumKnot->effort)));
		// check all ways from the node with the minimal length
        size_t i = 0;
        size_t length_size = minEdge->getNoFollowing();
        for(i=0; i<length_size; i++) {
            ROEdge *help = minEdge->getFollower(i);
            if( !(*visited)[help->getIndex()] //&&//!help->isExplored() &&
                && effort < storage->getEffort(help) ) {
                if(help!=from) {
                    frontierList.push(storage->add(help, effort, minimumKnot));
                }
            }
		}
	}
    if(!continueOnUnbuild) {
        MsgHandler::getErrorInstance()->inform(
            string("No connection between '") + from->getID()
            + string("' and '") + to->getID() + string("' found."));
    } else {
        WRITE_WARNING(string("No connection between '") + from->getID()+ string("' and '") + to->getID() + string("' found."));
    }
    clearTemporaryStorages(visited, storage);
    return ROEdgeVector();
}


ROEdgeVector
RODijkstraRouter::buildPathFrom(RODijkstraRouter::EdgeInfo *rbegin)
{
    ROEdgeVector tmp;
    while(rbegin!=0) {
        tmp.add(rbegin->edge);
        rbegin = rbegin->prev;
    }
    return tmp.getReverse();
}


void
RODijkstraRouter::clearTemporaryStorages(std::vector<bool> *edgeList,
                                 RODijkstraRouter::EdgeInfoCont *consecutionList)
{
    myReusableEdgeLists.addFreeInstance(edgeList);
    myReusableEdgeInfoLists.addFreeInstance(consecutionList);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



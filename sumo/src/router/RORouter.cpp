//---------------------------------------------------------------------------//
//                        RORouter.cpp -
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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <deque>
#include <algorithm>
#include <utils/common/MsgHandler.h>
#include "RONet.h"
#include "RORouter.h"
#include "ROEdge.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
RORouter::RORouter(RONet &net, ROEdgeCont *source)
    : _net(net), _source(source)
{
}


RORouter::~RORouter()
{
}


ROEdgeVector
RORouter::compute(ROEdge *from, ROEdge *to, long time, bool continueOnUnbuild)
{
    // check whether the route is already known
//!!!    if(_net.knowsRouteSnipplet(from, to)) {
//!!!        return _net.getRouteSnipplet(from, to); // !!! invalid over time
//!!!    }
    // otherwise, build, save and return a new route
    ROEdgeVector ret = dijkstraCompute(from, to, time, continueOnUnbuild);
//!!!    _net.addRouteSnipplet(ret);
    return ret;
}


ROEdgeVector
RORouter::dijkstraCompute(ROEdge *from, ROEdge *to, long time, bool continueOnUnbuild)
{
    // check the nodes
    if(from==0||to==0) {
        throw exception();
    }
    // init knots
    _source->init();
    //
    priority_queue<ROEdge*,
        vector<ROEdge*>,
        NodeByEffortComperator> frontierList;
    // add begin node
	ROEdge *actualKnot = from;
	if(from != 0) {
        frontierList.push(actualKnot);
        actualKnot->initRootDistance();
	}
    // loop
	bool found = false;
	while(!frontierList.empty()) {
        // use the node with the minimal length
        ROEdge *minimumKnot = frontierList.top();
        frontierList.pop();
            // check whether the destination node was already reached
		if(minimumKnot == to)
			found = true;
		minimumKnot->setExplored(true);
        float effort = minimumKnot->getNextEffort(time);
		// check all ways from the node with the minimal length
        size_t i = 0;
        size_t length_size = minimumKnot->getNoFollowing();
        for(i=0; i<length_size; i++) {
            ROEdge *help = minimumKnot->getFollower(i);
            bool newfront = false;
            if( !help->isExplored() &&
                effort < help->getEffort() ) {
                if(!help->isInFrontList())
                    newfront = true;
                help->setEffort(effort);
                help->setPrevKnot(minimumKnot);
            }
            if(newfront)
                frontierList.push(help);
		}
/*
        priority_queue<ROEdge*,
            vector<ROEdge*>,
            NodeByEffortComperator> tmp(frontierList);
        while(!tmp.empty()) {
            ROEdge *edge = tmp.top();
            tmp.pop();
            cout << edge->getID() << "(" << edge->getCost(time) << ", " << edge->getEffort() << "), ";
        }
        cout << endl;
*/
		if(found) {
            return buildPathFrom(to);
		}
	}
    cout << endl;
    if(!continueOnUnbuild) {
        MsgHandler::getErrorInstance()->inform(
            string("No connection between '") + from->getID()
            + string("' and '") + to->getID() + string("' found."));
    } else {
        MsgHandler::getWarningInstance()->inform(
            string("No connection between '") + from->getID()
            + string("' and '") + to->getID() + string("' found."));
    }
    return ROEdgeVector();
}


ROEdgeVector
RORouter::buildPathFrom(ROEdge *rbegin) {
    ROEdgeVector tmp;
    while(rbegin!=0) {
        tmp.add(rbegin);
        rbegin = rbegin->getPrevKnot();
    }
    return tmp.getReverse();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



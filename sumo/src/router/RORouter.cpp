#include <string>
#include <deque>
#include <algorithm>
#include "RONet.h"
#include "RORouter.h"
#include "ROEdge.h"

using namespace std;

RORouter::RORouter(RONet &net, ROEdgeCont *source)
    : _net(net), _source(source)
{
}

RORouter::~RORouter()
{
}

ROEdgeVector
RORouter::compute(ROEdge *from, ROEdge *to, long time)
{
    // check whether the route is already known
    if(_net.knowsRouteSnipplet(from, to)) {
        return _net.getRouteSnipplet(from, to); // !!! invalid over time
    }
    // otherwise, build, save and return a new route
    ROEdgeVector ret = dijkstraCompute(from, to, time);
    _net.addRouteSnipplet(ret);
    return ret;
}


ROEdgeVector
RORouter::dijkstraCompute(ROEdge *from, ROEdge *to, long time) {
    // retrieve the nodes
    // check the nodes
    if(from==0||to==0) {
        throw exception();
    }
    // init knots
    _source->init();
    //
    priority_queue<ROEdge*, 
        vector<ROEdge*>, 
        NodeByDistanceComperator> frontierList;
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
            NodeByDistanceComperator> tmp(frontierList);
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
    return buildPathFrom(to);
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

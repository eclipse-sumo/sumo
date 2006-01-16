#include "DFRONet.h"
#include <routing_df/DFDetector.h>

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)
#pragma warning(disable: 4503)


#include <iostream>
#include <routing_df/DFDetector.h>
#include <routing_df/DFRORouteDesc.h>

using namespace std;

DFRONet::DFRONet()
{
	ro = NULL;
}

DFRONet::DFRONet(RONet * Ro)
{
	ro = Ro;
}

DFRONet::~DFRONet()
{
    delete ro;
}

void
DFRONet::buildApproachList()
{
	std::vector<ROEdge *> r = ro->getMyEdgeCont()->getTempVector();
	std::vector<ROEdge *>::iterator rit = r.begin();
	for ( ; rit != r.end(); rit++ )
	{
		size_t i = 0;
        size_t length_size = (*rit)->getNoFollowing();
        for(i=0; i<length_size; i++)
		{
            ROEdge *help = (*rit)->getFollower(i);
			// add the connection help->*rit to myApproachingEdges
			if(myApproachingEdges.find(help)==myApproachingEdges.end()) {
                myApproachingEdges[help] = std::vector<ROEdge*>();
            }
			myApproachingEdges[help].push_back((*rit));
			// add the connection *rit->help to myApproachingEdges
            if(myApproachedEdges.find((*rit))==myApproachedEdges.end()) {
                myApproachedEdges[(*rit)] = std::vector<ROEdge*>();
            }
			myApproachedEdges[(*rit)].push_back(help);

		}
	}

	//debug
    /*
	std::cout << "approaching" << std::endl;
	std::map<std::string, std::vector<std::string> >::iterator it;
	for ( it = myApproachingEdges.begin(); it != myApproachingEdges.end(); it++ )
	{
		std::cout << it->first << std::endl;
		std::vector<std::string>::iterator ti;
		for ( ti = it->second.begin(); ti != it->second.end(); ti++ )
		{
			std::cout << "\t" << (*ti) << std::endl;
		}
	}
	std::cout << "approached" << std::endl;
	//std::map<std::string, std::vector<std::string> >::iterator it;
	for ( it = myApproachedEdges.begin(); it != myApproachedEdges.end(); it++ )
	{
		std::cout << it->first << std::endl;
		std::vector<std::string>::iterator ti;
		for ( ti = it->second.begin(); ti != it->second.end(); ti++ )
		{
			std::cout << "\t" << (*ti) << std::endl;
		}
	}
    */
}


size_t
DFRONet::numberOfEdgesBefore(ROEdge *edge) const
{
    if(myApproachingEdges.find(edge)==myApproachingEdges.end()) {
        return 0;
    }
    return myApproachingEdges.find(edge)->second.size();
}

const std::vector<ROEdge *> &
DFRONet::getEdgesBefore(ROEdge *edge) const
{
    assert(myApproachingEdges.find(edge)!=myApproachingEdges.end());
    return myApproachingEdges.find(edge)->second;
}


size_t
DFRONet::numberOfEdgesAfter(ROEdge *edge) const
{
    if(myApproachedEdges.find(edge)==myApproachedEdges.end()) {
        return 0;
    }
    return myApproachedEdges.find(edge)->second.size();
}


const std::vector<ROEdge *> &
DFRONet::getEdgesAfter(ROEdge *edge) const
{
    assert(myApproachedEdges.find(edge)!=myApproachedEdges.end());
    return myApproachedEdges.find(edge)->second;
}


void
DFRONet::buildDetectorEdgeDependencies(DFDetectorCon &detcont) const
{
    std::vector< DFDetector > &dets = detcont.getDetectors();
    std::map<ROEdge*, std::vector<std::string> > detectorsOnEdges;
    std::map<std::string, ROEdge*> detectorEdges;
    {
        for(std::vector< DFDetector >::iterator i=dets.begin(); i!=dets.end(); ++i) {
            ROEdge *e = getDetectorEdge(*i);

            if(myDetectorsOnEdges.find(e)==myDetectorsOnEdges.end()) {
                myDetectorsOnEdges[e] = std::vector<std::string>();
            }
            myDetectorsOnEdges[e].push_back((*i).getID());

            myDetectorEdges[(*i).getID()] = e;
        }
    }
}


void
DFRONet::computeTypes(DFDetectorCon &detcont) const
{
    std::vector< DFDetector > &dets = detcont.getDetectors();
    // build needed information. first
    buildDetectorEdgeDependencies(detcont);
    // compute detector types then
    {
        for(std::vector< DFDetector >::iterator i=dets.begin(); i!=dets.end(); ++i) {
            if(isSource(*i)) {
                (*i).setType(SOURCE_DETECTOR);
            }
            if(isHighwaySource(*i)) {
                (*i).setType(HIGHWAY_SOURCE_DETECTOR);
            }
            if(isDestination(*i)) {
                (*i).setType(SINK_DETEKTOR);
            }
            if((*i).getType()==TYPE_NOT_DEFINED) {
                (*i).setType(BETWEEN_DETECTOR);
            }
        }
    }
}



void
DFRONet::computeRoutesFor(ROEdge *edge, DFRORouteDesc base, int no,
                          std::vector<ROEdge*> &visited,
                          const DFDetector &det, DFRORouteCont &into) const
{
    visited.push_back(edge);
    if(myApproachedEdges.find(edge)==myApproachedEdges.end()) {
        // ok, no further connections to follow
        DFRORouteDesc tmp = base;
        tmp.duration += edge->getLength()/(100./3.6);
        tmp.probab = -1;
        into.addRouteDesc(tmp);
        return;
    }
    // follow next connections
    const std::vector<ROEdge*> &appr = myApproachedEdges.find(edge)->second;
    std::vector<ROEdge*>::const_iterator i;
    bool cont = true;
    while(cont) {
        cont = false;
        // get fastest next
        ROEdge* best = 0;
        {
            for(i=appr.begin(); i!=appr.end(); i++) {
                ROEdge *c = (*i);
                if(best==0) {
                    best = c;
                } else {
                    if(c->getSpeed()>best->getSpeed()) {
                        best = c;
                    } else if(c->getSpeed()==best->getSpeed() && c->getLaneNo()>best->getLaneNo()) {
                        best = c;
                    }
                }
            }
        }

        std::vector<bool> use(appr.size(), true);
        int idx = 0;
        for(i=appr.begin(); i!=appr.end(); i++, idx++) {
            ROEdge *c = (*i);
            if(find(base.edges2Pass.begin(),  base.edges2Pass.end(), c)!=base.edges2Pass.end()) {
                // already visited ?!
                use[idx] = false;
                continue;
            }

            if(myDetectorsOnEdges.find(c)!=myDetectorsOnEdges.end()) {
                // we have a detector, let's check what it does
                /* !!!
                string det = edge2det[c];
                if(sources.find(det)!=sources.end()) {
                    // ups - a source, may be a loop or something, we will do it anyhow
                    use[idx] = false;
                    continue;
                }
                if(destinations.find(det)!=destinations.end()) {
                        // oha, great, a destination - quit here
                    use[idx] = false;
                    continue;
                }
                */
            }
        }
        // get fastest nexts
        ROEdge *next = 0;
        std::vector<ROEdge*> nexts;
        for(i=appr.begin(), idx=0; i!=appr.end(); i++, idx++) {
            ROEdge *c = (*i);
            if(use[idx]) {
                if(next==0) {
                    next = c;
                } else {
                    if(c->getSpeed()>next->getSpeed()) {
                        next = c;
                    } else if(c->getSpeed()==next->getSpeed() && c->getLaneNo()>next->getLaneNo()) {
                        next = c;
                    }
                }
            }
        }
        if(next->getSpeed()<19||next->getLaneNo()<2) {
            next = 0;
        }
        for(i=appr.begin(), idx=0; i!=appr.end(); i++, idx++) {
            ROEdge *c = (*i);
            if(use[idx]&&next!=0&&c->getSpeed()==next->getSpeed()&&c->getLaneNo()==next->getLaneNo()) {
                DFRORouteDesc t = base;
                t.duration += c->getLength()/(100./3.6);
                t.edges2Pass.push_back(c);
                computeRoutesFor(c, t, 0, visited, det, into);
            } else {
                if(!use[idx]||c->getSpeed()!=next->getSpeed()||c->getLaneNo()!=next->getLaneNo()) {
                    DFRORouteDesc t = base;
                    t.edges2Pass.push_back(c);
                    t.duration += c->getLength()/(100./3.6);
                    into.addRouteDesc(t);
                    /*!!!
                    if(edge2det.find(c)!=edge2det.end()) {
                        string eee = edge2det[c];
                        Tendency ttt = tendencies[edge2det[c]];
                        t.probab = tendencies[edge2det[c]].qKFZ;
                    } else {
                        t.probab = -1;
                    }
                    */
                }
            }
        }
    }
}


void
DFRONet::buildRoutes(DFDetectorCon &detcont, DFRORouteCont &into) const
{
    // build needed information first
    buildDetectorEdgeDependencies(detcont);
    // then build the routes
    std::map<ROEdge*, bool > doneEdges;
    std::vector< DFDetector > &dets = detcont.getDetectors();
    for(std::vector< DFDetector >::iterator i=dets.begin(); i!=dets.end(); ++i) {
        if((*i).getType()==SINK_DETEKTOR) {
            // do not build routes for sinks
            continue;
        }
        ROEdge *e = getDetectorEdge(*i);
        if(doneEdges.find(e)!=doneEdges.end()) {
            // do not build routes twice
            continue;
        }
        doneEdges[e] = true;
        DFRORouteDesc rd;
        rd.edges2Pass.push_back(e);
        rd.duration = 0;
        std::vector<ROEdge*> visited;
        visited.push_back(e);
        computeRoutesFor(e, rd, 0, visited, (*i), into);
    }
}


ROEdge *
DFRONet::getDetectorEdge(const DFDetector &det) const
{
    string edgeName = det.getLaneID();
    edgeName = edgeName.substr(0, edgeName.rfind('_'));
    return ro->getEdge(edgeName);
}


bool
DFRONet::isSource(const DFDetector &det) const
{
    return isSource(det, getDetectorEdge(det), std::vector<ROEdge*>());
}

bool
DFRONet::isHighwaySource(const DFDetector &det) const
{
    return isHighwaySource(det, getDetectorEdge(det), std::vector<ROEdge*>());
}


bool
DFRONet::isDestination(const DFDetector &det) const
{
    return isDestination(det, getDetectorEdge(det), std::vector<ROEdge*>());
}


bool
DFRONet::isSource(const DFDetector &det, ROEdge *edge, std::vector<ROEdge*> seen) const
{
    // it's a source if no edges are approaching the edge
    if(myApproachingEdges.find(edge)==myApproachingEdges.end()||myApproachingEdges.find(edge)->second.size()==0) {
        return true;
    }
    // it is a source if it's outside a highway and
    // a) there is no detector
    // b) there is another detector
    if(edge->getSpeed()<19.4 &&
        (myDetectorsOnEdges.find(edge)==myDetectorsOnEdges.end()
        ||
        std::find(myDetectorsOnEdges.find(edge)->second.begin(), myDetectorsOnEdges.find(edge)->second.end(), det.getID())==myDetectorsOnEdges.find(edge)->second.end()) ) {
        return true;
    }
    // it is not a source if
    //  !!!
    if(myDetectorsOnEdges.find(edge)!=myDetectorsOnEdges.end()
        &&
        myDetectorEdges.find(det.getID())->second!=edge) {
        return false;
    }
    const std::vector<ROEdge*> &appr = myApproachingEdges.find(edge)->second;
    bool isall = true;
    seen.push_back(edge);
    for(int i=0; i<appr.size()&&isall; i++) {
        //printf("checking %s->\n", appr[i].c_str());
        bool had = std::find(seen.begin(), seen.end(), appr[i])!=seen.end();
        if(!had) {
            if(!isSource(det, appr[i], seen)) {
                isall = false;
            }
            //printf("<-yes ");
        } else {
            //printf("<-no ");
        }
        //isall ? printf(" YES\n") : printf(" NO\n");
    }
    return isall;
}

bool
DFRONet::isHighwaySource(const DFDetector &det, ROEdge *edge, std::vector<ROEdge*> seen) const
{
    // it's a source if no edges are approaching the edge
    if(myApproachingEdges.find(edge)==myApproachingEdges.end()||myApproachingEdges.find(edge)->second.size()==0) {
        return true;
    }
    if(edge->getSpeed()<19.4) {
        return false;
    }
    const std::vector<ROEdge*> &appr = myApproachingEdges.find(edge)->second;
    bool isall = true;
    seen.push_back(edge);
    for(int i=0; i<appr.size()&&isall; i++) {
        //printf("checking %s->\n", appr[i].c_str());
        bool had = std::find(seen.begin(), seen.end(), appr[i])!=seen.end();
        if(!had) {
            if(!isHighwaySource(det, appr[i], seen)) {
                isall = false;
            }
            //printf("<-yes ");
        } else {
            //printf("<-no ");
        }
        //isall ? printf(" YES\n") : printf(" NO\n");
    }
    return isall;
}

bool
DFRONet::isDestination(const DFDetector &det, ROEdge *edge, std::vector<ROEdge*> seen) const
{
    if(myApproachedEdges.find(edge)==myApproachedEdges.end()||myApproachedEdges.find(edge)->second.size()==0) {
        return true;
    }
    if(edge->getSpeed()<19.4 &&
        (myDetectorsOnEdges.find(edge)==myDetectorsOnEdges.end()
        ||
        std::find(myDetectorsOnEdges.find(edge)->second.begin(), myDetectorsOnEdges.find(edge)->second.end(), det.getID())==myDetectorsOnEdges.find(edge)->second.end()) ) {
        return true;
    }
    if(myDetectorsOnEdges.find(edge)!=myDetectorsOnEdges.end()
        &&
        myDetectorEdges.find(det.getID())->second!=edge) {
        return false;
    }
    const std::vector<ROEdge*> &appr  = myApproachedEdges.find(edge)->second;
    bool isall = true;
    seen.push_back(edge);
    for(int i=0; i<appr.size()&&isall; i++) {
        //printf("checking %s->\n", appr[i].c_str());
        bool had = std::find(seen.begin(), seen.end(), appr[i])!=seen.end();
        if(!had) {
            if(!isDestination(det, appr[i], seen)) {
                isall = false;
            }
            //printf("<-yes ");
        } else {
            //printf("<-no ");
        }
        //isall ? printf(" YES\n") : printf(" NO\n");
    }
    return isall;
}

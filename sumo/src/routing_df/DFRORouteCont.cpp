/****************************************************************************/
/// @file    DFRORouteCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// A container for routes
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fstream>
#include <cassert>
#include "DFRORouteDesc.h"
#include "DFRORouteCont.h"
#include "RODFNet.h"
#include <router/ROEdge.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
DFRORouteCont::RoutesMap::RoutesMap()
{
}


DFRORouteCont::RoutesMap::~RoutesMap()
{
    for (std::map<ROEdge*, RoutesMap*>::iterator i=splitMap.begin(); i!=splitMap.end(); ++i) {
        delete(*i).second;
    }
}

std::ostream &
operator<<(std::ostream &os, const DFRORouteCont::RoutesMap &rm)
{
    rm.write(os, 0);
    return os;
}


void
DFRORouteCont::RoutesMap::write(std::ostream &os, size_t offset) const
{
    for (size_t j=0; j<offset; ++j) {
        cout << " ";
    }
    if (lastDetectorEdge!=0) {
        os << lastDetectorEdge->getID() << ":";
    } else {
        os << "NULL:";
    }
    for (std::vector<ROEdge*>::const_iterator i=common.begin(); i!=common.end(); ++i) {
        cout << (*i)->getID() << " ";
    }
    cout << endl;
    for (std::map<ROEdge*, RoutesMap*>::const_iterator k=splitMap.begin(); k!=splitMap.end(); ++k) {
        (*k).second->write(os, offset + 3);
    }
}



DFRORouteCont::DFRORouteCont(const RODFNet &net)
        : myNet(net)
{}


DFRORouteCont::~DFRORouteCont()
{
}


void
DFRORouteCont::addRouteDesc(DFRORouteDesc &desc)
{
    // routes may be duplicate as in-between routes may have different starting points
    if (find_if(myRoutes.begin(), myRoutes.end(), route_finder(desc))==myRoutes.end()) {
        myNet.computeID4Route(desc);
        myRoutes.push_back(desc);
    } else {
        DFRORouteDesc &prev = *find_if(myRoutes.begin(), myRoutes.end(), route_finder(desc));
        prev.overallProb += desc.overallProb;
    }
}


bool
DFRORouteCont::removeRouteDesc(DFRORouteDesc &desc)
{
    std::vector<DFRORouteDesc>::const_iterator j = find_if(myRoutes.begin(), myRoutes.end(), route_finder(desc));
    if (j==myRoutes.end()) {
        return false;
    }
    return true;
}


bool
DFRORouteCont::readFrom(const std::string &)
{
    throw 1;
}


bool
DFRORouteCont::save(std::vector<std::string> &saved,
                    const std::string &prependix, OutputDevice& out)
{
    bool haveSavedOnAtLeast = false;
    for (std::vector<DFRORouteDesc>::const_iterator j=myRoutes.begin(); j!=myRoutes.end(); ++j) {
        const DFRORouteDesc &desc = (*j);
        if (find(saved.begin(), saved.end(), desc.routename)!=saved.end()) {
            continue;
        }
        saved.push_back((*j).routename);
        assert(desc.edges2Pass.size()>=1);
        out << "   <route id=\"" << prependix << desc.routename << "\" multi_ref=\"x\">";
        for (std::vector<ROEdge*>::const_iterator k=desc.edges2Pass.begin(); k!=desc.edges2Pass.end(); k++) {
            if (k!=desc.edges2Pass.begin()) {
                out << ' ';
            }
            out << (*k)->getID();
        }
        out << "</route>\n";
        haveSavedOnAtLeast = true;
    }
    return haveSavedOnAtLeast;
}


bool
DFRORouteCont::computed() const
{
    return myRoutes.size()!=0;
}


std::vector<DFRORouteDesc> &
DFRORouteCont::get()
{
    return myRoutes;
}


void
DFRORouteCont::sortByDistance()
{
    sort(myRoutes.begin(), myRoutes.end(), by_distance_sorter());
}


void
DFRORouteCont::setDets2Follow(const std::map<ROEdge*, std::vector<ROEdge*> > &d2f)
{
    myDets2Follow = d2f;
}


const std::map<ROEdge*, std::vector<ROEdge*> > &
DFRORouteCont::getDets2Follow() const
{
    return myDets2Follow;
}


void
DFRORouteCont::removeIllegal(const std::vector<std::vector<ROEdge*> > &illegals)
{
    for (std::vector<DFRORouteDesc>::iterator i=myRoutes.begin(); i!=myRoutes.end();) {
        DFRORouteDesc &desc = *i;
        bool remove = false;
        for (std::vector<std::vector<ROEdge*> >::const_iterator j=illegals.begin(); !remove&&j!=illegals.end(); ++j) {
            int noFound = 0;
            for (std::vector<ROEdge*>::const_iterator k=(*j).begin(); !remove&&k!=(*j).end(); ++k) {
                if (find(desc.edges2Pass.begin(), desc.edges2Pass.end(), *k)!=desc.edges2Pass.end()) {
                    noFound++;
                    if (noFound>1) {
                        remove = true;
                    }
                }
            }
        }
        if (remove) {
            i = myRoutes.erase(i);
        } else {
            ++i;
        }
    }
}


void
DFRORouteCont::determineEndDetector(const RODFNet &net, DFRORouteCont::RoutesMap *rmap) const
{
    rmap->lastDetectorEdge = 0;
    // get and set the last detector on this map's edges
    std::vector<ROEdge*>::const_iterator i1 = rmap->common.begin();
    for (; i1!=rmap->common.end(); ++i1) {
        if (net.hasDetector(*i1)) {
            rmap->lastDetectors = net.getDetectorList(*i1);
            rmap->lastDetectorEdge = *i1;
            break;
        }
    }
    // do this for the followers
    std::map<ROEdge*, RoutesMap*>::const_iterator i2 = rmap->splitMap.begin();
    for (; i2!=rmap->splitMap.end(); ++i2) {
        determineEndDetector(net, (*i2).second);
    }
}


DFRORouteCont::RoutesMap *
DFRORouteCont::getRouteMap(const RODFNet &net) const
{
    DFRORouteCont::RoutesMap *ret = new DFRORouteCont::RoutesMap();
    if (myRoutes.size()>0) {
        std::vector<DFRORouteDesc>::const_iterator i = myRoutes.begin();
        ret->common = (*i).edges2Pass;
        // from here on a valid map is stored in "ret" which must be split/extended
        //  by following routes

        // for each following route
        for (i=myRoutes.begin()+1; i!=myRoutes.end(); ++i) {
            DFRORouteCont::RoutesMap *curr = ret;
            const DFRORouteDesc &d = *i;
            // find the position where the current route splits from the one in curr
            std::vector<ROEdge*>::const_iterator j1, j2;
            j1 = d.edges2Pass.begin();
            j2 = curr->common.begin();
            // we'll iterate until the complete route is stored in the map
            while (j1!=d.edges2Pass.end()) {
                // let's find the first mismatch between the current route and the current route map entry
                while (j1!=d.edges2Pass.end()&&j2!=curr->common.end()&&(*j1)==(*j2)) {
                    ++j1;
                    ++j2;
                }
                // case 1: the current route ends before the end of the map-route
                //  (does this happen anyway?)
                if (j1==d.edges2Pass.end()&&j2!=curr->common.end()) {
                    cout << "End" << endl;
                }
                // case 2: the current route continues, the map-route not; probably an already
                //  existing split
                //  (should be the most common case)
                else if (j1!=d.edges2Pass.end()&&j2==curr->common.end()) {
                    // check whether the split for the net edge is already known by the split
                    ROEdge *next = *j1;
                    if (curr->splitMap.find(next)!=curr->splitMap.end()) {
                        // ok, it's already there; take the first "common" edge as the current position
                        //  within the route map (and the according route map entry)
                        curr = curr->splitMap[next];
                        assert(curr!=0);
                        j2 = curr->common.begin();
                    } else {
                        // nope, it's not in there; generate a new split entry
                        RoutesMap *newSplit = new RoutesMap();
                        for (; j1!=d.edges2Pass.end(); ++j1) {
                            newSplit->common.push_back(*j1);
                        }
                        curr->splitMap[next] = newSplit;
                        // ok, this should also end the loop for this route
                    }
                }
                // case 3: both end; this indicates the routes are same!?
                else if (j1==d.edges2Pass.end()&&j2==curr->common.end()) {
                    cout << "SAme end" << endl;
                }
                // case 3: both in between; a new split has to be inserted here, the old one should be replaces
                else if (j1!=d.edges2Pass.end()&&j2!=curr->common.end()) {
                    // build the continuation for the old map route
                    ROEdge *nextMap = *j2;
                    RoutesMap *mapSplit = new RoutesMap();
                    for (; j2!=curr->common.end(); ++j2) {
                        mapSplit->common.push_back(*j2);
                    }
                    curr->splitMap[nextMap] = mapSplit;
                    // build the continuation for the current route
                    ROEdge *nextCurr = *j1;
                    RoutesMap *currSplit = new RoutesMap();
                    for (; j1!=d.edges2Pass.end(); ++j1) {
                        currSplit->common.push_back(*j1);
                    }
                    curr->splitMap[nextCurr] = currSplit;

                    // let the current split contain edges up to the split edge only
                    std::vector<ROEdge*>::iterator q =
                        find(curr->common.begin(), curr->common.end(), *mapSplit->common.begin());
                    assert(q!=curr->common.end());
                    curr->common.erase(q, curr->common.end());

                    // the new split for this route gets curr
                    curr = currSplit;
                    // ok, this should also end the loop for this route
                } else {
                    throw 1;
                }
            }

        }
    }
    determineEndDetector(net, ret);
    return ret;
}



/****************************************************************************/


//---------------------------------------------------------------------------//
//                        NBLogicKeyBuilder.cpp -
//  The builder of logic keys
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
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
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
#include <strstream>
#include "NBNode.h"
#include "NBEdge.h"
#include "NBContHelper.h"
#include "NBLogicKeyBuilder.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static variable definitions
 * ======================================================================= */
char
NBLogicKeyBuilder::convert[] =
{
  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRST"
};


/* =========================================================================
 * member definitions
 * ======================================================================= */
std::string
NBLogicKeyBuilder::buildKey(NBNode *junction, const EdgeVector * const edges)
{
    ostringstream key;
    key << convert[edges->size()];
    for(EdgeVector::const_iterator i=edges->begin(); i!=edges->end(); i++) {
        // for every edge
        appendEdgeDescription(key, junction, edges, i);
    }
    return string(key.str());
}


string
NBLogicKeyBuilder::rotateKey(string key, int norot)
{
    for(int i=norot; i-->0; ) {
        key = rotateKey(key);
    }
    return key;
}



void
NBLogicKeyBuilder::appendEdgeDescription(std::ostringstream &to,
                                         NBNode *junction,
                                         const EdgeVector * const edges,
                                         EdgeVector::const_iterator &pos)
{
    NBEdge *edge = *pos;
    to << '_';
    // incoming edges will be leaded by a '+', outgoing by a '-'
    if(edge->getToNode()==junction) {
        to << '+';
    } else {
        to << '-';
    }
    // append the number of lanes as a digit
    size_t noLanes = edge->getNoLanes();
    to << (char) (noLanes + '0');
    // append the edge's priority within the junction
    to << convert[edge->getJunctionPriority(junction)];
    // the logic must only by similar for incoming edges
    //  so the outgoing ones will not be described
    if(edge->getToNode()==junction) {
        appendEdgesLaneDescriptions(to, edges, edge, pos);
    }
}


void
NBLogicKeyBuilder::appendEdgesLaneDescriptions(std::ostringstream &to,
                                               const EdgeVector * const edges,
                                               NBEdge *edge,
                                               EdgeVector::const_iterator &pos)
{
    size_t noLanes = edge->getNoLanes();
    // for each lane of the incoming edge
    for(size_t j=0; j<noLanes; j++) {
        // get the connections (edges and lanes) and their number
        const EdgeLaneVector *connected = edge->getEdgeLanesFromLane(j);
        size_t size = connected->size();
        to << convert[size];
        // append the connection descriptions
        appendLaneConnectionDescriptions(to, edges, connected, pos);
    }
}


void
NBLogicKeyBuilder::appendLaneConnectionDescriptions(std::ostringstream &to,
    const EdgeVector * const edges, const EdgeLaneVector *connected,
    EdgeVector::const_iterator &pos)
{
    size_t size = connected->size();
    // go through the connections (edges/lanes)
    for(size_t k=0; k<size; k++) {
        // mark blind connections
        if((*connected)[k].edge==0) {
            to << "0a";
        // compute detailed and rotation-invariant connection
        //  description
        } else {
            appendDetailedConnectionDescription(to, edges, (*connected)[k],
                pos);
        }
    }
}

void
NBLogicKeyBuilder::appendDetailedConnectionDescription(std::ostringstream &to,
    const EdgeVector * const edges, const EdgeLane &edgelane,
    EdgeVector::const_iterator &pos)
{
    // search for the connection end in the request
    size_t eOffset = 0;
    EdgeVector::const_iterator l = pos;
    while(true) {
        // if the connection end was found
        if((*l)==edgelane.edge) {
            // add the distance to the outgoing edge
            //  as a digit to the key
            to << (char) (eOffset + '0');
            // add the destination (outgoing) lane number
            //  as a char to the key
            to << convert[edgelane.lane];
            return;
        } else {
            eOffset++;
            l++;
            if(l==edges->end()) {
                l = edges->begin();
            }
        }
    }
}


string
NBLogicKeyBuilder::rotateKey(const string &run)
{
    int noedges = run.at(0) - 'a';
    string head = run.substr(0, 1);
    vector<string> sub;
    int beg = 2;
    for(int i=0; i<noedges; i++) {
        int end = run.length();
        if(i!=noedges-1)
            end = run.find('_', beg);
        sub.push_back(run.substr(beg, end-beg));
        beg = end + 1;
    }
    string ret = head + "_";
    vector<string>::iterator j;
    for(j=sub.begin()+1; j!=sub.end(); j++) {
        ret += (*j) + "_";
    }
    ret += sub.front();
    return ret;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBLogicKeyBuilder.icc"
//#endif

// Local Variables:
// mode:C++
// End:





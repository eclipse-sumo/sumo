#include <string>
#include <strstream>
#include "NBNode.h"
#include "NBEdge.h"
#include "NBContHelper.h"
#include "NBLogicKeyBuilder.h"

using namespace std;

/* =========================================================================
 * static variable definitions
 * ======================================================================= */
char
NBLogicKeyBuilder::convert[] =
{
  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRST"
};




std::string
NBLogicKeyBuilder::buildKey(NBNode *junction, const EdgeCont * const edges)
{
    ostringstream key;
    key << convert[edges->size()];
    for(EdgeCont::const_iterator i=edges->begin(); i!=edges->end(); i++) {
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
                                         const EdgeCont * const edges,
                                         EdgeCont::const_iterator &pos)
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
                                               const EdgeCont * const edges,
                                               NBEdge *edge,
                                               EdgeCont::const_iterator &pos)
{
    size_t noLanes = edge->getNoLanes();
    // for each lane of the incoming edge
    for(size_t j=0; j<noLanes; j++) {
        // get the connections (edges and lanes) and their number
        const EdgeLaneCont *connected = edge->getEdgeLanesFromLane(j);
        size_t size = connected->size();
        to << convert[size];
        // append the connection descriptions
        appendLaneConnectionDescriptions(to, edges, connected, pos);
    }
}


void
NBLogicKeyBuilder::appendLaneConnectionDescriptions(std::ostringstream &to,
                                                    const EdgeCont * const edges,
                                                    const EdgeLaneCont *connected,
                                                    EdgeCont::const_iterator &pos)
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
                                                       const EdgeCont * const edges,
                                                       const EdgeLane &edgelane,
                                                       EdgeCont::const_iterator &pos)
{
    // search for the connection end in the request
    size_t eOffset = 0;
    EdgeCont::const_iterator l = pos;
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



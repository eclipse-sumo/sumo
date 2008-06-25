/****************************************************************************/
/// @file    NBEdgeCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// A container for all of the nets edges
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

#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <utils/geom/GeomHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include "NBEdgeCont.h"
#include "NBNodeCont.h"
#include "NBHelpers.h"
#include "NBCont.h"
#include "NBTrafficLightLogicCont.h"
#include "NBDistrictCont.h"
#include <cmath>
#include "NBTypeCont.h"
#include <iostream>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// debug definitions
// ===========================================================================
#define DEBUG_OUT cout


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// some definitions (debugging only)
// ===========================================================================
#define DEBUG_OUT cout


// ===========================================================================
// method definitions
// ===========================================================================
NBEdgeCont::NBEdgeCont()
        : myEdgesSplit(0)
{}


NBEdgeCont::~NBEdgeCont()
{
    clear();
}

bool
NBEdgeCont::insert(NBEdge *edge)
{
    EdgeCont::iterator i = myEdges.find(edge->getID());
    if (i!=myEdges.end()) {
        return false;
    }
    if (OptionsCont::getOptions().isSet("edges-min-speed")) {
        if (edge->getSpeed()<OptionsCont::getOptions().getFloat("edges-min-speed")) {
            edge->getFromNode()->removeOutgoing(edge);
            edge->getToNode()->removeIncoming(edge);
            delete edge;
            return true;
        }
    }
    // check whether the edge is a named edge to keep
    if (!OptionsCont::getOptions().getBool("keep-edges.postload")
            &&
            OptionsCont::getOptions().isSet("keep-edges")) {

        if (!OptionsCont::getOptions().isInStringVector("keep-edges", edge->getID())) {
            edge->getFromNode()->removeOutgoing(edge);
            edge->getToNode()->removeIncoming(edge);
            delete edge;
            return true;
        }
    }
    // check whether the edge is a named edge to remove
    if (OptionsCont::getOptions().isSet("remove-edges")) {
        if (OptionsCont::getOptions().isInStringVector("remove-edges", edge->getID())) {
            edge->getFromNode()->removeOutgoing(edge);
            edge->getToNode()->removeIncoming(edge);
            delete edge;
            return true;
        }
    }
    // check whether the edge shall be removed due to a allow an unwished class
    if (OptionsCont::getOptions().isSet("remove-edges.by-vclass")) {
        int matching = 0;
        std::vector<SUMOVehicleClass> allowed = edge->getAllowedVehicleClasses();
        // !!! don't do this each time
        StringTokenizer st(OptionsCont::getOptions().getString("remove-edges.by-vclass"), ";");
        while (st.hasNext()) {
            SUMOVehicleClass vclass = getVehicleClassID(st.next());
            std::vector<SUMOVehicleClass>::iterator i = find(allowed.begin(), allowed.end(), vclass);
            if (i!=allowed.end()) {
                allowed.erase(i);
                matching++;
            }
        }
        // remove the edge if all allowed
        if (allowed.size()==0&&matching!=0) {
            edge->getFromNode()->removeOutgoing(edge);
            edge->getToNode()->removeIncoming(edge);
            delete edge;
            return true;
        }
    }
    if (OptionsCont::getOptions().getBool("dismiss-vclasses")) {
        edge->dismissVehicleClassInformation();
    }
    myEdges.insert(EdgeCont::value_type(edge->getID(), edge));
    return true;
}


NBEdge *
NBEdgeCont::retrieve(const string &id) const
{
    EdgeCont::const_iterator i = myEdges.find(id);
    if (i==myEdges.end()) return 0;
    return (*i).second;
}


void
NBEdgeCont::computeTurningDirections()
{
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->computeTurningDirections();
    }
}


void
NBEdgeCont::sortOutgoingLanesConnections()
{
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->sortOutgoingLanesConnections();
    }
}


void
NBEdgeCont::computeEdge2Edges()
{
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->computeEdge2Edges();
    }
}


void
NBEdgeCont::computeLanes2Edges()
{
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->computeLanes2Edges();
    }
}


void
NBEdgeCont::recheckLanes()
{
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->recheckLanes();
    }
}


/*
bool
NBEdgeCont::computeLinkPriorities(bool verbose)
{
    for(EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->computeLinkPriorities();
    }
    return true;
}
*/

void
NBEdgeCont::appendTurnarounds()
{
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->appendTurnaround();
    }
}


void
NBEdgeCont::writeXMLEdgeList(OutputDevice &into, std::vector<std::string> toAdd)
{
    into << "   <edges no=\"" << (myEdges.size()+toAdd.size()) << "\">";
    for (vector<string>::iterator j=toAdd.begin(); j!=toAdd.end(); j++) {
        if (j!=toAdd.begin()) {
            into << ' ';
        }
        into << (*j);
    }
    if (toAdd.size()!=0) {
        into << ' ';
    }

    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        if (i!=myEdges.begin()) {
            into << ' ';
        }
        into << (*i).first;
    }
    into << "</edges>\n";
}


void
NBEdgeCont::writeXMLStep1(OutputDevice &into)
{
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->writeXMLStep1(into);
    }
    into << "\n";
}


void
NBEdgeCont::writeXMLStep2(OutputDevice &into, bool includeInternal)
{
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->writeXMLStep2(into, includeInternal);
    }
    into << "\n";
}

int NBEdgeCont::size()
{
    return myEdges.size();
}


void
NBEdgeCont::clear()
{
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        delete((*i).second);
    }
    myEdges.clear();
}


bool
NBEdgeCont::splitAt(NBDistrictCont &dc, NBEdge *edge, NBNode *node)
{
    return splitAt(dc, edge, node,
                   edge->getID() + "[0]", edge->getID() + "[1]",
                   edge->myLanes.size(), edge->myLanes.size());
}


bool
NBEdgeCont::splitAt(NBDistrictCont &dc, NBEdge *edge, NBNode *node,
                    const std::string &firstEdgeName,
                    const std::string &secondEdgeName,
                    size_t noLanesFirstEdge, size_t noLanesSecondEdge)
{
    SUMOReal pos;
    pos = edge->getGeometry().nearest_position_on_line_to_point(node->getPosition());
    if (pos<=0) {
        pos = GeomHelper::nearest_position_on_line_to_point(
                  edge->myFrom->getPosition(), edge->myTo->getPosition(),
                  node->getPosition());
    }
    if (pos<=0||pos+POSITION_EPS>edge->getGeometry().length()) {
        return false;
    }
    return splitAt(dc, edge, pos, node, firstEdgeName, secondEdgeName,
                   noLanesFirstEdge, noLanesSecondEdge);
}

bool
NBEdgeCont::splitAt(NBDistrictCont &dc,
                    NBEdge *edge, SUMOReal pos, NBNode *node,
                    const std::string &firstEdgeName,
                    const std::string &secondEdgeName,
                    size_t noLanesFirstEdge, size_t noLanesSecondEdge)
{
    // build the new edges' geometries
    std::pair<Position2DVector, Position2DVector> geoms =
        edge->getGeometry().splitAt(pos);
    if (geoms.first[-1]!=node->getPosition()) {
        geoms.first.pop_back();
        geoms.first.push_back(node->getPosition());
    }

    if (geoms.second[0]!=node->getPosition()) {
        geoms.second.pop_front();
        geoms.second.push_front(node->getPosition());
    }
    // build and insert the edges
    NBEdge *one = new NBEdge(firstEdgeName, 
                             edge->myFrom, node, edge->myType, edge->mySpeed, noLanesFirstEdge,
                             edge->getPriority(), geoms.first, edge->myLaneSpreadFunction);
    size_t i;
    for (i=0; i<noLanesFirstEdge&&i<edge->getNoLanes(); i++) {
        one->setLaneSpeed(i, edge->getLaneSpeed(i));
    }
    NBEdge *two = new NBEdge(secondEdgeName, 
                             node, edge->myTo, edge->myType, edge->mySpeed, noLanesSecondEdge,
                             edge->getPriority(), geoms.second,
                             edge->myLaneSpreadFunction);
    for (i=0; i<noLanesSecondEdge&&i<edge->getNoLanes(); i++) {
        two->setLaneSpeed(i, edge->getLaneSpeed(i));
    }
    two->copyConnectionsFrom(edge);
    // replace information about this edge within the nodes
    edge->myFrom->replaceOutgoing(edge, one, 0);
    edge->myTo->replaceIncoming(edge, two, 0);
    // the edge is now occuring twice in both nodes...
    //  clean up
    edge->myFrom->removeDoubleEdges();
    edge->myTo->removeDoubleEdges();
    // add connections from the first to the second edge
    // check special case:
    //  one in, one out, the outgoing has one lane more
    if (noLanesFirstEdge==noLanesSecondEdge-1) {
        for (i=0; i<one->getNoLanes(); i++) {
            if (!one->addLane2LaneConnection(i, two, i+1, false)) {// !!! Bresenham, here!!!
                throw ProcessError("Could not set connection!");

            }
        }
        one->addLane2LaneConnection(0, two, 0, false);
    } else {
        for (i=0; i<one->getNoLanes()&&i<two->getNoLanes(); i++) {
            if (!one->addLane2LaneConnection(i, two, i, false)) {// !!! Bresenham, here!!!
                throw ProcessError("Could not set connection!");

            }
        }
    }
    // erase the splitted edge
    erase(dc, edge);
    insert(one);
    insert(two);
    myEdgesSplit++;
    return true;
}


void
NBEdgeCont::erase(NBDistrictCont &dc, NBEdge *edge)
{
    myEdges.erase(edge->getID());
    edge->myFrom->removeOutgoing(edge);
    edge->myTo->removeIncoming(edge);
    dc.removeFromSinksAndSources(edge);
    delete edge;
}


NBEdge *
NBEdgeCont::retrievePossiblySplitted(const std::string &id,
                                     const std::string &hint,
                                     bool incoming) const
{
    // try to retrieve using the given name (iterative)
    NBEdge *edge = retrieve(id);
    if (edge!=0) {
        return edge;
    }
    // now, we did not find it; we have to look over all possibilities
    EdgeVector hints;
    // check whether at least the hint was not splitted
    NBEdge *hintedge = retrieve(hint);
    if (hintedge==0) {
        hints = getGeneratedFrom(hint);
    } else {
        hints.push_back(hintedge);
    }
    EdgeVector candidates = getGeneratedFrom(id);
    for (EdgeVector::iterator i=hints.begin(); i!=hints.end(); i++) {
        NBEdge *hintedge = (*i);
        for (EdgeVector::iterator j=candidates.begin(); j!=candidates.end(); j++) {
            NBEdge *poss_searched = (*j);
            NBNode *node = incoming
                           ? poss_searched->myTo : poss_searched->myFrom;
            const EdgeVector &cont = incoming
                                     ? node->getOutgoingEdges() : node->getIncomingEdges();
            if (find(cont.begin(), cont.end(), hintedge)!=cont.end()) {
                return poss_searched;
            }
        }
    }
    return 0;
}


EdgeVector
NBEdgeCont::getGeneratedFrom(const std::string &id) const
{
    size_t len = id.length();
    EdgeVector ret;
    for (EdgeCont::const_iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        string curr = (*i).first;
        // the next check makes it possibly faster - we don not have
        //  to compare the names
        if (curr.length()<=len) {
            continue;
        }
        // the name must be the same as the given id but something
        //  beginning with a '[' must be appended to it
        if (curr.substr(0, len)==id&&curr[len]=='[') {
            ret.push_back((*i).second);
            continue;
        }
        // ok, maybe the edge is a compound made during joining of edges
        size_t pos = curr.find(id);
        // surely not
        if (pos==string::npos) {
            continue;
        }
        // check leading char
        if (pos>0) {
            if (curr[pos-1]!=']'&&curr[pos-1]!='+') {
                // actually, this is another id
                continue;
            }
        }
        if (pos+id.length()<curr.length()) {
            if (curr[pos+id.length()]!='['&&curr[pos+id.length()]!='+') {
                // actually, this is another id
                continue;
            }
        }
        ret.push_back((*i).second);
    }
    return ret;
}

std::vector<std::string>
NBEdgeCont::buildPossibilities(const std::vector<std::string> &s)
{
    std::vector<std::string> ret;
    for (std::vector<std::string>::const_iterator i=s.begin(); i!=s.end(); i++) {
        ret.push_back((*i) + "[0]");
        ret.push_back((*i) + "[1]");
    }
    return ret;
}



void
NBEdgeCont::joinSameNodeConnectingEdges(NBDistrictCont &dc,
                                        NBTrafficLightLogicCont &tlc,
                                        EdgeVector edges)
{
    // !!! Attention!
    //  No merging of the geometry to come is being done
    //  The connections are moved from one edge to another within
    //   the replacement where the edge is a node's incoming edge.

    // count the number of lanes, the speed and the id
    size_t nolanes = 0;
    SUMOReal speed = 0;
    int priority = 0;
    string id;
    sort(edges.begin(), edges.end(), NBContHelper::same_connection_edge_sorter());
    // retrieve the connected nodes
    NBEdge *tpledge = *(edges.begin());
    NBNode *from = tpledge->getFromNode();
    NBNode *to = tpledge->getToNode();
    EdgeVector::const_iterator i;
    for (i=edges.begin(); i!=edges.end(); i++) {
        // some assertions
        assert((*i)->getFromNode()==from);
        assert((*i)->getToNode()==to);
        // ad the number of lanes the current edge has
        nolanes += (*i)->getNoLanes();
        // build the id
        if (i!=edges.begin()) {
            id += "+";
        }
        id += (*i)->getID();
        // compute the speed
        speed += (*i)->getSpeed();
        // build the priority
        if (priority<(*i)->getPriority()) {
            priority = (*i)->getPriority();
        }
    }
    speed /= edges.size();
    // build the new edge
    NBEdge *newEdge = new NBEdge(id, from, to, "", speed,
                                 nolanes, priority, tpledge->myLaneSpreadFunction);
    insert(newEdge);
    // replace old edge by current within the nodes
    //  and delete the old
    from->replaceOutgoing(edges, newEdge);
    to->replaceIncoming(edges, newEdge);
    // patch connections
    //  add edge2edge-information
    for (i=edges.begin(); i!=edges.end(); i++) {
        EdgeVector ev = (*i)->getConnectedEdges();
        for (EdgeVector::iterator j=ev.begin(); j!=ev.end(); j++) {
            newEdge->addEdge2EdgeConnection(*j);
        }
    }
    //  move lane2lane-connections
    size_t currLane = 0;
    for (i=edges.begin(); i!=edges.end(); i++) {
        newEdge->moveOutgoingConnectionsFrom(*i, currLane, false);
        currLane += (*i)->getNoLanes();
    }
    // patch tl-information
    currLane = 0;
    for (i=edges.begin(); i!=edges.end(); i++) {
        size_t noLanes = (*i)->getNoLanes();
        for (size_t j=0; j<noLanes; j++, currLane++) {
            // replace in traffic lights
            tlc.replaceRemoved(*i, j, newEdge, currLane);
        }
    }
    // delete joined edges
    for (i=edges.begin(); i!=edges.end(); i++) {
        erase(dc, *i);
    }
}


NBEdge *
NBEdgeCont::retrievePossiblySplitted(const std::string &id, SUMOReal pos) const
{
    // check whether the edge was not split, yet
    NBEdge *edge = retrieve(id);
    if (edge!=0) {
        return edge;
    }
    // find the part of the edge which matches the position
    SUMOReal seen = 0;
    std::vector<string> names;
    names.push_back(id + "[1]");
    names.push_back(id + "[0]");
    while (true) {
        // retrieve the first subelement (to follow)
        string cid = names[names.size()-1];
        names.pop_back();
        edge = retrieve(cid);
        // The edge was splitted; check its subparts within the
        //  next step
        if (edge==0) {
            names.push_back(cid + "[1]");
            names.push_back(cid + "[0]");
        }
        // an edge with the name was found,
        //  check whether the position lies within it
        else {
            seen += edge->getLength();
            if (seen>=pos) {
                return edge;
            }
        }
    }
    return 0;
}


void
NBEdgeCont::search(NBEdge *e)
{
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        DEBUG_OUT << (*i).second << ", " << (*i).second->getID() << endl;
        if ((*i).second==e) {
            int checkdummy = 0;
            checkdummy += 2;
        }
    }
    DEBUG_OUT << "--------------------------------\n";
}


void
NBEdgeCont::normaliseEdgePositions()
{
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->normalisePosition();
    }
}


void
NBEdgeCont::reshiftEdgePositions(SUMOReal xoff, SUMOReal yoff, SUMOReal rot)
{
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->reshiftPosition(xoff, yoff, rot);
    }
}


void
NBEdgeCont::computeEdgeShapes()
{
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->computeEdgeShape();
    }
}


std::vector<std::string>
NBEdgeCont::getAllNames()
{
    std::vector<std::string> ret;
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); ++i) {
        ret.push_back((*i).first);
    }
    return ret;
}


bool
NBEdgeCont::savePlain(const std::string &file)
{
    OutputDevice& device = OutputDevice::getDevice(file);
    device.writeXMLHeader("edges");
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        NBEdge *e = (*i).second;
        device << "   <edge id=\"" << e->getID()
        << "\" fromnode=\"" << e->getFromNode()->getID()
        << "\" tonode=\"" << e->getToNode()->getID()
        << "\" nolanes=\"" << e->getNoLanes()
        << "\" speed=\"" << e->getSpeed() << "\"";
        // write the geometry only if larger than just the from/to positions
        if (e->getGeometry().size()>2) {
            device.setPrecision(10);
            device << " shape=\"" << e->getGeometry() << "\"";
            device.setPrecision();
        }
        // write the spread type if not default ("right")
        if (e->getLaneSpreadFunction()!=NBEdge::LANESPREAD_RIGHT) {
            device << " spread_type=\"center\"";
        }
        // write the vehicles class if restrictions exist
        if (!e->hasRestrictions()) {
            device << "/>\n";
        } else {
            device << ">\n";
            e->writeLanesPlain(device);
            device << "   </edge>\n";
        }
    }
    device.close();
    return true;
}


void
NBEdgeCont::removeUnwishedEdges(NBDistrictCont &dc, OptionsCont &)
{
    //
    std::vector<NBEdge*> toRemove;
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end();) {
        NBEdge *edge = (*i).second;
        if (!OptionsCont::getOptions().isInStringVector("keep-edges", edge->getID())) {
            edge->getFromNode()->removeOutgoing(edge);
            edge->getToNode()->removeIncoming(edge);
            toRemove.push_back(edge);
        }
        ++i;
    }
    for (std::vector<NBEdge*>::iterator j=toRemove.begin(); j!=toRemove.end(); ++j) {
        erase(dc, *j);
    }
}


void
NBEdgeCont::recomputeLaneShapes()
{
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); ++i) {
        (*i).second->computeLaneShapes();
    }
}


void
NBEdgeCont::splitGeometry(NBNodeCont &nc)
{
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); ++i) {
        if ((*i).second->getGeometry().size()<3) {
            continue;
        }
        (*i).second->splitGeometry(*this, nc);
    }
}


void
NBEdgeCont::recheckLaneSpread()
{
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); ++i) {
        string oppositeID;
        if ((*i).first[0]=='-') {
            oppositeID = (*i).first.substr(1);
        } else {
            oppositeID = "-" + (*i).first;
        }
        if (myEdges.find(oppositeID)!=myEdges.end()) {
            (*i).second->setLaneSpreadFunction(NBEdge::LANESPREAD_RIGHT);
            myEdges.find(oppositeID)->second->setLaneSpreadFunction(NBEdge::LANESPREAD_RIGHT);
        } else {
            (*i).second->setLaneSpreadFunction(NBEdge::LANESPREAD_CENTER);
        }
    }
}


void
NBEdgeCont::recheckEdgeGeomsForDoublePositions()
{
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); ++i) {
        (*i).second->recheckEdgeGeomForDoublePositions();
    }
}


size_t
NBEdgeCont::getNoEdgeSplits()
{
    return myEdgesSplit;
}

/****************************************************************************/

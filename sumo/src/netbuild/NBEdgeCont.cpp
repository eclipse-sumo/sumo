/****************************************************************************/
/// @file    NBEdgeCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Storage for edges, including some functionality operating on multiple edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/TplConvert.h>
#include <utils/options/OptionsCont.h>
#include "NBNetBuilder.h"
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NBEdgeCont::NBEdgeCont(NBNetBuilder &tc) throw()
        : myNetBuilder(tc), myEdgesSplit(0) {
}


NBEdgeCont::~NBEdgeCont() throw() {
    clear();
}


void
NBEdgeCont::applyOptions(OptionsCont &oc) {
    // set edges dismiss/accept options
    myEdgesMinSpeed = oc.isSet("edges-min-speed") ? oc.getFloat("edges-min-speed") : -1;
    myRemoveEdgesAfterJoining = OptionsCont::getOptions().getBool("keep-edges.postload");
    myEdges2Keep = oc.isSet("keep-edges") ? oc.getStringVector("keep-edges") : vector<string>();
    myEdges2Remove = oc.isSet("remove-edges") ? oc.getStringVector("remove-edges") : vector<string>();
    if (oc.isSet("remove-edges.by-vclass")) {
        vector<string> classes = oc.getStringVector("remove-edges.by-vclass");
        for (vector<string>::iterator i=classes.begin(); i!=classes.end(); ++i) {
            myVehicleClasses2Remove.insert(getVehicleClassID(*i));
        }
    }
    if (oc.isSet("keep-edges.by-vclass")) {
        vector<string> classes = oc.getStringVector("keep-edges.by-vclass");
        for (vector<string>::iterator i=classes.begin(); i!=classes.end(); ++i) {
            myVehicleClasses2Keep.insert(getVehicleClassID(*i));
        }
    }
    if (oc.isSet("keep-edges.in-boundary")) {
        vector<string> polyS = oc.getStringVector("keep-edges.in-boundary");
        // !!! throw something if length<4 || length%2!=0?
        vector<SUMOReal> poly;
        for (vector<string>::iterator i=polyS.begin(); i!=polyS.end(); ++i) {
            poly.push_back(TplConvert<char>::_2SUMOReal((*i).c_str())); // !!! may throw something anyhow...
        }
        if (poly.size()==4) {
            // prunning boundary (box)
            myPrunningBoundary.push_back(Position2D(poly[0], poly[1]));
            myPrunningBoundary.push_back(Position2D(poly[2], poly[1]));
            myPrunningBoundary.push_back(Position2D(poly[2], poly[3]));
            myPrunningBoundary.push_back(Position2D(poly[0], poly[3]));
        } else {
            for (vector<SUMOReal>::iterator j=poly.begin(); j!=poly.end();) {
                SUMOReal x = *j++;
                SUMOReal y = *j++;
                myPrunningBoundary.push_back(Position2D(x, y));
            }
        }
    }
}


void
NBEdgeCont::clear() throw() {
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        delete((*i).second);
    }
    myEdges.clear();
}



// ----- edge access methods
bool
NBEdgeCont::insert(NBEdge *edge, bool ignorePrunning) throw() {
    EdgeCont::iterator i = myEdges.find(edge->getID());
    if (i!=myEdges.end()) {
        return false;
    }
    if (ignorePrunning) {
        myEdges.insert(EdgeCont::value_type(edge->getID(), edge));
        return true;
    }
    // remove edges which allow a speed below a set one (set using "edges-min-speed")
    if (edge->getSpeed()<myEdgesMinSpeed) {
        edge->getFromNode()->removeOutgoing(edge);
        edge->getToNode()->removeIncoming(edge);
        delete edge;
        return true;
    }
    // check whether the edge is a named edge to keep
    if (!myRemoveEdgesAfterJoining && myEdges2Keep.size()!=0) {
        if (find(myEdges2Keep.begin(), myEdges2Keep.end(), edge->getID())==myEdges2Keep.end()) {
            edge->getFromNode()->removeOutgoing(edge);
            edge->getToNode()->removeIncoming(edge);
            delete edge;
            return true;
        }
    }
    // check whether the edge is a named edge to remove
    if (myEdges2Remove.size()!=0) {
        if (find(myEdges2Remove.begin(), myEdges2Remove.end(), edge->getID())!=myEdges2Remove.end()) {
            edge->getFromNode()->removeOutgoing(edge);
            edge->getToNode()->removeIncoming(edge);
            delete edge;
            return true;
        }
    }
    // check whether the edge shall be removed due to allowing unwished classes only
    if (myVehicleClasses2Remove.size()!=0) {
        int matching = 0;
        std::vector<SUMOVehicleClass> allowed = edge->getAllowedVehicleClasses();
        for (set<SUMOVehicleClass>::const_iterator i=myVehicleClasses2Remove.begin(); i!=myVehicleClasses2Remove.end(); ++i) {
            std::vector<SUMOVehicleClass>::iterator j = find(allowed.begin(), allowed.end(), *i);
            if (j!=allowed.end()) {
                allowed.erase(j);
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
    // check whether the edge shall be removed due to a allow an unwished class
    if (myVehicleClasses2Keep.size()!=0) {
        int matching = 0;
        std::vector<SUMOVehicleClass> allowed = edge->getAllowedVehicleClasses();
        for (set<SUMOVehicleClass>::const_iterator i=myVehicleClasses2Remove.begin(); i!=myVehicleClasses2Remove.end(); ++i) {
            std::vector<SUMOVehicleClass>::iterator j = find(allowed.begin(), allowed.end(), *i);
            if (j!=allowed.end()) {
                allowed.erase(j);
                matching++;
            }
        }
        // remove the edge if all allowed
        if (matching==0&&allowed.size()!=0) {
            edge->getFromNode()->removeOutgoing(edge);
            edge->getToNode()->removeIncoming(edge);
            delete edge;
            return true;
        }
    }
    // check whether the edge is within the prunning boundary
    if (myPrunningBoundary.size()!=0) {
        Position2D p;
        if (!(edge->getGeometry().getBoxBoundary().grow((SUMOReal) POSITION_EPS).overlapsWith(myPrunningBoundary))) {
            edge->getFromNode()->removeOutgoing(edge);
            edge->getToNode()->removeIncoming(edge);
            delete edge;
            return true;
        }
    }
    if(myNetBuilder.getTypeCont().knows(edge->getTypeID())&&myNetBuilder.getTypeCont().getShallBeDiscarded(edge->getTypeID())) {
            edge->getFromNode()->removeOutgoing(edge);
            edge->getToNode()->removeIncoming(edge);
            delete edge;
            return true;
    }

    if (OptionsCont::getOptions().getBool("dismiss-vclasses")) {
        edge->dismissVehicleClassInformation();
    }
    myEdges.insert(EdgeCont::value_type(edge->getID(), edge));
    return true;
}


NBEdge *
NBEdgeCont::retrieve(const string &id) const throw() {
    EdgeCont::const_iterator i = myEdges.find(id);
    if (i==myEdges.end()) return 0;
    return (*i).second;
}


NBEdge *
NBEdgeCont::retrievePossiblySplitted(const std::string &id,
                                     const std::string &hint,
                                     bool incoming) const throw() {
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


NBEdge *
NBEdgeCont::retrievePossiblySplitted(const std::string &id, SUMOReal pos) const throw() {
    // check whether the edge was not split, yet
    NBEdge *edge = retrieve(id);
    if (edge!=0) {
        return edge;
    }
    size_t maxLength = 0;
    string tid = id + "[";
    for (EdgeCont::const_iterator i=myEdges.begin(); i!=myEdges.end(); ++i) {
        if ((*i).first.find(tid)==0) {
            maxLength = MAX2(maxLength, (*i).first.length());
        }
    }
    // find the part of the edge which matches the position
    SUMOReal seen = 0;
    std::vector<string> names;
    names.push_back(id + "[1]");
    names.push_back(id + "[0]");
    while (names.size()>0) {
        // retrieve the first subelement (to follow)
        string cid = names[names.size()-1];
        names.pop_back();
        edge = retrieve(cid);
        // The edge was splitted; check its subparts within the
        //  next step
        if (edge==0) {
            if (cid.length()+3<maxLength) {
                names.push_back(cid + "[1]");
                names.push_back(cid + "[0]");
            }
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
NBEdgeCont::erase(NBDistrictCont &dc, NBEdge *edge) throw() {
    myEdges.erase(edge->getID());
    edge->myFrom->removeOutgoing(edge);
    edge->myTo->removeIncoming(edge);
    dc.removeFromSinksAndSources(edge);
    delete edge;
}



// ----- explicite edge manipulation methods
bool
NBEdgeCont::splitAt(NBDistrictCont &dc, NBEdge *edge, NBNode *node) throw(ProcessError) {
    return splitAt(dc, edge, node, edge->getID() + "[0]", edge->getID() + "[1]",
                   (unsigned int) edge->myLanes.size(), (unsigned int) edge->myLanes.size());
}


bool
NBEdgeCont::splitAt(NBDistrictCont &dc, NBEdge *edge, NBNode *node,
                    const std::string &firstEdgeName,
                    const std::string &secondEdgeName,
                    unsigned int noLanesFirstEdge, unsigned int noLanesSecondEdge) throw(ProcessError) {
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
                    unsigned int noLanesFirstEdge, unsigned int noLanesSecondEdge) throw(ProcessError) {
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
                             edge->getPriority(), geoms.first, true, edge->myLaneSpreadFunction);
    for (unsigned int i=0; i<noLanesFirstEdge&&i<edge->getNoLanes(); i++) {
        one->setLaneSpeed(i, edge->getLaneSpeed(i));
    }
    NBEdge *two = new NBEdge(secondEdgeName,
                             node, edge->myTo, edge->myType, edge->mySpeed, noLanesSecondEdge,
                             edge->getPriority(), geoms.second, true,
                             edge->myLaneSpreadFunction);
    for (unsigned int i=0; i<noLanesSecondEdge&&i<edge->getNoLanes(); i++) {
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
        for (unsigned int i=0; i<one->getNoLanes(); i++) {
            if (!one->addLane2LaneConnection(i, two, i+1, NBEdge::L2L_COMPUTED)) {// !!! Bresenham, here!!!
                throw ProcessError("Could not set connection!");
            }
        }
        one->addLane2LaneConnection(0, two, 0, NBEdge::L2L_COMPUTED);
    } else {
        for (unsigned int i=0; i<one->getNoLanes()&&i<two->getNoLanes(); i++) {
            if (!one->addLane2LaneConnection(i, two, i, NBEdge::L2L_COMPUTED)) {// !!! Bresenham, here!!!
                throw ProcessError("Could not set connection!");
            }
        }
    }
    // erase the splitted edge
    erase(dc, edge);
    insert(one, true);
    insert(two, true);
    myEdgesSplit++;
    return true;
}



// ----- container access methods
std::vector<std::string>
NBEdgeCont::getAllNames() const throw() {
    std::vector<std::string> ret;
    for (EdgeCont::const_iterator i=myEdges.begin(); i!=myEdges.end(); ++i) {
        ret.push_back((*i).first);
    }
    return ret;
}



// ----- processing methods
void
NBEdgeCont::computeTurningDirections() throw() {
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->computeTurningDirections();
    }
}


void
NBEdgeCont::sortOutgoingLanesConnections() throw() {
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->sortOutgoingLanesConnections();
    }
}


void
NBEdgeCont::computeEdge2Edges() throw() {
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->computeEdge2Edges();
    }
}


void
NBEdgeCont::computeLanes2Edges() throw() {
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->computeLanes2Edges();
    }
}


void
NBEdgeCont::recheckLanes() throw() {
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->recheckLanes();
    }
}


void
NBEdgeCont::appendTurnarounds(bool noTLSControlled) throw() {
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->appendTurnaround(noTLSControlled);
    }
}


void
NBEdgeCont::normaliseEdgePositions() throw() {
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->normalisePosition();
    }
}


void
NBEdgeCont::reshiftEdgePositions(SUMOReal xoff, SUMOReal yoff, SUMOReal rot) throw() {
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->reshiftPosition(xoff, yoff, rot);
    }
}


void
NBEdgeCont::computeEdgeShapes() throw() {
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->computeEdgeShape();
    }
}


void
NBEdgeCont::recomputeLaneShapes() throw() {
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); ++i) {
        (*i).second->computeLaneShapes();
    }
}


void
NBEdgeCont::joinSameNodeConnectingEdges(NBDistrictCont &dc,
                                        NBTrafficLightLogicCont &tlc,
                                        EdgeVector edges) throw() {
    // !!! Attention!
    //  No merging of the geometry to come is being done
    //  The connections are moved from one edge to another within
    //   the replacement where the edge is a node's incoming edge.

    // count the number of lanes, the speed and the id
    unsigned int nolanes = 0;
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
    insert(newEdge, true);
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
    unsigned int currLane = 0;
    for (i=edges.begin(); i!=edges.end(); i++) {
        newEdge->moveOutgoingConnectionsFrom(*i, currLane);
        currLane += (*i)->getNoLanes();
    }
    // patch tl-information
    currLane = 0;
    for (i=edges.begin(); i!=edges.end(); i++) {
        unsigned int noLanes = (*i)->getNoLanes();
        for (unsigned int j=0; j<noLanes; j++, currLane++) {
            // replace in traffic lights
            tlc.replaceRemoved(*i, j, newEdge, currLane);
        }
    }
    // delete joined edges
    for (i=edges.begin(); i!=edges.end(); i++) {
        erase(dc, *i);
    }
}


void
NBEdgeCont::removeUnwishedEdges(NBDistrictCont &dc) throw() {
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
NBEdgeCont::splitGeometry(NBNodeCont &nc) throw() {
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); ++i) {
        if ((*i).second->getGeometry().size()<3) {
            continue;
        }
        (*i).second->splitGeometry(*this, nc);
    }
}


void
NBEdgeCont::recheckLaneSpread() throw() {
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



// ----- output methods
void
NBEdgeCont::writeXMLStep1(OutputDevice &into) throw(IOError) {
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->writeXMLStep1(into);
    }
    into << "\n";
}


void
NBEdgeCont::writeXMLStep2(OutputDevice &into, bool includeInternal) throw(IOError) {
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        (*i).second->writeXMLStep2(into, includeInternal);
    }
    into << "\n";
}



void
NBEdgeCont::savePlain(const std::string &efile, const std::string &cfile) throw(IOError) {
    OutputDevice& edevice = OutputDevice::getDevice(efile);
    edevice.writeXMLHeader("edges");
    OutputDevice& cdevice = OutputDevice::getDevice(cfile);
    cdevice.writeXMLHeader("connections");
    for (EdgeCont::iterator i=myEdges.begin(); i!=myEdges.end(); i++) {
        // write the edge itself to the edges-files
        NBEdge *e = (*i).second;
        edevice << "   <edge id=\"" << e->getID()
        << "\" fromnode=\"" << e->getFromNode()->getID()
        << "\" tonode=\"" << e->getToNode()->getID()
        << "\" nolanes=\"" << e->getNoLanes()
        << "\" speed=\"" << e->getSpeed() << "\"";
        // write the geometry only if larger than just the from/to positions
        edevice << " shape=\"" << e->getGeometry() << "\"";
        // write the spread type if not default ("right")
        if (e->getLaneSpreadFunction()!=NBEdge::LANESPREAD_RIGHT) {
            edevice << " spread_type=\"center\"";
        }
        // write the vehicles class if restrictions exist
        if (!e->hasRestrictions()) {
            edevice << "/>\n";
        } else {
            edevice << ">\n";
            e->writeLanesPlain(edevice);
            edevice << "   </edge>\n";
        }
        // write this edge's connections to the connections-files
        unsigned int noLanes = e->getNoLanes();
        unsigned int noWritten = 0;
        for (unsigned int lane=0; lane<noLanes; ++lane) {
            vector<NBEdge::Connection> connections = e->getConnectionsFromLane(lane);
            for (vector<NBEdge::Connection>::iterator c=connections.begin(); c!=connections.end(); ++c) {
                if ((*c).toEdge!=0) {
                    cdevice << "	<connection from=\"" << e->getID()
                    << "\" to=\"" << (*c).toEdge->getID()
                    << "\" lane=\"" << (*c).fromLane << ":" << (*c).toLane;
                    cdevice << "\"/>\n";
                    ++noWritten;
                }
            }
        }
        if (noWritten>0) {
            cdevice << "\n";
        }
    }
    edevice.close();
    cdevice.close();
}



// ----- other
EdgeVector
NBEdgeCont::getGeneratedFrom(const std::string &id) const throw() {
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


/****************************************************************************/

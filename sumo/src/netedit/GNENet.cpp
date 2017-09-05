/****************************************************************************/
/// @file    GNENet.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A visual container for GNE-network-components such as GNEEdge and GNEJunction.
// GNE components wrap netbuild-components and supply visualisation and editing
// capabilities (adapted from GUINet)
//
// Workflow (rough draft)
//   use NILoader to fill
//   do netedit stuff
//   call compute to save results
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <map>
#include <set>
#include <vector>

#include <netbuild/NBAlgorithms.h>
#include <netwrite/NWFrame.h>
#include <netwrite/NWWriter_XML.h>
#include <utility>
#include <utils/common/MsgHandler.h>
#include <utils/common/RGBColor.h>
#include <utils/common/StringUtils.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/XMLSubSys.h>

#include "GNEAdditional.h"
#include "GNEAdditionalFrame.h"
#include "GNEApplicationWindow.h"
#include "GNEChange_Additional.h"
#include "GNEChange_Attribute.h"
#include "GNEChange_Connection.h"
#include "GNEChange_Crossing.h"
#include "GNEChange_Edge.h"
#include "GNEChange_Junction.h"
#include "GNEChange_Lane.h"
#include "GNEChange_Selection.h"
#include "GNEChange_Poly.h"
#include "GNEChange_POI.h"
#include "GNEConnection.h"
#include "GNECrossing.h"
#include "GNEDetector.h"
#include "GNEDetectorE2.h"
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNEPoly.h"
#include "GNEPOI.h"
#include "GNELane.h"
#include "GNENet.h"
#include "GNEStoppingPlace.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"
#include "GNERerouter.h"
#include "GNEAdditionalHandler.h"
#include "GNEDialog_FixAdditionalPositions.h"


FXIMPLEMENT_ABSTRACT(GNENet::GNEChange_ReplaceEdgeInTLS, GNEChange, NULL, 0)

// ===========================================================================
// static members
// ===========================================================================
const RGBColor GNENet::selectionColor(0, 0, 204, 255);
const RGBColor GNENet::selectedLaneColor(0, 0, 128, 255);
const RGBColor GNENet::selectedConnectionColor(0, 0, 100, 255);
const RGBColor GNENet::selectedAdditionalColor(0, 0, 150, 255);
const double GNENet::Z_INITIALIZED = 1;

// ===========================================================================
// member method definitions
// ===========================================================================
GNENet::GNENet(NBNetBuilder* netBuilder) :
    GUIGlObject(GLO_NETWORK, ""),
    ShapeContainer(),
    myViewNet(0),
    myNetBuilder(netBuilder),
    myJunctions(),
    myEdges(),
    myEdgeIDSupplier("gneE", netBuilder->getEdgeCont().getAllNames()),
    myJunctionIDSupplier("gneJ", netBuilder->getNodeCont().getAllNames()),
    myNeedRecompute(true),
    myAdditionalsSaved(true),
    myShapesSaved(true) {
    // set net in gIDStorage
    GUIGlObjectStorage::gIDStorage.setNetObject(this);

    // init junction and edges
    initJunctionsAndEdges();

    // check Z boundary
    if (myZBoundary.ymin() != Z_INITIALIZED) {
        myZBoundary.add(0, 0);
    }
}


GNENet::~GNENet() {
    // Drop Edges
    for (GNEEdges::iterator it = myEdges.begin(); it != myEdges.end(); it++) {
        it->second->decRef("GNENet::~GNENet");
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Deleting unreferenced " + toString(it->second->getTag()) + " '" + it->second->getID() + "' in GNENet destructor");
        }
        delete it->second;
    }
    // Drop junctions
    for (GNEJunctions::iterator it = myJunctions.begin(); it != myJunctions.end(); it++) {
        it->second->decRef("GNENet::~GNENet");
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Deleting unreferenced " + toString(it->second->getTag()) + " '" + it->second->getID() + "' in GNENet destructor");
        }
        delete it->second;
    }


    // show extra information for tests
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Deleting net builder in GNENet destructor");
    }
    delete myNetBuilder;
}


const Boundary&
GNENet::getBoundary() const {
    // SUMORTree is also a Boundary
    return myGrid;
}


GUIGLObjectPopupMenu*
GNENet::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow*
GNENet::getParameterWindow(GUIMainWindow&, GUISUMOAbstractView&) {
    return 0;
}


void
GNENet::drawGL(const GUIVisualizationSettings& /*s*/) const {
}


Boundary
GNENet::getCenteringBoundary() const {
    return getBoundary();
}


const Boundary&
GNENet::getZBoundary() const {
    return myZBoundary;
}


SUMORTree&
GNENet::getVisualisationSpeedUp() {
    return myGrid;
}


const SUMORTree&
GNENet::getVisualisationSpeedUp() const {
    return myGrid;
}


GNEJunction*
GNENet::createJunction(const Position& pos, GNEUndoList* undoList) {
    std::string id = myJunctionIDSupplier.getNext();
    NBNode* nbn = new NBNode(id, pos);
    GNEJunction* junction = new GNEJunction(*nbn, this);
    undoList->add(new GNEChange_Junction(junction, true), true);
    assert(myJunctions[id]);
    return junction;
}


GNEEdge*
GNENet::createEdge(
    GNEJunction* src, GNEJunction* dest, GNEEdge* tpl, GNEUndoList* undoList,
    const std::string& suggestedName,
    bool wasSplit,
    bool allowDuplicateGeom) {
    // prevent duplicate edge (same geometry)
    const EdgeVector& outgoing = src->getNBNode()->getOutgoingEdges();
    for (EdgeVector::const_iterator it = outgoing.begin(); it != outgoing.end(); it++) {
        if ((*it)->getToNode() == dest->getNBNode() && (*it)->getGeometry().size() == 2) {
            if (!allowDuplicateGeom) {
                return 0;
            }
        }
    }

    std::string id;
    if (suggestedName != "" && !retrieveEdge(suggestedName, false)) {
        id = suggestedName;
        reserveEdgeID(id);
    } else {
        id = myEdgeIDSupplier.getNext();
    }

    GNEEdge* edge;
    if (tpl) {
        NBEdge* nbeTpl = tpl->getNBEdge();
        NBEdge* nbe = new NBEdge(id, src->getNBNode(), dest->getNBNode(), nbeTpl);
        edge = new GNEEdge(*nbe, this, wasSplit);
    } else {
        // default if no template is given
        double defaultSpeed = 50 / 3.6;
        std::string defaultType = "";
        int defaultNrLanes = 1;
        int defaultPriority = 1;
        double defaultWidth = NBEdge::UNSPECIFIED_WIDTH;
        double defaultOffset = NBEdge::UNSPECIFIED_OFFSET;
        NBEdge* nbe = new NBEdge(id, src->getNBNode(), dest->getNBNode(),
                                 defaultType, defaultSpeed,
                                 defaultNrLanes, defaultPriority,
                                 defaultWidth,
                                 defaultOffset);
        edge = new GNEEdge(*nbe, this, wasSplit);
    }
    undoList->p_begin("create " + toString(SUMO_TAG_EDGE));
    undoList->add(new GNEChange_Edge(edge, true), true);
    src->setLogicValid(false, undoList);
    dest->setLogicValid(false, undoList);
    requireRecompute();
    undoList->p_end();
    assert(myEdges[id]);
    return edge;
}


void
GNENet::deleteJunction(GNEJunction* junction, GNEUndoList* undoList) {
    // we have to delete all incident edges because they cannot exist without that junction
    // all deletions must be undone/redone together so we start a new command group
    // @todo if any of those edges are dead-ends should we remove their orphan junctions as well?
    undoList->p_begin("delete " + toString(SUMO_TAG_JUNCTION));

    // delete all crossings vinculated with junction
    while (junction->getGNECrossings().size() > 0) {
        deleteCrossing(junction->getGNECrossings().front(), undoList);
    }

    // find all crossings of neightbour junctions that shares an edge of this junction
    std::vector<GNECrossing*> crossingsToRemove;
    std::vector<GNEJunction*> junctionNeighbours = junction->getJunctionNeighbours();
    for(std::vector<GNEJunction*>::iterator i = junctionNeighbours.begin(); i != junctionNeighbours.end(); i++) {
        std::vector<GNECrossing*> crossingsOfJunctionNeighbours = (*i)->getGNECrossings();
        // iterate over crossing of neighbour juntion
        for(std::vector<GNECrossing*>::iterator j = crossingsOfJunctionNeighbours.begin(); j != crossingsOfJunctionNeighbours.end(); j++) {
            // if at least one of the edges of junction to remove belongs to a crossing of the neighbour junction, delete it
            if((*j)->checkEdgeBelong(junction->getGNEEdges())) {
                crossingsToRemove.push_back(*j);
            }
        }
    }
    
    // delete crossings top remove
    for(std::vector<GNECrossing*>::iterator i = crossingsToRemove.begin(); i != crossingsToRemove.end(); i++) {
        deleteCrossing((*i), undoList);
    }

    // deleting edges changes in the underlying EdgeVector so we have to make a copy
    const EdgeVector incident = junction->getNBNode()->getEdges();
    for (EdgeVector::const_iterator it = incident.begin(); it != incident.end(); it++) {
        deleteEdge(myEdges[(*it)->getID()], undoList);
    }

    // remove any traffic lights from the traffic light container (avoids lots of warnings)
    junction->setAttribute(SUMO_ATTR_TYPE, toString(NODETYPE_PRIORITY), undoList);

    // save selection status
    if (gSelected.isSelected(GLO_JUNCTION, junction->getGlID())) {
        std::set<GUIGlID> deselected;
        deselected.insert(junction->getGlID());
        undoList->add(new GNEChange_Selection(this, std::set<GUIGlID>(), deselected, true), true);
    }

    // delete edge
    undoList->add(new GNEChange_Junction(junction, false), true);
    undoList->p_end();
}


void
GNENet::deleteEdge(GNEEdge* edge, GNEUndoList* undoList) {
    undoList->p_begin("delete " + toString(SUMO_TAG_EDGE));
    
    // obtain a copy of GNERerouters of edge
    std::vector<GNERerouter*> rerouters = edge->getGNERerouters();
    
    // delete additionals childs of edge
    std::vector<GNEAdditional*> copyOfEdgeAdditionals = edge->getAdditionalChilds();
    for (std::vector<GNEAdditional*>::iterator i = copyOfEdgeAdditionals.begin(); i != copyOfEdgeAdditionals.end(); i++) {
        undoList->add(new GNEChange_Additional((*i), false), true);
    }
    
    // delete additionals childs of lane
    for (std::vector<GNELane*>::const_iterator i = edge->getLanes().begin(); i != edge->getLanes().end(); i++) {
        std::vector<GNEAdditional*> copyOfLaneAdditionals = (*i)->getAdditionalChilds();
        for (std::vector<GNEAdditional*>::iterator j = copyOfLaneAdditionals.begin(); j != copyOfLaneAdditionals.end(); j++) {
            undoList->add(new GNEChange_Additional((*j), false), true);
        }
    }

    // remove edge from crossings related with this edge
    edge->getGNEJunctionSource()->removeEdgeFromCrossings(edge, undoList);
    edge->getGNEJunctionDestiny()->removeEdgeFromCrossings(edge, undoList);

    // invalidate junctions
    edge->getGNEJunctionSource()->setLogicValid(false, undoList);
    edge->getGNEJunctionDestiny()->setLogicValid(false, undoList);

    // save selection status
    if (gSelected.isSelected(GLO_EDGE, edge->getGlID())) {
        std::set<GUIGlID> deselected;
        deselected.insert(edge->getGlID());
        undoList->add(new GNEChange_Selection(this, std::set<GUIGlID>(), deselected, true), true);
    }

    // Delete edge
    undoList->add(new GNEChange_Edge(edge, false), true);

    // check if after removing there are Rerouters without edge Childs
    for (std::vector<GNERerouter*>::iterator i = rerouters.begin(); i != rerouters.end(); i++) {
        if ((*i)->getEdgeChilds().size() == 0) {
            undoList->add(new GNEChange_Additional((*i), false), true);
        }
    }
    // remove edge requieres always a recompute (due geometry and connections)
    requireRecompute();
    undoList->p_end();
}


void
GNENet::replaceIncomingEdge(GNEEdge* which, GNEEdge* by, GNEUndoList* undoList) {
    undoList->p_begin("replace " + toString(SUMO_TAG_EDGE));
    undoList->p_add(new GNEChange_Attribute(by, SUMO_ATTR_TO, which->getAttribute(SUMO_ATTR_TO)));
    
    // replace in additionals childs of edge
    std::vector<GNEAdditional*> copyOfEdgeAdditionals = which->getAdditionalChilds();
    for (std::vector<GNEAdditional*>::iterator i = copyOfEdgeAdditionals.begin(); i != copyOfEdgeAdditionals.end(); i++) {
        undoList->p_add(new GNEChange_Attribute(*i, SUMO_ATTR_EDGE, by->getID()));
    }
    
    // replace in additionals childs of lane
    for (std::vector<GNELane*>::const_iterator i = which->getLanes().begin(); i != which->getLanes().end(); i++) {
        std::vector<GNEAdditional*> copyOfLaneAdditionals = (*i)->getAdditionalChilds();
        for (std::vector<GNEAdditional*>::iterator j = copyOfLaneAdditionals.begin(); j != copyOfLaneAdditionals.end(); j++) {
            undoList->p_add(new GNEChange_Attribute(*i, SUMO_ATTR_LANE, by->getNBEdge()->getLaneID((*i)->getIndex())));
        }
    }
    // replace in rerouters 
    for(auto rerouter : which->getGNERerouters()) {
        replaceInListAttribute(rerouter, SUMO_ATTR_EDGES, which->getID(), by->getID(), undoList);
    }

    // replace in crossings 
    for(auto crossing : which->getGNEJunctionDestiny()->getGNECrossings()) {
        // if at least one of the edges of junction to remove belongs to a crossing of the source junction, delete it
        replaceInListAttribute(crossing, SUMO_ATTR_EDGES, which->getID(), by->getID(), undoList);
    }

    // fix connections (make a copy because they will be modified
    std::vector<NBEdge::Connection> connections = which->getNBEdge()->getConnections();
    for (auto con : connections) {
        undoList->add(new GNEChange_Connection(which, con, false, false), true);
        undoList->add(new GNEChange_Connection(by, con, false, true), true);
    }

    // save selection status
    if (gSelected.isSelected(GLO_EDGE, which->getGlID())) {
        std::set<GUIGlID> deselected;
        deselected.insert(which->getGlID());
        undoList->add(new GNEChange_Selection(this, std::set<GUIGlID>(), deselected, true), true);
    }
    undoList->add(new GNEChange_ReplaceEdgeInTLS(getTLLogicCont(), which->getNBEdge(), by->getNBEdge()), true);

    // Delete edge
    undoList->add(new GNEChange_Edge(which, false), true);

    undoList->p_end();
}


void
GNENet::deleteLane(GNELane* lane, GNEUndoList* undoList) {
    GNEEdge* edge = &lane->getParentEdge();
    if (edge->getNBEdge()->getNumLanes() == 1) {
        // remove the whole edge instead
        deleteEdge(edge, undoList);
    } else {
        undoList->p_begin("delete " + toString(SUMO_TAG_LANE));
        
        // delete additionals childs of lane
        std::vector<GNEAdditional*> copyOfAdditionals = lane->getAdditionalChilds();
        for (std::vector<GNEAdditional*>::const_iterator i = copyOfAdditionals.begin(); i != copyOfAdditionals.end(); i++) {
            undoList->add(new GNEChange_Additional((*i), false), true);
        }
        
        // invalidate junctions (saving connections)
        edge->getGNEJunctionSource()->setLogicValid(false, undoList);
        edge->getGNEJunctionDestiny()->setLogicValid(false, undoList);

        // save selection status
        if (gSelected.isSelected(GLO_EDGE, edge->getGlID())) {
            std::set<GUIGlID> deselected;
            deselected.insert(edge->getGlID());
            undoList->add(new GNEChange_Selection(this, std::set<GUIGlID>(), deselected, true), true);
        }

        // delete lane
        const NBEdge::Lane& laneAttrs = edge->getNBEdge()->getLaneStruct(lane->getIndex());
        undoList->add(new GNEChange_Lane(edge, lane, laneAttrs, false), true);
        if (gSelected.isSelected(GLO_LANE, lane->getGlID())) {
            std::set<GUIGlID> deselected;
            deselected.insert(lane->getGlID());
            undoList->add(new GNEChange_Selection(this, std::set<GUIGlID>(), deselected, true), true);
        }

        // remove lane requieres always a recompute (due geometry and connections)
        requireRecompute();
        undoList->p_end();
    }
}


void
GNENet::deleteConnection(GNEConnection* connection, GNEUndoList* undoList) {
    undoList->p_begin("delete " + toString(SUMO_TAG_CONNECTION));
    // obtain NBConnection to remove
    NBConnection deleted = connection->getNBConnection();
    GNEJunction* junctionDestiny = connection->getEdgeFrom()->getGNEJunctionDestiny();
    junctionDestiny->markAsModified(undoList);
    // check if GNEConnection was previouslyselected, and if true, unselect it.
    bool selected = gSelected.isSelected(GLO_CONNECTION, connection->getGlID());
    if(selected) {
        gSelected.deselect(connection->getGlID());
    }
    undoList->add(new GNEChange_Connection(connection->getEdgeFrom(), connection->getNBEdgeConnection(), selected, false), true);
    junctionDestiny->invalidateTLS(myViewNet->getUndoList(), deleted);
    // remove connection requieres always a recompute (due geometry and connections)
    requireRecompute();
    undoList->p_end();
}


void
GNENet::deleteCrossing(GNECrossing* crossing, GNEUndoList* undoList) {
    undoList->p_begin("delete crossing");
    // check if GNECrossing was previouslyselected, and if true, unselect it.
    bool selected = gSelected.isSelected(GLO_CROSSING, crossing->getGlID());
    if(selected) {
        gSelected.deselect(crossing->getGlID());
    }
    undoList->add(new GNEChange_Crossing(crossing->getParentJunction(), crossing->getNBCrossing()->edges,
                                         crossing->getNBCrossing()->width, crossing->getNBCrossing()->priority, selected, false), true);
    // remove crossing requieres always a recompute (due geometry and connections)
    requireRecompute();
    undoList->p_end();
}


void 
GNENet::deletePOI(GNEPOI* POI, GNEUndoList* undoList) {
    myViewNet->getUndoList()->p_begin("delete " + toString(POI->getTag()));
    // save selection status
    if (gSelected.isSelected(GLO_POI, POI->getGlID())) {
        std::set<GUIGlID> deselected;
        deselected.insert(POI->getGlID());
        undoList->add(new GNEChange_Selection(this, std::set<GUIGlID>(), deselected, true), true);
    }
    // delete POI
    myViewNet->getUndoList()->add(new GNEChange_POI(myViewNet->getNet(), POI, false), true);
    myViewNet->getUndoList()->p_end();
}


void 
GNENet::deletePolygon(GNEPoly* polygon, GNEUndoList* undoList) {
    myViewNet->getUndoList()->p_begin("delete " + toString(polygon->getTag()));
    // save selection status
    if (gSelected.isSelected(GLO_POLYGON, polygon->getGlID())) {
        std::set<GUIGlID> deselected;
        deselected.insert(polygon->getGlID());
        undoList->add(new GNEChange_Selection(this, std::set<GUIGlID>(), deselected, true), true);
    }
    // delete Polygon
    myViewNet->getUndoList()->add(new GNEChange_Poly(myViewNet->getNet(), polygon, false), true);
    myViewNet->getUndoList()->p_end();
}


void
GNENet::duplicateLane(GNELane* lane, GNEUndoList* undoList) {
    undoList->p_begin("duplicate " + toString(SUMO_TAG_LANE));
    GNEEdge* edge = &lane->getParentEdge();
    const NBEdge::Lane& laneAttrs = edge->getNBEdge()->getLaneStruct(lane->getIndex());
    GNELane* newLane = new GNELane(*edge, lane->getIndex());
    undoList->add(new GNEChange_Lane(edge, newLane, laneAttrs, true), true);
    requireRecompute();
    undoList->p_end();
}


bool
GNENet::restrictLane(SUMOVehicleClass vclass, GNELane* lane, GNEUndoList* undoList) {
    bool addRestriction = true;
    if (vclass == SVC_PEDESTRIAN) {
        GNEEdge& edge = lane->getParentEdge();
        for (std::vector<GNELane*>::const_iterator i = edge.getLanes().begin(); i != edge.getLanes().end(); i++) {
            if ((*i)->isRestricted(SVC_PEDESTRIAN)) {
                // prevent adding a 2nd sidewalk
                addRestriction = false;
            } else {
                // ensure that the sidewalk is used exclusively
                const SVCPermissions allOldWithoutPeds = edge.getNBEdge()->getPermissions((*i)->getIndex()) & ~SVC_PEDESTRIAN;
                (*i)->setAttribute(SUMO_ATTR_ALLOW, getVehicleClassNames(allOldWithoutPeds), undoList);
            }
        }
    }
    // restrict the lane
    if (addRestriction) {
        lane->setAttribute(SUMO_ATTR_ALLOW, toString(vclass), undoList);
        return true;
    } else {
        return false;
    }
}


bool
GNENet::revertLaneRestriction(GNELane* lane, GNEUndoList* undoList) {
    // First check that this lane is restricted
    if (lane->isRestricted(SVC_PEDESTRIAN) == false &&
            lane->isRestricted(SVC_BICYCLE) == false &&
            lane->isRestricted(SVC_BUS) == false) {
        return false;
    }

    // Get all possible vehicle classes
    std::vector<std::string> VClasses = SumoVehicleClassStrings.getStrings();
    // Change allow and disallow attributes of lane
    lane->setAttribute(SUMO_ATTR_ALLOW, joinToString(VClasses, " "), undoList);
    lane->setAttribute(SUMO_ATTR_DISALLOW, std::string(), undoList);
    return true;
}


bool
GNENet::addSRestrictedLane(SUMOVehicleClass vclass, GNEEdge& edge, GNEUndoList* undoList) {
    // First check that edge don't have a sidewalk
    for (std::vector<GNELane*>::const_iterator i = edge.getLanes().begin(); i != edge.getLanes().end(); i++) {
        if ((*i)->isRestricted(vclass)) {
            return false;
        }
    }
    // duplicate last lane
    duplicateLane(edge.getLanes().at(0), undoList);
    // transform the created (last) lane to a sidewalk
    return restrictLane(vclass, edge.getLanes()[0], undoList);
}


bool
GNENet::removeRestrictedLane(SUMOVehicleClass vclass, GNEEdge& edge, GNEUndoList* undoList) {
    // iterate over lanes of edge
    for (std::vector<GNELane*>::const_iterator i = edge.getLanes().begin(); i != edge.getLanes().end(); i++) {
        if ((*i)->isRestricted(vclass)) {
            // Delete lane
            deleteLane(*i, undoList);
            return true;
        }
    }
    return false;
}


void
GNENet::deleteGeometryOrEdge(GNEEdge* edge, const Position& pos, GNEUndoList* undoList) {
    if (!edge->deleteGeometry(pos, undoList)) {
        deleteEdge(edge, undoList);
    }
}


GNEJunction*
GNENet::splitEdge(GNEEdge* edge, const Position& pos, GNEUndoList* undoList, GNEJunction* newJunction) {
    undoList->p_begin("split " + toString(SUMO_TAG_EDGE));
    deleteEdge(edge, undoList); // still exists. we delete it so we can reuse the name in case of resplit
    // compute geometry
    const PositionVector& oldGeom = edge->getNBEdge()->getGeometry();
    const double linePos = oldGeom.nearest_offset_to_point2D(pos, false);
    std::pair<PositionVector, PositionVector> newGeoms = oldGeom.splitAt(linePos);
    // figure out the new name
    int posBase = 0;
    std::string baseName = edge->getMicrosimID();
    if (edge->wasSplit()) {
        const std::string::size_type sep_index = baseName.rfind('.');
        if (sep_index != std::string::npos) { // edge may have been renamed in between
            std::string posString = baseName.substr(sep_index + 1);
            try {
                posBase = GNEAttributeCarrier::parse<int>(posString.c_str());
                baseName = baseName.substr(0, sep_index); // includes the .
            } catch (NumberFormatException) {
            }
        }
    }
    baseName += '.';
    // create edges
    if (newJunction == 0) {
        newJunction = createJunction(pos, undoList);
    }
    GNEEdge* firstPart = createEdge(edge->getGNEJunctionSource(), newJunction, edge,
                                    undoList, baseName + toString(posBase), true);
    GNEEdge* secondPart = createEdge(newJunction, edge->getGNEJunctionDestiny(), edge,
                                     undoList, baseName + toString(posBase + (int)linePos), true);
    // fix geometry
    firstPart->setAttribute(GNE_ATTR_SHAPE_START, toString(newGeoms.first[0]), undoList);
    firstPart->setAttribute(GNE_ATTR_SHAPE_END, toString(newGeoms.first[-1]), undoList);
    newGeoms.first.pop_back();
    newGeoms.first.erase(newGeoms.first.begin());
    firstPart->setAttribute(SUMO_ATTR_SHAPE, toString(newGeoms.first), undoList);

    secondPart->setAttribute(GNE_ATTR_SHAPE_START, toString(newGeoms.second[0]), undoList);
    secondPart->setAttribute(GNE_ATTR_SHAPE_END, toString(newGeoms.second[-1]), undoList);
    newGeoms.second.pop_back();
    newGeoms.second.erase(newGeoms.second.begin());
    secondPart->setAttribute(SUMO_ATTR_SHAPE, toString(newGeoms.second), undoList);
    // fix connections
    std::vector<NBEdge::Connection>& connections = edge->getNBEdge()->getConnections();
    for (std::vector<NBEdge::Connection>::iterator con_it = connections.begin(); con_it != connections.end(); con_it++) {
        undoList->add(new GNEChange_Connection(secondPart, *con_it, false, true), true);
    }
    undoList->p_end();
    return newJunction;
}


void
GNENet::splitEdgesBidi(const std::set<GNEEdge*>& edges, const Position& pos, GNEUndoList* undoList) {
    GNEJunction* newJunction = 0;
    undoList->p_begin("split " + toString(SUMO_TAG_EDGE) + "s");
    for (std::set<GNEEdge*>::const_iterator it = edges.begin(); it != edges.end(); ++it) {
        newJunction = splitEdge(*it, pos, undoList, newJunction);
    }
    undoList->p_end();
}


void
GNENet::reverseEdge(GNEEdge* edge, GNEUndoList* undoList) {
    undoList->p_begin("reverse " + toString(SUMO_TAG_EDGE));
    deleteEdge(edge, undoList); // still exists. we delete it so we can reuse the name in case of resplit
    GNEEdge* reversed = createEdge(edge->getGNEJunctionDestiny(), edge->getGNEJunctionSource(), edge, undoList, edge->getID(), false, true);
    assert(reversed != 0);
    reversed->setAttribute(SUMO_ATTR_SHAPE, toString(edge->getNBEdge()->getInnerGeometry().reverse()), undoList);
    undoList->p_end();
}


GNEEdge*
GNENet::addReversedEdge(GNEEdge* edge, GNEUndoList* undoList) {
    undoList->p_begin("add reversed " + toString(SUMO_TAG_EDGE));
    GNEEdge* reversed = NULL;
    if (edge->getNBEdge()->getLaneSpreadFunction() == LANESPREAD_RIGHT || isRailway(edge->getNBEdge()->getPermissions())) {
        // for rail edges, we assume bi-directional tracks are wanted
        reversed = createEdge(edge->getGNEJunctionDestiny(), edge->getGNEJunctionSource(), edge, undoList, "-" + edge->getID(), false, true);
        assert(reversed != 0);
        reversed->setAttribute(SUMO_ATTR_SHAPE, toString(edge->getNBEdge()->getInnerGeometry().reverse()), undoList);
    } else {
        // if the edge is centered it should probably connect somewhere else
        // make it easy to move and reconnect it
        PositionVector orig = edge->getNBEdge()->getGeometry();
        PositionVector origInner = edge->getNBEdge()->getInnerGeometry();
        const double tentativeShift = edge->getNBEdge()->getTotalWidth() + 2;
        orig.move2side(-tentativeShift);
        origInner.move2side(-tentativeShift);
        GNEJunction* src = createJunction(orig.back(), undoList);
        GNEJunction* dest = createJunction(orig.front(), undoList);
        reversed = createEdge(src, dest, edge, undoList, "-" + edge->getID(), false, true);
        assert(reversed != 0);
        reversed->setAttribute(SUMO_ATTR_SHAPE, toString(origInner.reverse()), undoList);
        // select the new edge and its nodes
        std::set<GUIGlID> toSelect;
        toSelect.insert(reversed->getGlID());
        toSelect.insert(src->getGlID());
        toSelect.insert(dest->getGlID());
        undoList->add(new GNEChange_Selection(this, toSelect, gSelected.getSelected(), true), true);
    }
    undoList->p_end();
    return reversed;
}


void
GNENet::mergeJunctions(GNEJunction* moved, GNEJunction* target, GNEUndoList* undoList) {
    undoList->p_begin("merge " + toString(SUMO_TAG_JUNCTION) + "s");
    // position of moved and target are probably a bit different (snap radius)
    Position oldPos = moved->getNBNode()->getPosition();
    moved->moveJunctionGeometry(target->getNBNode()->getPosition());
    // register the move with undolist (must happend within the undo group)
    moved->commitGeometryMoving(oldPos, undoList);
    // deleting edges changes in the underlying EdgeVector so we have to make a copy
    const EdgeVector incoming = moved->getNBNode()->getIncomingEdges();
    for (EdgeVector::const_iterator it = incoming.begin(); it != incoming.end(); it++) {
        GNEEdge* oldEdge = myEdges[(*it)->getID()];
        remapEdge(oldEdge, oldEdge->getGNEJunctionSource(), target, undoList);
    }
    // deleting edges changes in the underlying EdgeVector so we have to make a copy
    const EdgeVector outgoing = moved->getNBNode()->getOutgoingEdges();
    for (EdgeVector::const_iterator it = outgoing.begin(); it != outgoing.end(); it++) {
        GNEEdge* oldEdge = myEdges[(*it)->getID()];
        remapEdge(oldEdge, target, oldEdge->getGNEJunctionDestiny(), undoList);
    }
    deleteJunction(moved, undoList);
    undoList->p_end();
}


void
GNENet::remapEdge(GNEEdge* oldEdge, GNEJunction* from, GNEJunction* to, GNEUndoList* undoList, bool keepEndpoints) {
    deleteEdge(oldEdge, undoList); // delete first so we can reuse the name, reference stays valid
    if (from != to) {
        GNEEdge* newEdge = createEdge(from, to, oldEdge, undoList, oldEdge->getMicrosimID(), false, true);
        newEdge->setAttribute(SUMO_ATTR_SHAPE, oldEdge->getAttribute(SUMO_ATTR_SHAPE), undoList);
        if (keepEndpoints) {
            // preserve endpoints even if they where not customized
            newEdge->setAttribute(GNE_ATTR_SHAPE_START, toString(oldEdge->getNBEdge()->getGeometry().front()), undoList);
            newEdge->setAttribute(GNE_ATTR_SHAPE_END, toString(oldEdge->getNBEdge()->getGeometry().back()), undoList);
        }
    }
    // @todo remap connectivity as well
}


bool 
GNENet::checkJunctionPosition(const Position &pos) {
    // Check that there isn't another junction in the same position as Pos
    for(GNEJunctions::const_iterator i = myJunctions.begin(); i != myJunctions.end(); i++) {
        if(i->second->getPositionInView() == pos) {
            return false;
        }
    }
    return true;
}


void
GNENet::save(OptionsCont& oc) {
    // compute without volatile options and update network
    computeAndUpdate(oc, false);
    // write network
    NWFrame::writeNetwork(oc, *myNetBuilder);
}


void
GNENet::saveAdditionals(const std::string& filename, bool volatileOptionsEnabled) {
    // obtain invalid stopping places and detectors
    std::vector<GNEStoppingPlace*> invalidStoppingPlaces;
    std::vector<GNEDetector*> invalidDetectors;
    for (auto i : myAdditionals) {
        GNEStoppingPlace* stoppingPlace = dynamic_cast<GNEStoppingPlace*>(i.second);
        GNEDetector* detector = dynamic_cast<GNEDetector*>(i.second);
        // check if has to be fixed
        if((stoppingPlace != NULL) && (stoppingPlace->areStoppingPlacesPositionsFixed() == false)) {
            invalidStoppingPlaces.push_back(stoppingPlace);
        } else if((detector != NULL) && (detector->isDetectorPositionFixed() == false)) {
            invalidDetectors.push_back(detector);
        }
    }
    // if there are invalid StoppingPlaces or detectors, open GNEDialog_FixAdditionalPositions
    if(invalidStoppingPlaces.size() > 0 || invalidDetectors.size() > 0) {
        // 0 -> Canceled Saving, with or whithout selecting invalid stopping places and E2
        // 1 -> Invalid stoppingPlaces and E2 fixed, friendlyPos enabled, or saved with invalid positions 
        GNEDialog_FixAdditionalPositions fixAdditionalPositionsDialog(myViewNet, invalidStoppingPlaces, invalidDetectors);
        if(fixAdditionalPositionsDialog.execute() == 0) {
            // Here a console message
            ;
        } else {
            // save additionals
            OutputDevice& device = OutputDevice::getDevice(filename);
            device.openTag("additionals");
            for (GNEAdditionals::const_iterator i = myAdditionals.begin(); i != myAdditionals.end(); ++i) {
                i->second->writeAdditional(device, volatileOptionsEnabled);
            }
            device.close();
        }
        // set focus again in viewNet
        myViewNet->setFocus();
    } else {
        OutputDevice& device = OutputDevice::getDevice(filename);
        device.openTag("additionals");
        for (GNEAdditionals::const_iterator i = myAdditionals.begin(); i != myAdditionals.end(); ++i) {
            i->second->writeAdditional(device, volatileOptionsEnabled);
        }
        device.close();
    }
    // change value of flag
    myAdditionalsSaved = true;
}


void
GNENet::savePlain(OptionsCont& oc) {
    // compute without volatile options
    computeAndUpdate(oc, false);
    NWWriter_XML::writeNetwork(oc, *myNetBuilder);
}


void
GNENet::saveJoined(OptionsCont& oc) {
    // compute without volatile options
    computeAndUpdate(oc, false);
    NWWriter_XML::writeJoinedJunctions(oc, myNetBuilder->getNodeCont());
}


void
GNENet::setViewNet(GNEViewNet* viewNet) {
    myViewNet = viewNet;
}


GNEJunction*
GNENet::retrieveJunction(const std::string& id, bool failHard) {
    if (myJunctions.count(id)) {
        return myJunctions[id];
    } else if (failHard) {
        // If junction wasn't found, throw exception
        throw UnknownElement("Junction " + id);
    } else {
        return NULL;
    }
}


GNEEdge*
GNENet::retrieveEdge(const std::string& id, bool failHard) {
    GNEEdges::const_iterator i = myEdges.find(id);
    // If edge was fund
    if (i != myEdges.end()) {
        return i->second;
    } else if (failHard) {
        // If edge wasn't found, throw exception
        throw UnknownElement("Edge " + id);
    } else {
        return NULL;
    }
}


GNEEdge* 
GNENet::retrieveEdge(GNEJunction *from, GNEJunction *to, bool failHard) {
    assert((from != NULL) && (to != NULL));
    // iterate over Junctions of net
    for(GNEEdges::const_iterator i = myEdges.begin(); i != myEdges.end(); i++) {
        if((i->second->getGNEJunctionSource() == from) && (i->second->getGNEJunctionDestiny() == to)) {
            return i->second;
        }
    }
    // if edge wasn' found, throw exception or return NULL
    if (failHard) {
        throw UnknownElement("Edge with from='" + from->getID() + "' and to='" + to->getID() + "'");
    } else {
        return NULL;
    }
}


std::vector<GNEEdge*>
GNENet::retrieveEdges(bool onlySelected) {
    std::vector<GNEEdge*> result;
    for (GNEEdges::const_iterator it = myEdges.begin(); it != myEdges.end(); ++it) {
        if (!onlySelected || gSelected.isSelected(GLO_EDGE, it->second->getGlID())) {
            result.push_back(it->second);
        }
    }
    return result;
}


std::vector<GNELane*>
GNENet::retrieveLanes(bool onlySelected) {
    std::vector<GNELane*> result;
    for (GNEEdges::const_iterator it = myEdges.begin(); it != myEdges.end(); ++it) {
        const GNEEdge::LaneVector& lanes = it->second->getLanes();
        for (GNEEdge::LaneVector::const_iterator it_lane = lanes.begin(); it_lane != lanes.end(); ++it_lane) {
            if (!onlySelected || gSelected.isSelected(GLO_LANE, (*it_lane)->getGlID())) {
                result.push_back(*it_lane);
            }
        }
    }
    return result;
}


GNELane*
GNENet::retrieveLane(const std::string& id, bool failHard) {
    const std::string edge_id = SUMOXMLDefinitions::getEdgeIDFromLane(id);
    GNEEdge* edge = retrieveEdge(edge_id, failHard);
    if (edge != 0) {
        const GNEEdge::LaneVector& lanes = edge->getLanes();
        for (GNEEdge::LaneVector::const_iterator it_lane = lanes.begin(); it_lane != lanes.end(); ++it_lane) {
            if ((*it_lane)->getID() == id) {
                return (*it_lane);
            }
        }
        if (failHard) {
            // Throw exception if failHard is enabled
            throw UnknownElement(toString(SUMO_TAG_LANE) + " " + id);
        }
    }
    return 0;
}


std::vector<GNEJunction*>
GNENet::retrieveJunctions(bool onlySelected) {
    std::vector<GNEJunction*> result;
    for (GNEJunctions::const_iterator it = myJunctions.begin(); it != myJunctions.end(); it++) {
        if (!onlySelected || gSelected.isSelected(GLO_JUNCTION, it->second->getGlID())) {
            result.push_back(it->second);
        }
    }
    return result;
}


void
GNENet::refreshElement(GUIGlObject* o) {
    myGrid.removeAdditionalGLObject(o);
    myGrid.addAdditionalGLObject(o);
    update();
}


void
GNENet::refreshAdditional(GNEAdditional* additional) {
    GNEAdditionals::iterator positionToRemove = myAdditionals.find(std::pair<std::string, SumoXMLTag>(additional->getID(), additional->getTag()));
    // Check if additional element exists before refresh
    if (positionToRemove != myAdditionals.end()) {
        myGrid.removeAdditionalGLObject(additional);
        myGrid.addAdditionalGLObject(additional);
        update();
    }
}


std::string
GNENet::generateVaporizerID() const {
    int counter = 0;
    while (myAdditionals.find(std::pair<std::string, SumoXMLTag>("vaporizer_" + toString(counter), SUMO_TAG_VAPORIZER)) != myAdditionals.end()) {
        counter++;
    }
    return "vaporizer_" + toString(counter);
}


std::vector<GNEAttributeCarrier*>
GNENet::retrieveAttributeCarriers(const std::set<GUIGlID>& ids, GUIGlObjectType type) {
    std::vector<GNEAttributeCarrier*> result;
    for (std::set<GUIGlID>::iterator it = ids.begin(); it != ids.end(); it++) {
        GUIGlObject* object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(*it);
        if (object != 0) {
            std::string id = object->getMicrosimID();
            GUIGlObjectStorage::gIDStorage.unblockObject(*it);
            GNEAttributeCarrier* ac = 0;
            switch (type) {
                case GLO_JUNCTION:
                    ac = dynamic_cast<GNEJunction*>(object);
                    break;
                case GLO_EDGE:
                    ac = dynamic_cast<GNEEdge*>(object);
                    break;
                case GLO_LANE:
                    ac = dynamic_cast<GNELane*>(object);
                    break;
                case GLO_ADDITIONAL:
                    ac = dynamic_cast<GNEAdditional*>(object);
                    break;
                case GLO_CONNECTION:
                    ac = dynamic_cast<GNEConnection*>(object);
                    break;
                case GLO_CROSSING:
                    ac = dynamic_cast<GNECrossing*>(object);
                    break;
                case GLO_POLYGON:
                    ac = dynamic_cast<GNEPoly*>(object);
                    break;
                case GLO_POI:
                    ac = dynamic_cast<GNEPOI*>(object);
                    break;
                default:
                    break;
            }
            if (ac == 0) {
                throw ProcessError("GUIGlObject does not match the declared type");
            } else {
                result.push_back(ac);
            }
        } else {
            throw ProcessError("Attempted to retrieve non-existant GUIGlObject");
        }
    }
    return result;
}


std::set<GUIGlID>
GNENet::getGlIDs(GUIGlObjectType type) {
    std::set<GUIGlID> result;
    switch (type) {
        case GLO_MAX: {
            std::set<GUIGlObjectType> knownTypes;
            knownTypes.insert(GLO_JUNCTION);
            knownTypes.insert(GLO_EDGE);
            knownTypes.insert(GLO_LANE);
            // knownTypes.insert(GLO_TLLOGIC); makes no sense to include them
            knownTypes.insert(GLO_ADDITIONAL);
            knownTypes.insert(GLO_CONNECTION);
            knownTypes.insert(GLO_CROSSING);
            knownTypes.insert(GLO_POLYGON);
            knownTypes.insert(GLO_POI);
            // obtain all GLIDS calling getGlIDs(...) recursively
            for (auto it : knownTypes) {
                const std::set<GUIGlID> tmp = getGlIDs(it);
                result.insert(tmp.begin(), tmp.end());
            }
            break;
        }
        case GLO_JUNCTION:
            for (auto it : myJunctions) {
                result.insert(it.second->getGlID());
            }
            break;
        case GLO_EDGE:
            for (auto it : myEdges) {
                result.insert(it.second->getGlID());
            }
            break;
        case GLO_LANE: {
            for (auto i : myEdges) {
                // iterate over every edge's lane
                for (auto j : i.second->getLanes()) {
                    result.insert(j->getGlID());
                }
            }
            break;
        }
        case GLO_TLLOGIC: {
            // return all junctions which have a traffic light (we do not have a GUIGlObject for each traffic light)
            for (auto it : myJunctions) {
                if (it.second->getNBNode()->isTLControlled()) {
                    result.insert(it.second->getGlID());
                }
            }
            break;
        }
        case GLO_ADDITIONAL: {
            // Iterate over all additionals of net
            for (auto it : myAdditionals) {
                // Insert every additional in result
                result.insert(it.second->getGlID());
            }
            break;
        }
        case GLO_CONNECTION: {
            for (auto i : myEdges) {
                // Iterate over edge's connections 
                for (auto j : i.second->getGNEConnections()) {
                    // Insert every connection of edge in result
                    result.insert(j->getGlID());
                }
            }
            break;
        }
        case GLO_CROSSING: {
            for (auto i : myJunctions) {
                // Iterate over junction's crossings
                for (auto j : i.second->getGNECrossings()) {
                    // Insert every crossing of junction in result
                    result.insert(j->getGlID());
                }
            }
            break;
        }
        case GLO_POLYGON: {
            for (auto i : myPolygons.getMyMap()) {
                result.insert(dynamic_cast<GNEPoly*>(i.second)->getGlID());
            }
            break;
        }
        case GLO_POI: {
            for (auto i : myPOIs.getMyMap()) {
                result.insert(dynamic_cast<GNEPOI*>(i.second)->getGlID());
            }
            break;
        }
        default: // add other types once we know them
            break;
    }
    return result;
}


void
GNENet::computeEverything(GNEApplicationWindow* window, bool force, bool volatileOptions, std::string additionalPath) {
    if (!myNeedRecompute) {
        if (force) {
            if(volatileOptions) {
                window->setStatusBarText("Forced computing junctions with volatile options ...");
            } else {
                window->setStatusBarText("Forced computing junctions ...");
            }
        } else {
            return;
        }
    } else {
        if(volatileOptions) {
            window->setStatusBarText("Computing junctions with volatile options ...");
        } else {
            window->setStatusBarText("Computing junctions  ...");
        }
    }
    // compute
    OptionsCont& oc = OptionsCont::getOptions();
    computeAndUpdate(oc, volatileOptions);

    // load additionanls if was recomputed with volatile options
    if(additionalPath != "") {
        // Create additional handler
        GNEAdditionalHandler additionalHandler(additionalPath, myViewNet, false);
        // Run parser
        if (!XMLSubSys::runParser(additionalHandler, additionalPath, false)) {
            WRITE_MESSAGE("Loading of " + additionalPath + " failed.");
        } else {
            // reset last tag (needed to avoid invalid E3s)
            additionalHandler.resetLastTag();
            update();
        }
    }
    window->getApp()->endWaitCursor();
    window->setStatusBarText("Finished computing junctions.");
    update();
}


void
GNENet::computeJunction(GNEJunction* junction) {
    // recompute tl-logics
    OptionsCont& oc = OptionsCont::getOptions();
    NBTrafficLightLogicCont& tllCont = getTLLogicCont();

    NBNode* nbn = junction->getNBNode();
    std::set<NBTrafficLightDefinition*> tldefs = nbn->getControllingTLS();
    for (std::set<NBTrafficLightDefinition*>::iterator it = tldefs.begin(); it != tldefs.end(); it++) {
        NBTrafficLightDefinition* def = *it;
        def->setParticipantsInformation();
        def->setTLControllingInformation();
        tllCont.computeSingleLogic(oc, def);
    }

    // @todo compute connections etc...
}


void
GNENet::requireRecompute() {
    myNeedRecompute = true;
}


bool
GNENet::netHasGNECrossings() const {
    for (auto n : myJunctions) {
        if (n.second->getGNECrossings().size() > 0) {
            return true;
        }
    }
    return false;
}


FXApp*
GNENet::getApp() {
    return myViewNet->getApp();
}


NBNetBuilder*
GNENet::getNetBuilder() const {
    return myNetBuilder;
}


bool
GNENet::joinSelectedJunctions(GNEUndoList* undoList) {
    std::vector<GNEJunction*> selectedJunctions = retrieveJunctions(true);
    if (selectedJunctions.size() < 2) {
        return false;
    }
    EdgeVector allIncoming;
    EdgeVector allOutgoing;
    std::set<NBNode*> cluster;
    for (auto it : selectedJunctions) {
        cluster.insert(it->getNBNode());
        const EdgeVector& incoming = it->getNBNode()->getIncomingEdges();
        allIncoming.insert(allIncoming.end(), incoming.begin(), incoming.end());
        const EdgeVector& outgoing = it->getNBNode()->getOutgoingEdges();
        allOutgoing.insert(allOutgoing.end(), outgoing.begin(), outgoing.end());
    }
    // create new junction
    Position pos;
    Position oldPos; 
    bool setTL;
    std::string id;
    TrafficLightType type;
    myNetBuilder->getNodeCont().analyzeCluster(cluster, id, pos, setTL, type);
    // save position
    oldPos = pos;

    // Check that there isn't another junction in the same position as Pos but doesn't belong to cluster
    for(GNEJunctions::const_iterator i = myJunctions.begin(); i != myJunctions.end(); i++) {
        if((i->second->getPositionInView() == pos) && (cluster.find(i->second->getNBNode()) == cluster.end())) {
            // show warning in gui testing debug mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Opening FXMessageBox of type 'question'");
            }
            // Ask confirmation to user
            FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
                                                    ("Position of joined " + toString(SUMO_TAG_JUNCTION)).c_str(), "%s",
                                                    ("There is another unselected " + toString(SUMO_TAG_JUNCTION) + " in the same position of joined " + toString(SUMO_TAG_JUNCTION) +
                                                     + ".\nIt will be joined with the other selected " + toString(SUMO_TAG_JUNCTION) + "s. Continue?").c_str());
            if (answer != 1) { // 1:yes, 2:no, 4:esc
                // write warning if netedit is running in testing mode
                if ((answer == 2) && (OptionsCont::getOptions().getBool("gui-testing-debug"))) {
                    WRITE_WARNING("Closed FXMessageBox of type 'question' with 'No'");
                } else if ((answer == 4) && (OptionsCont::getOptions().getBool("gui-testing-debug"))) {
                    WRITE_WARNING("Closed FXMessageBox of type 'question' with 'ESC'");
                }
                return false;
            } else {
                // write warning if netedit is running in testing mode
                if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                    WRITE_WARNING("Closed FXMessageBox of type 'question' with 'Yes'");
                }
                // select conflicted junction an join all again
                gSelected.select(i->second->getGlID());
                return joinSelectedJunctions(undoList);
            }
        }
    }

    // use checkJunctionPosition to avoid conflicts with junction in the same position as others
    while(checkJunctionPosition(pos) == false) {
        pos.setx(pos.x() + 0.1);
        pos.sety(pos.y() + 0.1);
    }

    // start with the join selected junctions
    undoList->p_begin("Join selected " + toString(SUMO_TAG_JUNCTION) + "s");

    // first remove all crossing of the involved junctions
    for(auto i : selectedJunctions) {
        for (auto j : i->getGNECrossings()) {
            deleteCrossing(j, undoList);
        }
    }

    // #3128 this is not undone when calling 'undo'
    myNetBuilder->getNodeCont().registerJoinedCluster(cluster);
    GNEJunction* joined = createJunction(pos, undoList);
    if (setTL) {
        joined->setAttribute(SUMO_ATTR_TYPE, toString(NODETYPE_TRAFFIC_LIGHT), undoList);
        // XXX ticket831
        //joined-><getTrafficLight>->setAttribute(SUMO_ATTR_TYPE, toString(type), undoList);
    }
    // remap edges
    for (EdgeVector::const_iterator it = allIncoming.begin(); it != allIncoming.end(); it++) {
        GNEEdge* oldEdge = myEdges[(*it)->getID()];
        remapEdge(oldEdge, oldEdge->getGNEJunctionSource(), joined, undoList, true);
    }
    for (EdgeVector::const_iterator it = allOutgoing.begin(); it != allOutgoing.end(); it++) {
        GNEEdge* oldEdge = myEdges[(*it)->getID()];
        remapEdge(oldEdge, joined, oldEdge->getGNEJunctionDestiny(), undoList, true);
    }
    // delete original junctions
    for (auto it : selectedJunctions) {
        deleteJunction(it, undoList);
    }
    joined->setAttribute(SUMO_ATTR_ID, id, undoList);

    // check if joined junction had to change their original position to avoid errors
    if(pos != oldPos) {
        joined->setAttribute(SUMO_ATTR_POSITION, toString(oldPos), undoList);
    }
    undoList->p_end();
    return true;
}


bool 
GNENet::cleanInvalidCrossings(GNEUndoList* undoList) {
    // obtain current net's crossings
    std::vector<GNECrossing*> myNetCrossings;
    for (auto it = myJunctions.begin(); it != myJunctions.end(); it++) {
        myNetCrossings.reserve(myNetCrossings.size() + it->second->getGNECrossings().size());
        myNetCrossings.insert(myNetCrossings.end(), it->second->getGNECrossings().begin(), it->second->getGNECrossings().end());
    }
    // obtain invalid crossigns
    std::vector<GNECrossing*> myInvalidCrossings;
    for (auto i = myNetCrossings.begin(); i != myNetCrossings.end(); i++) {
        if ((*i)->getNBCrossing()->valid == false) {
            myInvalidCrossings.push_back(*i);
        }
    }
    
    if (myInvalidCrossings.empty()) {
        // show warning in gui testing debug mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox of type 'warning'");
        }
        // open a dialog informing that there isn't crossing to remove
        FXMessageBox::warning(getApp(), MBOX_OK, 
            ("Clear " + toString(SUMO_TAG_CROSSING) + "s").c_str(), "%s",
            ("There is no invalid " + toString(SUMO_TAG_CROSSING) + "s to remove").c_str());
        // show warning in gui testing debug mode
        WRITE_WARNING("Closed FXMessageBox of type 'warning' with 'ESC'");
    } else {
        std::string plural = myInvalidCrossings.size() == 1? ("") : ("s");
        // show warning in gui testing debug mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox of type 'question'");
        }
        // Ask confirmation to user
        FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
            ("Clear " + toString(SUMO_TAG_CROSSING) + "s").c_str(), "%s",
            ("Clear " + toString(SUMO_TAG_CROSSING) + plural + " will be removed. Continue?").c_str());
            if (answer != 1) { // 1:yes, 2:no, 4:esc
                // write warning if netedit is running in testing mode
                if ((answer == 2) && (OptionsCont::getOptions().getBool("gui-testing-debug"))) {
                    WRITE_WARNING("Closed FXMessageBox of type 'question' with 'No'");
                }
                else if ((answer == 4) && (OptionsCont::getOptions().getBool("gui-testing-debug"))) {
                    WRITE_WARNING("Closed FXMessageBox of type 'question' with 'ESC'");
                }
            } else {
                undoList->p_begin("Clean " + toString(SUMO_TAG_CROSSING) + "s");
                for(auto i = myInvalidCrossings.begin(); i != myInvalidCrossings.end(); i++) {
                    deleteCrossing((*i), undoList);
                }
                undoList->p_end();
            }
    }
    return 1;
}


void
GNENet::removeSolitaryJunctions(GNEUndoList* undoList) {
    undoList->p_begin("Clean " + toString(SUMO_TAG_JUNCTION) + "s");
    std::vector<GNEJunction*> toRemove;
    for (GNEJunctions::const_iterator it = myJunctions.begin(); it != myJunctions.end(); it++) {
        GNEJunction* junction = it->second;
        if (junction->getNBNode()->getEdges().size() == 0) {
            toRemove.push_back(junction);
        }
    }
    for (auto it : toRemove) {
        deleteJunction(it, undoList);
    }
    undoList->p_end();
}


void
GNENet::replaceJunctionByGeometry(GNEJunction* junction, GNEUndoList* undoList) {
    undoList->p_begin("Replace junction by geometry");
    assert(junction->getNBNode()->checkIsRemovable());
    std::vector<std::pair<NBEdge*, NBEdge*> > toJoin = junction->getNBNode()->getEdgesToJoin();
    for (auto j : toJoin) {
        GNEEdge* begin = myEdges[j.first->getID()];
        GNEEdge* continuation = myEdges[j.second->getID()];
        // remove connections between the edges
        std::vector<NBEdge::Connection> connections = begin->getNBEdge()->getConnections();
        for (auto con : connections) {
            undoList->add(new GNEChange_Connection(begin, con, false, false), true);
        }
        // replace
        replaceIncomingEdge(continuation, begin, undoList);
        // fix shape
        PositionVector newShape = begin->getNBEdge()->getInnerGeometry();
        if (begin->getNBEdge()->hasDefaultGeometryEndpointAtNode(begin->getNBEdge()->getToNode())) {
            newShape.push_back(junction->getNBNode()->getPosition());
        } else {
            newShape.push_back(begin->getNBEdge()->getGeometry()[-1]);
        }
        if (continuation->getNBEdge()->hasDefaultGeometryEndpointAtNode(begin->getNBEdge()->getToNode())) {
            newShape.push_back_noDoublePos(junction->getNBNode()->getPosition());
        } else {
            newShape.push_back_noDoublePos(continuation->getNBEdge()->getGeometry()[0]);
        }
        newShape.append(continuation->getNBEdge()->getInnerGeometry());
        begin->setAttribute(GNE_ATTR_SHAPE_END, continuation->getAttribute(GNE_ATTR_SHAPE_END), undoList);
        begin->setAttribute(SUMO_ATTR_SHAPE, toString(newShape), undoList);
    }
    deleteJunction(junction, undoList);
    undoList->p_end();
}


void
GNENet::renameEdge(GNEEdge* edge, const std::string& newID) {
    myEdges.erase(edge->getNBEdge()->getID());
    myNetBuilder->getEdgeCont().rename(edge->getNBEdge(), newID);
    edge->setMicrosimID(newID);
    myEdges[newID] = edge;
    // rename all connections related to this edge
    for (auto i : edge->getLanes()) {
        i->updateConnectionIDs();
    }
}


void
GNENet::changeEdgeEndpoints(GNEEdge* edge, const std::string& newSource, const std::string& newDest) {
    NBNode* from = retrieveJunction(newSource)->getNBNode();
    NBNode* to = retrieveJunction(newDest)->getNBNode();
    edge->getNBEdge()->reinitNodes(from, to);
    requireRecompute();
    update();
}


GNEViewNet*
GNENet::getViewNet() const {
    return myViewNet;
}


NBTrafficLightLogicCont&
GNENet::getTLLogicCont() {
    return myNetBuilder->getTLLogicCont();
}


void
GNENet::renameJunction(GNEJunction* junction, const std::string& newID) {
    myJunctions.erase(junction->getNBNode()->getID());
    myNetBuilder->getNodeCont().rename(junction->getNBNode(), newID);
    junction->setMicrosimID(newID);
    myJunctions[newID] = junction;
}


void
GNENet::addExplicitTurnaround(std::string id) {
    myExplicitTurnarounds.insert(id);
}


void
GNENet::removeExplicitTurnaround(std::string id) {
    myExplicitTurnarounds.erase(id);
}


void
GNENet::moveSelection(const Position& moveSrc, const Position& moveDest) {
    Position delta = moveDest - moveSrc;
    // obtain Junctios to move
    std::set<GNEJunction*> junctionSet;
    std::vector<GNEJunction*> junctions = retrieveJunctions(true);
    // move junctions
    for (auto it : junctions) {
        Position newPos = it->getNBNode()->getPosition() + delta;
        it->moveJunctionGeometry(newPos);
        junctionSet.insert(it);
    }

    // move edge geometry (endpoints are already moved)
    std::vector<GNEEdge*> edges = retrieveEdges(true);
    for (auto it : edges) {
        if (it) {
            if (junctionSet.count(it->getGNEJunctionSource()) > 0 &&
                junctionSet.count(it->getGNEJunctionDestiny()) > 0) {
                // edge and its endpoints are selected, move all the inner points as well
                it->moveGeometry(delta);
            } else {
                // move only geometry near mouse
                it->moveGeometry(moveSrc, delta, true);
            }
        }
    }
}


void
GNENet::finishMoveSelection(GNEUndoList* undoList) {
    undoList->p_begin("move selection");
    // register moved junctions
    std::set<GNEJunction*> junctionSet;
    std::vector<GNEJunction*> junctions = retrieveJunctions(true);
    for (auto it : junctions) {
        junctionSet.insert(it);
    }

    // register moved edge geometry (endpoints are already moved)
    std::vector<GNEEdge*> edges = retrieveEdges(true);
    for (auto it : edges) {
        if (it) {
            const std::string& newShape = it->getAttribute(SUMO_ATTR_SHAPE);
            it->setAttribute(SUMO_ATTR_SHAPE, newShape, undoList);
        }
    }
    undoList->p_end();
}


void
GNENet::insertAdditional(GNEAdditional* additional, bool hardFail) {
    // Check if additional element exists before insertion
    if (myAdditionals.find(std::pair<std::string, SumoXMLTag>(additional->getID(), additional->getTag())) != myAdditionals.end()) {
        // Throw exception only if hardFail is enabled
        if (hardFail) {
            throw ProcessError(toString(additional->getTag()) + " with ID='" + additional->getID() + "' already exist");
        }
    } else {
        myAdditionals[std::pair<std::string, SumoXMLTag>(additional->getID(), additional->getTag())] = additional;
        myGrid.addAdditionalGLObject(additional);
        update();
        myAdditionalsSaved = false;
    }
}


void
GNENet::deleteAdditional(GNEAdditional* additional) {
    GNEAdditionals::iterator additionalToRemove = myAdditionals.find(std::pair<std::string, SumoXMLTag>(additional->getID(), additional->getTag()));
    // Check if additional element exists before deletion
    if (additionalToRemove == myAdditionals.end()) {
        throw ProcessError(toString(additional->getTag()) + " with ID='" + additional->getID() + "' doesn't exist");
    } else {
        myAdditionals.erase(additionalToRemove);
        myGrid.removeAdditionalGLObject(additional);
        update();
        myAdditionalsSaved = false;
    }
}


void
GNENet::updateAdditionalID(const std::string& oldID, GNEAdditional* additional) {
    GNEAdditionals::iterator additionalToUpdate = myAdditionals.find(std::pair<std::string, SumoXMLTag>(oldID, additional->getTag()));
    if (additionalToUpdate == myAdditionals.end()) {
        throw ProcessError(toString(additional->getTag()) + "  with old ID='" + oldID + "' doesn't exist");
    } else {
        // remove an insert additional again into container
        myAdditionals.erase(additionalToUpdate);
        myAdditionals[std::pair<std::string, SumoXMLTag>(additional->getID(), additional->getTag())] = additional;
        myAdditionalsSaved = false;
    }
}


GNEAdditional*
GNENet::retrieveAdditional(const std::string& id, bool hardFail) const {
    for (GNEAdditionals::const_iterator i = myAdditionals.begin(); i != myAdditionals.end(); i++) {
        if (i->second->getID() == id) {
            return i->second;
        }
    }
    if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant additional");
    } else {
        return NULL;
    }
}


std::vector<GNEAdditional*>
GNENet::retrieveAdditionals(bool onlySelected) {
    std::vector<GNEAdditional*> result;
    for (GNEAdditionals::const_iterator it = myAdditionals.begin(); it != myAdditionals.end(); ++it) {
        if (!onlySelected || gSelected.isSelected(GLO_ADDITIONAL, it->second->getGlID())) {
            result.push_back(it->second);
        }
    }
    return result;
}


GNEAdditional*
GNENet::getAdditional(SumoXMLTag type, const std::string& id) const {
    if (myAdditionals.empty()) {
        return NULL;
    } else if (myAdditionals.find(std::pair<std::string, SumoXMLTag>(id, type)) != myAdditionals.end())  {
        return myAdditionals.at(std::pair<std::string, SumoXMLTag>(id, type));
    } else {
        return NULL;
    }
}


std::string
GNENet::getAdditionalID(SumoXMLTag type, const GNELane* lane, const double pos) const {
    for (GNEAdditionals::const_iterator it = myAdditionals.begin(); it != myAdditionals.end(); ++it) {
        if ((it->second->getTag() == type) && (it->second->getLane() != NULL) && (it->second->getLane() == lane) && (fabs(it->second->getPositionInView().x() - pos) < POSITION_EPS)) {
            return it->second->getID();
        }
    }
    return "";
}


std::vector<GNEAdditional*>
GNENet::getAdditionals(SumoXMLTag type) const {
    std::vector<GNEAdditional*> vectorOfAdditionals;
    for (GNEAdditionals::const_iterator i = myAdditionals.begin(); i != myAdditionals.end(); i++) {
        if (type == SUMO_TAG_NOTHING || type == i->second->getTag()) {
            vectorOfAdditionals.push_back(i->second);
        }
    }
    return vectorOfAdditionals;
}


int
GNENet::getNumberOfAdditionals(SumoXMLTag type) const {
    int counter = 0;
    for (GNEAdditionals::const_iterator i = myAdditionals.begin(); i != myAdditionals.end(); i++) {
        if (type == SUMO_TAG_NOTHING || type == i->second->getTag()) {
            counter++;
        }
    }
    return counter;
}


const GNECalibratorRoute&
GNENet::getGNECalibratorRoute(const std::string& calibratorRouteID) const {
    std::vector<GNEAdditional*> calibrators = getAdditionals(SUMO_TAG_CALIBRATOR);
    for (std::vector<GNEAdditional*>::iterator i = calibrators.begin(); i != calibrators.end(); i++) {
        GNECalibrator* calibrator = dynamic_cast<GNECalibrator*>(*i);
        if (calibrator->routeExists(calibratorRouteID)) {
            return calibrator->getCalibratorRoute(calibratorRouteID);
        }
    }
    throw InvalidArgument("A " + toString(SUMO_TAG_VTYPE) + "'s " + toString(SUMO_TAG_VTYPE) + " with ID = '" + calibratorRouteID + "' doesn't exists");
}


const GNECalibratorVehicleType&
GNENet::getGNECalibratorVehicleType(const std::string& calibratorVehicleTypeID) const {
    std::vector<GNEAdditional*> calibrators = getAdditionals(SUMO_TAG_CALIBRATOR);
    for (std::vector<GNEAdditional*>::iterator i = calibrators.begin(); i != calibrators.end(); i++) {
        GNECalibrator* calibrator = dynamic_cast<GNECalibrator*>(*i);
        if (calibrator->vehicleTypeExists(calibratorVehicleTypeID)) {
            return calibrator->getCalibratorVehicleType(calibratorVehicleTypeID);
        }
    }
    throw InvalidArgument("A " + toString(SUMO_TAG_VTYPE) + "'s " + toString(SUMO_TAG_VTYPE) + " with ID = '" + calibratorVehicleTypeID + "' doesn't exists");
}


const GNECalibratorFlow&
GNENet::getGNECalibratorFlow(const std::string& calibratorFlowID) const {
    std::vector<GNEAdditional*> calibrators = getAdditionals(SUMO_TAG_CALIBRATOR);
    for (std::vector<GNEAdditional*>::iterator i = calibrators.begin(); i != calibrators.end(); i++) {
        GNECalibrator* calibrator = dynamic_cast<GNECalibrator*>(*i);
        if (calibrator->flowExists(calibratorFlowID)) {
            return calibrator->getCalibratorFlow(calibratorFlowID);
        }
    }
    throw InvalidArgument("A " + toString(SUMO_TAG_VTYPE) + "'s " + toString(SUMO_TAG_VTYPE) + " with ID = '" + calibratorFlowID + "' doesn't exists");
}


bool
GNENet::routeExists(const std::string& routeID) const {
    std::vector<GNEAdditional*> calibrators = getAdditionals(SUMO_TAG_CALIBRATOR);
    for (std::vector<GNEAdditional*>::iterator i = calibrators.begin(); i != calibrators.end(); i++) {
        GNECalibrator* calibrator = dynamic_cast<GNECalibrator*>(*i);
        if (calibrator->routeExists(routeID)) {
            return true;
        }
    }
    return false;
}


bool
GNENet::vehicleTypeExists(const std::string& vehicleTypeID) const {
    std::vector<GNEAdditional*> calibrators = getAdditionals(SUMO_TAG_CALIBRATOR);
    for (std::vector<GNEAdditional*>::iterator i = calibrators.begin(); i != calibrators.end(); i++) {
        GNECalibrator* calibrator = dynamic_cast<GNECalibrator*>(*i);
        if (calibrator->vehicleTypeExists(vehicleTypeID)) {
            return true;
        }
    }
    return false;
}


bool
GNENet::flowExists(const std::string& flowID) const {
    std::vector<GNEAdditional*> calibrators = getAdditionals(SUMO_TAG_CALIBRATOR);
    for (std::vector<GNEAdditional*>::iterator i = calibrators.begin(); i != calibrators.end(); i++) {
        GNECalibrator* calibrator = dynamic_cast<GNECalibrator*>(*i);
        if (calibrator->flowExists(flowID)) {
            return true;
        }
    }
    return false;
}


bool 
GNENet::addPolygon(const std::string& id, const std::string& type,const RGBColor& color, double layer, double angle, 
                   const std::string& imgFile, const PositionVector& shape, bool fill, bool /*ignorePruning*/) {
    // check if ID is duplicated
    if(myPolygons.get(id) == NULL) {
        // create poly
        GNEPoly* poly = new GNEPoly(this, id, type, shape, fill, color, layer, angle, imgFile, false, false);
        if(myPolygons.add(poly->getID(), poly)) {
            myViewNet->getUndoList()->p_begin("add " + toString(poly->getTag()));
            myViewNet->getUndoList()->add(new GNEChange_Poly(this, poly, true), true);
            myViewNet->getUndoList()->p_end();
            return true;
        }
        else {
            throw ProcessError("Error adding GNEPOly into shapeContainer");
        }
    } else {
        return false;
    }
}


GNEPoly*
GNENet::addPolygonForEditShapes(GNEJunction *junction) {
    // generate a ID for myEditJunctionShapePoly
    int counter = 0;
    std::string polyID = "junction_shape:" + toString(counter);
    while(myPolygons.get(polyID) != NULL) {
        counter++;
        polyID = "junction_shape:" + toString(counter);
    }
    // obtain shape of NBNode and close it
    PositionVector junctionShape = junction->getNBNode()->getShape();
    junctionShape.closePolygon();
    // create poly for edit shapes
    GNEPoly* shapePoly = new GNEPoly(this, "junction_shape:" + toString(counter), "junction_shape", junctionShape, true, RGBColor::GREEN, GLO_POLYGON, 0, "", false , false);
    shapePoly->setShapeEditedJunction(junction);
    shapePoly->setLineWidth(0.3);
    insertPolygonInView(shapePoly);
    return shapePoly;
}


bool 
GNENet::removePolygon(const std::string& id) {
    GNEPoly* p = dynamic_cast<GNEPoly*>(myPolygons.get(id));
    if (p == 0) {
        return false;
    } else {
        return myPolygons.remove(id);
    }
}


void 
GNENet::insertPolygonInView(GNEPoly* p) {
    if(p->isShapeVisible() == false) {
        myGrid.addAdditionalGLObject(p);
        myViewNet->update();
        p->setShapeVisible(true);
        // shapes has to be saved
        myShapesSaved = false;
    } else {
        throw ProcessError("Polygon was already inserted in view");
    }
}


void 
GNENet::removePolygonOfView(GNEPoly* p) {
    if(p->isShapeVisible()) {
        myGrid.removeAdditionalGLObject(p);
        myViewNet->update();
        p->setShapeVisible(false);
        // shapes has to be saved
        myShapesSaved = false;
    } else {
        throw ProcessError("Polygon wasn't already inserted in view");
    }
}


void 
GNENet::refreshPolygon(GNEPoly* p) {
    if(p->isShapeVisible() == false) {
        myGrid.removeAdditionalGLObject(p);
        myGrid.addAdditionalGLObject(p);
        myViewNet->update();
    } else {
        throw ProcessError("Polygon wasn't inserted in view");
    }
}


std::string 
GNENet::generatePolyID() const {
    int counter = 0;
    std::string newID = "poly_" + toString(counter);
    // generate new IDs to find a non-assigned ID
    while(myPolygons.get(newID) != NULL) {
        counter++;
        newID = "poly_" + toString(counter);
    }
    return newID;
}


GNEPoly* 
GNENet::retrievePolygon(const std::string & id, bool failHard) const {
    if (myPolygons.get(id) != 0) {
        return reinterpret_cast<GNEPoly*>(myPolygons.get(id));
    } else if (failHard) {
        // If Polygon wasn't found, throw exception
        throw UnknownElement("Polygon " + id);
    } else {
        return NULL;
    }
}


void
GNENet::changePolygonID(GNEPoly* poly, const std::string &OldID) {
    if (myPolygons.get(OldID) == 0) {
        throw UnknownElement("Polygon " + OldID);
    } else {
        myPolygons.changeID(OldID, poly->getID());
    }

}


bool 
GNENet::addPOI(const std::string& id, const std::string& type, const RGBColor& color, double layer, double angle, 
               const std::string& imgFile, const Position& pos, double width, double height, bool /*ignorePruning */) {
    // check if ID is duplicated
    if(myPOIs.get(id) == NULL) {
        // create poly
        GNEPOI* poi = new GNEPOI(this, id, type, color, layer, angle, imgFile, pos, width, height, false);
        if(myPOIs.add(poi->getID(), poi)) {
            myViewNet->getUndoList()->p_begin("add " + toString(poi->getTag()));
            myViewNet->getUndoList()->add(new GNEChange_POI(this, poi, true), true);
            myViewNet->getUndoList()->p_end();
            return true;
        } else {
            throw ProcessError("Error adding GNEPOI into shapeContainer");
        }
    } else {
        return false;
    }
}


bool 
GNENet::removePOI(const std::string& id) {
    GNEPOI* p = dynamic_cast<GNEPOI*>(myPOIs.get(id));
    if (p == 0) {
        return false;
    } else {
        return myPOIs.remove(id);
    }
}


void 
GNENet::insertPOIInView(GNEPOI* p) {
    if(p->isShapeVisible() == false) {
        myGrid.addAdditionalGLObject(p);
        myViewNet->update();
        p->setShapeVisible(true);
        // shapes has to be saved
        myShapesSaved = false;
    } else {
        throw ProcessError("POI was already inserted in view");
    }
}


void 
GNENet::removePOIOfView(GNEPOI* p) {
    if(p->isShapeVisible()) {
        myGrid.removeAdditionalGLObject(p);
        myViewNet->update();
        p->setShapeVisible(false);
        // shapes has to be saved
        myShapesSaved = false;
    } else {
        throw ProcessError("POI wasn't already inserted in view");
    }
}


void 
GNENet::refreshPOI(GNEPOI* p) {
    if(p->isShapeVisible()) {
        myGrid.removeAdditionalGLObject(p);
        myGrid.addAdditionalGLObject(p);
        myViewNet->update();
    } else {
        throw ProcessError("POI wasn't inserted in view");
    }
}


std::string 
GNENet::generatePOIID() const {
    int counter = 0;
    std::string newID = "POI_" + toString(counter);
    // generate new IDs to find a non-assigned ID
    while(myPOIs.get(newID) != NULL) {
        counter++;
        newID = "POI_" + toString(counter);
    }
    return newID;
}


GNEPOI* 
GNENet::retrievePOI(const std::string & id, bool failHard) const {
    if (myPOIs.get(id) != 0) {
        return reinterpret_cast<GNEPOI*>(myPOIs.get(id));
    } else if (failHard) {
        // If POI wasn't found, throw exception
        throw UnknownElement("POI " + id);
    } else {
        return NULL;
    }
}


void
GNENet::changePOIID(GNEPOI* POI, const std::string &OldID) {
    if (myPOIs.get(OldID) == 0) {
        throw UnknownElement("POI " + OldID);
    } else {
        myPOIs.changeID(OldID, POI->getID());
    }

}

void GNENet::saveShapes(const std::string & filename) {
    // save Shapes
    OutputDevice& device = OutputDevice::getDevice(filename);
    device.openTag("additionals");
    // write only visible polygons
    for (auto i = myPolygons.getMyMap().begin(); i != myPolygons.getMyMap().end(); i++) {
        GNEPoly* polygon = dynamic_cast<GNEPoly*>(i->second);
        if(polygon->isShapeVisible()) {
            polygon->writeShape(device);
        }
    }
    // write only visible POIs
    for (auto i = myPOIs.getMyMap().begin(); i != myPOIs.getMyMap().end(); i++) {
        GNEPOI* POI = dynamic_cast<GNEPOI*>(i->second);
        if(POI->isShapeVisible()) {
            POI->writeShape(device);
        }
    }
    device.close();
    // change flag to true
    myShapesSaved = true;
}


bool 
GNENet::isShapeSelected(SumoXMLTag tag, const std::string & ID) const {
    if(tag == SUMO_TAG_POLY) {
        return gSelected.isSelected(GLO_POLYGON, retrievePolygon(ID)->getGlID());
    } else if(tag == SUMO_TAG_POI) {
        return gSelected.isSelected(GLO_POI, retrievePOI(ID)->getGlID());
    } else{
        throw ProcessError("Invalid Shape");
    }
}

// ===========================================================================
// private
// ===========================================================================

void 
GNENet::initJunctionsAndEdges() {
    // init junctions (by default Crossing and walking areas aren't created)
    NBNodeCont& nc = myNetBuilder->getNodeCont();
    const std::vector<std::string>& nodeNames = nc.getAllNames();
    for (std::vector<std::string>::const_iterator name_it = nodeNames.begin(); name_it != nodeNames.end(); ++name_it) {
        NBNode* nbn = nc.retrieve(*name_it);
        registerJunction(new GNEJunction(*nbn, this, true));
    }

    // init edges
    NBEdgeCont& ec = myNetBuilder->getEdgeCont();
    const std::vector<std::string>& edgeNames = ec.getAllNames();
    for (std::vector<std::string>::const_iterator name_it = edgeNames.begin(); name_it != edgeNames.end(); ++name_it) {
        NBEdge* nbe = ec.retrieve(*name_it);
        registerEdge(new GNEEdge(*nbe, this, false, true));
        if (myGrid.getWidth() > 10e16 || myGrid.getHeight() > 10e16) {
            throw ProcessError("Network size exceeds 1 Lightyear. Please reconsider your inputs.\n");
        }
    }

    // make sure myGrid is initialized even for an empty net
    if (myEdges.size() == 0) {
        myGrid.add(Boundary(0, 0, 100, 100));
    }

    // sort nodes edges so that arrows can be drawn correctly
    NBNodesEdgesSorter::sortNodesEdges(nc);
}


void
GNENet::insertJunction(GNEJunction* junction) {
    myNetBuilder->getNodeCont().insert(junction->getNBNode());
    registerJunction(junction);
}


void
GNENet::insertEdge(GNEEdge* edge) {
    NBEdge* nbe = edge->getNBEdge();
    myNetBuilder->getEdgeCont().insert(nbe); // should we ignore pruning double edges?
    // if this edge was previouls extracted from the edgeContainer we have to rewire the nodes
    nbe->getFromNode()->addOutgoingEdge(nbe);
    nbe->getToNode()->addIncomingEdge(nbe);
    registerEdge(edge);
}


GNEJunction*
GNENet::registerJunction(GNEJunction* junction) {
    junction->incRef("GNENet::registerJunction");
    junction->setResponsible(false);
    myJunctions[junction->getMicrosimID()] = junction;
    myGrid.add(junction->getBoundary());
    myGrid.addAdditionalGLObject(junction);
    // @todo let Boundary class track z-coordinate natively
    const double z = junction->getNBNode()->getPosition().z();
    if (z != 0) {
        myZBoundary.add(z, Z_INITIALIZED);
    }
    update();
    return junction;
}


GNEEdge*
GNENet::registerEdge(GNEEdge* edge) {
    edge->incRef("GNENet::registerEdge");
    edge->setResponsible(false);
    // add edge to internal container of GNENet
    myEdges[edge->getMicrosimID()] = edge;
    // add edge to grid
    myGrid.add(edge->getBoundary());
    myGrid.addAdditionalGLObject(edge);
    // Add references into GNEJunctions
    edge->getGNEJunctionSource()->addOutgoingGNEEdge(edge);
    edge->getGNEJunctionDestiny()->addIncomingGNEEdge(edge);
    // update view
    update();
    return edge;
}


void
GNENet::deleteSingleJunction(GNEJunction* junction) {
    myGrid.removeAdditionalGLObject(junction);
    myJunctions.erase(junction->getMicrosimID());
    myNetBuilder->getNodeCont().extract(junction->getNBNode());
    junction->decRef("GNENet::deleteSingleJunction");
    junction->setResponsible(true);
    // selection status is lost when removing junction via undo and the selection operation was not part of a command group
    gSelected.deselect(junction->getGlID());
    update();
}


void
GNENet::deleteSingleEdge(GNEEdge* edge) {
    // remove edge from visual grid and container
    myGrid.removeAdditionalGLObject(edge);
    myEdges.erase(edge->getMicrosimID());
    // extract edge of district container
    myNetBuilder->getEdgeCont().extract(myNetBuilder->getDistrictCont(), edge->getNBEdge());
    edge->decRef("GNENet::deleteSingleEdge");
    edge->setResponsible(true);
    // selection status is lost when removing edge via undo and the selection operation was not part of a command group
    gSelected.deselect(edge->getGlID());
    // Remove refrences from GNEJunctions
    edge->getGNEJunctionSource()->removeOutgoingGNEEdge(edge);
    edge->getGNEJunctionDestiny()->removeIncomingGNEEdge(edge);
    // invalidate junction logic
    update();
}


void
GNENet::update() {
    if (myViewNet) {
        myViewNet->update();
    }
}


void
GNENet::reserveEdgeID(const std::string& id) {
    myEdgeIDSupplier.avoid(id);
}


void
GNENet::reserveJunctionID(const std::string& id) {
    myJunctionIDSupplier.avoid(id);
}


void
GNENet::initGNEConnections() {
    for (GNEEdges::const_iterator it = myEdges.begin(); it != myEdges.end(); it++) {
        it->second->remakeGNEConnections();
    }
    for (GNEEdges::const_iterator it = myEdges.begin(); it != myEdges.end(); it++) {
        it->second->updateGeometry();
    }
}


void
GNENet::computeAndUpdate(OptionsCont& oc, bool volatileOptions) {
    // make sure we only add turn arounds to edges which currently exist within the network
    std::set<std::string> liveExplicitTurnarounds;
    for (std::set<std::string>::const_iterator it = myExplicitTurnarounds.begin(); it != myExplicitTurnarounds.end(); it++) {
        if (myEdges.count(*it) > 0) {
            liveExplicitTurnarounds.insert(*it);
        }
    }
    myNetBuilder->compute(oc, liveExplicitTurnarounds, volatileOptions);
    // update ids if necessary
    if (oc.getBool("numerical-ids") || oc.isSet("reserved-ids")) {
        GNEEdges newEdgeMap;
        GNEJunctions newJunctionMap;
        for (GNEEdges::const_iterator it = myEdges.begin(); it != myEdges.end(); it++) {
            it->second->setMicrosimID(it->second->getNBEdge()->getID());
            newEdgeMap[it->second->getNBEdge()->getID()] = it->second;
        }
        for (GNEJunctions::const_iterator it = myJunctions.begin(); it != myJunctions.end(); it++) {
            newJunctionMap[it->second->getNBNode()->getID()] = it->second;
            it->second->setMicrosimID(it->second->getNBNode()->getID());
        }
        myEdges = newEdgeMap;
        myJunctions = newJunctionMap;
    }
    // update rtree if necessary
    if (!oc.getBool("offset.disable-normalization")) {
        for (GNEEdges::const_iterator it = myEdges.begin(); it != myEdges.end(); it++) {
            refreshElement(it->second);
        }
    }
    myGrid.reset();
    myGrid.add(GeoConvHelper::getFinal().getConvBoundary());
    // if volatile options are true
    if(volatileOptions) {

        // clear all additionals of grid
        GNEAdditionals copyOfAdditionals = myAdditionals;
        for (GNEAdditionals::iterator it = copyOfAdditionals.begin(); it != copyOfAdditionals.end(); it++) {
            myGrid.removeAdditionalGLObject(it->second);
        }
        // remove all edges of grid and net
        GNEEdges copyOfEdges = myEdges;
        for (GNEEdges::iterator it = copyOfEdges.begin(); it != copyOfEdges.end(); it++) {
            myGrid.removeAdditionalGLObject(it->second);
            myEdges.erase(it->second->getMicrosimID());
        }
        // removes all junctions of grid and net
        GNEJunctions copyOfJunctions = myJunctions;
        for (GNEJunctions::iterator it = copyOfJunctions.begin(); it != copyOfJunctions.end(); it++) {
            myGrid.removeAdditionalGLObject(it->second);
            myJunctions.erase(it->second->getMicrosimID());
        }

        // clear undo list
        myViewNet->getUndoList()->clear();

        // clear additionals (must be do it separated)
        myAdditionals.clear();

        // init again junction an edges
        initJunctionsAndEdges();
    }
    
    // update precomputed geometries
    initGNEConnections();

    for (GNEJunctions::const_iterator it = myJunctions.begin(); it != myJunctions.end(); it++) {
        it->second->setLogicValid(true);
        // updated shape
        it->second->updateGeometry();
        refreshElement(it->second);
    }

    myNeedRecompute = false;
}


bool
GNENet::isAdditionalsSaved() const {
    return myAdditionalsSaved;
}


bool
GNENet::isShapesSaved() const {
    return myShapesSaved;
}


void 
GNENet::replaceInListAttribute(GNEAttributeCarrier* ac, SumoXMLAttr key, const std::string& which, const std::string& by, GNEUndoList* undoList) {
    assert(GNEAttributeCarrier::isList(ac->getTag(), key));
    std::vector<std::string> values = GNEAttributeCarrier::parse<std::vector<std::string> >(ac->getAttribute(key));
    std::vector<std::string> newValues;
    for (auto v : values) {
        newValues.push_back(v == which ? by : v);
    }
    ac->setAttribute(key, toString(newValues), undoList);
}

/****************************************************************************/

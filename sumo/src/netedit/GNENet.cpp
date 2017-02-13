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
#include <utils/common/TplConvert.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/xml/SUMOXMLDefinitions.h>

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
#include "GNEConnection.h"
#include "GNECrossing.h"
#include "GNEDetector.h"
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNELane.h"
#include "GNENet.h"
#include "GNEStoppingPlace.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
const RGBColor GNENet::selectionColor(0, 0, 204, 255);
const RGBColor GNENet::selectedLaneColor(0, 0, 128, 255);
const RGBColor GNENet::selectedConnectionColor(0, 0, 100, 255);
const SUMOReal GNENet::Z_INITIALIZED = 1;

// ===========================================================================
// member method definitions
// ===========================================================================
GNENet::GNENet(NBNetBuilder* netBuilder) :
    GUIGlObject(GLO_NETWORK, ""),
    myViewNet(0),
    myNetBuilder(netBuilder),
    myJunctions(),
    myEdges(),
    myEdgeIDSupplier("gneE", netBuilder->getEdgeCont().getAllNames()),
    myJunctionIDSupplier("gneJ", netBuilder->getNodeCont().getAllNames()),
    myShapeContainer(myGrid),
    myNeedRecompute(true) {
    GUIGlObjectStorage::gIDStorage.setNetObject(this);

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

    /*
    // initialise detector storage for gui
    initDetectors();
    // initialise the tl-map
    initTLMap();
    // initialise edge storage for gui
    GUIEdge::fill(myEdgeWrapper);
    */
    //if (myGrid.count() == 0) // myGrid methods will return garbage

    // Init AdditionalHandler

    if (myZBoundary.ymin() != Z_INITIALIZED) {
        myZBoundary.add(0, 0);
    }
}


GNENet::~GNENet() {
    /*
    // delete allocated wrappers
    //  of junctions
    for (std::vector<GUIJunctionWrapper*>::iterator i1=myJunctionWrapper.begin(); i1!=myJunctionWrapper.end(); i1++) {
        delete(*i1);
    }
    //  of additional structures
    GUIGlObject_AbstractAdd::clearDictionary();
    //  of tl-logics
    for (Logics2WrapperMap::iterator i3=myLogics2Wrapper.begin(); i3!=myLogics2Wrapper.end(); i3++) {
        delete(*i3).second;
    }
    //  of detectors
    for (std::map<std::string, GUIDetectorWrapper*>::iterator i=myDetectorDict.begin(); i!=myDetectorDict.end(); ++i) {
        delete(*i).second;
    }
    */
    for (GNEEdges::iterator it = myEdges.begin(); it != myEdges.end(); it++) {
        it->second->decRef("GNENet::~GNENet");
        delete it->second;
    }
    for (GNEJunctions::iterator it = myJunctions.begin(); it != myJunctions.end(); it++) {
        it->second->decRef("GNENet::~GNENet");
        delete it->second;
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
        SUMOReal defaultSpeed = 50 / 3.6;
        std::string defaultType = "";
        int defaultNrLanes = 1;
        int defaultPriority = 1;
        SUMOReal defaultWidth = NBEdge::UNSPECIFIED_WIDTH;
        SUMOReal defaultOffset = NBEdge::UNSPECIFIED_OFFSET;
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

    // deleting edges changes in the underlying EdgeVector so we have to make a copy
    const EdgeVector incident = junction->getNBNode()->getEdges();
    for (EdgeVector::const_iterator it = incident.begin(); it != incident.end(); it++) {
        deleteEdge(myEdges[(*it)->getID()], undoList);
    }

    // remove any traffic lights from the traffic light container (avoids lots of warnings)
    junction->setAttribute(SUMO_ATTR_TYPE, toString(NODETYPE_PRIORITY), undoList);
    undoList->add(new GNEChange_Junction(junction, false), true);
    if (gSelected.isSelected(GLO_JUNCTION, junction->getGlID())) {
        std::set<GUIGlID> deselected;
        deselected.insert(junction->getGlID());
        undoList->add(new GNEChange_Selection(this, std::set<GUIGlID>(), deselected, true), true);
    }
    undoList->p_end();
}


void
GNENet::deleteEdge(GNEEdge* edge, GNEUndoList* undoList) {
    undoList->p_begin("delete " + toString(SUMO_TAG_EDGE));
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

    // invalidate junction (saving connections)
    edge->getGNEJunctionSource()->removeFromCrossings(edge, undoList);
    edge->getGNEJunctionDestiny()->removeFromCrossings(edge, undoList);
    edge->getGNEJunctionSource()->setLogicValid(false, undoList);
    edge->getGNEJunctionDestiny()->setLogicValid(false, undoList);

    // Delete edge
    undoList->add(new GNEChange_Edge(edge, false), true);

    // If previously was selected
    if (gSelected.isSelected(GLO_EDGE, edge->getGlID())) {
        std::set<GUIGlID> deselected;
        deselected.insert(edge->getGlID());
        undoList->add(new GNEChange_Selection(this, std::set<GUIGlID>(), deselected, true), true);
    }

    requireRecompute();
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
        edge->getGNEJunctionSource()->removeFromCrossings(edge, undoList);
        edge->getGNEJunctionDestiny()->removeFromCrossings(edge, undoList);
        edge->getGNEJunctionSource()->setLogicValid(false, undoList);
        edge->getGNEJunctionDestiny()->setLogicValid(false, undoList);

        // delete lane
        const NBEdge::Lane& laneAttrs = edge->getNBEdge()->getLaneStruct(lane->getIndex());
        undoList->add(new GNEChange_Lane(edge, lane, laneAttrs, false), true);
        if (gSelected.isSelected(GLO_LANE, lane->getGlID())) {
            std::set<GUIGlID> deselected;
            deselected.insert(lane->getGlID());
            undoList->add(new GNEChange_Selection(this, std::set<GUIGlID>(), deselected, true), true);
        }
        requireRecompute();
        undoList->p_end();
    }
}


void
GNENet::deleteConnection(GNEConnection* connection, GNEUndoList* undoList) {
    undoList->p_begin("delete " + toString(SUMO_TAG_CONNECTION));
    NBConnection deleted = connection->getNBConnection();
    GNEJunction* affected = connection->getEdgeFrom()->getGNEJunctionDestiny();
    affected->markAsModified(undoList);
    undoList->add(new GNEChange_Connection(connection->getEdgeFrom(), connection->getNBEdgeConnection(), false), true);
    affected->invalidateTLS(myViewNet->getUndoList(), deleted);
    if (gSelected.isSelected(GLO_CONNECTION, connection->getGlID())) {
        std::set<GUIGlID> deselected;
        deselected.insert(connection->getGlID());
        undoList->add(new GNEChange_Selection(this, std::set<GUIGlID>(), deselected, true), true);
    }
    requireRecompute();
    undoList->p_end();
}


void
GNENet::deleteCrossing(GNECrossing* crossing, GNEUndoList* undoList) {
    undoList->p_begin("delete crossing");
    undoList->add(new GNEChange_Crossing(crossing->getParentJunction(), crossing->getNBCrossing().edges,
                                         crossing->getNBCrossing().width, crossing->getNBCrossing().priority, false), true);
    if (gSelected.isSelected(GLO_CROSSING, crossing->getGlID())) {
        std::set<GUIGlID> deselected;
        deselected.insert(crossing->getGlID());
        undoList->add(new GNEChange_Selection(this, std::set<GUIGlID>(), deselected, true), true);
    }
    requireRecompute();
    undoList->p_end();
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
    const SUMOReal linePos = oldGeom.nearest_offset_to_point2D(pos, false);
    std::pair<PositionVector, PositionVector> newGeoms = oldGeom.splitAt(linePos);
    // figure out the new name
    int posBase = 0;
    std::string baseName = edge->getMicrosimID();
    if (edge->wasSplit()) {
        const std::string::size_type sep_index = baseName.rfind('.');
        if (sep_index != std::string::npos) { // edge may have been renamed in between
            std::string posString = baseName.substr(sep_index + 1);
            try {
                posBase = TplConvert::_2int(posString.c_str());
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
        undoList->add(new GNEChange_Connection(secondPart, *con_it, true), true);
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
    GNEEdge* reversed = 0;
    if (edge->getNBEdge()->getLaneSpreadFunction() == LANESPREAD_RIGHT) {
        GNEEdge* reversed = createEdge(edge->getGNEJunctionDestiny(), edge->getGNEJunctionSource(), edge, undoList, "-" + edge->getID(), false, true);
        assert(reversed != 0);
        reversed->setAttribute(SUMO_ATTR_SHAPE, toString(edge->getNBEdge()->getInnerGeometry().reverse()), undoList);
    } else {
        // if the edge is centered it should probably connect somewhere else
        // make it easy to move and reconnect it
        PositionVector orig = edge->getNBEdge()->getGeometry();
        PositionVector origInner = edge->getNBEdge()->getInnerGeometry();
        const SUMOReal tentativeShift = edge->getNBEdge()->getTotalWidth() + 2;
        orig.move2side(-tentativeShift);
        origInner.move2side(-tentativeShift);
        GNEJunction* src = createJunction(orig.back(), undoList);
        GNEJunction* dest = createJunction(orig.front(), undoList);
        GNEEdge* reversed = createEdge(src, dest, edge, undoList, "-" + edge->getID(), false, true);
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
    moved->move(target->getNBNode()->getPosition());
    // register the move with undolist (must happend within the undo group)
    moved->registerMove(undoList);
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
            newEdge->setAttribute(GNE_ATTR_SHAPE_START, oldEdge->getAttribute(GNE_ATTR_SHAPE_START), undoList);
            newEdge->setAttribute(GNE_ATTR_SHAPE_END, oldEdge->getAttribute(GNE_ATTR_SHAPE_END), undoList);
        }
    }
    // @todo remap connectivity as well
}


void
GNENet::save(OptionsCont& oc) {
    // compute and update network
    computeAndUpdate(oc);
    // write network
    NWFrame::writeNetwork(oc, *myNetBuilder);
}


void
GNENet::saveAdditionals(const std::string& filename) {
    OutputDevice& device = OutputDevice::getDevice(filename);
    device.openTag("additionals");
    for (GNEAdditionals::const_iterator i = myAdditionals.begin(); i != myAdditionals.end(); ++i) {
        i->second->writeAdditional(device);
    }
    device.close();
}


void
GNENet::savePlain(OptionsCont& oc) {
    computeAndUpdate(oc);
    NWWriter_XML::writeNetwork(oc, *myNetBuilder);
}


void
GNENet::saveJoined(OptionsCont& oc) {
    computeAndUpdate(oc);
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
            for (std::set<GUIGlObjectType>::const_iterator it = knownTypes.begin(); it != knownTypes.end(); it++) {
                const std::set<GUIGlID> tmp = getGlIDs(*it);
                result.insert(tmp.begin(), tmp.end());
            }
            break;
        }
        case GLO_JUNCTION:
            for (GNEJunctions::const_iterator it = myJunctions.begin(); it != myJunctions.end(); it++) {
                result.insert(it->second->getGlID());
            }
            break;
        case GLO_EDGE:
            for (GNEEdges::const_iterator it = myEdges.begin(); it != myEdges.end(); it++) {
                result.insert(it->second->getGlID());
            }
            break;
        case GLO_LANE: {
            for (GNEEdges::const_iterator it = myEdges.begin(); it != myEdges.end(); it++) {
                const std::set<GUIGlID> laneIDs = it->second->getLaneGlIDs();
                for (std::set<GUIGlID>::const_iterator lid_it = laneIDs.begin(); lid_it != laneIDs.end(); lid_it++) {
                    result.insert(*lid_it);
                }
            }
            break;
        }
        case GLO_TLLOGIC: {
            // return all junctions which have a traffic light (we do not have a GUIGlObject for each traffic light)
            for (GNEJunctions::const_iterator it = myJunctions.begin(); it != myJunctions.end(); it++) {
                if (it->second->getNBNode()->isTLControlled()) {
                    result.insert(it->second->getGlID());
                }
            }
            break;
        }
        case GLO_ADDITIONAL: {
            // Iterate over all additionals of net
            for (GNEAdditionals::iterator it = myAdditionals.begin(); it != myAdditionals.end(); it++) {
                // Insert every additional in result
                result.insert(it->second->getGlID());
            }
            break;
        }
        case GLO_CONNECTION: {
            for (GNEEdges::const_iterator i = myEdges.begin(); i != myEdges.end(); i++) {
                // Get connections of edge
                const std::vector<GNEConnection*>& connections = i->second->getGNEConnections();
                // Iterate over connections
                for (std::vector<GNEConnection*>::const_iterator j = connections.begin(); j != connections.end(); j++) {
                    // Insert every connection of edge it in result
                    result.insert((*j)->getGlID());
                }
            }
            break;
        }
        default: // add other types once we know them
            break;
    }
    return result;
}


void
GNENet::computeEverything(GNEApplicationWindow* window, bool force) {
    if (!myNeedRecompute) {
        if (force) {
            window->setStatusBarText("Forced computing junctions ...");
        } else {
            return;
        }
    } else {
        window->setStatusBarText("Computing junctions ...");
    }
    // compute
    OptionsCont& oc = OptionsCont::getOptions();
    computeAndUpdate(oc);
    WRITE_MESSAGE("\nFinished computing junctions.");

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
GNENet::netHasCrossings() const {
    for (std::map<std::string, NBNode*>::const_iterator i = myNetBuilder->getNodeCont().begin(); i != myNetBuilder->getNodeCont().end() ; i++) {
        if (i->second->getCrossings().size() > 0) {
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


void
GNENet::joinSelectedJunctions(GNEUndoList* undoList) {
    std::vector<GNEJunction*> selected = retrieveJunctions(true);
    if (selected.size() < 2) {
        return;
    }
    undoList->p_begin("Join selected " + toString(SUMO_TAG_JUNCTION) + "s");

    EdgeVector allIncoming;
    EdgeVector allOutgoing;
    std::set<NBNode*> cluster;
    for (std::vector<GNEJunction*>::iterator it = selected.begin(); it != selected.end(); it++) {
        cluster.insert((*it)->getNBNode());
        const EdgeVector& incoming = (*it)->getNBNode()->getIncomingEdges();
        allIncoming.insert(allIncoming.end(), incoming.begin(), incoming.end());
        const EdgeVector& outgoing = (*it)->getNBNode()->getOutgoingEdges();
        allOutgoing.insert(allOutgoing.end(), outgoing.begin(), outgoing.end());
    }
    // create new junction
    Position pos;
    bool setTL;
    std::string id;
    TrafficLightType type;
    myNetBuilder->getNodeCont().analyzeCluster(cluster, id, pos, setTL, type);
    // XXX this is not undone when calling 'undo'
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
    for (std::vector<GNEJunction*>::iterator it = selected.begin(); it != selected.end(); it++) {
        deleteJunction(*it, undoList);
    }
    joined->setAttribute(SUMO_ATTR_ID, id, undoList);
    undoList->p_end();
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
    for (std::vector<GNEJunction*>::iterator it = toRemove.begin(); it != toRemove.end(); ++it) {
        deleteJunction(*it, undoList);
    }
    undoList->p_end();
}


void
GNENet::replaceJunctionByGeometry(GNEJunction* junction, GNEUndoList* undoList) {
    undoList->p_begin("Replace junction by geometry");
    assert(junction->getNBNode()->checkIsRemovable());
    std::vector<std::pair<NBEdge*, NBEdge*> > toJoin = junction->getNBNode()->getEdgesToJoin();
    for (std::vector<std::pair<NBEdge*, NBEdge*> >::iterator j = toJoin.begin(); j != toJoin.end(); j++) {
        GNEEdge* begin = myEdges[(*j).first->getID()];
        GNEEdge* continuation = myEdges[(*j).second->getID()];
        deleteEdge(begin, undoList);
        deleteEdge(continuation, undoList);
        GNEEdge* newEdge = createEdge(begin->getGNEJunctionSource(), continuation->getGNEJunctionDestiny(), begin, undoList, begin->getMicrosimID(), false, true);
        PositionVector newShape = begin->getNBEdge()->getInnerGeometry();
        newShape.push_back(junction->getNBNode()->getPosition());
        newShape.append(continuation->getNBEdge()->getInnerGeometry());
        newEdge->setAttribute(SUMO_ATTR_SHAPE, toString(newShape), undoList);
        // @todo what about trafficlights at the end of oontinuation?
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
    // move junctions
    std::set<GNEJunction*> junctionSet;
    std::vector<GNEJunction*> junctions = retrieveJunctions(true);
    for (std::vector<GNEJunction*>::iterator it = junctions.begin(); it != junctions.end(); it++) {
        Position newPos = (*it)->getNBNode()->getPosition() + delta;
        (*it)->move(newPos);
        junctionSet.insert(*it);
    }

    // move edge geometry (endpoints are already moved)
    std::vector<GNEEdge*> edges = retrieveEdges(true);
    for (std::vector<GNEEdge*>::iterator it = edges.begin(); it != edges.end(); it++) {
        GNEEdge* edge = *it;
        if (edge) {
            if (junctionSet.count(edge->getGNEJunctionSource()) > 0 &&
                    junctionSet.count(edge->getGNEJunctionDestiny()) > 0) {
                // edge and its endpoints are selected, move all the inner points as well
                edge->moveGeometry(delta);
            } else {
                // move only geometry near mouse
                edge->moveGeometry(moveSrc, delta, true);
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
    for (std::vector<GNEJunction*>::iterator it = junctions.begin(); it != junctions.end(); it++) {
        (*it)->registerMove(undoList);
        junctionSet.insert(*it);
    }

    // register moved edge geometry (endpoints are already moved)
    std::vector<GNEEdge*> edges = retrieveEdges(true);
    for (std::vector<GNEEdge*>::iterator it = edges.begin(); it != edges.end(); it++) {
        GNEEdge* edge = *it;
        if (edge) {
            const std::string& newShape = edge->getAttribute(SUMO_ATTR_SHAPE);
            edge->setAttribute(SUMO_ATTR_SHAPE, newShape, undoList);
        }
    }
    undoList->p_end();
}


ShapeContainer&
GNENet::getShapeContainer() {
    return myShapeContainer;
}


void
GNENet::insertAdditional(GNEAdditional* additional, bool hardFail) {
    // Check if additional element exists before insertion
    if (myAdditionals.find(std::pair<std::string, SumoXMLTag>(additional->getID(), additional->getTag())) != myAdditionals.end()) {
        // Throw exception only if hardFail is enabled
        if (hardFail) {
            throw ProcessError(toString(additional->getTag()) + "  with ID='" + additional->getID() + "' already exist");
        }
    } else {
        myAdditionals[std::pair<std::string, SumoXMLTag>(additional->getID(), additional->getTag())] = additional;
        myGrid.addAdditionalGLObject(additional);
        update();
    }
}


void
GNENet::deleteAdditional(GNEAdditional* additional) {
    GNEAdditionals::iterator additionalToRemove = myAdditionals.find(std::pair<std::string, SumoXMLTag>(additional->getID(), additional->getTag()));
    // Check if additional element exists before deletion
    if (additionalToRemove == myAdditionals.end()) {
        throw ProcessError(toString(additional->getTag()) + "  with ID='" + additional->getID() + "' doesn't exist");
    } else {
        myAdditionals.erase(additionalToRemove);
        myGrid.removeAdditionalGLObject(additional);
        update();
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
GNENet::getAdditionalID(SumoXMLTag type, const GNELane* lane, const SUMOReal pos) const {
    for (GNEAdditionals::const_iterator it = myAdditionals.begin(); it != myAdditionals.end(); ++it) {
        if ((it->second->getTag() == type) && (it->second->getLane() != NULL) && (it->second->getLane() == lane) && (fabs(it->second->getPositionInView().x() - pos) < POSITION_EPS)) {
            return it->second->getID();
        }
    }
    return "";
}


std::vector<GNEAdditional*>
GNENet::getAdditionals(SumoXMLTag type) {
    std::vector<GNEAdditional*> vectorOfAdditionals;
    for (GNEAdditionals::iterator i = myAdditionals.begin(); i != myAdditionals.end(); i++) {
        if (type == SUMO_TAG_NOTHING || type == i->second->getTag()) {
            vectorOfAdditionals.push_back(i->second);
        }
    }
    return vectorOfAdditionals;
}


int
GNENet::getNumberOfAdditionals(SumoXMLTag type) {
    int counter = 0;
    for (GNEAdditionals::iterator i = myAdditionals.begin(); i != myAdditionals.end(); i++) {
        if (type == SUMO_TAG_NOTHING || type == i->second->getTag()) {
            counter++;
        }
    }
    return counter;
}

// ===========================================================================
// private
// ===========================================================================

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
    const SUMOReal z = junction->getNBNode()->getPosition().z();
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
GNENet::computeAndUpdate(OptionsCont& oc) {
    // make sure we only add turn arounds to edges which currently exist within the network
    std::set<std::string> liveExplicitTurnarounds;
    for (std::set<std::string>::const_iterator it = myExplicitTurnarounds.begin(); it != myExplicitTurnarounds.end(); it++) {
        if (myEdges.count(*it) > 0) {
            liveExplicitTurnarounds.insert(*it);
        }
    }
    myNetBuilder->compute(oc, liveExplicitTurnarounds, false);
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
    myGrid.add(GeoConvHelper::getProcessing().getConvBoundary());
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


/****************************************************************************/

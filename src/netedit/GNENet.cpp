/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
// WorkrouteFlow (rough draft)
//   use NILoader to fill
//   do netedit stuff
//   call compute to save results
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <netbuild/NBAlgorithms.h>
#include <netbuild/NBNetBuilder.h>
#include <netedit/additionals/GNEAdditionalHandler.h>
#include <netedit/demandelements/GNEVehicleType.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/additionals/GNEPOI.h>
#include <netedit/additionals/GNEPoly.h>
#include <netedit/demandelements/GNERouteHandler.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_Connection.h>
#include <netedit/changes/GNEChange_Crossing.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/changes/GNEChange_Edge.h>
#include <netedit/changes/GNEChange_Junction.h>
#include <netedit/changes/GNEChange_Lane.h>
#include <netedit/changes/GNEChange_Shape.h>
#include <netedit/dialogs/GNEDialog_FixAdditionalElements.h>
#include <netedit/dialogs/GNEDialog_FixDemandElements.h>
#include <netedit/frames/GNEInspectorFrame.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/netelements/GNECrossing.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <netwrite/NWFrame.h>
#include <netwrite/NWWriter_SUMO.h>
#include <netwrite/NWWriter_XML.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/XMLSubSys.h>

#include "GNEApplicationWindow.h"
#include "GNENet.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNEViewParent.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXIMPLEMENT_ABSTRACT(GNENet::GNEChange_ReplaceEdgeInTLS, GNEChange, nullptr, 0)

// ===========================================================================
// static members
// ===========================================================================

const double GNENet::Z_INITIALIZED = 1;

// ===========================================================================
// member method definitions
// ===========================================================================

GNENet::GNENet(NBNetBuilder* netBuilder) :
    GUIGlObject(GLO_NETWORK, ""),
    ShapeContainer(),
    myViewNet(nullptr),
    myNetBuilder(netBuilder),
    myEdgeIDSupplier("gneE", netBuilder->getEdgeCont().getAllNames()),
    myJunctionIDSupplier("gneJ", netBuilder->getNodeCont().getAllNames()),
    myNeedRecompute(true),
    myNetSaved(true),
    myAdditionalsSaved(true),
    myTLSProgramsSaved(true),
    myDemandElementsSaved(true),
    myUpdateGeometryEnabled(true),
    myAllowUndoShapes(true) {
    // set net in gIDStorage
    GUIGlObjectStorage::gIDStorage.setNetObject(this);
    // Write GL debug information
    WRITE_GLDEBUG("initJunctionsAndEdges function called in GNENet constructor");
    // init junction and edges
    initJunctionsAndEdges();
    // check Z boundary
    if (myZBoundary.ymin() != Z_INITIALIZED) {
        myZBoundary.add(0, 0);
    }
    // fill additionals with tags (note: this include the TAZS)
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_ADDITIONAL, false);
    for (auto i : listOfTags) {
        myAttributeCarriers.additionals.insert(std::make_pair(i, std::map<std::string, GNEAdditional*>()));
    }
    listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_TAZ, false);
    for (auto i : listOfTags) {
        myAttributeCarriers.additionals.insert(std::make_pair(i, std::map<std::string, GNEAdditional*>()));
    }

    // fill demand elements with tags
    listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_DEMANDELEMENT, false);
    for (auto i : listOfTags) {
        myAttributeCarriers.demandElements.insert(std::make_pair(i, std::map<std::string, GNEDemandElement*>()));
    }
    listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_STOP, false);
    for (auto i : listOfTags) {
        myAttributeCarriers.demandElements.insert(std::make_pair(i, std::map<std::string, GNEDemandElement*>()));
    }
}


GNENet::~GNENet() {
    // Decrease reference of Polys (needed after volatile recomputing)
    for (auto i : myPolygons) {
        dynamic_cast<GNEAttributeCarrier*>(i.second)->decRef("GNENet::~GNENet");
    }
    // Decrease reference of POIs (needed after volatile recomputing)
    for (auto i : myPOIs) {
        dynamic_cast<GNEAttributeCarrier*>(i.second)->decRef("GNENet::~GNENet");
    }
    // Drop Edges
    for (auto it : myAttributeCarriers.edges) {
        it.second->decRef("GNENet::~GNENet");
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + it.second->getTagStr() + " '" + it.second->getID() + "' in GNENet destructor");
        delete it.second;
    }
    // Drop junctions
    for (auto it : myAttributeCarriers.junctions) {
        it.second->decRef("GNENet::~GNENet");
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + it.second->getTagStr() + " '" + it.second->getID() + "' in GNENet destructor");
        delete it.second;
    }
    // Drop Additionals (Only used for additionals that were inserted without using GNEChange_Additional)
    for (auto it : myAttributeCarriers.additionals) {
        for (auto j : it.second) {
            // decrease reference manually (because it was increased manually in GNEAdditionalHandler)
            j.second->decRef();
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + j.second->getTagStr() + " '" + j.second->getID() + "' in GNENet destructor");
            delete j.second;
        }
    }
    // Drop demand elements (Only used for demand elements that were inserted without using GNEChange_DemandElement, for example the default VType")
    for (auto it : myAttributeCarriers.demandElements) {
        for (auto j : it.second) {
            // decrease reference manually (because it was increased manually in GNERouteHandler)
            j.second->decRef();
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + j.second->getTagStr() + " '" + j.second->getID() + "' in GNENet destructor");
            delete j.second;
        }
    }
    // delete RouteCalculator instance of GNEDemandElement
    GNEDemandElement::deleteRouteCalculatorInstance();
    // show extra information for tests
    WRITE_DEBUG("Deleting net builder in GNENet destructor");
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
GNENet::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    // Nets lanes don't have attributes
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, 2);
    // close building
    ret->closeBuilding();
    return ret;
}


void
GNENet::drawGL(const GUIVisualizationSettings& /*s*/) const {
}


bool
GNENet::addPolygon(const std::string& id, const std::string& type, const RGBColor& color, double layer, double angle,
                   const std::string& imgFile, bool relativePath, const PositionVector& shape, bool geo, bool fill, double lineWidth, bool /*ignorePruning*/) {
    // check if ID is duplicated
    if (myPolygons.get(id) == nullptr) {
        // create poly
        GNEPoly* poly = new GNEPoly(this, id, type, shape, geo, fill, lineWidth, color, layer, angle, imgFile, relativePath, false, false);
        if (myAllowUndoShapes) {
            myViewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_POLY));
            myViewNet->getUndoList()->add(new GNEChange_Shape(poly, true), true);
            myViewNet->getUndoList()->p_end();
        } else {
            // insert shape without allowing undo/redo
            insertShape(poly, true);
            poly->incRef("addPolygon");
        }
        return true;
    } else {
        return false;
    }
}


bool
GNENet::addPOI(const std::string& id, const std::string& type, const RGBColor& color, const Position& pos, bool geo,
               const std::string& lane, double posOverLane, double posLat, double layer, double angle,
               const std::string& imgFile, bool relativePath, double width, double height, bool /*ignorePruning*/) {
    // check if ID is duplicated
    if (myPOIs.get(id) == nullptr) {
        // create POI or POILane depending of parameter lane
        if (lane == "") {
            // create POI
            GNEPOI* poi = new GNEPOI(this, id, type, color, pos, geo, layer, angle, imgFile, relativePath, width, height, false);
            if (myPOIs.add(poi->getID(), poi)) {
                if (myAllowUndoShapes) {
                    myViewNet->getUndoList()->p_begin("add " + poi->getTagStr());
                    myViewNet->getUndoList()->add(new GNEChange_Shape(poi, true), true);
                    myViewNet->getUndoList()->p_end();
                } else {
                    // insert shape without allowing undo/redo
                    insertShape(poi, true);
                    poi->incRef("addPOI");
                }
                return true;
            } else {
                throw ProcessError("Error adding GNEPOI into shapeContainer");
            }
        } else {
            // create POI over lane
            GNELane* retrievedLane = retrieveLane(lane);
            GNEPOI* poi = new GNEPOI(this, id, type, color, layer, angle, imgFile, relativePath, retrievedLane, posOverLane, posLat, width, height, false);
            if (myPOIs.add(poi->getID(), poi)) {
                if (myAllowUndoShapes) {
                    myViewNet->getUndoList()->p_begin("add " + poi->getTagStr());
                    myViewNet->getUndoList()->add(new GNEChange_Shape(poi, true), true);
                    myViewNet->getUndoList()->p_end();
                } else {
                    // insert shape without allowing undo/redo
                    insertShape(poi, true);
                    poi->incRef("addPOI");
                }
                return true;
            } else {
                throw ProcessError("Error adding GNEPOI over lane into shapeContainer");
            }
        }
    } else {
        return false;
    }
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
    assert(myAttributeCarriers.junctions[id]);
    return junction;
}


GNEEdge*
GNENet::createEdge(
    GNEJunction* src, GNEJunction* dest, GNEEdge* tpl, GNEUndoList* undoList,
    const std::string& suggestedName,
    bool wasSplit,
    bool allowDuplicateGeom,
    bool recomputeConnections) {
    // prevent duplicate edge (same geometry)
    const EdgeVector& outgoing = src->getNBNode()->getOutgoingEdges();
    for (EdgeVector::const_iterator it = outgoing.begin(); it != outgoing.end(); it++) {
        if ((*it)->getToNode() == dest->getNBNode() && (*it)->getGeometry().size() == 2) {
            if (!allowDuplicateGeom) {
                return nullptr;
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
        const OptionsCont& oc = OptionsCont::getOptions();
        double defaultSpeed = oc.getFloat("default.speed");
        std::string defaultType = "";
        int defaultNrLanes = oc.getInt("default.lanenumber");
        int defaultPriority = oc.getInt("default.priority");
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
    if (recomputeConnections) {
        src->setLogicValid(false, undoList);
        dest->setLogicValid(false, undoList);
    }
    requireRecompute();
    undoList->p_end();
    assert(myAttributeCarriers.edges[id]);
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
    for (auto i : junctionNeighbours) {
        // iterate over crossing of neighbour juntion
        for (auto j : i->getGNECrossings()) {
            // if at least one of the edges of junction to remove belongs to a crossing of the neighbour junction, delete it
            if (j->checkEdgeBelong(junction->getGNEEdges())) {
                crossingsToRemove.push_back(j);
            }
        }
    }

    // delete crossings top remove
    for (auto i : crossingsToRemove) {
        deleteCrossing(i, undoList);
    }

    // deleting edges changes in the underlying EdgeVector so we have to make a copy
    const EdgeVector incident = junction->getNBNode()->getEdges();
    for (auto it : incident) {
        deleteEdge(myAttributeCarriers.edges[it->getID()], undoList, true);
    }

    // remove any traffic lights from the traffic light container (avoids lots of warnings)
    junction->setAttribute(SUMO_ATTR_TYPE, toString(NODETYPE_PRIORITY), undoList);

    // delete edge
    undoList->add(new GNEChange_Junction(junction, false), true);
    undoList->p_end();
}


void
GNENet::deleteEdge(GNEEdge* edge, GNEUndoList* undoList, bool recomputeConnections) {
    undoList->p_begin("delete " + toString(SUMO_TAG_EDGE));
    // delete all shapes children of edge
    while (edge->getShapeChildren().size() > 0) {
        deleteShape(edge->getShapeChildren().front(), undoList);
    }
    // delete all shapes children of lane
    for (auto i : edge->getLanes()) {
        while (i->getShapeChildren().size() > 0) {
            deleteShape(i->getShapeChildren().front(), undoList);
        }
    }
    // delete all additionals children of edge
    while (edge->getAdditionalChildren().size() > 0) {
        deleteAdditional(edge->getAdditionalChildren().front(), undoList);
    }
    // delete all additionals children of lane
    for (auto i : edge->getLanes()) {
        while (i->getAdditionalChildren().size() > 0) {
            deleteAdditional(i->getAdditionalChildren().front(), undoList);
        }
    }
    // delete all demand element children of edge
    while (edge->getDemandElementChildren().size() > 0) {
        deleteDemandElement(edge->getDemandElementChildren().front(), undoList);
    }
    // delete all demand element children of lane
    for (auto i : edge->getLanes()) {
        while (i->getDemandElementChildren().size() > 0) {
            deleteDemandElement(i->getDemandElementChildren().front(), undoList);
        }
    }
    // remove edge from crossings related with this edge
    edge->getGNEJunctionSource()->removeEdgeFromCrossings(edge, undoList);
    edge->getGNEJunctionDestiny()->removeEdgeFromCrossings(edge, undoList);
    // update affected connections
    if (recomputeConnections) {
        edge->getGNEJunctionSource()->setLogicValid(false, undoList);
        edge->getGNEJunctionDestiny()->setLogicValid(false, undoList);
    } else {
        edge->getGNEJunctionSource()->removeConnectionsTo(edge, undoList, true);
        edge->getGNEJunctionSource()->removeConnectionsFrom(edge, undoList, true);
    }
    // if junction source is a TLS and after deletion will have only an edge, remove TLS
    if (edge->getGNEJunctionSource()->getNBNode()->isTLControlled() && (edge->getGNEJunctionSource()->getGNEOutgoingEdges().size() <= 1)) {
        edge->getGNEJunctionSource()->setAttribute(SUMO_ATTR_TYPE, toString(NODETYPE_PRIORITY), undoList);
    }
    // if junction destiny is a TLS and after deletion will have only an edge, remove TLS
    if (edge->getGNEJunctionDestiny()->getNBNode()->isTLControlled() && (edge->getGNEJunctionDestiny()->getGNEIncomingEdges().size() <= 1)) {
        edge->getGNEJunctionDestiny()->setAttribute(SUMO_ATTR_TYPE, toString(NODETYPE_PRIORITY), undoList);
    }
    // Delete edge
    undoList->add(new GNEChange_Edge(edge, false), true);
    // remove edge requieres always a recompute (due geometry and connections)
    requireRecompute();
    // finish delete edge
    undoList->p_end();
}


void
GNENet::replaceIncomingEdge(GNEEdge* which, GNEEdge* by, GNEUndoList* undoList) {
    undoList->p_begin("replace " + toString(SUMO_TAG_EDGE));
    undoList->p_add(new GNEChange_Attribute(by, this, SUMO_ATTR_TO, which->getAttribute(SUMO_ATTR_TO)));
    // replace in additionals children of edge
    while (which->getAdditionalChildren().size() > 0) {
        undoList->p_add(new GNEChange_Attribute(which->getAdditionalChildren().front(), this, SUMO_ATTR_EDGE, by->getID()));
    }
    // replace in additionals children of lane
    for (auto i : which->getLanes()) {
        std::vector<GNEAdditional*> copyOfLaneAdditionals = i->getAdditionalChildren();
        for (auto j : copyOfLaneAdditionals) {
            undoList->p_add(new GNEChange_Attribute(j, this, SUMO_ATTR_LANE, by->getNBEdge()->getLaneID(i->getIndex())));
        }
    }
    // replace in demand elements children of edge
    while (which->getDemandElementChildren().size() > 0) {
        undoList->p_add(new GNEChange_Attribute(which->getDemandElementChildren().front(), this, SUMO_ATTR_EDGE, by->getID()));
    }
    // replace in demand elements children of lane
    for (auto i : which->getLanes()) {
        std::vector<GNEDemandElement*> copyOfLaneDemandElements = i->getDemandElementChildren();
        for (auto j : copyOfLaneDemandElements) {
            undoList->p_add(new GNEChange_Attribute(j, this, SUMO_ATTR_LANE, by->getNBEdge()->getLaneID(i->getIndex())));
        }
    }
    // replace in shapes children of lane
    for (auto i : which->getLanes()) {
        std::vector<GNEShape*> copyOfLaneShapes = i->getShapeChildren();
        for (auto j : copyOfLaneShapes) {
            undoList->p_add(new GNEChange_Attribute(j, this, SUMO_ATTR_LANE, by->getNBEdge()->getLaneID(i->getIndex())));
        }
    }
    // replace in rerouters
    for (auto rerouter : which->getAdditionalParents()) {
        replaceInListAttribute(rerouter, SUMO_ATTR_EDGES, which->getID(), by->getID(), undoList);
    }
    // replace in crossings
    for (auto crossing : which->getGNEJunctionDestiny()->getGNECrossings()) {
        // if at least one of the edges of junction to remove belongs to a crossing of the source junction, delete it
        replaceInListAttribute(crossing, SUMO_ATTR_EDGES, which->getID(), by->getID(), undoList);
    }
    // fix connections (make a copy because they will be modified
    std::vector<NBEdge::Connection> connections = which->getNBEdge()->getConnections();
    for (auto con : connections) {
        undoList->add(new GNEChange_Connection(which, con, false, false), true);
        undoList->add(new GNEChange_Connection(by, con, false, true), true);
    }
    undoList->add(new GNEChange_ReplaceEdgeInTLS(getTLLogicCont(), which->getNBEdge(), by->getNBEdge()), true);
    // Delete edge
    undoList->add(new GNEChange_Edge(which, false), true);
    // finish replace edge
    undoList->p_end();
}


void
GNENet::deleteLane(GNELane* lane, GNEUndoList* undoList, bool recomputeConnections) {
    GNEEdge* edge = &lane->getParentEdge();
    if (edge->getNBEdge()->getNumLanes() == 1) {
        // remove the whole edge instead
        deleteEdge(edge, undoList, recomputeConnections);
    } else {
        undoList->p_begin("delete " + toString(SUMO_TAG_LANE));
        // delete additionals children of lane
        while (lane->getAdditionalChildren().size() > 0) {
            deleteAdditional(lane->getAdditionalChildren().front(), undoList);
        }
        // delete demand element children of lane
        while (lane->getDemandElementChildren().size() > 0) {
            deleteDemandElement(lane->getDemandElementChildren().front(), undoList);
        }
        // delete POIShapes of Lane
        while (lane->getShapeChildren().size() > 0) {
            undoList->add(new GNEChange_Shape(lane->getShapeChildren().front(), false), true);
        }
        // update affected connections
        if (recomputeConnections) {
            edge->getGNEJunctionSource()->setLogicValid(false, undoList);
            edge->getGNEJunctionDestiny()->setLogicValid(false, undoList);
        } else {
            edge->getGNEJunctionSource()->removeConnectionsTo(edge, undoList, true, lane->getIndex());
            edge->getGNEJunctionSource()->removeConnectionsFrom(edge, undoList, true, lane->getIndex());
        }
        // delete lane
        const NBEdge::Lane& laneAttrs = edge->getNBEdge()->getLaneStruct(lane->getIndex());
        undoList->add(new GNEChange_Lane(edge, lane, laneAttrs, false, recomputeConnections), true);
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
    undoList->add(new GNEChange_Connection(connection->getEdgeFrom(), connection->getNBEdgeConnection(), connection->isAttributeCarrierSelected(), false), true);
    junctionDestiny->invalidateTLS(undoList, deleted);
    // remove connection requieres always a recompute (due geometry and connections)
    requireRecompute();
    undoList->p_end();
}


void
GNENet::deleteCrossing(GNECrossing* crossing, GNEUndoList* undoList) {
    undoList->p_begin("delete crossing");
    // remove it using GNEChange_Crossing
    undoList->add(new GNEChange_Crossing(crossing->getParentJunction(), crossing->getNBCrossing()->edges,
                                         crossing->getNBCrossing()->width, crossing->getNBCrossing()->priority,
                                         crossing->getNBCrossing()->customTLIndex,
                                         crossing->getNBCrossing()->customTLIndex2,
                                         crossing->getNBCrossing()->customShape,
                                         crossing->isAttributeCarrierSelected(),
                                         false), true);
    // remove crossing requieres always a recompute (due geometry and connections)
    requireRecompute();
    undoList->p_end();
}


void
GNENet::deleteShape(GNEShape* shape, GNEUndoList* undoList) {
    undoList->p_begin("delete " + shape->getTagStr());
    // delete shape
    undoList->add(new GNEChange_Shape(shape, false), true);
    undoList->p_end();
}


void
GNENet::deleteAdditional(GNEAdditional* additional, GNEUndoList* undoList) {
    undoList->p_begin("delete " + additional->getTagStr());
    // first remove all additional children of this additional calling this function recursively
    while (additional->getAdditionalChildren().size() > 0) {
        deleteAdditional(additional->getAdditionalChildren().front(), undoList);
    }
    // remove additional
    undoList->add(new GNEChange_Additional(additional, false), true);
    undoList->p_end();
}


void
GNENet::deleteDemandElement(GNEDemandElement* demandElement, GNEUndoList* undoList) {
    // check that default VTypes aren't removed
    if ((demandElement->getTagProperty().getTag() == SUMO_TAG_VTYPE) && (GNEAttributeCarrier::parse<bool>(demandElement->getAttribute(GNE_ATTR_DEFAULT_VTYPE)))) {
        throw ProcessError("Trying to delete a default Vehicle Type");
    } else {
        undoList->p_begin("delete " + demandElement->getTagStr());
        // first remove all demand element children of this demandElement calling this function recursively
        while (demandElement->getDemandElementChildren().size() > 0) {
            deleteDemandElement(demandElement->getDemandElementChildren().front(), undoList);
        }
        // remove demandElement
        undoList->add(new GNEChange_DemandElement(demandElement, false), true);
        undoList->p_end();
    }
}


void
GNENet::duplicateLane(GNELane* lane, GNEUndoList* undoList, bool recomputeConnections) {
    undoList->p_begin("duplicate " + toString(SUMO_TAG_LANE));
    GNEEdge* edge = &lane->getParentEdge();
    const NBEdge::Lane& laneAttrs = edge->getNBEdge()->getLaneStruct(lane->getIndex());
    if (recomputeConnections) {
        edge->getGNEJunctionSource()->setLogicValid(false, undoList);
        edge->getGNEJunctionSource()->setLogicValid(false, undoList);
    }
    GNELane* newLane = new GNELane(*edge, lane->getIndex());
    undoList->add(new GNEChange_Lane(edge, newLane, laneAttrs, true, recomputeConnections), true);
    requireRecompute();
    undoList->p_end();
}


bool
GNENet::restrictLane(SUMOVehicleClass vclass, GNELane* lane, GNEUndoList* undoList) {
    bool addRestriction = true;
    if (vclass == SVC_PEDESTRIAN) {
        GNEEdge& edge = lane->getParentEdge();
        for (auto i : edge.getLanes()) {
            if (i->isRestricted(SVC_PEDESTRIAN)) {
                // prevent adding a 2nd sidewalk
                addRestriction = false;
            } else {
                // ensure that the sidewalk is used exclusively
                const SVCPermissions allOldWithoutPeds = edge.getNBEdge()->getPermissions(i->getIndex()) & ~SVC_PEDESTRIAN;
                i->setAttribute(SUMO_ATTR_ALLOW, getVehicleClassNames(allOldWithoutPeds), undoList);
            }
        }
    }
    // restrict the lane
    if (addRestriction) {
        const double width = (vclass == SVC_PEDESTRIAN || vclass == SVC_BICYCLE
                              ? OptionsCont::getOptions().getFloat("default.sidewalk-width")
                              : OptionsCont::getOptions().getFloat("default.lanewidth"));
        lane->setAttribute(SUMO_ATTR_ALLOW, toString(vclass), undoList);
        lane->setAttribute(SUMO_ATTR_WIDTH, toString(width), undoList);
        return true;
    } else {
        return false;
    }
}


bool
GNENet::addRestrictedLane(SUMOVehicleClass vclass, GNEEdge& edge, int index, GNEUndoList* undoList) {
    // First check that edge don't have a restricted lane of the given vclass
    for (auto i : edge.getLanes()) {
        if (i->isRestricted(vclass)) {
            return false;
        }
    }
    // check that index is correct (index == size adds to the left of the leftmost lane)
    const int numLanes = (int)edge.getLanes().size();
    if (index > numLanes) {
        return false;
    }
    if (index < 0) {
        // guess index from vclass
        if (vclass == SVC_PEDESTRIAN) {
            index = 0;
        } else if (vclass == SVC_BICYCLE) {
            // add bikelanes to the left of an existing sidewalk
            index = edge.getLanes()[0]->isRestricted(SVC_PEDESTRIAN) ? 1 : 0;
        } else if (vclass == SVC_IGNORING || vclass == SVC_BUS) {
            // add greenVerge to the left of an existing sidewalk or bikeLane
            // add busLane to the left of an existing sidewalk, bikeLane or greenVerge
            index = 0;
            while (index < numLanes && (edge.getNBEdge()->getPermissions(index) & ~(SVC_PEDESTRIAN | SVC_BICYCLE)) == 0) {
                index++;
            }
        }
    }
    // duplicate selected lane
    duplicateLane(edge.getLanes().at(MIN2(index, numLanes - 1)), undoList, true);
    // transform the created lane
    return restrictLane(vclass, edge.getLanes().at(index), undoList);
}


bool
GNENet::removeRestrictedLane(SUMOVehicleClass vclass, GNEEdge& edge, GNEUndoList* undoList) {
    // iterate over lanes of edge
    for (auto i : edge.getLanes()) {
        if (i->isRestricted(vclass)) {
            // Delete lane
            deleteLane(i, undoList, true);
            return true;
        }
    }
    return false;
}


GNEJunction*
GNENet::splitEdge(GNEEdge* edge, const Position& pos, GNEUndoList* undoList, GNEJunction* newJunction) {
    undoList->p_begin("split " + toString(SUMO_TAG_EDGE));
    if (newJunction == nullptr) {
        newJunction = createJunction(pos, undoList);
    }
    // compute geometry
    const PositionVector& oldGeom = edge->getNBEdge()->getGeometry();
    const double linePos = oldGeom.nearest_offset_to_point2D(pos, false);
    std::pair<PositionVector, PositionVector> newGeoms = oldGeom.splitAt(linePos);
    const std::string shapeEnd = edge->getAttribute(GNE_ATTR_SHAPE_END);
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
            } catch (NumberFormatException&) {
            }
        }
    }
    baseName += '.';
    // create a new edge from the new junction to the previous destination
    GNEEdge* secondPart = createEdge(newJunction, edge->getGNEJunctionDestiny(), edge,
                                     undoList, baseName + toString(posBase + (int)linePos), true, false, false);
    // fix connections from the split edge (must happen before changing SUMO_ATTR_TO)
    edge->getGNEJunctionDestiny()->replaceIncomingConnections(edge, secondPart, undoList);
    // remove affected crossings from junction (must happen before changing SUMO_ATTR_TO)
    std::vector<NBNode::Crossing> affectedCrossings;
    for (GNECrossing* crossing : edge->getGNEJunctionDestiny()->getGNECrossings()) {
        if (crossing->checkEdgeBelong(edge)) {
            NBNode::Crossing nbC = *crossing->getNBCrossing();
            undoList->add(new GNEChange_Crossing(edge->getGNEJunctionDestiny(), nbC, false), true);
            EdgeVector newEdges;
            for (NBEdge* nbEdge : nbC.edges) {
                if (nbEdge == edge->getNBEdge()) {
                    newEdges.push_back(secondPart->getNBEdge());
                } else {
                    newEdges.push_back(nbEdge);
                }
            }
            nbC.edges = newEdges;
            affectedCrossings.push_back(nbC);
        }
    }
    // modify the edge so that it ends at the new junction (and all incoming connections are preserved
    undoList->p_add(new GNEChange_Attribute(edge, this, SUMO_ATTR_TO, newJunction->getID()));
    // fix first part of geometry
    newGeoms.first.pop_back();
    newGeoms.first.erase(newGeoms.first.begin());
    edge->setAttribute(GNE_ATTR_SHAPE_END, "", undoList);
    edge->setAttribute(SUMO_ATTR_SHAPE, toString(newGeoms.first), undoList);
    // fix second part of geometry
    secondPart->setAttribute(GNE_ATTR_SHAPE_END, shapeEnd, undoList);
    newGeoms.second.pop_back();
    newGeoms.second.erase(newGeoms.second.begin());
    secondPart->setAttribute(SUMO_ATTR_SHAPE, toString(newGeoms.second), undoList);
    // reconnect across the split
    for (int i = 0; i < (int)edge->getLanes().size(); ++i) {
        undoList->add(new GNEChange_Connection(edge, NBEdge::Connection(i, secondPart->getNBEdge(), i), false, true), true);
    }
    // re-add modified crossings
    for (const auto& nbC : affectedCrossings) {
        undoList->add(new GNEChange_Crossing(secondPart->getGNEJunctionDestiny(), nbC, true), true);
    }
    undoList->p_end();
    return newJunction;
}


void
GNENet::splitEdgesBidi(GNEEdge* edge, GNEEdge* oppositeEdge, const Position& pos, GNEUndoList* undoList) {
    GNEJunction* newJunction = nullptr;
    undoList->p_begin("split " + toString(SUMO_TAG_EDGE) + "s");
    // split edge and save created junction
    newJunction = splitEdge(edge, pos, undoList, newJunction);
    // split second edge
    splitEdge(oppositeEdge, pos, undoList, newJunction);
    undoList->p_end();
}


void
GNENet::reverseEdge(GNEEdge* edge, GNEUndoList* undoList) {
    undoList->p_begin("reverse " + toString(SUMO_TAG_EDGE));
    deleteEdge(edge, undoList, false); // still exists. we delete it so we can reuse the name in case of resplit
    GNEEdge* reversed = createEdge(edge->getGNEJunctionDestiny(), edge->getGNEJunctionSource(), edge, undoList, edge->getID(), false, true);
    assert(reversed != 0);
    reversed->setAttribute(SUMO_ATTR_SHAPE, toString(edge->getNBEdge()->getInnerGeometry().reverse()), undoList);
    reversed->setAttribute(GNE_ATTR_SHAPE_START, edge->getAttribute(GNE_ATTR_SHAPE_END), undoList);
    reversed->setAttribute(GNE_ATTR_SHAPE_END, edge->getAttribute(GNE_ATTR_SHAPE_START), undoList);
    undoList->p_end();
}


GNEEdge*
GNENet::addReversedEdge(GNEEdge* edge, GNEUndoList* undoList) {
    undoList->p_begin("add reversed " + toString(SUMO_TAG_EDGE));
    GNEEdge* reversed = nullptr;
    if (edge->getNBEdge()->getLaneSpreadFunction() == LANESPREAD_RIGHT || isRailway(edge->getNBEdge()->getPermissions())) {
        // for rail edges, we assume bi-directional tracks are wanted
        reversed = createEdge(edge->getGNEJunctionDestiny(), edge->getGNEJunctionSource(), edge, undoList, "-" + edge->getID(), false, true);
        assert(reversed != 0);
        reversed->setAttribute(SUMO_ATTR_SHAPE, toString(edge->getNBEdge()->getInnerGeometry().reverse()), undoList);
        reversed->setAttribute(GNE_ATTR_SHAPE_START, edge->getAttribute(GNE_ATTR_SHAPE_END), undoList);
        reversed->setAttribute(GNE_ATTR_SHAPE_END, edge->getAttribute(GNE_ATTR_SHAPE_START), undoList);
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
        reversed->setAttribute(SUMO_ATTR_SHAPE, toString(origInner.reverse()), undoList);
        // select the new edge and its nodes
        reversed->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
        src->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
        dest->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
    }
    undoList->p_end();
    return reversed;
}


void
GNENet::mergeJunctions(GNEJunction* moved, GNEJunction* target, GNEUndoList* undoList) {
    undoList->p_begin("merge " + toString(SUMO_TAG_JUNCTION) + "s");
    // place moved junction in the same position of target junction
    moved->setAttribute(SUMO_ATTR_POSITION, target->getAttribute(SUMO_ATTR_POSITION), undoList);
    // deleting edges changes in the underlying EdgeVector so we have to make a copy
    const EdgeVector incoming = moved->getNBNode()->getIncomingEdges();
    for (NBEdge* edge : incoming) {
        // delete edges between the merged junctions
        GNEEdge* e = myAttributeCarriers.edges[edge->getID()];
        assert(e != 0);
        if (e->getGNEJunctionSource() == target) {
            deleteEdge(e, undoList, false);
        } else {
            undoList->p_add(new GNEChange_Attribute(e, this, SUMO_ATTR_TO, target->getID()));
        }
    }
    // deleting edges changes in the underlying EdgeVector so we have to make a copy
    const EdgeVector outgoing = moved->getNBNode()->getOutgoingEdges();
    for (NBEdge* edge : outgoing) {
        // delete edges between the merged junctions
        GNEEdge* e = myAttributeCarriers.edges[edge->getID()];
        assert(e != 0);
        if (e->getGNEJunctionDestiny() == target) {
            deleteEdge(e, undoList, false);
        } else {
            undoList->p_add(new GNEChange_Attribute(e, this, SUMO_ATTR_FROM, target->getID()));
        }
    }
    // deleted moved junction
    deleteJunction(moved, undoList);
    undoList->p_end();
}


bool
GNENet::checkJunctionPosition(const Position& pos) {
    // Check that there isn't another junction in the same position as Pos
    for (auto i : myAttributeCarriers.junctions) {
        if (i.second->getPositionInView() == pos) {
            return false;
        }
    }
    return true;
}


void
GNENet::requiereSaveNet(bool value) {
    if (myNetSaved == true) {
        WRITE_DEBUG("net has to be saved");
        std::string additionalsSaved = (myAdditionalsSaved ? "saved" : "unsaved");
        std::string demandElementsSaved = (myDemandElementsSaved ? "saved" : "unsaved");
        WRITE_DEBUG("Current saving Status: net unsaved, additionals " + additionalsSaved + ", demand elements " + demandElementsSaved);
    }
    myNetSaved = !value;
}


bool
GNENet::isNetSaved() const {
    return myNetSaved;
}


void
GNENet::save(OptionsCont& oc) {
    // compute without volatile options and update network
    computeAndUpdate(oc, false);
    // write network
    NWFrame::writeNetwork(oc, *myNetBuilder);
    myNetSaved = true;
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

    // Create default vehicle Type (it has to be created here due myViewNet was previously nullptr)
    GNEVehicleType* defaultVehicleType = new GNEVehicleType(myViewNet, DEFAULT_VTYPE_ID, SVC_PASSENGER, SUMO_TAG_VTYPE);
    myAttributeCarriers.demandElements.at(defaultVehicleType->getTagProperty().getTag()).insert(std::make_pair(defaultVehicleType->getID(), defaultVehicleType));
    defaultVehicleType->incRef("GNENet::DEFAULT_VEHTYPE");

    // Create default Bike Type (it has to be created here due myViewNet was previously nullptr)
    GNEVehicleType* defaultBikeType = new GNEVehicleType(myViewNet, DEFAULT_BIKETYPE_ID, SVC_BICYCLE, SUMO_TAG_VTYPE);
    myAttributeCarriers.demandElements.at(defaultBikeType->getTagProperty().getTag()).insert(std::make_pair(defaultBikeType->getID(), defaultBikeType));
    defaultBikeType->incRef("GNENet::DEFAULT_BIKETYPE_ID");

    // Create default person Type (it has to be created here due myViewNet was previously nullptr)
    GNEVehicleType* defaultPersonType = new GNEVehicleType(myViewNet, DEFAULT_PEDTYPE_ID, SVC_PEDESTRIAN, SUMO_TAG_PTYPE);
    myAttributeCarriers.demandElements.at(defaultPersonType->getTagProperty().getTag()).insert(std::make_pair(defaultPersonType->getID(), defaultPersonType));
    defaultPersonType->incRef("GNENet::DEFAULT_PEDTYPE_ID");

    // create instance of RouteCalculator
    GNEDemandElement::createRouteCalculatorInstance(this);
}


GNEJunction*
GNENet::retrieveJunction(const std::string& id, bool failHard) {
    if (myAttributeCarriers.junctions.count(id)) {
        return myAttributeCarriers.junctions[id];
    } else if (failHard) {
        // If junction wasn't found, throw exception
        throw UnknownElement("Junction " + id);
    } else {
        return nullptr;
    }
}


const GNENet::AttributeCarriers&
GNENet::getAttributeCarriers() const {
    return myAttributeCarriers;
}


GNEEdge*
GNENet::retrieveEdge(const std::string& id, bool failHard) {
    auto i = myAttributeCarriers.edges.find(id);
    // If edge was found
    if (i != myAttributeCarriers.edges.end()) {
        return i->second;
    } else if (failHard) {
        // If edge wasn't found, throw exception
        throw UnknownElement("Edge " + id);
    } else {
        return nullptr;
    }
}


GNEEdge*
GNENet::retrieveEdge(GNEJunction* from, GNEJunction* to, bool failHard) {
    assert((from != nullptr) && (to != nullptr));
    // iterate over Junctions of net
    for (auto i : myAttributeCarriers.edges) {
        if ((i.second->getGNEJunctionSource() == from) && (i.second->getGNEJunctionDestiny() == to)) {
            return i.second;
        }
    }
    // if edge wasn' found, throw exception or return nullptr
    if (failHard) {
        throw UnknownElement("Edge with from='" + from->getID() + "' and to='" + to->getID() + "'");
    } else {
        return nullptr;
    }
}


GNEPoly*
GNENet::retrievePolygon(const std::string& id, bool failHard) const {
    if (myPolygons.get(id) != 0) {
        return reinterpret_cast<GNEPoly*>(myPolygons.get(id));
    } else if (failHard) {
        // If Polygon wasn't found, throw exception
        throw UnknownElement("Polygon " + id);
    } else {
        return nullptr;
    }
}


GNEPOI*
GNENet::retrievePOI(const std::string& id, bool failHard) const {
    if (myPOIs.get(id) != 0) {
        return reinterpret_cast<GNEPOI*>(myPOIs.get(id));
    } else if (failHard) {
        // If POI wasn't found, throw exception
        throw UnknownElement("POI " + id);
    } else {
        return nullptr;
    }
}


GNEConnection*
GNENet::retrieveConnection(const std::string& id, bool failHard) const {
    // iterate over junctions
    for (auto i : myAttributeCarriers.junctions) {
        // iterate over connections
        for (auto j : i.second->getGNEConnections()) {
            if (j->getID() == id) {
                return j;
            }
        }
    }
    if (failHard) {
        // If POI wasn't found, throw exception
        throw UnknownElement("Connection " + id);
    } else {
        return nullptr;
    }
}


std::vector<GNEConnection*>
GNENet::retrieveConnections(bool onlySelected) const {
    std::vector<GNEConnection*> result;
    // iterate over junctions
    for (auto i : myAttributeCarriers.junctions) {
        // iterate over connections
        for (auto j : i.second->getGNEConnections()) {
            if (!onlySelected || j->isAttributeCarrierSelected()) {
                result.push_back(j);
            }
        }
    }
    return result;
}


GNECrossing*
GNENet::retrieveCrossing(const std::string& id, bool failHard) const {
    // iterate over junctions
    for (auto i : myAttributeCarriers.junctions) {
        // iterate over crossings
        for (auto j : i.second->getGNECrossings()) {
            if (j->getID() == id) {
                return j;
            }
        }
    }
    if (failHard) {
        // If POI wasn't found, throw exception
        throw UnknownElement("Crossing " + id);
    } else {
        return nullptr;
    }
}


std::vector<GNECrossing*>
GNENet::retrieveCrossings(bool onlySelected) const {
    std::vector<GNECrossing*> result;
    // iterate over junctions
    for (auto i : myAttributeCarriers.junctions) {
        // iterate over crossings
        for (auto j : i.second->getGNECrossings()) {
            if (!onlySelected || j->isAttributeCarrierSelected()) {
                result.push_back(j);
            }
        }
    }
    return result;
}


std::vector<GNEEdge*>
GNENet::retrieveEdges(bool onlySelected) {
    std::vector<GNEEdge*> result;
    // returns edges depending of selection
    for (auto i : myAttributeCarriers.edges) {
        if (!onlySelected || i.second->isAttributeCarrierSelected()) {
            result.push_back(i.second);
        }
    }
    return result;
}


std::vector<GNELane*>
GNENet::retrieveLanes(bool onlySelected) {
    std::vector<GNELane*> result;
    // returns lanes depending of selection
    for (auto i : myAttributeCarriers.edges) {
        for (auto j : i.second->getLanes()) {
            if (!onlySelected || j->isAttributeCarrierSelected()) {
                result.push_back(j);
            }
        }
    }
    return result;
}


GNELane*
GNENet::retrieveLane(const std::string& id, bool failHard, bool checkVolatileChange) {
    const std::string edge_id = SUMOXMLDefinitions::getEdgeIDFromLane(id);
    GNEEdge* edge = retrieveEdge(edge_id, failHard);
    if (edge != nullptr) {
        GNELane* lane = nullptr;
        // search  lane in lane's edges
        for (auto it : edge->getLanes()) {
            if (it->getID() == id) {
                lane = it;
            }
        }
        // throw exception or return nullptr if lane wasn't found
        if (lane == nullptr) {
            if (failHard) {
                // Throw exception if failHard is enabled
                throw UnknownElement(toString(SUMO_TAG_LANE) + " " + id);
            }
        } else {
            // check if the recomputing with volatile option has changed the number of lanes (needed for additionals and demand elements)
            if (checkVolatileChange && (myEdgesAndNumberOfLanes.count(edge_id) == 1) && myEdgesAndNumberOfLanes[edge_id] != (int)edge->getLanes().size()) {
                return edge->getLanes().at(lane->getIndex() + 1);
            }
            return lane;
        }
    } else if (failHard) {
        // Throw exception if failHard is enabled
        throw UnknownElement(toString(SUMO_TAG_EDGE) + " " + edge_id);
    }
    return nullptr;
}


std::vector<GNEJunction*>
GNENet::retrieveJunctions(bool onlySelected) {
    std::vector<GNEJunction*> result;
    // returns junctions depending of selection
    for (auto i : myAttributeCarriers.junctions) {
        if (!onlySelected || i.second->isAttributeCarrierSelected()) {
            result.push_back(i.second);
        }
    }
    return result;
}


std::vector<GNEShape*>
GNENet::retrieveShapes(SumoXMLTag shapeTag, bool onlySelected) {
    std::vector<GNEShape*> result;
    // return dependingn of shape type
    if (shapeTag == SUMO_TAG_POLY) {
        // return all polys depending of onlySelected
        for (auto it : getPolygons()) {
            GNEShape* shape = dynamic_cast<GNEShape*>(it.second);
            if (!onlySelected || shape->isAttributeCarrierSelected()) {
                result.push_back(shape);
            }
        }
    } else {
        // check if we need to return a POI or POILane
        for (auto it : getPOIs()) {
            GNEPOI* poi = dynamic_cast<GNEPOI*>(it.second);
            if (poi && (poi->getTagProperty().getTag() == shapeTag)) {
                // return all POIs or POILanes depending of onlySelected
                if (!onlySelected || poi->isAttributeCarrierSelected()) {
                    result.push_back(poi);
                }
            }
        }
    }
    return result;
}


std::vector<GNEShape*>
GNENet::retrieveShapes(bool onlySelected) {
    std::vector<GNEShape*> result;
    // return all polygons and POIs
    for (const auto& it : getPolygons()) {
        GNEPoly* poly = dynamic_cast<GNEPoly*>(it.second);
        if (!onlySelected || poly->isAttributeCarrierSelected()) {
            result.push_back(poly);
        }
    }
    for (const auto& it : getPOIs()) {
        GNEPOI* poi = dynamic_cast<GNEPOI*>(it.second);
        if (!onlySelected || poi->isAttributeCarrierSelected()) {
            result.push_back(poi);
        }
    }
    return result;
}


void
GNENet::addGLObjectIntoGrid(GUIGlObject* o) {
    myGrid.addAdditionalGLObject(o);
}


void
GNENet::removeGLObjectFromGrid(GUIGlObject* o) {
    myGrid.removeAdditionalGLObject(o);
}


GNEAttributeCarrier*
GNENet::retrieveAttributeCarrier(GUIGlID id, bool failHard) {
    // obtain blocked GUIGlObject
    GUIGlObject* object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
    // Make sure that object exists
    if (object != nullptr) {
        // unblock and try to parse to AtributeCarrier
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        GNEAttributeCarrier* ac = dynamic_cast<GNEAttributeCarrier*>(object);
        // If was sucesfully parsed, return it
        if (ac == nullptr) {
            throw ProcessError("GUIGlObject does not match the declared type");
        } else {
            return ac;
        }
    } else if (failHard) {
        throw ProcessError("Attempted to retrieve non-existant GUIGlObject");
    } else {
        return nullptr;
    }
}


std::vector<GNEAttributeCarrier*>
GNENet::retrieveAttributeCarriers(SumoXMLTag type) {
    std::vector<GNEAttributeCarrier*> result;
    if (type == SUMO_TAG_NOTHING) {
        // return all elements
        for (auto i : myAttributeCarriers.junctions) {
            result.push_back(i.second);
            for (auto j : i.second->getGNECrossings()) {
                result.push_back(j);
            }
        }
        for (auto i : myAttributeCarriers.edges) {
            result.push_back(i.second);
            for (auto j : i.second->getLanes()) {
                result.push_back(j);
            }
            for (auto j : i.second->getGNEConnections()) {
                result.push_back(j);
            }
        }
        for (auto i : myAttributeCarriers.additionals) {
            for (auto j : i.second) {
                result.push_back(j.second);
            }
        }
        for (auto i : myPolygons) {
            result.push_back(dynamic_cast<GNEPoly*>(i.second));
        }
        for (auto i : myPOIs) {
            result.push_back(dynamic_cast<GNEPOI*>(i.second));
        }
        for (auto i : myAttributeCarriers.demandElements) {
            for (auto j : i.second) {
                result.push_back(j.second);
            }
        }
    } else if (GNEAttributeCarrier::getTagProperties(type).isAdditional() || GNEAttributeCarrier::getTagProperties(type).isTAZ()) {
        // only returns additionals of a certain type.
        for (auto i : myAttributeCarriers.additionals.at(type)) {
            result.push_back(i.second);
        }
    } else if (GNEAttributeCarrier::getTagProperties(type).isDemandElement() || GNEAttributeCarrier::getTagProperties(type).isStop()) {
        // only returns demand elements of a certain type.
        for (auto i : myAttributeCarriers.demandElements.at(type)) {
            result.push_back(i.second);
        }
    } else {
        // return only a part of elements, depending of type
        switch (type) {
            case SUMO_TAG_JUNCTION:
                for (auto i : myAttributeCarriers.junctions) {
                    result.push_back(i.second);
                }
                break;
            case SUMO_TAG_EDGE:
                for (auto i : myAttributeCarriers.edges) {
                    result.push_back(i.second);
                }
                break;
            case SUMO_TAG_LANE:
                for (auto i : myAttributeCarriers.edges) {
                    for (auto j : i.second->getLanes()) {
                        result.push_back(j);
                    }
                }
                break;
            case SUMO_TAG_CONNECTION:
                for (auto i : myAttributeCarriers.edges) {
                    for (auto j : i.second->getGNEConnections()) {
                        result.push_back(j);
                    }
                }
                break;
            case SUMO_TAG_CROSSING:
                for (auto i : myAttributeCarriers.junctions) {
                    for (auto j : i.second->getGNECrossings()) {
                        result.push_back(j);
                    }
                }
                break;
            case SUMO_TAG_POLY:
                for (auto i : myPolygons) {
                    result.push_back(dynamic_cast<GNEPoly*>(i.second));
                }
                break;
            case SUMO_TAG_POI:
            case SUMO_TAG_POILANE:
                for (auto i : myPOIs) {
                    result.push_back(dynamic_cast<GNEPOI*>(i.second));
                }
                break;
            default:
                // return nothing
                break;
        }
    }
    return result;
}


void
GNENet::computeNetwork(GNEApplicationWindow* window, bool force, bool volatileOptions, std::string additionalPath, std::string demandPath) {
    if (!myNeedRecompute) {
        if (force) {
            if (volatileOptions) {
                window->setStatusBarText("Forced computing junctions with volatile options ...");
            } else {
                window->setStatusBarText("Forced computing junctions ...");
            }
        } else {
            return;
        }
    } else {
        if (volatileOptions) {
            window->setStatusBarText("Computing junctions with volatile options ...");
        } else {
            window->setStatusBarText("Computing junctions  ...");
        }
    }
    // save current number of lanes for every edge if recomputing is with volatile options
    if (volatileOptions) {
        for (auto it : myAttributeCarriers.edges) {
            myEdgesAndNumberOfLanes[it.second->getID()] = (int)it.second->getLanes().size();
        }
    }

    // compute and update
    OptionsCont& oc = OptionsCont::getOptions();
    computeAndUpdate(oc, volatileOptions);

    // load additionals if was recomputed with volatile options
    if (additionalPath != "") {
        // fill additionals with tags
        auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_ADDITIONAL, false);
        for (auto i : listOfTags) {
            myAttributeCarriers.additionals.insert(std::make_pair(i, std::map<std::string, GNEAdditional*>()));
        }
        listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_TAZ, false);
        for (auto i : listOfTags) {
            myAttributeCarriers.additionals.insert(std::make_pair(i, std::map<std::string, GNEAdditional*>()));
        }
        // Create additional handler
        GNEAdditionalHandler additionalHandler(additionalPath, myViewNet);
        // Run parser
        if (!XMLSubSys::runParser(additionalHandler, additionalPath, false)) {
            WRITE_MESSAGE("Loading of " + additionalPath + " failed.");
        } else {
            // update view
            update();
        }
        // clear myEdgesAndNumberOfLanes after reload additionals
        myEdgesAndNumberOfLanes.clear();
    }
    // load demand elements if was recomputed with volatile options
    if (demandPath != "") {
        // fill demandElements with tags
        auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_DEMANDELEMENT, false);
        for (auto i : listOfTags) {
            myAttributeCarriers.demandElements.insert(std::make_pair(i, std::map<std::string, GNEDemandElement*>()));
        }
        listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_STOP, false);
        for (auto i : listOfTags) {
            myAttributeCarriers.demandElements.insert(std::make_pair(i, std::map<std::string, GNEDemandElement*>()));
        }
        // Create demandElement handler
        GNERouteHandler demandElementHandler(demandPath, myViewNet, false);
        // Run parser
        if (!XMLSubSys::runParser(demandElementHandler, demandPath, false)) {
            WRITE_MESSAGE("Loading of " + demandPath + " failed.");
        } else {
            // update view
            update();
        }
        // clear myEdgesAndNumberOfLanes after reload demandElements
        myEdgesAndNumberOfLanes.clear();
    }
    window->getApp()->endWaitCursor();
    window->setStatusBarText("Finished computing junctions.");
}


void
GNENet::computeDemandElements(GNEApplicationWindow* window) {
    window->setStatusBarText("Computing demand elements ...");
    // iterate over all demand elements and compute
    for (const auto& i : myAttributeCarriers.demandElements) {
        for (const auto& j : i.second) {
            j.second->compute();
        }
    }
    window->setStatusBarText("Finished computing demand elements.");
}


void
GNENet::computeJunction(GNEJunction* junction) {
    // recompute tl-logics
    OptionsCont& oc = OptionsCont::getOptions();
    NBTrafficLightLogicCont& tllCont = getTLLogicCont();
    // iterate over traffic lights definitions. Make a copy because invalid
    // definitions will be removed (and would otherwise destroy the iterator)
    const std::set<NBTrafficLightDefinition*> tlsDefs = junction->getNBNode()->getControllingTLS();
    for (auto it : tlsDefs) {
        it->setParticipantsInformation();
        it->setTLControllingInformation();
        tllCont.computeSingleLogic(oc, it);
    }

    // @todo compute connections etc...
}


void
GNENet::requireRecompute() {
    myNeedRecompute = true;
}


bool
GNENet::netHasGNECrossings() const {
    for (auto n : myAttributeCarriers.junctions) {
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
    std::set<NBNode*, ComparatorIdLess> cluster;
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
    std::string id = "cluster";
    TrafficLightType type;
    SumoXMLNodeType nodeType = NODETYPE_UNKNOWN;
    myNetBuilder->getNodeCont().analyzeCluster(cluster, id, pos, setTL, type, nodeType);
    // save position
    oldPos = pos;

    // Check that there isn't another junction in the same position as Pos but doesn't belong to cluster
    for (auto i : myAttributeCarriers.junctions) {
        if ((i.second->getPositionInView() == pos) && (cluster.find(i.second->getNBNode()) == cluster.end())) {
            // show warning in gui testing debug mode
            WRITE_DEBUG("Opening FXMessageBox 'Join non-selected junction'");
            // Ask confirmation to user
            FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
                                                   ("Position of joined " + toString(SUMO_TAG_JUNCTION)).c_str(), "%s",
                                                   ("There is another unselected " + toString(SUMO_TAG_JUNCTION) + " in the same position of joined " + toString(SUMO_TAG_JUNCTION) +
                                                    + ".\nIt will be joined with the other selected " + toString(SUMO_TAG_JUNCTION) + "s. Continue?").c_str());
            if (answer != 1) { // 1:yes, 2:no, 4:esc
                // write warning if netedit is running in testing mode
                if (answer == 2) {
                    WRITE_DEBUG("Closed FXMessageBox 'Join non-selected junction' with 'No'");
                } else if (answer == 4) {
                    WRITE_DEBUG("Closed FXMessageBox 'Join non-selected junction' with 'ESC'");
                }
                return false;
            } else {
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Closed FXMessageBox 'Join non-selected junction' with 'Yes'");
                // select conflicted junction an join all again
                i.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                return joinSelectedJunctions(undoList);
            }
        }
    }

    // use checkJunctionPosition to avoid conflicts with junction in the same position as others
    while (checkJunctionPosition(pos) == false) {
        pos.setx(pos.x() + 0.1);
        pos.sety(pos.y() + 0.1);
    }

    // start with the join selected junctions
    undoList->p_begin("Join selected " + toString(SUMO_TAG_JUNCTION) + "s");
    GNEJunction* joined = createJunction(pos, undoList);
    joined->setAttribute(SUMO_ATTR_TYPE, toString(nodeType), undoList); // i.e. rail crossing
    if (setTL) {
        joined->setAttribute(SUMO_ATTR_TLTYPE, toString(type), undoList);
    }

    // #3128 this is not undone when calling 'undo'
    myNetBuilder->getNodeCont().registerJoinedCluster(cluster);

    // first remove all crossing of the involved junctions and edges
    // (otherwise edge removal will trigger discarding)
    std::vector<NBNode::Crossing> oldCrossings;
    for (auto i : selectedJunctions) {
        while (i->getGNECrossings().size() > 0) {
            GNECrossing* crossing = i->getGNECrossings().front();
            oldCrossings.push_back(*crossing->getNBCrossing());
            deleteCrossing(crossing, undoList);
        }
    }

    // preserve old connections
    for (auto it : selectedJunctions) {
        it->setLogicValid(false, undoList);
    }
    // remap edges
    for (auto it : allIncoming) {
        undoList->p_add(new GNEChange_Attribute(myAttributeCarriers.edges[it->getID()], this, SUMO_ATTR_TO, joined->getID()));
    }

    EdgeSet edgesWithin;
    for (auto it : allOutgoing) {
        // delete edges within the cluster
        GNEEdge* e = myAttributeCarriers.edges[it->getID()];
        assert(e != 0);
        if (e->getGNEJunctionDestiny() == joined) {
            edgesWithin.insert(it);
            deleteEdge(e, undoList, false);
        } else {
            undoList->p_add(new GNEChange_Attribute(myAttributeCarriers.edges[it->getID()], this, SUMO_ATTR_FROM, joined->getID()));
        }
    }

    // remap all crossing of the involved junctions and edges
    for (auto nbc : oldCrossings) {
        bool keep = true;
        for (NBEdge* e : nbc.edges) {
            if (edgesWithin.count(e) != 0) {
                keep = false;
                break;
            }
        };
        if (keep) {
            undoList->add(new GNEChange_Crossing(joined, nbc.edges, nbc.width,
                                                 nbc.priority || joined->getNBNode()->isTLControlled(),
                                                 nbc.customTLIndex, nbc.customTLIndex2, nbc.customShape,
                                                 false, true), true);
        }
    }

    // delete original junctions
    for (auto it : selectedJunctions) {
        deleteJunction(it, undoList);
    }
    joined->setAttribute(SUMO_ATTR_ID, id, undoList);


    // check if joined junction had to change their original position to avoid errors
    if (pos != oldPos) {
        joined->setAttribute(SUMO_ATTR_POSITION, toString(oldPos), undoList);
    }
    undoList->p_end();
    return true;
}


bool
GNENet::cleanInvalidCrossings(GNEUndoList* undoList) {
    // obtain current net's crossings
    std::vector<GNECrossing*> myNetCrossings;
    for (auto it : myAttributeCarriers.junctions) {
        myNetCrossings.reserve(myNetCrossings.size() + it.second->getGNECrossings().size());
        myNetCrossings.insert(myNetCrossings.end(), it.second->getGNECrossings().begin(), it.second->getGNECrossings().end());
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
        WRITE_DEBUG("Opening FXMessageBox 'No crossing to remove'");
        // open a dialog informing that there isn't crossing to remove
        FXMessageBox::warning(getApp(), MBOX_OK,
                              ("Clear " + toString(SUMO_TAG_CROSSING) + "s").c_str(), "%s",
                              ("There is no invalid " + toString(SUMO_TAG_CROSSING) + "s to remove").c_str());
        // show warning in gui testing debug mode
        WRITE_DEBUG("Closed FXMessageBox 'No crossing to remove' with 'OK'");
    } else {
        std::string plural = myInvalidCrossings.size() == 1 ? ("") : ("s");
        // show warning in gui testing debug mode
        WRITE_DEBUG("Opening FXMessageBox 'clear crossings'");
        // Ask confirmation to user
        FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
                                               ("Clear " + toString(SUMO_TAG_CROSSING) + "s").c_str(), "%s",
                                               ("Clear " + toString(SUMO_TAG_CROSSING) + plural + " will be removed. Continue?").c_str());
        if (answer != 1) { // 1:yes, 2:no, 4:esc
            // write warning if netedit is running in testing mode
            if (answer == 2) {
                WRITE_DEBUG("Closed FXMessageBox 'clear crossings' with 'No'");
            } else if (answer == 4) {
                WRITE_DEBUG("Closed FXMessageBox 'clear crossings' with 'ESC'");
            }
        } else {
            undoList->p_begin("Clean " + toString(SUMO_TAG_CROSSING) + "s");
            for (auto i = myInvalidCrossings.begin(); i != myInvalidCrossings.end(); i++) {
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
    for (auto it : myAttributeCarriers.junctions) {
        GNEJunction* junction = it.second;
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
GNENet::cleanUnusedRoutes(GNEUndoList* undoList) {
    // first declare a vector to save all routes without children
    std::vector<GNEDemandElement*> routesWithoutChildren;
    routesWithoutChildren.reserve(myAttributeCarriers.demandElements.at(SUMO_TAG_ROUTE).size());
    // iterate over routes
    for (const auto& i : myAttributeCarriers.demandElements.at(SUMO_TAG_ROUTE)) {
        if (i.second->getDemandElementChildren().empty()) {
            routesWithoutChildren.push_back(i.second);
        }
    }
    // finally remove all routesWithoutChildren
    if (routesWithoutChildren.size() > 0) {
        // begin undo list
        undoList->p_begin("clean unused routes");
        // iterate over routesWithoutChildren
        for (const auto& i : routesWithoutChildren) {
            // due route doesn't have children, simply call GNEChange_DemandElement
            undoList->add(new GNEChange_DemandElement(i, false), true);
        }
        // update view
        myViewNet->update();
        // end undo list
        undoList->p_end();
    }
}


void
GNENet::joinRoutes(GNEUndoList* undoList) {
    // first declare a sorted set of sorted route's edges in string format
    std::set<std::pair<std::string, GNEDemandElement*> > mySortedRoutes;
    // iterate over routes and save it in mySortedRoutes  (only if it doesn't have Stop Children)
    for (const auto& i : myAttributeCarriers.demandElements.at(SUMO_TAG_ROUTE)) {
        // first check route has stops
        bool hasStops = false;
        for (const auto& j : i.second->getDemandElementChildren()) {
            if (j->getTagProperty().isStop()) {
                hasStops = true;
            }
        }
        if (!hasStops) {
            mySortedRoutes.insert(std::make_pair(GNEAttributeCarrier::parseIDs(i.second->getEdgeParents()), i.second));
        }
    }
    // now declare a matrix in which organice routes to be merged
    std::vector<std::vector<GNEDemandElement*> > routesToMerge;
    auto index = mySortedRoutes.begin();
    // iterate over mySortedRoutes
    for (auto i = mySortedRoutes.begin(); i != mySortedRoutes.end(); i++) {
        if (routesToMerge.empty()) {
            routesToMerge.push_back({i->second});
        } else {
            if (index->first == i->first) {
                routesToMerge.back().push_back(i->second);
            } else {
                routesToMerge.push_back({i->second});
                index = i;
            }
        }
    }
    // now check if there is routes to merge
    bool thereIsRoutesToMerge = false;
    for (const auto& i : routesToMerge) {
        if (i.size() > 1) {
            thereIsRoutesToMerge = true;
        }
    }
    // if exist
    if (thereIsRoutesToMerge) {
        // begin undo list
        undoList->p_begin("merge routes");
        // iterate over route to edges
        for (const auto& i : routesToMerge) {
            if (i.size() > 1) {
                // iterate over duplicated routes
                for (int j = 1; j < (int)i.size(); j++) {
                    // move all vehicles of every duplicated route
                    while (i.at(j)->getDemandElementChildren().size() > 0) {
                        i.at(j)->getDemandElementChildren().front()->setAttribute(SUMO_ATTR_ROUTE, i.at(0)->getID(), undoList);
                    }
                    // finally remove route
                    undoList->add(new GNEChange_DemandElement(i.at(j), false), true);
                }
            }
        }
        // update view
        myViewNet->update();
        // end undo list
        undoList->p_end();
    }
}


void
GNENet::cleanInvalidDemandElements(GNEUndoList* undoList) {
    // first declare a vector to save all invalid demand elements
    std::vector<GNEDemandElement*> invalidDemandElements;
    invalidDemandElements.reserve(myAttributeCarriers.demandElements.at(SUMO_TAG_ROUTE).size() +
                                  myAttributeCarriers.demandElements.at(SUMO_TAG_FLOW).size() +
                                  myAttributeCarriers.demandElements.at(SUMO_TAG_TRIP).size());
    // iterate over routes
    for (const auto& i : myAttributeCarriers.demandElements.at(SUMO_TAG_ROUTE)) {
        if (!i.second->isDemandElementValid()) {
            invalidDemandElements.push_back(i.second);
        }
    }
    // iterate over flows
    for (const auto& i : myAttributeCarriers.demandElements.at(SUMO_TAG_FLOW)) {
        if (!i.second->isDemandElementValid()) {
            invalidDemandElements.push_back(i.second);
        }
    }
    // iterate over trip
    for (const auto& i : myAttributeCarriers.demandElements.at(SUMO_TAG_TRIP)) {
        if (!i.second->isDemandElementValid()) {
            invalidDemandElements.push_back(i.second);
        }
    }
    // continue if there is invalidDemandElements to remove
    if (invalidDemandElements.size() > 0) {
        // begin undo list
        undoList->p_begin("remove invalid demand elements");
        // iterate over invalidDemandElements
        for (const auto& i : invalidDemandElements) {
            // simply call GNEChange_DemandElement
            undoList->add(new GNEChange_DemandElement(i, false), true);
        }
        // update view
        myViewNet->update();
        // end undo list
        undoList->p_end();
    }
}

void
GNENet::replaceJunctionByGeometry(GNEJunction* junction, GNEUndoList* undoList) {
    assert(junction->getNBNode()->checkIsRemovable());
    // start operation
    undoList->p_begin("Replace junction by geometry");
    // obtain Edges to join
    std::vector<std::pair<NBEdge*, NBEdge*> > toJoin = junction->getNBNode()->getEdgesToJoin();
    // clear connections of junction to replace
    clearJunctionConnections(junction, undoList);
    // iterate over NBEdges to join
    for (auto j : toJoin) {
        // obtain GNEEdges
        GNEEdge* begin = myAttributeCarriers.edges[j.first->getID()];
        GNEEdge* continuation = myAttributeCarriers.edges[j.second->getID()];
        // remove connections between the edges
        std::vector<NBEdge::Connection> connections = begin->getNBEdge()->getConnections();
        for (auto con : connections) {
            undoList->add(new GNEChange_Connection(begin, con, false, false), true);
        }
        // fix shape of replaced edge
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
        // replace incoming edge
        replaceIncomingEdge(continuation, begin, undoList);

        newShape.append(continuation->getNBEdge()->getInnerGeometry());
        begin->setAttribute(GNE_ATTR_SHAPE_END, continuation->getAttribute(GNE_ATTR_SHAPE_END), undoList);
        begin->setAttribute(SUMO_ATTR_ENDOFFSET, continuation->getAttribute(SUMO_ATTR_ENDOFFSET), undoList);
        begin->setAttribute(SUMO_ATTR_SHAPE, toString(newShape), undoList);
    }
    //delete replaced junction
    deleteJunction(junction, undoList);
    // finish operation
    undoList->p_end();
}


void
GNENet::splitJunction(GNEJunction* junction, bool reconnect, GNEUndoList* undoList) {
    std::vector<std::pair<Position, std::string> > endpoints = junction->getNBNode()->getEndPoints();
    if (endpoints.size() < 2) {
        return;
    }
    // start operation
    undoList->p_begin("Split junction");
    // record connections
    std::map<GNEEdge*, std::vector<NBEdge::Connection>> straightConnections;
    for (GNEEdge* e : junction->getGNEIncomingEdges()) {
        for (const auto& c : e->getNBEdge()->getConnections()) {
            if (c.fromLane >= 0 && junction->getNBNode()->getDirection(e->getNBEdge(), c.toEdge) == LINKDIR_STRAIGHT) {
                straightConnections[e].push_back(c);
            }
        };
    }
    //std::cout << "split junction at endpoints:\n";

    junction->setLogicValid(false, undoList);
    for (const auto& pair : endpoints) {
        const Position& pos = pair.first;
        const std::string& origID = pair.second;
        GNEJunction* newJunction = createJunction(pos, undoList);
        std::string newID = origID != "" ? origID : newJunction->getID();
        // make a copy because the original vectors are modified during iteration
        const std::vector<GNEEdge*> incoming = junction->getGNEIncomingEdges();
        const std::vector<GNEEdge*> outgoing = junction->getGNEOutgoingEdges();
        //std::cout << "  checkEndpoint " << pair.first << " " << pair.second << " newID=" << newID << "\n";
        for (GNEEdge* e : incoming) {
            //std::cout << "   incoming " << e->getID() << " pos=" << pos << " origTo=" << e->getNBEdge()->getParameter("origTo") << " newID=" << newID << "\n";
            if (e->getNBEdge()->getGeometry().back().almostSame(pos) || e->getNBEdge()->getParameter("origTo") == newID) {
                //std::cout << "     match\n";
                undoList->p_add(new GNEChange_Attribute(e, this, SUMO_ATTR_TO, newJunction->getID()));
            }
        }
        for (GNEEdge* e : outgoing) {
            //std::cout << "   outgoing " << e->getID() << " pos=" << pos << " origFrom=" << e->getNBEdge()->getParameter("origFrom") << " newID=" << newID << "\n";
            if (e->getNBEdge()->getGeometry().front().almostSame(pos) || e->getNBEdge()->getParameter("origFrom") == newID) {
                //std::cout << "     match\n";
                undoList->p_add(new GNEChange_Attribute(e, this, SUMO_ATTR_FROM, newJunction->getID()));
            }
        }
        if (newID != newJunction->getID()) {
            if (newJunction->isValid(SUMO_ATTR_ID, newID)) {
                undoList->p_add(new GNEChange_Attribute(newJunction, this, SUMO_ATTR_ID, newID));
            } else {
                WRITE_WARNING("Could not rename split node to '" + newID + "'");
            }
        }
    }
    // recreate edges from straightConnections
    if (reconnect) {
        for (const auto& item : straightConnections) {
            GNEEdge* in = item.first;
            std::map<NBEdge*, GNEEdge*> newEdges;
            for (auto& c : item.second) {
                GNEEdge* out = retrieveEdge(c.toEdge->getID());
                GNEEdge* newEdge = nullptr;
                if (in->getGNEJunctionDestiny() == out->getGNEJunctionSource()) {
                    continue;
                }
                if (newEdges.count(c.toEdge) == 0) {
                    newEdge = createEdge(in->getGNEJunctionDestiny(), out->getGNEJunctionSource(), in, undoList);
                    newEdges[c.toEdge] = newEdge;
                    newEdge->setAttribute(SUMO_ATTR_NUMLANES, "1", undoList);
                } else {
                    newEdge = newEdges[c.toEdge];
                    duplicateLane(newEdge->getLanes().back(), undoList, true);
                }
                // copy permissions
                newEdge->getLanes().back()->setAttribute(SUMO_ATTR_ALLOW,
                        in->getLanes()[c.fromLane]-> getAttribute(SUMO_ATTR_ALLOW), undoList);
            }
        }
    }

    deleteJunction(junction, undoList);
    // finish operation
    undoList->p_end();
}



void
GNENet::clearJunctionConnections(GNEJunction* junction, GNEUndoList* undoList) {
    undoList->p_begin("clear junction connections");
    std::vector<GNEConnection*> connections = junction->getGNEConnections();
    // Iterate over all connections and clear it
    for (auto i : connections) {
        deleteConnection(i, undoList);
    }
    undoList->p_end();
}


void
GNENet::resetJunctionConnections(GNEJunction* junction, GNEUndoList* undoList) {
    undoList->p_begin("reset junction connections");
    // first clear connections
    clearJunctionConnections(junction, undoList);
    // invalidate logic to create new connections in the next recomputing
    junction->setLogicValid(false, undoList);
    undoList->p_end();
}


void
GNENet::renameEdge(GNEEdge* edge, const std::string& newID) {
    myAttributeCarriers.edges.erase(edge->getNBEdge()->getID());
    myNetBuilder->getEdgeCont().rename(edge->getNBEdge(), newID);
    edge->setMicrosimID(newID);
    myAttributeCarriers.edges[newID] = edge;
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


std::vector<GNEAttributeCarrier*>
GNENet::getSelectedAttributeCarriers(bool ignoreCurrentSupermode) {
    // declare vector to save result
    std::vector<GNEAttributeCarrier*> result;
    result.reserve(gSelected.getSelected().size());
    // iterate over all elements of global selection
    for (auto i : gSelected.getSelected()) {
        // obtain AC
        GNEAttributeCarrier* AC = retrieveAttributeCarrier(i, false);
        // check if attribute carrier exist and is selected
        if (AC && AC->isAttributeCarrierSelected()) {
            // now check if selected supermode is correct
            if (ignoreCurrentSupermode ||
                    ((myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) && !AC->getTagProperty().isDemandElement()) ||
                    ((myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) && AC->getTagProperty().isDemandElement())) {
                // add it into result vector
                result.push_back(AC);
            }
        }
    }
    return result;
}


NBTrafficLightLogicCont&
GNENet::getTLLogicCont() {
    return myNetBuilder->getTLLogicCont();
}


NBEdgeCont&
GNENet::getEdgeCont() {
    return myNetBuilder->getEdgeCont();
}


void
GNENet::renameJunction(GNEJunction* junction, const std::string& newID) {
    std::string oldID = junction->getID();
    myAttributeCarriers.junctions.erase(junction->getNBNode()->getID());
    myNetBuilder->getNodeCont().rename(junction->getNBNode(), newID);
    junction->setMicrosimID(newID);
    myAttributeCarriers.junctions[newID] = junction;
    // build crossings
    junction->getNBNode()->buildCrossings();
}


void
GNENet::addExplicitTurnaround(std::string id) {
    myExplicitTurnarounds.insert(id);
}


void
GNENet::removeExplicitTurnaround(std::string id) {
    myExplicitTurnarounds.erase(id);
}


GNEAdditional*
GNENet::retrieveAdditional(SumoXMLTag type, const std::string& id, bool hardFail) const {
    if ((myAttributeCarriers.additionals.count(type) > 0) && (myAttributeCarriers.additionals.at(type).count(id) != 0)) {
        return myAttributeCarriers.additionals.at(type).at(id);
    } else if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant additional");
    } else {
        return nullptr;
    }
}


std::vector<GNEAdditional*>
GNENet::retrieveAdditionals(bool onlySelected) const {
    std::vector<GNEAdditional*> result;
    // returns additionals depending of selection
    for (auto i : myAttributeCarriers.additionals) {
        for (auto j : i.second) {
            if (!onlySelected || j.second->isAttributeCarrierSelected()) {
                result.push_back(j.second);
            }
        }
    }
    return result;
}


int
GNENet::getNumberOfAdditionals(SumoXMLTag type) const {
    int counter = 0;
    for (auto i : myAttributeCarriers.additionals) {
        if ((type == SUMO_TAG_NOTHING) || (type == i.first)) {
            counter += (int)i.second.size();
        }
    }
    return counter;
}


void
GNENet::updateAdditionalID(const std::string& oldID, GNEAdditional* additional) {
    if (myAttributeCarriers.additionals.at(additional->getTagProperty().getTag()).count(oldID) == 0) {
        throw ProcessError(additional->getTagStr() + " with old ID='" + oldID + "' doesn't exist");
    } else {
        // remove an insert additional again into container
        myAttributeCarriers.additionals.at(additional->getTagProperty().getTag()).erase(oldID);
        myAttributeCarriers.additionals.at(additional->getTagProperty().getTag()).insert(std::make_pair(additional->getID(), additional));
        // additionals has to be saved
        requiereSaveAdditionals(true);
    }
}


void
GNENet::requiereSaveAdditionals(bool value) {
    if (myAdditionalsSaved == true) {
        WRITE_DEBUG("Additionals has to be saved");
        std::string netSaved = (myNetSaved ? "saved" : "unsaved");
        std::string demandElementsSaved = (myDemandElementsSaved ? "saved" : "unsaved");
        WRITE_DEBUG("Current saving Status: net " + netSaved + ", additionals unsaved, demand elements " + demandElementsSaved);
    }
    myAdditionalsSaved = !value;
    if (myViewNet != nullptr) {
        if (myAdditionalsSaved) {
            myViewNet->getViewParent()->getGNEAppWindows()->disableSaveAdditionalsMenu();
        } else {
            myViewNet->getViewParent()->getGNEAppWindows()->enableSaveAdditionalsMenu();
        }
    }
}


void
GNENet::saveAdditionals(const std::string& filename) {
    // obtain invalid additionals depending of number of their lane parents
    std::vector<GNEAdditional*> invalidSingleLaneAdditionals;
    std::vector<GNEAdditional*> invalidMultiLaneAdditionals;
    // iterate over additionals and obtain invalids
    for (auto i : myAttributeCarriers.additionals) {
        for (auto j : i.second) {
            // check if has to be fixed
            if (j.second->getTagProperty().hasAttribute(SUMO_ATTR_LANE) && !j.second->isAdditionalValid()) {
                invalidSingleLaneAdditionals.push_back(j.second);
            } else if (j.second->getTagProperty().hasAttribute(SUMO_ATTR_LANES) && !j.second->isAdditionalValid()) {
                invalidMultiLaneAdditionals.push_back(j.second);
            }
        }
    }
    // if there are invalid StoppingPlaces or detectors, open GNEDialog_FixAdditionalElements
    if (invalidSingleLaneAdditionals.size() > 0 || invalidMultiLaneAdditionals.size() > 0) {
        // 0 -> Canceled Saving, with or whithout selecting invalid stopping places and E2
        // 1 -> Invalid stoppingPlaces and E2 fixed, friendlyPos enabled, or saved with invalid positions
        GNEDialog_FixAdditionalElements fixAdditionalElementsDialog(myViewNet, invalidSingleLaneAdditionals, invalidMultiLaneAdditionals);
        if (fixAdditionalElementsDialog.execute() == 0) {
            // show debug information
            WRITE_DEBUG("Additionals saving aborted");
        } else {
            saveAdditionalsConfirmed(filename);
            // change value of flag
            myAdditionalsSaved = true;
            // show debug information
            WRITE_DEBUG("Additionals saved after dialog");
        }
        // update view
        myViewNet->update();
        // set focus again in viewNet
        myViewNet->setFocus();
    } else {
        saveAdditionalsConfirmed(filename);
        // change value of flag
        myAdditionalsSaved = true;
        // show debug information
        WRITE_DEBUG("Additionals saved");
    }
}


bool
GNENet::isAdditionalsSaved() const {
    return myAdditionalsSaved;
}


std::string
GNENet::generateAdditionalID(SumoXMLTag type) const {
    int counter = 0;
    while (myAttributeCarriers.additionals.at(type).count(toString(type) + "_" + toString(counter)) != 0) {
        counter++;
    }
    return (toString(type) + "_" + toString(counter));
}


GNEDemandElement*
GNENet::retrieveDemandElement(SumoXMLTag type, const std::string& id, bool hardFail) const {
    if ((myAttributeCarriers.demandElements.count(type) > 0) && (myAttributeCarriers.demandElements.at(type).count(id) != 0)) {
        return myAttributeCarriers.demandElements.at(type).at(id);
    } else if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant demand element");
    } else {
        return nullptr;
    }
}


std::vector<GNEDemandElement*>
GNENet::retrieveDemandElements(bool onlySelected) const {
    std::vector<GNEDemandElement*> result;
    // returns demand elements depending of selection
    for (auto i : myAttributeCarriers.demandElements) {
        for (auto j : i.second) {
            if (!onlySelected || j.second->isAttributeCarrierSelected()) {
                result.push_back(j.second);
            }
        }
    }
    return result;
}


int
GNENet::getNumberOfDemandElements(SumoXMLTag type) const {
    int counter = 0;
    for (auto i : myAttributeCarriers.demandElements) {
        if ((type == SUMO_TAG_NOTHING) || (type == i.first)) {
            counter += (int)i.second.size();
        }
    }
    return counter;
}


void
GNENet::updateDemandElementID(const std::string& oldID, GNEDemandElement* demandElement) {
    if (myAttributeCarriers.demandElements.at(demandElement->getTagProperty().getTag()).count(oldID) == 0) {
        throw ProcessError(demandElement->getTagStr() + " with old ID='" + oldID + "' doesn't exist");
    } else {
        // remove an insert demand element again into container
        myAttributeCarriers.demandElements.at(demandElement->getTagProperty().getTag()).erase(oldID);
        myAttributeCarriers.demandElements.at(demandElement->getTagProperty().getTag()).insert(std::make_pair(demandElement->getID(), demandElement));
        // remove an insert demand element again into vehicleDepartures container
        if (demandElement->getTagProperty().isVehicle()) {
            if (myAttributeCarriers.vehicleDepartures.count(demandElement->getBegin() + "_" + oldID) == 0) {
                throw ProcessError(demandElement->getTagStr() + " with old ID='" + oldID + "' doesn't exist");
            } else {
                myAttributeCarriers.vehicleDepartures.erase(demandElement->getBegin() + "_" + oldID);
                myAttributeCarriers.vehicleDepartures.insert(std::make_pair(demandElement->getBegin() + "_" + demandElement->getID(), demandElement));
            }
        }
        // demand elements has to be saved
        requiereSaveDemandElements(true);
    }
}


void
GNENet::updateDemandElementBegin(const std::string& oldBegin, GNEDemandElement* demandElement) {
    if (myAttributeCarriers.vehicleDepartures.count(oldBegin + "_" + demandElement->getID()) == 0) {
        throw ProcessError(demandElement->getTagStr() + " with old begin='" + oldBegin + "' doesn't exist");
    } else {
        // remove an insert demand element again into vehicleDepartures container
        if (demandElement->getTagProperty().isVehicle()) {
            myAttributeCarriers.vehicleDepartures.erase(oldBegin + "_" + demandElement->getID());
            myAttributeCarriers.vehicleDepartures.insert(std::make_pair(demandElement->getBegin() + "_" + demandElement->getID(), demandElement));
        }
    }
}


void
GNENet::requiereSaveDemandElements(bool value) {
    if (myDemandElementsSaved == true) {
        WRITE_DEBUG("DemandElements has to be saved");
        std::string netSaved = (myNetSaved ? "saved" : "unsaved");
        std::string additionalsSaved = (myAdditionalsSaved ? "saved" : "unsaved");
        WRITE_DEBUG("Current saving Status: net " + netSaved + ", additionals " + additionalsSaved + ", demand elements unsaved");
    }
    myDemandElementsSaved = !value;
    if (myViewNet != nullptr) {
        if (myDemandElementsSaved) {
            myViewNet->getViewParent()->getGNEAppWindows()->disableSaveDemandElementsMenu();
        } else {
            myViewNet->getViewParent()->getGNEAppWindows()->enableSaveDemandElementsMenu();
        }
    }
}


void
GNENet::saveDemandElements(const std::string& filename) {
    // first recompute demand elements
    computeDemandElements(myViewNet->getViewParent()->getGNEAppWindows());
    // obtain invalid demandElements depending of number of their lane parents
    std::vector<GNEDemandElement*> invalidSingleLaneDemandElements;
    // iterate over demandElements and obtain invalids
    for (auto i : myAttributeCarriers.demandElements) {
        for (auto j : i.second) {
            // check if has to be fixed
            if (!j.second->isDemandElementValid()) {
                invalidSingleLaneDemandElements.push_back(j.second);
            }
        }
    }
    // if there are invalid demand elements, open GNEDialog_FixDemandElements
    if (invalidSingleLaneDemandElements.size() > 0) {
        // 0 -> Canceled Saving, with or whithout selecting invalid demand elements
        // 1 -> Invalid demand elements fixed, friendlyPos enabled, or saved with invalid positions
        GNEDialog_FixDemandElements fixDemandElementsDialog(myViewNet, invalidSingleLaneDemandElements);
        if (fixDemandElementsDialog.execute() == 0) {
            // show debug information
            WRITE_DEBUG("demand elements saving aborted");
        } else {
            saveDemandElementsConfirmed(filename);
            // change value of flag
            myDemandElementsSaved = true;
            // show debug information
            WRITE_DEBUG("demand elements saved after dialog");
        }
        // update view
        myViewNet->update();
        // set focus again in viewNet
        myViewNet->setFocus();
    } else {
        saveDemandElementsConfirmed(filename);
        // change value of flag
        myDemandElementsSaved = true;
        // show debug information
        WRITE_DEBUG("demand elements saved");
    }
}


bool
GNENet::isDemandElementsSaved() const {
    return myDemandElementsSaved;
}


std::string
GNENet::generateDemandElementID(const std::string& prefix, SumoXMLTag type) const {
    int counter = 0;
    if ((type == SUMO_TAG_VEHICLE) || (type == SUMO_TAG_TRIP) || (type == SUMO_TAG_ROUTEFLOW) || (type == SUMO_TAG_FLOW)) {
        // special case for vehicles (Vehicles, Flows, Trips and routeFlows share nameSpaces)
        while ((myAttributeCarriers.demandElements.at(SUMO_TAG_VEHICLE).count(prefix + toString(type) + "_" + toString(counter)) != 0) ||
                (myAttributeCarriers.demandElements.at(SUMO_TAG_TRIP).count(prefix + toString(type) + "_" + toString(counter)) != 0) ||
                (myAttributeCarriers.demandElements.at(SUMO_TAG_ROUTEFLOW).count(prefix + toString(type) + "_" + toString(counter)) != 0) ||
                (myAttributeCarriers.demandElements.at(SUMO_TAG_FLOW).count(prefix + toString(type) + "_" + toString(counter)) != 0)) {
            counter++;
        }
    } else if ((type == SUMO_TAG_PERSON) || (type == SUMO_TAG_PERSONFLOW)) {
        // special case for persons (person and personFlows share nameSpaces)
        while ((myAttributeCarriers.demandElements.at(SUMO_TAG_PERSON).count(prefix + toString(type) + "_" + toString(counter)) != 0) ||
                (myAttributeCarriers.demandElements.at(SUMO_TAG_PERSONFLOW).count(prefix + toString(type) + "_" + toString(counter)) != 0)) {
            counter++;
        }
    } else {
        while (myAttributeCarriers.demandElements.at(type).count(prefix + toString(type) + "_" + toString(counter)) != 0) {
            counter++;
        }
    }
    return (prefix + toString(type) + "_" + toString(counter));
}


void
GNENet::saveAdditionalsConfirmed(const std::string& filename) {
    OutputDevice& device = OutputDevice::getDevice(filename);
    device.writeXMLHeader("additional", "additional_file.xsd");
    // now write all route probes (see Ticket #4058)
    for (auto i : myAttributeCarriers.additionals) {
        if (i.first == SUMO_TAG_ROUTEPROBE) {
            for (auto j : i.second) {
                j.second->writeAdditional(device);
            }
        }
    }
    // now write all stoppingPlaces
    for (auto i : myAttributeCarriers.additionals) {
        if (GNEAttributeCarrier::getTagProperties(i.first).isStoppingPlace()) {
            for (auto j : i.second) {
                // only save stoppingPlaces that doesn't have Additional parents, because they are automatically writed by writeAdditional(...) parent's function
                if (j.second->getAdditionalParents().empty()) {
                    j.second->writeAdditional(device);
                }
            }
        }
    }
    // now write all detectors
    for (auto i : myAttributeCarriers.additionals) {
        if (GNEAttributeCarrier::getTagProperties(i.first).isDetector()) {
            for (auto j : i.second) {
                // only save Detectors that doesn't have Additional parents, because they are automatically writed by writeAdditional(...) parent's function
                if (j.second->getAdditionalParents().empty()) {
                    j.second->writeAdditional(device);
                }
            }
        }
    }
    // now write rest of additionals
    for (auto i : myAttributeCarriers.additionals) {
        const auto& tagValue = GNEAttributeCarrier::getTagProperties(i.first);
        if (!tagValue.isStoppingPlace() && !tagValue.isDetector() && (i.first != SUMO_TAG_ROUTEPROBE) && (i.first != SUMO_TAG_VTYPE) && (i.first != SUMO_TAG_ROUTE)) {
            for (auto j : i.second) {
                // only save additionals that doesn't have Additional parents, because they are automatically writed by writeAdditional(...) parent's function
                if (j.second->getAdditionalParents().empty()) {
                    j.second->writeAdditional(device);
                }
            }
        }
    }
    // now write shapes and POIs
    for (const auto& i : myPolygons) {
        dynamic_cast<GNEShape*>(i.second)->writeShape(device);
    }
    for (const auto& i : myPOIs) {
        dynamic_cast<GNEShape*>(i.second)->writeShape(device);
    }
    device.close();
}


void
GNENet::saveDemandElementsConfirmed(const std::string& filename) {
    OutputDevice& device = OutputDevice::getDevice(filename);
    device.writeXMLHeader("routes", "routes_file.xsd");
    // first  write all vehicle types
    for (auto i : myAttributeCarriers.demandElements.at(SUMO_TAG_VTYPE)) {
        i.second->writeDemandElement(device);
    }
    // first  write all person types
    for (auto i : myAttributeCarriers.demandElements.at(SUMO_TAG_PTYPE)) {
        i.second->writeDemandElement(device);
    }
    // now write all routes (and their associated stops)
    for (auto i : myAttributeCarriers.demandElements.at(SUMO_TAG_ROUTE)) {
        i.second->writeDemandElement(device);
    }
    // finally write all vehicles and persons sorted by depart time (and their associated stops, personPlans, etc.)
    for (auto i : myAttributeCarriers.vehicleDepartures) {
        i.second->writeDemandElement(device);
    }
    device.close();
}


GNEPoly*
GNENet::addPolygonForEditShapes(GNENetElement* netElement, const PositionVector& shape, bool fill, RGBColor col) {
    if (shape.size() > 0) {
        // create poly for edit shapes
        GNEPoly* shapePoly = new GNEPoly(this, "edit_shape", "edit_shape", shape, false, fill, 0.3, col, GLO_POLYGON, 0, "", false, false, false);
        shapePoly->setShapeEditedElement(netElement);
        myGrid.addAdditionalGLObject(shapePoly);
        myViewNet->update();
        return shapePoly;
    } else {
        throw ProcessError("shape cannot be empty");
    }
}


void
GNENet::removePolygonForEditShapes(GNEPoly* polygon) {
    if (polygon) {
        // remove it from Inspector Frame
        myViewNet->getViewParent()->getInspectorFrame()->getAttributesEditor()->removeEditedAC(polygon);
        // Remove from grid
        myGrid.removeAdditionalGLObject(polygon);
        myViewNet->update();
    } else {
        throw ProcessError("Polygon for edit shapes has to be inicializated");
    }
}


std::string
GNENet::generateShapeID(SumoXMLTag shapeTag) const {
    // generate tag depending of type of shape
    if (shapeTag == SUMO_TAG_POLY) {
        int counter = 0;
        std::string newID = "poly_" + toString(counter);
        // generate new IDs to find a non-assigned ID
        while (myPolygons.get(newID) != nullptr) {
            counter++;
            newID = "poly_" + toString(counter);
        }
        return newID;
    } else {
        int counter = 0;
        std::string newID = "POI_" + toString(counter);
        // generate new IDs to find a non-assigned ID
        while (myPOIs.get(newID) != nullptr) {
            counter++;
            newID = "POI_" + toString(counter);
        }
        return newID;
    }
}


void
GNENet::changeShapeID(GNEShape* s, const std::string& OldID) {
    if (s->getTagProperty().getTag() == SUMO_TAG_POLY) {
        if (myPolygons.get(OldID) == 0) {
            throw UnknownElement("Polygon " + OldID);
        } else {
            myPolygons.changeID(OldID, s->getID());
        }
    } else {
        if (myPOIs.get(OldID) == 0) {
            throw UnknownElement("POI " + OldID);
        } else {
            myPOIs.changeID(OldID, s->getID());
        }
    }
}


int
GNENet::getNumberOfShapes() const {
    return (int)(myPolygons.size() + myPOIs.size());
}


void
GNENet::requiereSaveTLSPrograms() {
    if (myTLSProgramsSaved == true) {
        WRITE_DEBUG("TLSPrograms has to be saved");
    }
    myTLSProgramsSaved = false;
    myViewNet->getViewParent()->getGNEAppWindows()->enableSaveTLSProgramsMenu();
}


void
GNENet::saveTLSPrograms(const std::string& filename) {
    // open output device
    OutputDevice& device = OutputDevice::getDevice(filename);
    device.openTag("additionals");
    // write traffic lights using NWWriter
    NWWriter_SUMO::writeTrafficLights(device, getTLLogicCont());
    device.close();
    // change flag to true
    myTLSProgramsSaved = true;
    // show debug information
    WRITE_DEBUG("TLSPrograms saved");
}


int
GNENet::getNumberOfTLSPrograms() const {
    return -1;
}

void
GNENet::enableUpdateGeometry() {
    myUpdateGeometryEnabled = true;
}


void
GNENet::disableUpdateGeometry() {
    myUpdateGeometryEnabled = false;
}


bool
GNENet::isUpdateGeometryEnabled() const {
    return myUpdateGeometryEnabled;
}

// ---------------------------------------------------------------------------
// GNENet - protected methods
// ---------------------------------------------------------------------------

bool
GNENet::additionalExist(GNEAdditional* additional) const {
    // first check that additional pointer is valid
    if (additional) {
        return myAttributeCarriers.additionals.at(additional->getTagProperty().getTag()).find(additional->getID()) !=
               myAttributeCarriers.additionals.at(additional->getTagProperty().getTag()).end();
    } else {
        throw ProcessError("Invalid additional pointer");
    }
}


void
GNENet::insertAdditional(GNEAdditional* additional) {
    // Check if additional element exists before insertion
    if (!additionalExist(additional)) {
        myAttributeCarriers.additionals.at(additional->getTagProperty().getTag()).insert(std::make_pair(additional->getID(), additional));
        // only add drawable elements in grid
        if (additional->getTagProperty().isDrawable() && additional->getTagProperty().isPlacedInRTree()) {
            myGrid.addAdditionalGLObject(additional);
        }
        // check if additional is selected
        if (additional->isAttributeCarrierSelected()) {
            additional->selectAttributeCarrier(false);
        }
        // update geometry after insertion of additionals if myUpdateGeometryEnabled is enabled
        if (myUpdateGeometryEnabled) {
            additional->updateGeometry();
        }
        // additionals has to be saved
        requiereSaveAdditionals(true);
    } else {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' already exist");
    }
}


bool
GNENet::deleteAdditional(GNEAdditional* additional, bool updateViewAfterDeleting) {
    // first check that additional pointer is valid
    if (additionalExist(additional)) {
        // obtain demand element and erase it from container
        auto it = myAttributeCarriers.additionals.at(additional->getTagProperty().getTag()).find(additional->getID());
        myAttributeCarriers.additionals.at(additional->getTagProperty().getTag()).erase(it);
        // remove it from Inspector Frame
        myViewNet->getViewParent()->getInspectorFrame()->getAttributesEditor()->removeEditedAC(additional);
        // only remove drawable elements of grid
        if (additional->getTagProperty().isDrawable() && additional->getTagProperty().isPlacedInRTree()) {
            myGrid.removeAdditionalGLObject(additional);
        }
        // check if additional is selected
        if (additional->isAttributeCarrierSelected()) {
            additional->unselectAttributeCarrier(false);
        }
        // check if view has to be updated
        if (updateViewAfterDeleting) {
            myViewNet->update();
        }
        // additionals has to be saved
        requiereSaveAdditionals(true);
        // additional removed, then return true
        return true;
    } else {
        throw ProcessError("Invalid additional pointer");
    }
}


bool
GNENet::demandElementExist(GNEDemandElement* demandElement) const {
    // first check that demandElement pointer is valid
    if (demandElement) {
        return myAttributeCarriers.demandElements.at(demandElement->getTagProperty().getTag()).find(demandElement->getID()) !=
               myAttributeCarriers.demandElements.at(demandElement->getTagProperty().getTag()).end();
    } else {
        throw ProcessError("Invalid demandElement pointer");
    }
}


void
GNENet::insertDemandElement(GNEDemandElement* demandElement) {
    // Check if demandElement element exists before insertion
    if (!demandElementExist(demandElement)) {
        // insert in demandElements container
        myAttributeCarriers.demandElements.at(demandElement->getTagProperty().getTag()).insert(std::make_pair(demandElement->getID(), demandElement));
        // also insert in vehicleDepartures container if it's either a vehicle or a person
        if (demandElement->getTagProperty().isVehicle() || demandElement->getTagProperty().isPerson()) {
            if (myAttributeCarriers.vehicleDepartures.count(demandElement->getBegin() + "_" + demandElement->getID()) != 0) {
                throw ProcessError(demandElement->getTagStr() + " with departure ='" + demandElement->getBegin() + "_" + demandElement->getID() + "' already inserted");
            } else {
                myAttributeCarriers.vehicleDepartures.insert(std::make_pair(demandElement->getBegin() + "_" + demandElement->getID(), demandElement));
            }
        }
        // only add drawable elements in grid
        if (demandElement->getTagProperty().isDrawable() && demandElement->getTagProperty().isPlacedInRTree()) {
            myGrid.addAdditionalGLObject(demandElement);
        }
        // check if demandElement is selected
        if (demandElement->isAttributeCarrierSelected()) {
            demandElement->selectAttributeCarrier(false);
        }
        // update geometry after insertion of demandElements if myUpdateGeometryEnabled is enabled
        if (myUpdateGeometryEnabled) {
            demandElement->updateGeometry();
        }
        // demandElements has to be saved
        requiereSaveDemandElements(true);
    } else {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' already exist");
    }
}


bool
GNENet::deleteDemandElement(GNEDemandElement* demandElement, bool updateViewAfterDeleting) {
    // first check that demandElement pointer is valid
    if (demandElementExist(demandElement)) {
        // obtain demand element and erase it from container
        auto it = myAttributeCarriers.demandElements.at(demandElement->getTagProperty().getTag()).find(demandElement->getID());
        myAttributeCarriers.demandElements.at(demandElement->getTagProperty().getTag()).erase(it);
        // remove it from Inspector Frame
        myViewNet->getViewParent()->getInspectorFrame()->getAttributesEditor()->removeEditedAC(demandElement);
        // also remove fromvehicleDepartures container if it's either a vehicle or a person
        if (demandElement->getTagProperty().isVehicle() || demandElement->getTagProperty().isPerson()) {
            if (myAttributeCarriers.vehicleDepartures.count(demandElement->getBegin() + "_" + demandElement->getID()) == 0) {
                throw ProcessError(demandElement->getTagStr() + " with departure ='" + demandElement->getBegin() + "_" + demandElement->getID() + "' doesn't exist");
            } else {
                myAttributeCarriers.vehicleDepartures.erase(demandElement->getBegin() + "_" + demandElement->getID());
            }
        }
        // only remove drawable elements of grid
        if (demandElement->getTagProperty().isDrawable() && demandElement->getTagProperty().isPlacedInRTree()) {
            myGrid.removeAdditionalGLObject(demandElement);
        }
        // check if demandElement is selected
        if (demandElement->isAttributeCarrierSelected()) {
            demandElement->unselectAttributeCarrier(false);
        }
        // check if view has to be updated
        if (updateViewAfterDeleting) {
            myViewNet->update();
        }
        // demandElements has to be saved
        requiereSaveDemandElements(true);
        // demandElement removed, then return true
        return true;
    } else {
        throw ProcessError("Invalid demandElement pointer");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNENet::initJunctionsAndEdges() {
    // init junctions (by default Crossing and walking areas aren't created)
    NBNodeCont& nodeContainer = myNetBuilder->getNodeCont();
    for (auto name_it : nodeContainer.getAllNames()) {
        NBNode* nbn = nodeContainer.retrieve(name_it);
        registerJunction(new GNEJunction(*nbn, this, true));
    }

    // init edges
    NBEdgeCont& ec = myNetBuilder->getEdgeCont();
    for (auto name_it : ec.getAllNames()) {
        NBEdge* nbe = ec.retrieve(name_it);
        registerEdge(new GNEEdge(*nbe, this, false, true));
        if (myGrid.getWidth() > 10e16 || myGrid.getHeight() > 10e16) {
            throw ProcessError("Network size exceeds 1 Lightyear. Please reconsider your inputs.\n");
        }
    }

    // make sure myGrid is initialized even for an empty net
    if (myAttributeCarriers.edges.size() == 0) {
        myGrid.add(Boundary(0, 0, 100, 100));
    }

    // sort nodes edges so that arrows can be drawn correctly
    NBNodesEdgesSorter::sortNodesEdges(nodeContainer);
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
    // increase reference
    junction->incRef("GNENet::registerJunction");
    junction->setResponsible(false);
    myAttributeCarriers.junctions[junction->getMicrosimID()] = junction;
    // add it into grid
    myGrid.add(junction->getCenteringBoundary());
    myGrid.addAdditionalGLObject(junction);
    // update geometry
    junction->updateGeometry();
    // check if junction is selected
    if (junction->isAttributeCarrierSelected()) {
        junction->selectAttributeCarrier(false);
    }
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
    myAttributeCarriers.edges[edge->getMicrosimID()] = edge;
    // add edge to grid
    myGrid.add(edge->getCenteringBoundary());
    myGrid.addAdditionalGLObject(edge);
    // check if edge is selected
    if (edge->isAttributeCarrierSelected()) {
        edge->selectAttributeCarrier(false);
    }
    // Add references into GNEJunctions
    edge->getGNEJunctionSource()->addOutgoingGNEEdge(edge);
    edge->getGNEJunctionDestiny()->addIncomingGNEEdge(edge);
    // update view
    update();
    return edge;
}


void
GNENet::deleteSingleJunction(GNEJunction* junction, bool updateViewAfterDeleting) {
    // remove it from Inspector Frame
    myViewNet->getViewParent()->getInspectorFrame()->getAttributesEditor()->removeEditedAC(junction);
    // Remove from grid and container
    myGrid.removeAdditionalGLObject(junction);
    // check if junction is selected
    if (junction->isAttributeCarrierSelected()) {
        junction->unselectAttributeCarrier(false);
    }
    myAttributeCarriers.junctions.erase(junction->getMicrosimID());
    myNetBuilder->getNodeCont().extract(junction->getNBNode());
    junction->decRef("GNENet::deleteSingleJunction");
    junction->setResponsible(true);
    // check if view has to be updated
    if (updateViewAfterDeleting) {
        myViewNet->update();
    }
}


void
GNENet::deleteSingleEdge(GNEEdge* edge, bool updateViewAfterDeleting) {
    // remove it from Inspector Frame
    myViewNet->getViewParent()->getInspectorFrame()->getAttributesEditor()->removeEditedAC(edge);
    // remove edge from visual grid and container
    myGrid.removeAdditionalGLObject(edge);
    // check if junction is selected
    if (edge->isAttributeCarrierSelected()) {
        edge->unselectAttributeCarrier(false);
    }
    myAttributeCarriers.edges.erase(edge->getMicrosimID());
    // extract edge of district container
    myNetBuilder->getEdgeCont().extract(myNetBuilder->getDistrictCont(), edge->getNBEdge());
    edge->decRef("GNENet::deleteSingleEdge");
    edge->setResponsible(true);
    // Remove refrences from GNEJunctions
    edge->getGNEJunctionSource()->removeOutgoingGNEEdge(edge);
    edge->getGNEJunctionDestiny()->removeIncomingGNEEdge(edge);
    // check if view has to be updated
    if (updateViewAfterDeleting) {
        myViewNet->update();
    }
}


void
GNENet::insertShape(GNEShape* shape, bool updateViewAfterDeleting) {
    // add shape depending of their type and if is selected
    if (shape->getTagProperty().getTag() == SUMO_TAG_POLY) {
        GUIPolygon* poly = dynamic_cast<GUIPolygon*>(shape);
        // all polys are placed over RTree
        myGrid.addAdditionalGLObject(poly);
        myPolygons.add(shape->getID(), poly);
    } else {
        GUIPointOfInterest* poi = dynamic_cast<GUIPointOfInterest*>(shape);
        // Only certain POIs are placed in RTrees
        if (shape->getTagProperty().isPlacedInRTree()) {
            myGrid.addAdditionalGLObject(poi);
        }
        myPOIs.add(shape->getID(), poi);

    }
    // check if shape has to be selected
    if (shape->isAttributeCarrierSelected()) {
        shape->selectAttributeCarrier(false);
    }
    // insert shape requieres always save additionals
    requiereSaveAdditionals(true);
    // after inserting, update geometry (needed for POILanes
    shape->updateGeometry();
    // check if view has to be updated
    if (updateViewAfterDeleting) {
        myViewNet->update();
    }
}


void
GNENet::removeShape(GNEShape* shape, bool updateViewAfterDeleting) {
    // remove it from Inspector Frame
    myViewNet->getViewParent()->getInspectorFrame()->getAttributesEditor()->removeEditedAC(shape);
    if (shape->getTagProperty().getTag() == SUMO_TAG_POLY) {
        GUIPolygon* poly = dynamic_cast<GUIPolygon*>(shape);
        myGrid.removeAdditionalGLObject(poly);
        myPolygons.remove(shape->getID(), false);
    } else {
        GUIPointOfInterest* poi = dynamic_cast<GUIPointOfInterest*>(shape);
        // only certain POIS are placed in RTREE
        if (shape->getTagProperty().isPlacedInRTree()) {
            myGrid.removeAdditionalGLObject(poi);
        }
        myPOIs.remove(shape->getID(), false);
    }
    // check if shape has to be unselected
    if (shape->isAttributeCarrierSelected()) {
        shape->unselectAttributeCarrier(false);
    }
    // remove shape requires always save additionals
    requiereSaveAdditionals(true);
    // check if view has to be updated
    if (updateViewAfterDeleting) {
        myViewNet->update();
    }
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
    for (const auto& i : myAttributeCarriers.edges) {
        // remake connections
        i.second->remakeGNEConnections();
        // update geometry of connections
        for (const auto& j : i.second->getGNEConnections()) {
            j->updateGeometry();
        }
    }
}


void
GNENet::computeAndUpdate(OptionsCont& oc, bool volatileOptions) {
    // make sure we only add turn arounds to edges which currently exist within the network
    std::set<std::string> liveExplicitTurnarounds;
    for (auto it : myExplicitTurnarounds) {
        if (myAttributeCarriers.edges.count(it) > 0) {
            liveExplicitTurnarounds.insert(it);
        }
    }
    // removes all junctions of grid
    WRITE_GLDEBUG("Removing junctions during recomputing");
    for (const auto& it : myAttributeCarriers.junctions) {
        myGrid.removeAdditionalGLObject(it.second);
    }
    // remove all edges from grid
    WRITE_GLDEBUG("Removing edges during recomputing");
    for (const auto& it : myAttributeCarriers.edges) {
        myGrid.removeAdditionalGLObject(it.second);
    }
    // compute using NetBuilder
    myNetBuilder->compute(oc, liveExplicitTurnarounds, volatileOptions);
    // update ids if necessary
    if (oc.getBool("numerical-ids") || oc.isSet("reserved-ids")) {
        std::map<std::string, GNEEdge*> newEdgeMap;
        std::map<std::string, GNEJunction*> newJunctionMap;
        // fill newEdgeMap
        for (auto it : myAttributeCarriers.edges) {
            it.second->setMicrosimID(it.second->getNBEdge()->getID());
            newEdgeMap[it.second->getNBEdge()->getID()] = it.second;
        }
        for (auto it : myAttributeCarriers.junctions) {
            newJunctionMap[it.second->getNBNode()->getID()] = it.second;
            it.second->setMicrosimID(it.second->getNBNode()->getID());
        }
        myAttributeCarriers.edges = newEdgeMap;
        myAttributeCarriers.junctions = newJunctionMap;
    }
    // update rtree if necessary
    if (!oc.getBool("offset.disable-normalization")) {
        for (auto it : myAttributeCarriers.edges) {
            // refresh edge geometry
            it.second->updateGeometry();
        }
    }
    // Clear current inspected ACs in inspectorFrame if a previous net was loaded
    if (myViewNet != nullptr) {
        myViewNet->getViewParent()->getInspectorFrame()->clearInspectedAC();
    }
    // Reset Grid
    myGrid.reset();
    myGrid.add(GeoConvHelper::getFinal().getConvBoundary());
    // if volatile options are true
    if (volatileOptions) {
        // check that viewNet exist
        if (myViewNet == nullptr) {
            throw ProcessError("ViewNet doesn't exist");
        }
        // disable update geometry before clear undo list
        myUpdateGeometryEnabled = false;
        // clear undo list (This will be remove additionals and shapes)
        myViewNet->getUndoList()->p_clear();
        // remove all edges of net (It was already removed from grid)
        auto copyOfEdges = myAttributeCarriers.edges;
        for (auto it : copyOfEdges) {
            myAttributeCarriers.edges.erase(it.second->getMicrosimID());
        }
        // removes all junctions of net  (It was already removed from grid)
        auto copyOfJunctions = myAttributeCarriers.junctions;
        for (auto it : copyOfJunctions) {
            myAttributeCarriers.junctions.erase(it.second->getMicrosimID());
        }
        // clear rest of additional that weren't removed during cleaning of undo list
        for (const auto& it : myAttributeCarriers.additionals) {
            for (const auto& j : it.second) {
                // only remove drawable additionals
                if (j.second->getTagProperty().isDrawable()) {
                    myGrid.removeAdditionalGLObject(j.second);
                }
            }
        }
        myAttributeCarriers.additionals.clear();
        // fill additionals with tags (note: this include the TAZS)
        auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_ADDITIONAL, false);
        for (auto i : listOfTags) {
            myAttributeCarriers.additionals.insert(std::make_pair(i, std::map<std::string, GNEAdditional*>()));
        }
        listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_TAZ, false);
        for (auto i : listOfTags) {
            myAttributeCarriers.additionals.insert(std::make_pair(i, std::map<std::string, GNEAdditional*>()));
        }
        // clear rest of polygons that weren't removed during cleaning of undo list
        for (const auto& it : myPolygons) {
            myGrid.removeAdditionalGLObject(dynamic_cast<GUIGlObject*>(it.second));
        }
        myPolygons.clear();
        // clear rest of POIs that weren't removed during cleaning of undo list
        for (const auto& it : myPOIs) {
            myGrid.removeAdditionalGLObject(dynamic_cast<GUIGlObject*>(it.second));
        }
        myPOIs.clear();
        // clear rest of demand elements that weren't removed during cleaning of undo list
        for (const auto& it : myAttributeCarriers.demandElements) {
            for (const auto& j : it.second) {
                // only remove drawable additionals
                if (j.second->getTagProperty().isDrawable()) {
                    myGrid.removeAdditionalGLObject(j.second);
                }
            }
        }
        myAttributeCarriers.additionals.clear();
        // fill demand elements with tags
        listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_DEMANDELEMENT, false);
        for (auto i : listOfTags) {
            myAttributeCarriers.demandElements.insert(std::make_pair(i, std::map<std::string, GNEDemandElement*>()));
        }
        listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_STOP, false);
        for (auto i : listOfTags) {
            myAttributeCarriers.demandElements.insert(std::make_pair(i, std::map<std::string, GNEDemandElement*>()));
        }
        // enable update geometry again
        myUpdateGeometryEnabled = true;
        // Write GL debug information
        WRITE_GLDEBUG("initJunctionsAndEdges function called in computeAndUpdate(...) due recomputing with volatile options");
        // init again junction an edges (Additionals and shapes will be loaded after the end of this function)
        initJunctionsAndEdges();
    } else {
        // insert all junctions of grid again
        WRITE_GLDEBUG("Add junctions during recomputing after calling myNetBuilder->compute(...)");
        for (const auto& it : myAttributeCarriers.junctions) {
            myGrid.addAdditionalGLObject(it.second);
        }
        // insert all edges from grid again
        WRITE_GLDEBUG("Add egdges during recomputing after calling myNetBuilder->compute(...)");
        for (const auto& it : myAttributeCarriers.edges) {
            myGrid.addAdditionalGLObject(it.second);
        }
        // remake connections
        for (auto it : myAttributeCarriers.edges) {
            it.second->remakeGNEConnections();
        }
        // iterate over junctions of net
        for (const auto& it : myAttributeCarriers.junctions) {
            // undolist may not yet exist but is also not needed when just marking junctions as valid
            it.second->setLogicValid(true, nullptr);
            // updated geometry
            it.second->updateGeometryAfterNetbuild();
        }
        // iterate over all edges of net
        for (const auto& it : myAttributeCarriers.edges) {
            // update geometry
            it.second->updateGeometry();
        }
    }
    // net recomputed, then return false;
    myNeedRecompute = false;
}


void
GNENet::replaceInListAttribute(GNEAttributeCarrier* ac, SumoXMLAttr key, const std::string& which, const std::string& by, GNEUndoList* undoList) {
    assert(ac->getTagProperty().getAttributeProperties(key).isList());
    std::vector<std::string> values = GNEAttributeCarrier::parse<std::vector<std::string> >(ac->getAttribute(key));
    std::vector<std::string> newValues;
    for (auto v : values) {
        newValues.push_back(v == which ? by : v);
    }
    ac->setAttribute(key, toString(newValues), undoList);
}

/****************************************************************************/

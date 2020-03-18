/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNENetHelper.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2020
///
// Helper for GNENet
/****************************************************************************/

#include <netbuild/NBAlgorithms.h>
#include <netbuild/NBNetBuilder.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/additional/GNEPOI.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/data/GNEDataSet.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <netedit/elements/demand/GNEVehicleType.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNEJunction.h>
#include <netedit/elements/network/GNELane.h>
#include <utils/router/DijkstraRouter.h>

#include "GNENetHelper.h"

// ---------------------------------------------------------------------------
// GNENetHelper::AttributeCarriers - methods
// ---------------------------------------------------------------------------

GNENetHelper::AttributeCarriers::AttributeCarriers(GNENet* net) :
    myNet(net) {
    // fill tags
    fillTags();
}


void
GNENetHelper::AttributeCarriers::fillTags() {
    // fill additionals with tags (note: this include the TAZS)
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNETagProperties::TagType::ADDITIONALELEMENT, false);
    for (const auto& additionalTag : listOfTags) {
        additionals.insert(std::make_pair(additionalTag, std::map<std::string, GNEAdditional*>()));
    }
    listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNETagProperties::TagType::TAZ, false);
    for (const auto& tazTag : listOfTags) {
        additionals.insert(std::make_pair(tazTag, std::map<std::string, GNEAdditional*>()));
    }
    // fill demand elements with tags
    listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNETagProperties::TagType::DEMANDELEMENT, false);
    for (const auto& demandTag : listOfTags) {
        demandElements.insert(std::make_pair(demandTag, std::map<std::string, GNEDemandElement*>()));
    }
    listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNETagProperties::TagType::STOP, false);
    for (const auto& stopTag : listOfTags) {
        demandElements.insert(std::make_pair(stopTag, std::map<std::string, GNEDemandElement*>()));
    }
}


GNENetHelper::AttributeCarriers::~AttributeCarriers() {
    // Drop Edges
    for (const auto& edge : edges) {
        edge.second->decRef("GNENetHelper::~GNENet");
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + edge.second->getTagStr() + " '" + edge.second->getID() + "' in AttributeCarriers destructor");
        delete edge.second;
    }
    // Drop junctions
    for (const auto& junction : junctions) {
        junction.second->decRef("GNENetHelper::~GNENet");
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + junction.second->getTagStr() + " '" + junction.second->getID() + "' in AttributeCarriers destructor");
        delete junction.second;
    }
    // Drop Additionals (Only used for additionals that were inserted without using GNEChange_Additional)
    for (const auto& additionalTag : additionals) {
        for (const auto& additional : additionalTag.second) {
            // decrease reference manually (because it was increased manually in GNEAdditionalHandler)
            additional.second->decRef();
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + additional.second->getTagStr() + " '" + additional.second->getID() + "' in AttributeCarriers destructor");
            delete additional.second;
        }
    }
    // Drop demand elements (Only used for demand elements that were inserted without using GNEChange_DemandElement, for example the default VType")
    for (const auto& demandElementTag : demandElements) {
        for (const auto& demandElement : demandElementTag.second) {
            // decrease reference manually (because it was increased manually in GNERouteHandler)
            demandElement.second->decRef();
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + demandElement.second->getTagStr() + " '" + demandElement.second->getID() + "' in AttributeCarriers destructor");
            delete demandElement.second;
        }
    }
    // Decrease reference of Polys (needed after volatile recomputing)
    for (const auto& polygon : myPolygons) {
        dynamic_cast<GNEAttributeCarrier*>(polygon.second)->decRef("GNENet::~GNENet");
    }
    // Decrease reference of POIs (needed after volatile recomputing)
    for (const auto& POI : myPOIs) {
        dynamic_cast<GNEAttributeCarrier*>(POI.second)->decRef("GNENet::~GNENet");
    }
}


void
GNENetHelper::AttributeCarriers::updateID(GNEAttributeCarrier* AC, const std::string newID) {
    if (AC->getTagProperty().getTag() == SUMO_TAG_JUNCTION) {
        updateJunctionID(AC, newID);
    }
    else if (AC->getTagProperty().getTag() == SUMO_TAG_EDGE) {
        updateEdgeID(AC, newID);
    }
    else if (AC->getTagProperty().isAdditionalElement() || AC->getTagProperty().isTAZ()) {
        updateAdditionalID(AC, newID);
    }
    else if (AC->getTagProperty().isShape()) {
        updateShapeID(AC, newID);
    }
    else if (AC->getTagProperty().isDemandElement()) {
        updateDemandElementID(AC, newID);
    }
    else if (AC->getTagProperty().isDataElement()) {
        updateDataSetID(AC, newID);
    }
    else {
        throw ProcessError("Unknow Attribute Carrier");
    }
}


std::vector<GNEGenericData*>
GNENetHelper::AttributeCarriers::retrieveGenericDatas(const SumoXMLTag genericDataTag, const double begin, const double end) {
    // declare generic data vector
    std::vector<GNEGenericData*> genericDatas;
    // iterate over all data sets
    for (const auto& dataSet : dataSets) {
        for (const auto& interval : dataSet.second->getDataIntervalChildren()) {
            // check interval
            if ((interval.second->getAttributeDouble(SUMO_ATTR_BEGIN) >= begin) && (interval.second->getAttributeDouble(SUMO_ATTR_END) <= end)) {
                // iterate over generic datas
                for (const auto& genericData : interval.second->getGenericDataChildren()) {
                    if (genericData->getTagProperty().getTag() == genericDataTag) {
                        genericDatas.push_back(genericData);
                    }
                }
            }
        }
    }
    return genericDatas;
}


bool 
GNENetHelper::AttributeCarriers::addShape(GNEShape* shape) {
    if (shape == nullptr) {
        return false;
    } else if (shape->getTagProperty().getTag() == SUMO_TAG_POLY) {
        return myPolygons.add(shape->getID(), dynamic_cast<SUMOPolygon*>(shape));
    } else {
        return myPOIs.add(shape->getID(), dynamic_cast<PointOfInterest*>(shape));
    }
}


bool 
GNENetHelper::AttributeCarriers::removeShape(GNEShape* shape) {
    if (shape == nullptr) {
        return false;
    } else if (shape->getTagProperty().getTag() == SUMO_TAG_POLY) {
        return myPolygons.remove(shape->getID(), false);
    } else {
        return myPOIs.remove(shape->getID(), false);
    }
}


void 
GNENetHelper::AttributeCarriers::clearShapes() {
    myPolygons.clear();
    myPOIs.clear();
}


void
GNENetHelper::AttributeCarriers::updateJunctionID(GNEAttributeCarrier* AC, const std::string& newID) {
    if (junctions.count(AC->getID()) == 0) {
        throw ProcessError(AC->getTagStr() + " with ID='" + AC->getID() + "' doesn't exist in AttributeCarriers.junction");
    } else if (junctions.count(newID) != 0) {
        throw ProcessError("There is another " + AC->getTagStr() + " with new ID='" + newID + "' in junctions");
    } else {
        // retrieve junction
        GNEJunction* junction = junctions.at(AC->getID());
        // remove junction from container
        junctions.erase(junction->getNBNode()->getID());
        // rename in NetBuilder
        myNet->getNetBuilder()->getNodeCont().rename(junction->getNBNode(), newID);
        // update microsim ID
        junction->setMicrosimID(newID);
        // add it into junctions again
        junctions[AC->getID()] = junction;
        // build crossings
        junction->getNBNode()->buildCrossings();
        // net has to be saved
        myNet->requireSaveNet(true);
    }
}


void
GNENetHelper::AttributeCarriers::updateEdgeID(GNEAttributeCarrier* AC, const std::string& newID) {
    if (edges.count(AC->getID()) == 0) {
        throw ProcessError(AC->getTagStr() + " with ID='" + AC->getID() + "' doesn't exist in AttributeCarriers.edge");
    } else if (edges.count(newID) != 0) {
        throw ProcessError("There is another " + AC->getTagStr() + " with new ID='" + newID + "' in edges");
    } else {
        // retrieve edge
        GNEEdge* edge = edges.at(AC->getID());
        // remove edge from container
        edges.erase(edge->getNBEdge()->getID());
        // rename in NetBuilder
        myNet->getNetBuilder()->getEdgeCont().rename(edge->getNBEdge(), newID);
        // update microsim ID
        edge->setMicrosimID(newID);
        // add it into edges again
        edges[AC->getID()] = edge;
        // rename all connections related to this edge
        for (const auto& lane : edge->getLanes()) {
            lane->updateConnectionIDs();
        }
        // net has to be saved
        myNet->requireSaveNet(true);
    }
}


void
GNENetHelper::AttributeCarriers::updateAdditionalID(GNEAttributeCarrier* AC, const std::string& newID) {
    if (additionals.at(AC->getTagProperty().getTag()).count(AC->getID()) == 0) {
        throw ProcessError(AC->getTagStr() + " with ID='" + AC->getID() + "' doesn't exist in AttributeCarriers.additionals");
    } else if (additionals.at(AC->getTagProperty().getTag()).count(newID) != 0) {
        throw ProcessError("There is another " + AC->getTagStr() + " with new ID='" + newID + "' in AttributeCarriers.additionals");
    } else {
        // retrieve additional 
        GNEAdditional* additional = additionals.at(AC->getTagProperty().getTag()).at(AC->getID());
        // remove additional from container
        additionals.at(additional->getTagProperty().getTag()).erase(additional->getID());
        // set new ID in additional
        additional->setMicrosimID(newID);
        // insert additional again in container
        additionals.at(additional->getTagProperty().getTag()).insert(std::make_pair(additional->getID(), additional));
        // additionals has to be saved
        myNet->requireSaveAdditionals(true);
    }
}


void
GNENetHelper::AttributeCarriers::updateShapeID(GNEAttributeCarrier* AC, const std::string& newID) {
    // first check if we're editing a Poly or a POI
    if (AC->getTagProperty().getTag() == SUMO_TAG_POLY) {
        // check that exist in shape container
        if (myPolygons.get(AC->getID()) == 0) {
            throw UnknownElement("Polygon " + AC->getID());
        } else {
            // change polygon ID
            myPolygons.changeID(AC->getID(), newID);
        }
    } else if ((AC->getTagProperty().getTag() == SUMO_TAG_POI) || (AC->getTagProperty().getTag() == SUMO_TAG_POILANE)) {
        // check that exist in shape container
        if (myPOIs.get(AC->getID()) == 0) {
            throw UnknownElement("POI " + AC->getID());
        }
        else {
            // change POI ID
            myPOIs.changeID(AC->getID(), newID);
        }
    } else {
        throw ProcessError("Invalid GNEShape");
    }
}


void
GNENetHelper::AttributeCarriers::updateDemandElementID(GNEAttributeCarrier* AC, const std::string& newID) {
    if (demandElements.at(AC->getTagProperty().getTag()).count(AC->getID()) == 0) {
        throw ProcessError(AC->getTagStr() + " with ID='" + AC->getID() + "' doesn't exist in AttributeCarriers.demandElements");
    } else if (demandElements.at(AC->getTagProperty().getTag()).count(newID) != 0) {
        throw ProcessError("There is another " + AC->getTagStr() + " with new ID='" + newID + "' in AttributeCarriers.demandElements");
    } else {
        // retrieve demand element 
        GNEDemandElement* demandElement = demandElements.at(AC->getTagProperty().getTag()).at(AC->getID());
        // remove demand from container
        demandElements.at(demandElement->getTagProperty().getTag()).erase(demandElement->getID());
        // if is vehicle, remove it from vehicleDepartures
        if (demandElement->getTagProperty().isVehicle()) {
            if (vehicleDepartures.count(demandElement->getBegin() + "_" + demandElement->getID()) == 0) {
                throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in AttributeCarriers.vehicleDepartures");
            } else {
                vehicleDepartures.erase(demandElement->getBegin() + "_" + demandElement->getID());
            }
        }
        // set new ID in demand
        demandElement->setMicrosimID(newID);
        // insert demand again in container
        demandElements.at(demandElement->getTagProperty().getTag()).insert(std::make_pair(demandElement->getID(), demandElement));
        // if is vehicle, add it into vehicleDepartures
        if (demandElement->getTagProperty().isVehicle()) {
            vehicleDepartures.insert(std::make_pair(demandElement->getBegin() + "_" + demandElement->getID(), demandElement));
        }
        // demandElements has to be saved
        myNet->requireSaveDemandElements(true);
    }
}


void
GNENetHelper::AttributeCarriers::updateDataSetID(GNEAttributeCarrier* AC, const std::string& newID) {
    if (dataSets.count(AC->getID()) == 0) {
        throw ProcessError(AC->getTagStr() + " with ID='" + AC->getID() + "' doesn't exist in AttributeCarriers.dataSets");
    } else if (dataSets.count(newID) != 0) {
        throw ProcessError("There is another " + AC->getTagStr() + " with new ID='" + newID + "' in dataSets");
    } else {
        // retrieve dataSet
        GNEDataSet* dataSet = dataSets.at(AC->getID());
        // remove dataSet from container
        dataSets.erase(dataSet->getID());
        // set new ID in dataSet
        dataSet->setDataSetID(newID);
        // insert dataSet again in container
        dataSets[dataSet->getID()] = dataSet;
        // data sets has to be saved
        myNet->requireSaveDataElements(true);
        // update interval toolbar
        myNet->getViewNet()->getIntervalBar().updateIntervalBar();
    }
}

// ---------------------------------------------------------------------------
// GNENetHelper::PathCalculator - methods
// ---------------------------------------------------------------------------

GNENetHelper::PathCalculator::PathCalculator(const GNENet* net) :
    myNet(net),
    myDijkstraRouter(nullptr) {
    // create myDijkstraRouter
    myDijkstraRouter = new DijkstraRouter<NBRouterEdge, NBVehicle>(
        myNet->getNetBuilder()->getEdgeCont().getAllRouterEdges(),
        true, &NBRouterEdge::getTravelTimeStatic, nullptr, true);
}


GNENetHelper::PathCalculator::~PathCalculator() {
    delete myDijkstraRouter;
}


void
GNENetHelper::PathCalculator::updatePathCalculator() {
    // simply delete and create myDijkstraRouter again
    if (myDijkstraRouter) {
        delete myDijkstraRouter;
    }
    myDijkstraRouter = new DijkstraRouter<NBRouterEdge, NBVehicle>(
        myNet->getNetBuilder()->getEdgeCont().getAllRouterEdges(),
        true, &NBRouterEdge::getTravelTimeStatic, nullptr, true);
}


std::vector<GNEEdge*>
GNENetHelper::PathCalculator::calculatePath(const SUMOVehicleClass vClass, const std::vector<GNEEdge*>& partialEdges) const {
    // declare a solution vector
    std::vector<GNEEdge*> solution;
    // calculate route depending of number of partial edges
    if (partialEdges.size() == 1) {
        // if there is only one partialEdges, route has only one edge
        solution.push_back(partialEdges.front());
    } else {
        // declare temporal vehicle
        NBVehicle tmpVehicle("temporalNBVehicle", vClass);
        // obtain pointer to GNENet
        GNENet* net = partialEdges.front()->getNet();
        // iterate over every selected edges
        for (int i = 1; i < (int)partialEdges.size(); i++) {
            // declare a temporal route in which save route between two last edges
            std::vector<const NBRouterEdge*> partialRoute;
            myDijkstraRouter->compute(partialEdges.at(i - 1)->getNBEdge(), partialEdges.at(i)->getNBEdge(), &tmpVehicle, 10, partialRoute);
            // save partial route in solution
            for (const auto& edgeID : partialRoute) {
                solution.push_back(net->retrieveEdge(edgeID->getID()));
            }
        }
    }
    // filter solution
    auto solutionIt = solution.begin();
    // iterate over solution
    while (solutionIt != solution.end()) {
        if ((solutionIt + 1) != solution.end()) {
            // if next edge is the same of current edge, remove it
            if (*solutionIt == *(solutionIt + 1)) {
                solutionIt = solution.erase(solutionIt);
            } else {
                solutionIt++;
            }
        } else {
            solutionIt++;
        }
    }
    return solution;
}


std::vector<GNEEdge*>
GNENetHelper::PathCalculator::calculatePath(const SUMOVehicleClass vClass, const std::vector<std::string>& partialEdgesStr) const {
    // declare a vector of GNEEdges
    std::vector<GNEEdge*> partialEdges;
    partialEdges.reserve(partialEdgesStr.size());
    // convert to vector of GNEEdges
    for (const auto& egeID : partialEdgesStr) {
        partialEdges.push_back(myNet->retrieveEdge(egeID));
    }
    // calculate DijkstraRoute using partialEdges
    return calculatePath(vClass, partialEdges);
}


bool
GNENetHelper::PathCalculator::consecutiveEdgesConnected(const SUMOVehicleClass vClass, const GNEEdge* from, const GNEEdge* to) const {
    // check conditions
    if ((from == nullptr) || (to == nullptr)) {
        // edges cannot be null
        return false;
    } else if (from == to) {
        // the same edge cannot be consecutive of itself
        return false;
    } else if (vClass == SVC_PEDESTRIAN) {
        // for pedestrians consecutive edges are always connected
        return true;
    } else {
        // declare temporal vehicle
        NBVehicle tmpVehicle("temporalNBVehicle", vClass);
        // declare a temporal route in which save route between two last edges
        std::vector<const NBRouterEdge*> solution;
        // calculate route betwen from and to edge
        myDijkstraRouter->compute(from->getNBEdge(), to->getNBEdge(), &tmpVehicle, 10, solution);
        // check if soultion is enmpty
        if (solution.size() == 2) {
            return true;
        }
        else {
            return false;
        }
    }
}

// ---------------------------------------------------------------------------
// GNENetHelper::GNEChange_ReplaceEdgeInTLS - methods
// ---------------------------------------------------------------------------

GNENetHelper::GNEChange_ReplaceEdgeInTLS::GNEChange_ReplaceEdgeInTLS(NBTrafficLightLogicCont& tllcont, NBEdge* replaced, NBEdge* by) :
    GNEChange(true),
    myTllcont(tllcont),
    myReplaced(replaced),
    myBy(by) {
}


GNENetHelper::GNEChange_ReplaceEdgeInTLS::~GNEChange_ReplaceEdgeInTLS() {};


FXString 
GNENetHelper::GNEChange_ReplaceEdgeInTLS::undoName() const {
    return "Redo replace in TLS";
}


FXString 
GNENetHelper::GNEChange_ReplaceEdgeInTLS::redoName() const {
    return "Undo replace in TLS";
}


void 
GNENetHelper::GNEChange_ReplaceEdgeInTLS::undo() {
    myTllcont.replaceRemoved(myBy, -1, myReplaced, -1);
}


void 
GNENetHelper::GNEChange_ReplaceEdgeInTLS::redo() {
    myTllcont.replaceRemoved(myReplaced, -1, myBy, -1);
}


bool 
GNENetHelper::GNEChange_ReplaceEdgeInTLS::trueChange() {
    return myReplaced != myBy;
}

/****************************************************************************/

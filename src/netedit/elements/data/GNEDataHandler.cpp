/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDataHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// Builds data objects for netedit
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/MsgHandler.h>

#include "GNEDataHandler.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEAdditionalHandler::HierarchyInsertedDatas method definitions
// ---------------------------------------------------------------------------

void
GNEDataHandler::HierarchyInsertedDatas::insertElement(SumoXMLTag tag) {
    myInsertedElements.push_back(std::make_pair(tag, nullptr));
}


void
GNEDataHandler::HierarchyInsertedDatas::commitElementInsertion(GNEGenericData* genericData) {
    myInsertedElements.back().second = genericData;
}


void
GNEDataHandler::HierarchyInsertedDatas::popElement() {
    if (!myInsertedElements.empty()) {
        myInsertedElements.pop_back();
    }
}


GNEGenericData*
GNEDataHandler::HierarchyInsertedDatas::retrieveParentGenericData(GNEViewNet* viewNet, SumoXMLTag expectedTag) const {
    if (myInsertedElements.size() < 2) {
        // currently we're finding parent additional in the additional XML root
        WRITE_WARNING("A " + toString(myInsertedElements.back().first) + " must be declared within the definition of a " + toString(expectedTag) + ".");
        return nullptr;
    } else {
        if (myInsertedElements.size() < 2) {
            // additional was hierarchically bad loaded, then return nullptr
            return nullptr;
        } else if ((myInsertedElements.end() - 2)->second == nullptr) {
            WRITE_WARNING(toString(expectedTag) + " parent of " + toString((myInsertedElements.end() - 1)->first) + " was not loaded sucesfully.");
            // parent additional wasn't sucesfully loaded, then return nullptr
            return nullptr;
        }
        /*
        GNEGenericData* retrievedAdditional = viewNet->getNet()->retrieveAdditional((myInsertedElements.end() - 2)->first, (myInsertedElements.end() - 2)->second->getID(), false);
        if (retrievedAdditional == nullptr) {
            // additional doesn't exist
            WRITE_WARNING("A " + toString((myInsertedElements.end() - 1)->first) + " must be declared within the definition of a " + toString(expectedTag) + ".");
            return nullptr;
        } else if (retrievedAdditional->getTagProperty().getTag() != expectedTag) {
            // invalid parent additional
            WRITE_WARNING("A " + toString((myInsertedElements.end() - 1)->first) + " cannot be declared within the definition of a " + retrievedAdditional->getTagStr() + ".");
            return nullptr;
        } else {
            return retrievedAdditional;
        }
        */
        return nullptr;
    }
}


GNEGenericData*
GNEDataHandler::HierarchyInsertedDatas::getLastInsertedGenericData() const {
    // ierate in reverse mode over myInsertedElements to obtain last inserted additional
    for (std::vector<std::pair<SumoXMLTag, GNEGenericData*> >::const_reverse_iterator i = myInsertedElements.rbegin(); i != myInsertedElements.rend(); i++) {
        // we need to avoid Tag Param because isn't an additional
        if (i->first != SUMO_TAG_PARAM) {
            return i->second;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// GNEAdditionalHandler::HierarchyInsertedDatas method definitions
// ---------------------------------------------------------------------------

GNEDataHandler::GNEDataHandler(const std::string& file, GNEViewNet* viewNet) :
    SUMOSAXHandler(file),
    myViewNet(viewNet) {
}


GNEDataHandler::~GNEDataHandler() {}


void
GNEDataHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // Obtain tag of element
    SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // check if we're parsing a parameter
    if (tag == SUMO_TAG_PARAM) {
        // push element int stack
        // myHierarchyInsertedDatas.insertElement(tag);
        // parse parameter
        parseParameter(attrs);
    } else if (tag != SUMO_TAG_NOTHING) {
        // push element int stack
        if (tag == SUMO_TAG_TRAIN_STOP) {
            // ensure that access elements can find their parent in myHierarchyInsertedDatas
            tag = SUMO_TAG_BUS_STOP;
        }
        // myHierarchyInsertedDatas.insertElement(tag);
        // build data
        buildData(myViewNet, true, tag, attrs, &myHierarchyInsertedGenericDatas);
        }
    }
}


void
GNEDataHandler::myEndElement(int element) {
    // Obtain tag of element
    SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    switch (tag) {
        case SUMO_TAG_TAZ: {
            /*
            GNETAZ* TAZ = dynamic_cast<GNETAZ*>(myHierarchyInsertedDatas.getLastInsertedData());
            if (TAZ != nullptr) {
                if (TAZ->getTAZShape().size() == 0) {
                    Boundary b;
                    if (TAZ->getChildDatas().size() > 0) {
                        for (const auto& i : TAZ->getChildDatas()) {
                            b.add(i->getCenteringBoundary());
                        }
                        PositionVector boundaryShape;
                        boundaryShape.push_back(Position(b.xmin(), b.ymin()));
                        boundaryShape.push_back(Position(b.xmax(), b.ymin()));
                        boundaryShape.push_back(Position(b.xmax(), b.ymax()));
                        boundaryShape.push_back(Position(b.xmin(), b.ymax()));
                        boundaryShape.push_back(Position(b.xmin(), b.ymin()));
                        TAZ->setAttribute(SUMO_ATTR_SHAPE, toString(boundaryShape), myViewNet->getUndoList());
                    }
                }
            }
            */
            break;
        }
        default:
            break;
    }
}


bool
GNEDataHandler::buildData(GNEViewNet* viewNet, bool allowUndoRedo, SumoXMLTag tag, const SUMOSAXAttributes& attrs) {
    // Call parse and build depending of tag
    switch (tag) {
        /*
        case SUMO_TAG_BUS_STOP:
        case SUMO_TAG_TRAIN_STOP:
            return parseAndBuildBusStop(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_ACCESS:
            return parseAndBuildAccess(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_CONTAINER_STOP:
            return parseAndBuildContainerStop(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_CHARGING_STATION:
            return parseAndBuildChargingStation(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_E1DETECTOR:
        case SUMO_TAG_INDUCTION_LOOP:
            return parseAndBuildDetectorE1(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_E2DETECTOR:
        case SUMO_TAG_E2DETECTOR_MULTILANE:
        case SUMO_TAG_LANE_AREA_DETECTOR:
            return parseAndBuildDetectorE2(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_E3DETECTOR:
        case SUMO_TAG_ENTRY_EXIT_DETECTOR:
            return parseAndBuildDetectorE3(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_DET_ENTRY:
            return parseAndBuildDetectorEntry(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_DET_EXIT:
            return parseAndBuildDetectorExit(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_INSTANT_INDUCTION_LOOP:
            return parseAndBuildDetectorE1Instant(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_ROUTEPROBE:
            return parseAndBuildRouteProbe(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_VAPORIZER:
            return parseAndBuildVaporizer(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_TAZ:
            return parseAndBuildTAZ(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_TAZSOURCE:
            return parseAndBuildTAZSource(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_TAZSINK:
            return parseAndBuildTAZSink(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_VSS:
            return parseAndBuildVariableSpeedSign(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_STEP:
            return parseAndBuildVariableSpeedSignStep(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_CALIBRATOR:
        case SUMO_TAG_LANECALIBRATOR:
            return parseAndBuildCalibrator(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_PARKING_AREA:
            return parseAndBuildParkingArea(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_PARKING_SPACE:
            return parseAndBuildParkingSpace(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_FLOW_CALIBRATOR:
            return parseAndBuildCalibratorFlow(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_REROUTER:
            return parseAndBuildRerouter(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_INTERVAL:
            return parseAndBuildRerouterInterval(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_CLOSING_LANE_REROUTE:
            return parseAndBuildRerouterClosingLaneReroute(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_CLOSING_REROUTE:
            return parseAndBuildRerouterClosingReroute(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_DEST_PROB_REROUTE:
            return parseAndBuildRerouterDestProbReroute(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_PARKING_ZONE_REROUTE:
            return parseAndBuildRerouterParkingAreaReroute(viewNet, allowUndoRedo, attrs, insertedDatas);
        case SUMO_TAG_ROUTE_PROB_REROUTE:
            return parseAndBuildRerouterRouteProbReroute(viewNet, allowUndoRedo, attrs, insertedDatas);
        */
        default:
            return false;
    }
}


GNEEdgeData*
GNEDataHandler::buildEdgeData(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNEEdge* edge) {
    /*
    if (viewNet->getNet()->retrieveData(SUMO_TAG_BUS_STOP, id, false) == nullptr) {
        GNEBusStop* busStop = new GNEBusStop(id, lane, viewNet, startPos, endPos, parametersSet, name, lines, personCapacity, friendlyPosition, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_BUS_STOP));
            viewNet->getUndoList()->add(new GNEChange_Data(busStop, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertData(busStop);
            lane->addChildData(busStop);
            busStop->incRef("buildBusStop");
        }
        return busStop;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_BUS_STOP) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
    */
    return nullptr;
}

bool
GNEDataHandler::parseAndBuildEdgeData(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs) {
    /*
    bool abort = false;
    // parse attributes of bus stop
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_BUS_STOP, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_BUS_STOP, SUMO_ATTR_LANE, abort);
    std::string startPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_BUS_STOP, SUMO_ATTR_STARTPOS, abort);
    std::string endPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_BUS_STOP, SUMO_ATTR_ENDPOS, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_BUS_STOP, SUMO_ATTR_NAME, abort);
    std::vector<std::string> lines = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, id, SUMO_TAG_BUS_STOP, SUMO_ATTR_LINES, abort);
    const int personCapacity = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, id, SUMO_TAG_BUS_STOP, SUMO_ATTR_PERSON_CAPACITY, abort);
    bool friendlyPosition = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_BUS_STOP, SUMO_ATTR_FRIENDLY_POS, abort);
    // parse Netedit attributes
    bool blockMovement = false;
    if (attrs.hasAttribute(GNE_ATTR_BLOCK_MOVEMENT)) {
        blockMovement = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_BUS_STOP, GNE_ATTR_BLOCK_MOVEMENT, abort);
    }
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = viewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (viewNet->getNet()->retrieveData(SUMO_TAG_BUS_STOP, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_BUS_STOP) + " with the same ID='" + id + "'.");
        } else if (lane == nullptr) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(SUMO_TAG_BUS_STOP) + " '" + id + "' is not known.");
        } else {
            // declare variables for start and end position
            double startPosDouble = 0;
            double endPosDouble = lane->getParentEdge()->getNBEdge()->getFinalLength();
            const double stoppingPlaceLength = (endPosDouble - startPosDouble);
            int parametersSet = 0;
            // check if startPos and endPos were defined
            if (GNEAttributeCarrier::canParse<double>(startPos)) {
                startPosDouble = GNEAttributeCarrier::parse<double>(startPos);
                parametersSet |= STOPPINGPLACE_STARTPOS_SET;
            }
            if (GNEAttributeCarrier::canParse<double>(endPos)) {
                endPosDouble = GNEAttributeCarrier::parse<double>(endPos);
                parametersSet |= STOPPINGPLACE_ENDPOS_SET;
            }
            // check if stoppingPlace has to be adjusted
            SUMORouteHandler::StopPos checkStopPosResult = SUMORouteHandler::checkStopPos(startPosDouble, endPosDouble, lane->getParentEdge()->getNBEdge()->getFinalLength(), POSITION_EPS, friendlyPosition);
            // update start and end positions depending of checkStopPosResult
            if (checkStopPosResult == SUMORouteHandler::StopPos::STOPPOS_INVALID_STARTPOS) {
                startPosDouble = 0;
                endPosDouble = stoppingPlaceLength;
            } else if (checkStopPosResult == SUMORouteHandler::StopPos::STOPPOS_INVALID_ENDPOS) {
                startPosDouble = lane->getParentEdge()->getNBEdge()->getFinalLength() - stoppingPlaceLength;
                endPosDouble = lane->getParentEdge()->getNBEdge()->getFinalLength();
            } else if (checkStopPosResult == SUMORouteHandler::StopPos::STOPPOS_INVALID_LANELENGTH) {
                // Write error if position isn't valid
                WRITE_WARNING("Invalid position for " + toString(SUMO_TAG_BUS_STOP) + " with ID = '" + id + "'.");
                return false;
            }
            // save ID of last created element
            GNEData* dataCreated = buildBusStop(viewNet, allowUndoRedo, id, lane, startPosDouble, endPosDouble, parametersSet,
                                               name, lines, personCapacity, friendlyPosition, blockMovement);
            // check if insertion has to be commited
            if (insertedDatas) {
                insertedDatas->commitElementInsertion(dataCreated);
            }
            return true;
        }
    }
    */
    return false;
}



// ===========================================================================
// private method definitions
// ===========================================================================

void
GNEDataHandler::parseParameter(const SUMOSAXAttributes& attrs) {
    /*
    // we have two cases: if we're parsing a Shape or we're parsing an Data
    if (getLastParameterised()) {
        bool ok = true;
        std::string key;
        if (attrs.hasAttribute(SUMO_ATTR_KEY)) {
            // obtain key
            key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, ok);
            if (key.empty()) {
                WRITE_WARNING("Error parsing key from shape parameter. Key cannot be empty");
                ok = false;
            }
            if (!SUMOXMLDefinitions::isValidTypeID(key)) {
                WRITE_WARNING("Error parsing key from shape parameter. Key contains invalid characters");
                ok = false;
            }
        } else {
            WRITE_WARNING("Error parsing key from shape parameter. Key doesn't exist");
            ok = false;
        }
        // circumventing empty string test
        const std::string val = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
        if (!SUMOXMLDefinitions::isValidAttribute(val)) {
            WRITE_WARNING("Error parsing value from shape parameter. Value contains invalid characters");
            ok = false;
        }
        // set parameter in last inserted data
        if (ok) {
            WRITE_DEBUG("Inserting parameter '" + key + "|" + val + "' into shape.");
            getLastParameterised()->setParameter(key, val);
        }
    } else if (myHierarchyInsertedDatas.getLastInsertedData()) {
        // first check if given data supports parameters
        if (myHierarchyInsertedDatas.getLastInsertedData()->getTagProperty().hasParameters()) {
            bool ok = true;
            std::string key;
            if (attrs.hasAttribute(SUMO_ATTR_KEY)) {
                // obtain key
                key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, ok);
                if (key.empty()) {
                    WRITE_WARNING("Error parsing key from data parameter. Key cannot be empty");
                    ok = false;
                }
                if (!SUMOXMLDefinitions::isValidTypeID(key)) {
                    WRITE_WARNING("Error parsing key from data parameter. Key contains invalid characters");
                    ok = false;
                }
            } else {
                WRITE_WARNING("Error parsing key from data parameter. Key doesn't exist");
                ok = false;
            }
            // circumventing empty string test
            const std::string val = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
            if (!SUMOXMLDefinitions::isValidAttribute(val)) {
                WRITE_WARNING("Error parsing value from data parameter. Value contains invalid characters");
                ok = false;
            }
            // set parameter in last inserted data
            if (ok) {
                WRITE_DEBUG("Inserting parameter '" + key + "|" + val + "' into data " + myHierarchyInsertedDatas.getLastInsertedData()->getTagStr() + ".");
                myHierarchyInsertedDatas.getLastInsertedData()->setParameter(key, val);
            }
        } else {
            WRITE_WARNING("Datas of type '" + myHierarchyInsertedDatas.getLastInsertedData()->getTagStr() + "' doesn't support parameters");
        }
    } else {
        WRITE_WARNING("Parameters has to be declared within the definition of an data or a shape element");
    }
    */
}

/****************************************************************************/

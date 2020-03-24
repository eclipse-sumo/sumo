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
/// @file    GNEAdditionalHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
// Builds trigger objects for netedit
/****************************************************************************/
#include <config.h>
#include <utils/xml/XMLSubSys.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <utils/options/OptionsCont.h>
#include <utils/vehicle/SUMORouteHandler.h>

#include "GNEAdditionalHandler.h"
#include "GNEBusStop.h"
#include "GNEAccess.h"
#include "GNECalibrator.h"
#include "GNECalibratorFlow.h"
#include "GNEChargingStation.h"
#include "GNEClosingLaneReroute.h"
#include "GNEClosingReroute.h"
#include "GNEContainerStop.h"
#include "GNEDestProbReroute.h"
#include "GNEDetectorE1.h"
#include "GNEDetectorE2.h"
#include "GNEDetectorE3.h"
#include "GNEDetectorEntryExit.h"
#include "GNEDetectorE1Instant.h"
#include "GNEParkingArea.h"
#include "GNEParkingSpace.h"
#include "GNERerouter.h"
#include "GNERerouterInterval.h"
#include "GNERouteProbReroute.h"
#include "GNEParkingAreaReroute.h"
#include "GNERouteProbe.h"
#include "GNEVaporizer.h"
#include "GNEVariableSpeedSign.h"
#include "GNEVariableSpeedSignStep.h"
#include "GNETAZ.h"
#include "GNETAZSourceSink.h"


// ===========================================================================
// GNEAdditionalHandler method definitions
// ===========================================================================

GNEAdditionalHandler::GNEAdditionalHandler(const std::string& file, GNEViewNet* viewNet, GNEAdditional* additionalParent) :
    ShapeHandler(file, *viewNet->getNet()->getAttributeCarriers()),
    myViewNet(viewNet) {
    // check if we're loading values of another additionals (example: Rerouter values)
    if (additionalParent) {
        myHierarchyInsertedAdditionals.insertElement(additionalParent->getTagProperty().getTag());
        myHierarchyInsertedAdditionals.commitElementInsertion(additionalParent);
    }
    // define default values for shapes
    setDefaults("", RGBColor::RED, Shape::DEFAULT_LAYER_POI, true);
}


GNEAdditionalHandler::~GNEAdditionalHandler() {}


void
GNEAdditionalHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // Obtain tag of element
    SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // check if we're parsing a parameter
    if (tag == SUMO_TAG_PARAM) {
        // push element int stack
        myHierarchyInsertedAdditionals.insertElement(tag);
        // parse parameter
        parseParameter(attrs);
    } else if (tag != SUMO_TAG_NOTHING) {
        // push element int stack
        if (tag == SUMO_TAG_TRAIN_STOP) {
            // ensure that access elements can find their parent in myHierarchyInsertedAdditionals
            tag = SUMO_TAG_BUS_STOP;
        }
        myHierarchyInsertedAdditionals.insertElement(tag);
        // Call parse and build depending of tag
        switch (tag) {
            case SUMO_TAG_POLY:
                return parseAndBuildPoly(attrs);
            case SUMO_TAG_POI:
                return parseAndBuildPOI(attrs);
            default:
                // build additional
                buildAdditional(myViewNet, true, tag, attrs, &myHierarchyInsertedAdditionals);
        }
    }
}


void
GNEAdditionalHandler::myEndElement(int element) {
    // Obtain tag of element
    SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    switch (tag) {
        case SUMO_TAG_TAZ: {
            GNETAZ* TAZ = dynamic_cast<GNETAZ*>(myHierarchyInsertedAdditionals.getLastInsertedAdditional());
            if (TAZ != nullptr) {
                if (TAZ->getTAZShape().size() == 0) {
                    Boundary b;
                    if (TAZ->getChildAdditionals().size() > 0) {
                        for (const auto& i : TAZ->getChildAdditionals()) {
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
            break;
        }
        default:
            break;
    }
    // pop last inserted element
    myHierarchyInsertedAdditionals.popElement();
    // execute myEndElement of ShapeHandler (needed to update myLastParameterised)
    ShapeHandler::myEndElement(element);
}


Position
GNEAdditionalHandler::getLanePos(const std::string& poiID, const std::string& laneID, double lanePos, double lanePosLat) {
    std::string edgeID;
    int laneIndex;
    NBHelpers::interpretLaneID(laneID, edgeID, laneIndex);
    NBEdge* edge = myViewNet->getNet()->retrieveEdge(edgeID)->getNBEdge();
    if (edge == nullptr || laneIndex < 0 || edge->getNumLanes() <= laneIndex) {
        WRITE_ERROR("Lane '" + laneID + "' to place poi '" + poiID + "' on is not known.");
        return Position::INVALID;
    }
    if (lanePos < 0) {
        lanePos = edge->getLength() + lanePos;
    }
    if (lanePos < 0 || lanePos > edge->getLength()) {
        WRITE_WARNING("lane position " + toString(lanePos) + " for poi '" + poiID + "' is not valid.");
    }
    return edge->getLanes()[laneIndex].shape.positionAtOffset(lanePos, -lanePosLat);
}


bool
GNEAdditionalHandler::buildAdditional(GNEViewNet* viewNet, bool allowUndoRedo, SumoXMLTag tag, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    // Call parse and build depending of tag
    switch (tag) {
        case SUMO_TAG_BUS_STOP:
        case SUMO_TAG_TRAIN_STOP:
            return parseAndBuildBusStop(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_ACCESS:
            return parseAndBuildAccess(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_CONTAINER_STOP:
            return parseAndBuildContainerStop(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_CHARGING_STATION:
            return parseAndBuildChargingStation(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_E1DETECTOR:
        case SUMO_TAG_INDUCTION_LOOP:
            return parseAndBuildDetectorE1(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_E2DETECTOR:
        case SUMO_TAG_E2DETECTOR_MULTILANE:
        case SUMO_TAG_LANE_AREA_DETECTOR:
            return parseAndBuildDetectorE2(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_E3DETECTOR:
        case SUMO_TAG_ENTRY_EXIT_DETECTOR:
            return parseAndBuildDetectorE3(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_DET_ENTRY:
            return parseAndBuildDetectorEntry(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_DET_EXIT:
            return parseAndBuildDetectorExit(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_INSTANT_INDUCTION_LOOP:
            return parseAndBuildDetectorE1Instant(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_ROUTEPROBE:
            return parseAndBuildRouteProbe(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_VAPORIZER:
            return parseAndBuildVaporizer(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_TAZ:
            return parseAndBuildTAZ(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_TAZSOURCE:
            return parseAndBuildTAZSource(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_TAZSINK:
            return parseAndBuildTAZSink(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_VSS:
            return parseAndBuildVariableSpeedSign(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_STEP:
            return parseAndBuildVariableSpeedSignStep(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_CALIBRATOR:
        case SUMO_TAG_LANECALIBRATOR:
            return parseAndBuildCalibrator(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_PARKING_AREA:
            return parseAndBuildParkingArea(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_PARKING_SPACE:
            return parseAndBuildParkingSpace(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_FLOW_CALIBRATOR:
            return parseAndBuildCalibratorFlow(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_REROUTER:
            return parseAndBuildRerouter(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_INTERVAL:
            return parseAndBuildRerouterInterval(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_CLOSING_LANE_REROUTE:
            return parseAndBuildRerouterClosingLaneReroute(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_CLOSING_REROUTE:
            return parseAndBuildRerouterClosingReroute(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_DEST_PROB_REROUTE:
            return parseAndBuildRerouterDestProbReroute(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_PARKING_ZONE_REROUTE:
            return parseAndBuildRerouterParkingAreaReroute(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        case SUMO_TAG_ROUTE_PROB_REROUTE:
            return parseAndBuildRerouterRouteProbReroute(viewNet, allowUndoRedo, attrs, insertedAdditionals);
        default:
            return false;
    }
}


GNEAdditional*
GNEAdditionalHandler::buildBusStop(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane,
        const double startPos, const double endPos, const int parametersSet,
        const std::string& name, const std::vector<std::string>& lines, int personCapacity, double parkingLength,
        bool friendlyPosition, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, id, false) == nullptr) {
        GNEBusStop* busStop = new GNEBusStop(id, lane, viewNet, startPos, endPos, parametersSet, name, lines, personCapacity, parkingLength, friendlyPosition, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_BUS_STOP));
            viewNet->getUndoList()->add(new GNEChange_Additional(busStop, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(busStop);
            lane->addChildAdditional(busStop);
            busStop->incRef("buildBusStop");
        }
        return busStop;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_BUS_STOP) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


GNEAdditional*
GNEAdditionalHandler::buildAccess(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* busStop, GNELane* lane, double pos, const std::string& length, bool friendlyPos, bool blockMovement) {
    // Check if busStop parent and lane is correct
    if (lane == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_ACCESS) + " in netedit; " +  toString(SUMO_TAG_LANE) + " doesn't exist.");
    } else if (busStop == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_ACCESS) + " in netedit; " +  toString(SUMO_TAG_BUS_STOP) + " parent doesn't exist.");
    } else if (!accessCanBeCreated(busStop, lane->getParentEdge())) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_ACCESS) + " in netedit; " +  toString(SUMO_TAG_BUS_STOP) + " parent already owns a Acces in the edge '" + lane->getParentEdge()->getID() + "'");
    } else {
        GNEAccess* access = new GNEAccess(busStop, lane, viewNet, pos, length, friendlyPos, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_ACCESS));
            viewNet->getUndoList()->add(new GNEChange_Additional(access, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(access);
            lane->addChildAdditional(access);
            busStop->addChildAdditional(access);
            access->incRef("buildAccess");
        }
        return access;
    }
}


GNEAdditional*
GNEAdditionalHandler::buildContainerStop(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, const double startPos, const double endPos, const int parametersSet,
        const std::string& name, const std::vector<std::string>& lines, bool friendlyPosition, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, id, false) == nullptr) {
        GNEContainerStop* containerStop = new GNEContainerStop(id, lane, viewNet, startPos, endPos, parametersSet, name, lines, friendlyPosition, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CONTAINER_STOP));
            viewNet->getUndoList()->add(new GNEChange_Additional(containerStop, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(containerStop);
            lane->addChildAdditional(containerStop);
            containerStop->incRef("buildContainerStop");
        }
        return containerStop;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CONTAINER_STOP) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


GNEAdditional*
GNEAdditionalHandler::buildChargingStation(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, const double startPos, const double endPos, const int parametersSet,
        const std::string& name, double chargingPower, double efficiency, bool chargeInTransit, SUMOTime chargeDelay, bool friendlyPosition, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_CHARGING_STATION, id, false) == nullptr) {
        GNEChargingStation* chargingStation = new GNEChargingStation(id, lane, viewNet, startPos, endPos, parametersSet, name, chargingPower, efficiency, chargeInTransit, chargeDelay, friendlyPosition, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CHARGING_STATION));
            viewNet->getUndoList()->add(new GNEChange_Additional(chargingStation, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(chargingStation);
            lane->addChildAdditional(chargingStation);
            chargingStation->incRef("buildChargingStation");
        }
        return chargingStation;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CHARGING_STATION) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


GNEAdditional*
GNEAdditionalHandler::buildParkingArea(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, const double startPos, const double endPos, const int parametersSet,
                                       const std::string& name, bool friendlyPosition, int roadSideCapacity, bool onRoad, double width, const std::string& length, double angle, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, id, false) == nullptr) {
        GNEParkingArea* parkingArea = new GNEParkingArea(id, lane, viewNet, startPos, endPos, parametersSet, name, friendlyPosition, roadSideCapacity, onRoad, width, length, angle, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_PARKING_AREA));
            viewNet->getUndoList()->add(new GNEChange_Additional(parkingArea, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(parkingArea);
            lane->addChildAdditional(parkingArea);
            parkingArea->incRef("buildParkingArea");
        }
        return parkingArea;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_PARKING_AREA) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


GNEAdditional*
GNEAdditionalHandler::buildParkingSpace(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* parkingAreaParent, Position pos, double width, double length, double angle, bool blockMovement) {
    GNEParkingSpace* parkingSpace = new GNEParkingSpace(viewNet, parkingAreaParent, pos, width, length, angle, blockMovement);
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_PARKING_SPACE));
        viewNet->getUndoList()->add(new GNEChange_Additional(parkingSpace, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        viewNet->getNet()->getAttributeCarriers()->insertAdditional(parkingSpace);
        parkingAreaParent->addChildAdditional(parkingSpace);
        parkingSpace->incRef("buildParkingSpace");
    }
    return parkingSpace;
}


GNEAdditional*
GNEAdditionalHandler::buildDetectorE1(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double pos, SUMOTime freq, const std::string& filename, const std::string& vehicleTypes, const std::string& name, bool friendlyPos, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_E1DETECTOR, id, false) == nullptr) {
        GNEDetectorE1* detectorE1 = new GNEDetectorE1(id, lane, viewNet, pos, freq, filename, vehicleTypes, name, friendlyPos, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E1DETECTOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(detectorE1, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(detectorE1);
            lane->addChildAdditional(detectorE1);
            detectorE1->incRef("buildDetectorE1");
        }
        return detectorE1;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_E1DETECTOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


GNEAdditional*
GNEAdditionalHandler::buildSingleLaneDetectorE2(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double pos, double length, SUMOTime freq, const std::string& filename,
        const std::string& vehicleTypes, const std::string& name, SUMOTime timeThreshold, double speedThreshold, double jamThreshold, bool friendlyPos, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_E2DETECTOR, id, false) == nullptr) {
        GNEDetectorE2* detectorE2 = new GNEDetectorE2(id, lane, viewNet, pos, length, freq, filename, vehicleTypes, name, timeThreshold, speedThreshold, jamThreshold, friendlyPos, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E2DETECTOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(detectorE2, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(detectorE2);
            lane->addChildAdditional(detectorE2);
            detectorE2->incRef("buildDetectorE2");
        }
        return detectorE2;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_E2DETECTOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


GNEAdditional*
GNEAdditionalHandler::buildMultiLaneDetectorE2(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, const std::vector<GNELane*>& lanes, double pos, double endPos, SUMOTime freq, const std::string& filename,
        const std::string& vehicleTypes, const std::string& name, SUMOTime timeThreshold, double speedThreshold, double jamThreshold, bool friendlyPos, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_E2DETECTOR_MULTILANE, id, false) == nullptr) {
        GNEDetectorE2* detectorE2 = new GNEDetectorE2(id, lanes, viewNet, pos, endPos, freq, filename, vehicleTypes, name, timeThreshold, speedThreshold, jamThreshold, friendlyPos, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E2DETECTOR_MULTILANE));
            viewNet->getUndoList()->add(new GNEChange_Additional(detectorE2, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(detectorE2);
            for (auto i : lanes) {
                i->addChildAdditional(detectorE2);
            }
            detectorE2->incRef("buildDetectorE2Multilane");
        }
        // check E2 integrity
        detectorE2->checkE2MultilaneIntegrity();
        return detectorE2;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_E2DETECTOR_MULTILANE) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


GNEAdditional*
GNEAdditionalHandler::buildDetectorE3(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, Position pos, SUMOTime freq, const std::string& filename, const std::string& vehicleTypes,
                                      const std::string& name, SUMOTime timeThreshold, double speedThreshold, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_E3DETECTOR, id, false) == nullptr) {
        GNEDetectorE3* detectorE3 = new GNEDetectorE3(id, viewNet, pos, freq, filename, vehicleTypes, name, timeThreshold, speedThreshold, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E3DETECTOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(detectorE3, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(detectorE3);
            detectorE3->incRef("buildDetectorE3");
        }
        return detectorE3;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_E3DETECTOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


GNEAdditional*
GNEAdditionalHandler::buildDetectorEntry(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* E3Parent, GNELane* lane, double pos, bool friendlyPos, bool blockMovement) {
    // Check if Detector E3 parent and lane is correct
    if (lane == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_LANE) + " doesn't exist.");
    } else if (E3Parent == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_E3DETECTOR) + " parent doesn't exist.");
    } else {
        GNEDetectorEntryExit* entry = new GNEDetectorEntryExit(SUMO_TAG_DET_ENTRY, viewNet, E3Parent, lane, pos, friendlyPos, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_DET_ENTRY));
            viewNet->getUndoList()->add(new GNEChange_Additional(entry, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(entry);
            lane->addChildAdditional(entry);
            E3Parent->addChildAdditional(entry);
            entry->incRef("buildDetectorEntry");
        }
        return entry;
    }
}


GNEAdditional*
GNEAdditionalHandler::buildDetectorExit(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* E3Parent, GNELane* lane, double pos, bool friendlyPos, bool blockMovement) {
    // Check if Detector E3 parent and lane is correct
    if (lane == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_EXIT) + " in netedit; " +  toString(SUMO_TAG_LANE) + " doesn't exist.");
    } else if (E3Parent == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_EXIT) + " in netedit; " +  toString(SUMO_TAG_E3DETECTOR) + " parent doesn't exist.");
    } else {
        GNEDetectorEntryExit* exit = new GNEDetectorEntryExit(SUMO_TAG_DET_EXIT, viewNet, E3Parent, lane, pos, friendlyPos, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_DET_EXIT));
            viewNet->getUndoList()->add(new GNEChange_Additional(exit, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(exit);
            lane->addChildAdditional(exit);
            E3Parent->addChildAdditional(exit);
            exit->incRef("buildDetectorExit");
        }
        return exit;
    }
}


GNEAdditional*
GNEAdditionalHandler::buildDetectorE1Instant(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double pos, const std::string& filename, const std::string& vehicleTypes, const std::string& name, bool friendlyPos, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_INSTANT_INDUCTION_LOOP, id, false) == nullptr) {
        GNEDetectorE1Instant* detectorE1Instant = new GNEDetectorE1Instant(id, lane, viewNet, pos, filename, vehicleTypes, name, friendlyPos, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_INSTANT_INDUCTION_LOOP));
            viewNet->getUndoList()->add(new GNEChange_Additional(detectorE1Instant, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(detectorE1Instant);
            lane->addChildAdditional(detectorE1Instant);
            detectorE1Instant->incRef("buildDetectorE1Instant");
        }
        return detectorE1Instant;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_INSTANT_INDUCTION_LOOP) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


GNEAdditional*
GNEAdditionalHandler::buildCalibrator(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double pos, const std::string& name, const std::string& outfile, const SUMOTime freq, const std::string& routeprobe, bool centerAfterCreation) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_CALIBRATOR, id, false) == nullptr) {
        GNECalibrator* calibrator = new GNECalibrator(id, viewNet, lane, pos, freq, name, outfile, routeprobe);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CALIBRATOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(calibrator, true), true);
            viewNet->getUndoList()->p_end();
            // center after creation
            if (centerAfterCreation) {
                viewNet->centerTo(calibrator->getPositionInView(), false);
            }
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(calibrator);
            lane->addChildAdditional(calibrator);
            calibrator->incRef("buildCalibrator");
        }
        return calibrator;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CALIBRATOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


GNEAdditional*
GNEAdditionalHandler::buildCalibrator(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNEEdge* edge, double pos, const std::string& name, const std::string& outfile, const SUMOTime freq, const std::string& routeprobe, bool centerAfterCreation) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_CALIBRATOR, id, false) == nullptr) {
        GNECalibrator* calibrator = new GNECalibrator(id, viewNet, edge, pos, freq, name, outfile, routeprobe);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CALIBRATOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(calibrator, true), true);
            viewNet->getUndoList()->p_end();
            // center after creation
            if (centerAfterCreation) {
                viewNet->centerTo(calibrator->getPositionInView(), false);
            }
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(calibrator);
            edge->addChildAdditional(calibrator);
            calibrator->incRef("buildCalibrator");
        }
        return calibrator;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CALIBRATOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


GNEAdditional*
GNEAdditionalHandler::buildCalibratorFlow(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* calibratorParent, GNEDemandElement* route, GNEDemandElement* vType,
        const std::string& vehsPerHour, const std::string& speed, const RGBColor& color, const std::string& departLane, const std::string& departPos,
        const std::string& departSpeed, const std::string& arrivalLane, const std::string& arrivalPos, const std::string& arrivalSpeed, const std::string& line,
        int personNumber, int containerNumber, bool reroute, const std::string& departPosLat, const std::string& arrivalPosLat, SUMOTime begin, SUMOTime end) {

    // create Flow and add it to calibrator parent
    GNECalibratorFlow* flow = new GNECalibratorFlow(calibratorParent, vType, route, vehsPerHour, speed, color, departLane, departPos, departSpeed,
            arrivalLane, arrivalPos, arrivalSpeed, line, personNumber, containerNumber, reroute,
            departPosLat, arrivalPosLat, begin, end);
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + flow->getTagStr());
        viewNet->getUndoList()->add(new GNEChange_Additional(flow, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        calibratorParent->addChildAdditional(flow);
        flow->incRef("buildCalibratorFlow");
    }
    return flow;
}


GNEAdditional*
GNEAdditionalHandler::buildRerouter(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, Position pos, const std::vector<GNEEdge*>& edges, double prob, const std::string& name, const std::string& file, bool off, SUMOTime timeThreshold, const std::string& vTypes, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_REROUTER, id, false) == nullptr) {
        GNERerouter* rerouter = new GNERerouter(id, viewNet, pos, edges, name, file, prob, off, timeThreshold, vTypes, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_REROUTER));
            viewNet->getUndoList()->add(new GNEChange_Additional(rerouter, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(rerouter);
            // add this rerouter as parent of all edges
            for (auto i : edges) {
                i->addParentAdditional(rerouter);
            }
            rerouter->incRef("buildRerouter");
        }
        // parse rerouter children
        if (!file.empty()) {
            // we assume that rerouter values files is placed in the same folder as the additional file
            std::string currentAdditionalFilename = FileHelpers::getFilePath(OptionsCont::getOptions().getString("additional-files"));
            // Create additional handler for parse rerouter values
            GNEAdditionalHandler rerouterValuesHandler(currentAdditionalFilename + file, viewNet, rerouter);
            // disable validation for rerouters
            XMLSubSys::setValidation("never", "auto");
            // Run parser
            if (!XMLSubSys::runParser(rerouterValuesHandler, currentAdditionalFilename + file, false)) {
                WRITE_MESSAGE("Loading of " + file + " failed.");
            }
            // enable validation for rerouters
            XMLSubSys::setValidation("auto", "auto");
        }
        return rerouter;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_REROUTER) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


GNEAdditional*
GNEAdditionalHandler::buildRerouterInterval(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* rerouterParent, SUMOTime begin, SUMOTime end) {
    // check if new interval will produce a overlapping
    if (checkOverlappingRerouterIntervals(rerouterParent, begin, end)) {
        // create rerouter interval and add it into rerouter parent
        GNERerouterInterval* rerouterInterval = new GNERerouterInterval(rerouterParent, begin, end);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + rerouterInterval->getTagStr());
            viewNet->getUndoList()->add(new GNEChange_Additional(rerouterInterval, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            rerouterParent->addChildAdditional(rerouterInterval);
            rerouterInterval->incRef("buildRerouterInterval");
        }
        return rerouterInterval;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_INTERVAL) + " with begin '" + toString(begin) + "' and '" + toString(end) + "' in '" + rerouterParent->getID() + "' due overlapping.");
    }
}


GNEAdditional*
GNEAdditionalHandler::buildClosingLaneReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* rerouterIntervalParent, GNELane* closedLane, SVCPermissions permissions) {
    // create closing lane reorute
    GNEClosingLaneReroute* closingLaneReroute = new GNEClosingLaneReroute(rerouterIntervalParent, closedLane, permissions);
    // add it to interval parent depending of allowUndoRedo
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + closingLaneReroute->getTagStr());
        viewNet->getUndoList()->add(new GNEChange_Additional(closingLaneReroute, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        rerouterIntervalParent->addChildAdditional(closingLaneReroute);
        closingLaneReroute->incRef("buildClosingLaneReroute");
    }
    return closingLaneReroute;
}


GNEAdditional*
GNEAdditionalHandler::buildClosingReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* rerouterIntervalParent, GNEEdge* closedEdge, SVCPermissions permissions) {
    // create closing reroute
    GNEClosingReroute* closingReroute = new GNEClosingReroute(rerouterIntervalParent, closedEdge, permissions);
    // add it to interval parent depending of allowUndoRedo
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + closingReroute->getTagStr());
        viewNet->getUndoList()->add(new GNEChange_Additional(closingReroute, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        rerouterIntervalParent->addChildAdditional(closingReroute);
        closingReroute->incRef("buildClosingReroute");
    }
    return closingReroute;
}


GNEAdditional*
GNEAdditionalHandler::builDestProbReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* rerouterIntervalParent, GNEEdge* newEdgeDestination, double probability) {
    // create dest probability reroute
    GNEDestProbReroute* destProbReroute = new GNEDestProbReroute(rerouterIntervalParent, newEdgeDestination, probability);
    // add it to interval parent depending of allowUndoRedo
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + destProbReroute->getTagStr());
        viewNet->getUndoList()->add(new GNEChange_Additional(destProbReroute, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        rerouterIntervalParent->addChildAdditional(destProbReroute);
        destProbReroute->incRef("builDestProbReroute");
    }
    return destProbReroute;
}


GNEAdditional*
GNEAdditionalHandler::builParkingAreaReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* rerouterIntervalParent, GNEAdditional* newParkingArea, double probability, bool visible) {
    // create dest probability reroute
    GNEParkingAreaReroute* parkingAreaReroute = new GNEParkingAreaReroute(rerouterIntervalParent, newParkingArea, probability, visible);
    // add it to interval parent depending of allowUndoRedo
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + parkingAreaReroute->getTagStr());
        viewNet->getUndoList()->add(new GNEChange_Additional(parkingAreaReroute, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        rerouterIntervalParent->addChildAdditional(parkingAreaReroute);
        parkingAreaReroute->incRef("builParkingAreaReroute");
    }
    return parkingAreaReroute;
}


GNEAdditional*
GNEAdditionalHandler::buildRouteProbReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* rerouterIntervalParent, const std::string& newRouteId, double probability) {
    // create rout prob rereoute
    GNERouteProbReroute* routeProbReroute = new GNERouteProbReroute(rerouterIntervalParent, newRouteId, probability);
    // add it to interval parent depending of allowUndoRedo
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + routeProbReroute->getTagStr());
        viewNet->getUndoList()->add(new GNEChange_Additional(routeProbReroute, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        rerouterIntervalParent->addChildAdditional(routeProbReroute);
        routeProbReroute->incRef("buildRouteProbReroute");
    }
    return routeProbReroute;
}


GNEAdditional*
GNEAdditionalHandler::buildRouteProbe(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNEEdge* edge, const std::string& freq, const std::string& name, const std::string& file, SUMOTime begin, bool centerAfterCreation) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_ROUTEPROBE, id, false) == nullptr) {
        GNERouteProbe* routeProbe = new GNERouteProbe(id, viewNet, edge, freq, name, file, begin);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_ROUTEPROBE));
            viewNet->getUndoList()->add(new GNEChange_Additional(routeProbe, true), true);
            viewNet->getUndoList()->p_end();
            // center after creation
            if (centerAfterCreation) {
                viewNet->centerTo(routeProbe->getPositionInView(), false);
            }
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(routeProbe);
            edge->addChildAdditional(routeProbe);
            routeProbe->incRef("buildRouteProbe");
        }
        return routeProbe;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_ROUTEPROBE) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


GNEAdditional*
GNEAdditionalHandler::buildVariableSpeedSign(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, Position pos, const std::vector<GNELane*>& lanes, const std::string& name, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_VSS, id, false) == nullptr) {
        GNEVariableSpeedSign* variableSpeedSign = new GNEVariableSpeedSign(id, viewNet, pos, lanes, name, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_VSS));
            viewNet->getUndoList()->add(new GNEChange_Additional(variableSpeedSign, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(variableSpeedSign);
            // add this VSS as parent of all edges
            for (auto i : lanes) {
                i->addParentAdditional(variableSpeedSign);
            }
            variableSpeedSign->incRef("buildVariableSpeedSign");
        }
        return variableSpeedSign;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_VSS) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


GNEAdditional*
GNEAdditionalHandler::buildVariableSpeedSignStep(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* VSSParent, double time, double speed) {
    // create Variable Speed Sign
    GNEVariableSpeedSignStep* variableSpeedSignStep = new GNEVariableSpeedSignStep(VSSParent, time, speed);
    // add it depending of allow undoRedo
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + variableSpeedSignStep->getTagStr());
        viewNet->getUndoList()->add(new GNEChange_Additional(variableSpeedSignStep, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        VSSParent->addChildAdditional(variableSpeedSignStep);
        variableSpeedSignStep->incRef("buildVariableSpeedSignStep");
    }
    return variableSpeedSignStep;
}


GNEAdditional*
GNEAdditionalHandler::buildVaporizer(GNEViewNet* viewNet, bool allowUndoRedo, GNEEdge* edge, SUMOTime startTime, SUMOTime endTime, const std::string& name, bool centerAfterCreation) {
    GNEVaporizer* vaporizer = new GNEVaporizer(viewNet, edge, startTime, endTime, name);
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_VAPORIZER));
        viewNet->getUndoList()->add(new GNEChange_Additional(vaporizer, true), true);
        viewNet->getUndoList()->p_end();
        // center after creation
        if (centerAfterCreation) {
            viewNet->centerTo(vaporizer->getPositionInView(), false);
        }
    } else {
        viewNet->getNet()->getAttributeCarriers()->insertAdditional(vaporizer);
        edge->addChildAdditional(vaporizer);
        vaporizer->incRef("buildVaporizer");
    }
    return vaporizer;
}


GNEAdditional*
GNEAdditionalHandler::buildTAZ(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, const PositionVector& shape, const RGBColor& color, const std::vector<GNEEdge*>& edges, bool blockMovement) {
    GNETAZ* TAZ = new GNETAZ(id, viewNet, shape, color, blockMovement);
    // disable updating geometry of TAZ children during insertion (because in large nets provokes slowdowns)
    viewNet->getNet()->disableUpdateGeometry();
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_TAZ));
        viewNet->getUndoList()->add(new GNEChange_Additional(TAZ, true), true);
        // create TAZEdges
        for (auto i : edges) {
            // create TAZ Source using GNEChange_Additional
            GNETAZSourceSink* TAZSource = new GNETAZSourceSink(SUMO_TAG_TAZSOURCE, TAZ, i, 1);
            viewNet->getUndoList()->add(new GNEChange_Additional(TAZSource, true), true);
            // create TAZ Sink using GNEChange_Additional
            GNETAZSourceSink* TAZSink = new GNETAZSourceSink(SUMO_TAG_TAZSINK, TAZ, i, 1);
            viewNet->getUndoList()->add(new GNEChange_Additional(TAZSink, true), true);
        }
        viewNet->getUndoList()->p_end();
    } else {
        viewNet->getNet()->getAttributeCarriers()->insertAdditional(TAZ);
        TAZ->incRef("buildTAZ");
        for (auto i : edges) {
            // create TAZ Source
            GNETAZSourceSink* TAZSource = new GNETAZSourceSink(SUMO_TAG_TAZSOURCE, TAZ, i, 1);
            TAZSource->incRef("buildTAZ");
            TAZ->addChildAdditional(TAZSource);
            // create TAZ Sink
            GNETAZSourceSink* TAZSink = new GNETAZSourceSink(SUMO_TAG_TAZSINK, TAZ, i, 1);
            TAZSink->incRef("buildTAZ");
            TAZ->addChildAdditional(TAZSink);
        }
    }
    // enable updating geometry again and update geometry of TAZ
    viewNet->getNet()->enableUpdateGeometry();
    // update TAZ Frame
    TAZ->updateGeometry();
    TAZ->updateParentAdditional();
    return TAZ;
}


GNEAdditional*
GNEAdditionalHandler::buildTAZSource(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* TAZ, GNEEdge* edge, double departWeight) {
    GNEAdditional* TAZSink = nullptr;
    // first check if a TAZSink in the same edge for the same TAZ
    for (auto i : TAZ->getChildAdditionals()) {
        if ((i->getTagProperty().getTag() == SUMO_TAG_TAZSINK) && (i->getAttribute(SUMO_ATTR_EDGE) == edge->getID())) {
            TAZSink = i;
        }
    }
    // check if TAZSink has to be created
    if (TAZSink == nullptr) {
        // Create TAZ with weight 0 (default)
        TAZSink = new GNETAZSourceSink(SUMO_TAG_TAZSINK, TAZ, edge, 1);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_TAZSINK));
            viewNet->getUndoList()->add(new GNEChange_Additional(TAZSink, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(TAZSink);
            TAZSink->incRef("buildTAZSource");
        }
    }
    // now check check if TAZSource exist
    GNEAdditional* TAZSource = nullptr;
    // first check if a TAZSink in the same edge for the same TAZ
    for (auto i : TAZ->getChildAdditionals()) {
        if ((i->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) && (i->getAttribute(SUMO_ATTR_EDGE) == edge->getID())) {
            TAZSource = i;
        }
    }
    // check if TAZSource has to be created
    if (TAZSource == nullptr) {
        // Create TAZ only with departWeight
        TAZSource = new GNETAZSourceSink(SUMO_TAG_TAZSOURCE, TAZ, edge, departWeight);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_TAZSOURCE));
            viewNet->getUndoList()->add(new GNEChange_Additional(TAZSource, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(TAZSource);
            TAZSource->incRef("buildTAZSource");
        }
    } else {
        // update TAZ Attribute
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("update " + toString(SUMO_TAG_TAZSOURCE));
            TAZSource->setAttribute(SUMO_ATTR_WEIGHT, toString(departWeight), viewNet->getUndoList());
            viewNet->getUndoList()->p_end();
        } else {
            TAZSource->setAttribute(SUMO_ATTR_WEIGHT, toString(departWeight), nullptr);
            TAZSource->incRef("buildTAZSource");
        }
    }
    return TAZSource;
}


GNEAdditional*
GNEAdditionalHandler::buildTAZSink(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* TAZ, GNEEdge* edge, double arrivalWeight) {
    GNEAdditional* TAZSource = nullptr;
    // first check if a TAZSink in the same edge for the same TAZ
    for (auto i : TAZ->getChildAdditionals()) {
        if ((i->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) && (i->getAttribute(SUMO_ATTR_EDGE) == edge->getID())) {
            TAZSource = i;
        }
    }
    // check if TAZSource has to be created
    if (TAZSource == nullptr) {
        // Create TAZ with empty value
        TAZSource = new GNETAZSourceSink(SUMO_TAG_TAZSOURCE, TAZ, edge, 1);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_TAZSOURCE));
            viewNet->getUndoList()->add(new GNEChange_Additional(TAZSource, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(TAZSource);
            TAZSource->incRef("buildTAZSink");
        }
    }
    GNEAdditional* TAZSink = nullptr;
    // first check if a TAZSink in the same edge for the same TAZ
    for (auto i : TAZ->getChildAdditionals()) {
        if ((i->getTagProperty().getTag() == SUMO_TAG_TAZSINK) && (i->getAttribute(SUMO_ATTR_EDGE) == edge->getID())) {
            TAZSink = i;
        }
    }
    // check if TAZSink has to be created
    if (TAZSink == nullptr) {
        // Create TAZ only with arrivalWeight
        TAZSink = new GNETAZSourceSink(SUMO_TAG_TAZSINK, TAZ, edge, arrivalWeight);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_TAZSINK));
            viewNet->getUndoList()->add(new GNEChange_Additional(TAZSink, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->getAttributeCarriers()->insertAdditional(TAZSink);
            TAZSink->incRef("buildTAZSink");
        }
    } else {
        // update TAZ Attribute
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("update " + toString(SUMO_TAG_TAZSINK));
            TAZSink->setAttribute(SUMO_ATTR_WEIGHT, toString(arrivalWeight), viewNet->getUndoList());
            viewNet->getUndoList()->p_end();
        } else {
            TAZSink->setAttribute(SUMO_ATTR_WEIGHT, toString(arrivalWeight), nullptr);
            TAZSink->incRef("buildTAZSink");
        }
    }
    return TAZSink;
}


double
GNEAdditionalHandler::getPosition(double pos, GNELane& lane, bool friendlyPos, const std::string& additionalID) {
    if (pos < 0) {
        pos = lane.getLaneShapeLength() + pos;
    }
    if (pos > lane.getLaneShapeLength()) {
        if (friendlyPos) {
            pos = lane.getLaneShapeLength() - (double) 0.1;
        } else {
            WRITE_WARNING("The position of additional '" + additionalID + "' lies beyond the lane's '" + lane.getID() + "' length.");
        }
    }
    return pos;
}


bool GNEAdditionalHandler::checkAndFixDetectorPosition(double& pos, const double laneLength, const bool friendlyPos) {
    if (fabs(pos) > laneLength) {
        if (!friendlyPos) {
            return false;
        } else if (pos < 0) {
            pos = 0;
        } else if (pos > laneLength) {
            pos = laneLength - 0.01;
        }
    }
    return true;
}


bool GNEAdditionalHandler::fixE2DetectorPosition(double& pos, double& length, const double laneLength, const bool friendlyPos) {
    if ((pos < 0) || ((pos + length) > laneLength)) {
        if (!friendlyPos) {
            return false;
        } else if (pos < 0) {
            pos = 0;
        } else if (pos > laneLength) {
            pos = laneLength - 0.01;
            length = 0;
        } else if ((pos + length) > laneLength) {
            length = laneLength - pos - 0.01;
        }
    }
    return true;
}


bool
GNEAdditionalHandler::accessCanBeCreated(GNEAdditional* busStopParent, GNEEdge* edge) {
    // check that busStopParent is a busStop
    assert(busStopParent->getTagProperty().getTag() == SUMO_TAG_BUS_STOP);
    // check if exist another acces for the same busStop in the given edge
    for (auto i : busStopParent->getChildAdditionals()) {
        for (auto j : edge->getLanes()) {
            if (i->getAttribute(SUMO_ATTR_LANE) == j->getID()) {
                return false;
            }
        }
    }
    return true;
}


bool
GNEAdditionalHandler::checkOverlappingRerouterIntervals(GNEAdditional* rerouter, SUMOTime newBegin, SUMOTime newEnd) {
    // check that rerouter is correct
    assert(rerouter->getTagProperty().getTag() == SUMO_TAG_REROUTER);
    // declare a vector to keep sorted rerouter children
    std::vector<std::pair<SUMOTime, SUMOTime>> sortedIntervals;
    // iterate over child additional
    for (auto i : rerouter->getChildAdditionals()) {
        sortedIntervals.push_back(std::make_pair((SUMOTime)0., (SUMOTime)0.));
        // set begin and end
        sortedIntervals.back().first = TIME2STEPS(i->getAttributeDouble(SUMO_ATTR_BEGIN));
        sortedIntervals.back().second = TIME2STEPS(i->getAttributeDouble(SUMO_ATTR_END));
    }
    // add new intervals
    sortedIntervals.push_back(std::make_pair(newBegin, newEnd));
    // sort children
    std::sort(sortedIntervals.begin(), sortedIntervals.end());
    // check overlapping after sorting
    for (int i = 0; i < (int)sortedIntervals.size() - 1; i++) {
        if (sortedIntervals.at(i).second > sortedIntervals.at(i + 1).first) {
            return false;
        }
    }
    return true;
}




bool
GNEAdditionalHandler::parseAndBuildVaporizer(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of Vaporizer
    const std::string edgeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_VAPORIZER, SUMO_ATTR_ID, abort);
    SUMOTime begin = GNEAttributeCarrier::parseAttributeFromXML<SUMOTime>(attrs, "", SUMO_TAG_VAPORIZER, SUMO_ATTR_BEGIN, abort);
    SUMOTime end = GNEAttributeCarrier::parseAttributeFromXML<SUMOTime>(attrs, "", SUMO_TAG_VAPORIZER, SUMO_ATTR_END, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_VAPORIZER, SUMO_ATTR_NAME, abort);
    // extra check for center element after creation
    bool centerAfterCreation = attrs.hasAttribute(GNE_ATTR_CENTER_AFTER_CREATION);
    // Continue if all parameters were successfully loaded
    if (!abort) {
        // get GNEEdge
        GNEEdge* edge = viewNet->getNet()->retrieveEdge(edgeID, false);
        // check that all parameters are valid
        if (edge == nullptr) {
            WRITE_WARNING("The edge '" + edgeID + "' to use within the " + toString(SUMO_TAG_VAPORIZER) + " is not known.");
        } else if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_VAPORIZER, edgeID, false) != nullptr) {
            WRITE_WARNING("There is already a " + toString(SUMO_TAG_VAPORIZER) + " in the edge '" + edgeID + "'.");
        } else if (begin > end) {
            WRITE_WARNING("Time interval of " + toString(SUMO_TAG_VAPORIZER) + " isn't valid. Attribute '" + toString(SUMO_ATTR_BEGIN) + "' is greater than attribute '" + toString(SUMO_ATTR_END) + "'.");
        } else {
            // build vaporizer
            GNEAdditional* additionalCreated = buildVaporizer(viewNet, allowUndoRedo, edge, begin, end, name, centerAfterCreation);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildTAZ(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of Vaporizer
    const std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_TAZ, SUMO_ATTR_ID, abort);
    const PositionVector shape = GNEAttributeCarrier::parseAttributeFromXML<PositionVector>(attrs, id, SUMO_TAG_TAZ, SUMO_ATTR_SHAPE, abort);
    RGBColor color = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, id, SUMO_TAG_TAZ, SUMO_ATTR_COLOR, abort);
    // parse Netedit attributes
    bool blockMovement = false;
    if (attrs.hasAttribute(GNE_ATTR_BLOCK_MOVEMENT)) {
        blockMovement = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_TAZ, GNE_ATTR_BLOCK_MOVEMENT, abort);
    }
    // check edges
    std::vector<std::string> edgeIDs;
    if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        std::string parsedAttribute = attrs.get<std::string>(SUMO_ATTR_EDGES, id.c_str(), abort, false);
        edgeIDs = GNEAttributeCarrier::parse<std::vector<std::string> >(parsedAttribute);
    }
    // check if all edge IDs are valid
    std::vector<GNEEdge*> edges;
    for (auto i : edgeIDs) {
        GNEEdge* edge = viewNet->getNet()->retrieveEdge(i, false);
        if (edge == nullptr) {
            WRITE_WARNING("Invalid " + toString(SUMO_TAG_EDGE) + " with ID = '" + i + "' within taz '" + id + "'.");
            abort = true;
        } else {
            edges.push_back(edge);
        }
    }
    // Continue if all parameters were successfully loaded
    if (!abort) {
        // check that all parameters are valid
        if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_TAZ, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_TAZ) + " with the same ID='" + id + "'.");
        } else {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildTAZ(viewNet, allowUndoRedo, id, shape, color, edges, blockMovement);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildTAZSource(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of Vaporizer
    const std::string edgeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_TAZSOURCE, SUMO_ATTR_ID, abort);
    const double departWeight = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, edgeID, SUMO_TAG_TAZSOURCE, SUMO_ATTR_WEIGHT, abort);
    // Continue if all parameters were successfully loaded
    if (!abort) {
        // get edge and TAZ
        GNEEdge* edge = viewNet->getNet()->retrieveEdge(edgeID, false);
        GNEAdditional* TAZ = nullptr;
        // obtain parent depending if we're loading or creating it using GNEAdditionalFrame
        if (insertedAdditionals) {
            TAZ = insertedAdditionals->retrieveParentAdditional(viewNet, SUMO_TAG_TAZ);
        } else {
            bool ok = true;
            TAZ = viewNet->getNet()->retrieveAdditional(SUMO_TAG_TAZ, attrs.get<std::string>(GNE_ATTR_PARENT, "", ok));
        }
        // check that all parameters are valid
        if (edge == nullptr) {
            WRITE_WARNING("The edge '" + edgeID + "' to use within the " + toString(SUMO_TAG_TAZSOURCE) + " is not known.");
        } else if (TAZ == nullptr) {
            WRITE_WARNING("A " + toString(SUMO_TAG_TAZSOURCE) + " must be declared within the definition of a " + toString(SUMO_TAG_TAZ) + ".");
        } else {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildTAZSource(viewNet, allowUndoRedo, TAZ, edge, departWeight);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildTAZSink(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of Vaporizer
    const std::string edgeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_TAZSINK, SUMO_ATTR_ID, abort);
    const double arrivalWeight = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, edgeID, SUMO_TAG_TAZSINK, SUMO_ATTR_WEIGHT, abort);
    // Continue if all parameters were successfully loaded
    if (!abort) {
        // get edge and TAZ
        GNEEdge* edge = viewNet->getNet()->retrieveEdge(edgeID, false);
        GNEAdditional* TAZ = nullptr;
        // obtain parent depending if we're loading or creating it using GNEAdditionalFrame
        if (insertedAdditionals) {
            TAZ = insertedAdditionals->retrieveParentAdditional(viewNet, SUMO_TAG_TAZ);
        } else {
            bool ok = true;
            TAZ = viewNet->getNet()->retrieveAdditional(SUMO_TAG_TAZ, attrs.get<std::string>(GNE_ATTR_PARENT, "", ok));
        }
        // check that all parameters are valid
        if (edge == nullptr) {
            WRITE_WARNING("The edge '" + edgeID + "' to use within the " + toString(SUMO_TAG_TAZSINK) + " is not known.");
        } else if (TAZ == nullptr) {
            WRITE_WARNING("A " + toString(SUMO_TAG_TAZSINK) + " must be declared within the definition of a " + toString(SUMO_TAG_TAZ) + ".");
        } else {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildTAZSink(viewNet, allowUndoRedo, TAZ, edge, arrivalWeight);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildRouteProbe(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of RouteProbe
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_ROUTEPROBE, SUMO_ATTR_ID, abort);
    std::string edgeId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_ROUTEPROBE, SUMO_ATTR_EDGE, abort);
    std::string freq = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_ROUTEPROBE, SUMO_ATTR_FREQUENCY, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_ROUTEPROBE, SUMO_ATTR_NAME, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_ROUTEPROBE, SUMO_ATTR_FILE, abort);
    SUMOTime begin = GNEAttributeCarrier::parseAttributeFromXML<SUMOTime>(attrs, id, SUMO_TAG_ROUTEPROBE, SUMO_ATTR_BEGIN, abort);
    // extra check for center element after creation
    bool centerAfterCreation = attrs.hasAttribute(GNE_ATTR_CENTER_AFTER_CREATION);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get edge
        GNEEdge* edge = viewNet->getNet()->retrieveEdge(edgeId, false);
        // check that all elements are valid
        if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_ROUTEPROBE, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_ROUTEPROBE) + " with the same ID='" + id + "'.");
        } else if (edge == nullptr) {
            // Write error if lane isn't valid
            WRITE_WARNING("The edge '" + edgeId + "' to use within the " + toString(SUMO_TAG_ROUTEPROBE) + " '" + id + "' is not known.");
        } else {
            // Freq needs an extra check, because it can be empty
            if (GNEAttributeCarrier::canParse<double>(freq)) {
                if (GNEAttributeCarrier::parse<double>(freq) < 0) {
                    WRITE_WARNING(toString(SUMO_ATTR_FREQUENCY) + "of " + toString(SUMO_TAG_ROUTEPROBE) + "'" + id + "' cannot be negative.");
                    freq = "";
                }
            } else {
                if (freq.empty()) {
                    WRITE_WARNING(toString(SUMO_ATTR_FREQUENCY) + "of " + toString(SUMO_TAG_ROUTEPROBE) + "'" + id + "' cannot be parsed to float.");
                }
                freq = "";
            }
            // save ID of last created element
            GNEAdditional* additionalCreated = buildRouteProbe(viewNet, allowUndoRedo, id, edge, freq, name, file, begin, centerAfterCreation);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildCalibratorFlow(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of calibrator flows
    std::string vehicleTypeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_TYPE, abort);
    std::string routeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_ROUTE, abort);
    std::string vehsPerHour = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_VEHSPERHOUR, abort);
    std::string speed = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_SPEED, abort);
    RGBColor color = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_COLOR, abort);
    std::string departLane = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_DEPARTLANE, abort);
    std::string departPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_DEPARTPOS, abort);
    std::string departSpeed = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_DEPARTSPEED, abort);
    std::string arrivalLane = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_ARRIVALLANE, abort);
    std::string arrivalPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_ARRIVALPOS, abort);
    std::string arrivalSpeed = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_ARRIVALSPEED, abort);
    std::string line = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_LINE, abort);
    int personNumber = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_PERSON_NUMBER, abort);
    int containerNumber = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_CONTAINER_NUMBER, abort);
    bool reroute = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_REROUTE, abort);
    std::string departPosLat = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_DEPARTPOS_LAT, abort);
    std::string arrivalPosLat = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_ARRIVALPOS_LAT, abort);
    SUMOTime begin = GNEAttributeCarrier::parseAttributeFromXML<SUMOTime>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_BEGIN, abort);
    SUMOTime end = GNEAttributeCarrier::parseAttributeFromXML<SUMOTime>(attrs, "", SUMO_TAG_FLOW_CALIBRATOR, SUMO_ATTR_END, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain route, vehicle type and calibrator parent
        GNEDemandElement* route = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, routeID, false);
        GNEDemandElement* vtype = viewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, vehicleTypeID, false);
        GNEAdditional* calibrator = nullptr;
        // obtain parent depending if we're loading or creating it using GNEAdditionalFrame
        if (insertedAdditionals) {
            calibrator = insertedAdditionals->retrieveParentAdditional(viewNet, SUMO_TAG_CALIBRATOR);
        } else {
            bool ok = true;
            calibrator = viewNet->getNet()->retrieveAdditional(SUMO_TAG_CALIBRATOR, attrs.get<std::string>(GNE_ATTR_PARENT, "", ok));
        }
        // check that all elements are valid
        if (route == nullptr) {
            WRITE_WARNING(toString(SUMO_TAG_FLOW_CALIBRATOR) + " cannot be created; their " + toString(SUMO_TAG_ROUTE) + " with ID = '" + routeID + "' doesn't exist");
            abort = true;
        } else if (vtype == nullptr) {
            WRITE_WARNING(toString(SUMO_TAG_FLOW_CALIBRATOR) + " cannot be created; their " + toString(SUMO_TAG_VTYPE) + " with ID = '" + vehicleTypeID + "' doesn't exist");
            abort = true;
        } else if ((vehsPerHour.empty()) && (speed.empty())) {
            WRITE_WARNING(toString(SUMO_TAG_FLOW_CALIBRATOR) + " cannot be created; At least parameters " + toString(SUMO_ATTR_VEHSPERHOUR) + " or " + toString(SUMO_ATTR_SPEED) + " has to be defined");
            abort = true;
        } else if (calibrator != nullptr) {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildCalibratorFlow(viewNet, allowUndoRedo, calibrator, route, vtype, vehsPerHour, speed, color, departLane, departPos, departSpeed, arrivalLane, arrivalPos, arrivalSpeed,
                                               line, personNumber, containerNumber, reroute, departPosLat, arrivalPosLat, begin, end);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


void
GNEAdditionalHandler::parseAndBuildPoly(const SUMOSAXAttributes& attrs) {
    bool abort = false;
    // parse attributes of polygons
    std::string polygonID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_POLY, SUMO_ATTR_ID, abort);
    PositionVector shape = GNEAttributeCarrier::parseAttributeFromXML<PositionVector>(attrs, polygonID, SUMO_TAG_POLY, SUMO_ATTR_SHAPE, abort);
    double layer = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, polygonID, SUMO_TAG_POLY, SUMO_ATTR_LAYER, abort);
    bool fill = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, "", SUMO_TAG_POLY, SUMO_ATTR_FILL, abort);
    double lineWidth = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, polygonID, SUMO_TAG_POLY, SUMO_ATTR_LINEWIDTH, abort);
    std::string type = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, polygonID, SUMO_TAG_POLY, SUMO_ATTR_TYPE, abort);
    RGBColor color = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, polygonID, SUMO_TAG_POLY, SUMO_ATTR_COLOR, abort);
    double angle = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, polygonID, SUMO_TAG_POLY, SUMO_ATTR_ANGLE, abort);
    std::string imgFile = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, polygonID, SUMO_TAG_POLY, SUMO_ATTR_IMGFILE, abort);
    bool relativePath = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, polygonID, SUMO_TAG_POLY, SUMO_ATTR_RELATIVEPATH, abort);
    // check if ID is valid
    if (SUMOXMLDefinitions::isValidTypeID(polygonID) == false) {
        WRITE_WARNING("Invalid characters for polygon ID");
        abort = true;
    }
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // check if shape must be loaded as geo attribute
        bool geo = false;
        const GeoConvHelper* gch = myGeoConvHelper != nullptr ? myGeoConvHelper : &GeoConvHelper::getFinal();
        if (attrs.getOpt<bool>(SUMO_ATTR_GEO, polygonID.c_str(), abort, false)) {
            geo = true;
            bool success = true;
            for (int i = 0; i < (int)shape.size(); i++) {
                success &= gch->x2cartesian_const(shape[i]);
            }
            if (!success) {
                WRITE_WARNING("Unable to project coordinates for polygon '" + polygonID + "'.");
                return;
            }
        }
        // check if img file is absolute
        if (imgFile != "" && !FileHelpers::isAbsolute(imgFile)) {
            imgFile = FileHelpers::getConfigurationRelative(getFileName(), imgFile);
        }
        // create polygon, or show an error if polygon already exists
        if (!myViewNet->getNet()->getAttributeCarriers()->addPolygon(polygonID, type, color, layer, angle, imgFile, relativePath, shape, geo, fill, lineWidth, false)) {
            WRITE_WARNING("Polygon with ID '" + polygonID + "' already exists.");
        } else {
            // update myLastParameterised with the last inserted Polygon
            myLastParameterised = myViewNet->getNet()->getAttributeCarriers()->getPolygons().get(polygonID);
        }
    }
}


bool
GNEAdditionalHandler::parseAndBuildVariableSpeedSign(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of VSS
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_VSS, SUMO_ATTR_ID, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_VSS, SUMO_ATTR_NAME, abort);
    GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_VSS, SUMO_ATTR_FILE, abort); // deprecated
    std::string lanesIDs = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_VSS, SUMO_ATTR_LANES, abort);
    Position pos = GNEAttributeCarrier::parseAttributeFromXML<Position>(attrs, id, SUMO_TAG_VSS, SUMO_ATTR_POSITION, abort);
    // parse Netedit attributes
    bool blockMovement = false;
    if (attrs.hasAttribute(GNE_ATTR_BLOCK_MOVEMENT)) {
        blockMovement = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_VSS, GNE_ATTR_BLOCK_MOVEMENT, abort);
    }
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain lanes
        std::vector<GNELane*> lanes;
        if (GNEAttributeCarrier::canParse<std::vector<GNELane*> >(viewNet->getNet(), lanesIDs, true)) {
            lanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(viewNet->getNet(), lanesIDs);
        }
        // check that all elements are valid
        if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_VSS, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_VSS) + " with the same ID='" + id + "'.");
        } else if (lanes.size() == 0) {
            WRITE_WARNING("A Variable Speed Sign needs at least one lane.");
        } else {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildVariableSpeedSign(viewNet, allowUndoRedo, id, pos, lanes, name, blockMovement);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildVariableSpeedSignStep(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // Load step values
    double time = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", SUMO_TAG_STEP, SUMO_ATTR_TIME, abort);
    double speed = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", SUMO_TAG_STEP, SUMO_ATTR_SPEED, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get Variable Speed Signal
        GNEAdditional* variableSpeedSign = nullptr;
        // obtain parent depending if we're loading or creating it using GNEAdditionalFrame
        if (insertedAdditionals) {
            variableSpeedSign = insertedAdditionals->retrieveParentAdditional(viewNet, SUMO_TAG_VSS);
        } else {
            bool ok = true;
            variableSpeedSign = viewNet->getNet()->retrieveAdditional(SUMO_TAG_VSS, attrs.get<std::string>(GNE_ATTR_PARENT, "", ok));
        }
        // check that all parameters are valid
        if (variableSpeedSign != nullptr) {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildVariableSpeedSignStep(viewNet, allowUndoRedo, variableSpeedSign, time, speed);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildRerouter(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_REROUTER, SUMO_ATTR_ID, abort);
    std::string edgesIDs = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_REROUTER, SUMO_ATTR_EDGES, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_REROUTER, SUMO_ATTR_NAME, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_REROUTER, SUMO_ATTR_FILE, abort);
    double probability = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, SUMO_TAG_REROUTER, SUMO_ATTR_PROB, abort);
    bool off = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_REROUTER, SUMO_ATTR_OFF, abort);
    SUMOTime timeThreshold = attrs.getOptSUMOTimeReporting(SUMO_ATTR_HALTING_TIME_THRESHOLD, id.c_str(), abort, 0);
    const std::string vTypes = attrs.getOpt<std::string>(SUMO_ATTR_VTYPES, id.c_str(), abort, "");
    Position pos = GNEAttributeCarrier::parseAttributeFromXML<Position>(attrs, id, SUMO_TAG_REROUTER, SUMO_ATTR_POSITION, abort);
    // parse Netedit attributes
    bool blockMovement = false;
    if (attrs.hasAttribute(GNE_ATTR_BLOCK_MOVEMENT)) {
        blockMovement = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_REROUTER, GNE_ATTR_BLOCK_MOVEMENT, abort);
    }
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain edges
        std::vector<GNEEdge*> edges;
        if (GNEAttributeCarrier::canParse<std::vector<GNEEdge*> >(viewNet->getNet(), edgesIDs, true)) {
            edges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(viewNet->getNet(), edgesIDs);
        }
        // check that all elements are valid
        if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_REROUTER, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_REROUTER) + " with the same ID='" + id + "'.");
        } else if (edges.size() == 0) {
            WRITE_WARNING("A rerouter needs at least one Edge");
        } else {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildRerouter(viewNet, allowUndoRedo, id, pos, edges, probability, name,
                                               file, off, timeThreshold, vTypes, blockMovement);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildRerouterInterval(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of Rerouter
    SUMOTime begin = GNEAttributeCarrier::parseAttributeFromXML<SUMOTime>(attrs, "", SUMO_TAG_INTERVAL, SUMO_ATTR_BEGIN, abort);
    SUMOTime end = GNEAttributeCarrier::parseAttributeFromXML<SUMOTime>(attrs, "", SUMO_TAG_INTERVAL, SUMO_ATTR_END, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain rerouter
        GNEAdditional* rerouter;
        // obtain parent depending if we're loading or creating it using GNEAdditionalFrame
        if (insertedAdditionals) {
            rerouter = insertedAdditionals->retrieveParentAdditional(viewNet, SUMO_TAG_REROUTER);
        } else {
            bool ok = true;
            rerouter = viewNet->getNet()->retrieveAdditional(SUMO_TAG_REROUTER, attrs.get<std::string>(GNE_ATTR_PARENT, "", ok));
        }
        // special case for load multiple intervals in the same rerouter
        if (rerouter == nullptr) {
            GNEAdditional* lastInsertedRerouterInterval = nullptr;
            // obtain parent depending if we're loading or creating it using GNEAdditionalFrame
            if (insertedAdditionals) {
                lastInsertedRerouterInterval = insertedAdditionals->retrieveParentAdditional(viewNet, SUMO_TAG_INTERVAL);
            } else {
                bool ok = true;
                lastInsertedRerouterInterval = viewNet->getNet()->retrieveAdditional(SUMO_TAG_INTERVAL, attrs.get<std::string>(GNE_ATTR_PARENT, "", ok));
            }
            if (lastInsertedRerouterInterval) {
                rerouter = lastInsertedRerouterInterval->getParentAdditionals().at(0);
            }
        }
        // check that rerouterInterval can be created
        if (begin >= end) {
            WRITE_WARNING(toString(SUMO_TAG_INTERVAL) + " cannot be created; Attribute " + toString(SUMO_ATTR_END) + " must be greather than " + toString(SUMO_ATTR_BEGIN) + ".");
        } else if (rerouter != nullptr) {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildRerouterInterval(viewNet, allowUndoRedo, rerouter, begin, end);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildRerouterClosingLaneReroute(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string laneID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_CLOSING_LANE_REROUTE, SUMO_ATTR_ID, abort);
    std::string allow = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_CLOSING_LANE_REROUTE, SUMO_ATTR_ALLOW, abort);
    std::string disallow = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_CLOSING_LANE_REROUTE, SUMO_ATTR_DISALLOW, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain lane and rerouter interval
        GNELane* lane = viewNet->getNet()->retrieveLane(laneID, false, true);
        GNEAdditional* rerouterInterval = nullptr;
        // obtain parent depending if we're loading or creating it using GNEAdditionalFrame
        if (insertedAdditionals) {
            rerouterInterval = insertedAdditionals->retrieveParentAdditional(viewNet, SUMO_TAG_INTERVAL);
        } else {
            bool ok = true;
            rerouterInterval = viewNet->getNet()->retrieveAdditional(SUMO_TAG_INTERVAL, attrs.get<std::string>(GNE_ATTR_PARENT, "", ok));
        }
        // check that all elements are valid
        if (lane == nullptr) {
            WRITE_WARNING("The lane '" + laneID + "' to use within the " + toString(SUMO_TAG_CLOSING_LANE_REROUTE) + " is not known.");
        } else if (rerouterInterval != nullptr) {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildClosingLaneReroute(viewNet, allowUndoRedo, rerouterInterval, lane, parseVehicleClasses(allow, disallow));
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildRerouterClosingReroute(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string edgeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_CLOSING_REROUTE, SUMO_ATTR_ID, abort);
    std::string allow = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_CLOSING_REROUTE, SUMO_ATTR_ALLOW, abort);
    std::string disallow = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_CLOSING_REROUTE, SUMO_ATTR_DISALLOW, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain edge and rerouter interval
        GNEEdge* edge = viewNet->getNet()->retrieveEdge(edgeID, false);
        GNEAdditional* rerouterInterval = nullptr;
        // obtain parent depending if we're loading or creating it using GNEAdditionalFrame
        if (insertedAdditionals) {
            rerouterInterval = insertedAdditionals->retrieveParentAdditional(viewNet, SUMO_TAG_INTERVAL);
        } else {
            bool ok = true;
            rerouterInterval = viewNet->getNet()->retrieveAdditional(SUMO_TAG_INTERVAL, attrs.get<std::string>(GNE_ATTR_PARENT, "", ok));
        }
        // check that all elements are valid
        if (edge == nullptr) {
            WRITE_WARNING("The edge '" + edgeID + "' to use within the " + toString(SUMO_TAG_CLOSING_REROUTE) + " is not known.");
        } else if (rerouterInterval != nullptr) {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildClosingReroute(viewNet, allowUndoRedo, rerouterInterval, edge, parseVehicleClasses(allow, disallow));
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildRerouterDestProbReroute(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string edgeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_DEST_PROB_REROUTE, SUMO_ATTR_ID, abort);
    double probability = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", SUMO_TAG_DEST_PROB_REROUTE, SUMO_ATTR_PROB, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain edge and rerouter interval
        GNEEdge* edge = viewNet->getNet()->retrieveEdge(edgeID, false);
        GNEAdditional* rerouterInterval = nullptr;
        // obtain parent depending if we're loading or creating it using GNEAdditionalFrame
        if (insertedAdditionals) {
            rerouterInterval = insertedAdditionals->retrieveParentAdditional(viewNet, SUMO_TAG_INTERVAL);
        } else {
            bool ok = true;
            rerouterInterval = viewNet->getNet()->retrieveAdditional(SUMO_TAG_INTERVAL, attrs.get<std::string>(GNE_ATTR_PARENT, "", ok));
        }
        // check that all elements are valid
        if (edge == nullptr) {
            WRITE_WARNING("The edge '" + edgeID + "' to use within the " + toString(SUMO_TAG_DEST_PROB_REROUTE) + " is not known.");
        } else if (rerouterInterval != nullptr) {
            // save ID of last created element
            GNEAdditional* additionalCreated = builDestProbReroute(viewNet, allowUndoRedo, rerouterInterval, edge, probability);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildRerouterParkingAreaReroute(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string parkingAreaID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_PARKING_ZONE_REROUTE, SUMO_ATTR_ID, abort);
    double probability = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", SUMO_TAG_PARKING_ZONE_REROUTE, SUMO_ATTR_PROB, abort);
    bool visible = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, "", SUMO_TAG_PARKING_ZONE_REROUTE, SUMO_ATTR_VISIBLE, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain edge and rerouter interval
        GNEAdditional* parkingArea = viewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, parkingAreaID, false);
        GNEAdditional* rerouterInterval = nullptr;
        // obtain parent depending if we're loading or creating it using GNEAdditionalFrame
        if (insertedAdditionals) {
            rerouterInterval = insertedAdditionals->retrieveParentAdditional(viewNet, SUMO_TAG_INTERVAL);
        } else {
            bool ok = true;
            rerouterInterval = viewNet->getNet()->retrieveAdditional(SUMO_TAG_INTERVAL, attrs.get<std::string>(GNE_ATTR_PARENT, "", ok));
        }
        // check that all elements are valid
        if (parkingArea == nullptr) {
            WRITE_WARNING("The parkingArea '" + parkingAreaID + "' to use within the " + toString(SUMO_TAG_PARKING_ZONE_REROUTE) + " is not known.");
        } else if (rerouterInterval != nullptr) {
            // save ID of last created element
            GNEAdditional* additionalCreated = builParkingAreaReroute(viewNet, allowUndoRedo, rerouterInterval, parkingArea, probability, visible);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildRerouterRouteProbReroute(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string routeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_ROUTE_PROB_REROUTE, SUMO_ATTR_ID, abort);
    double probability = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", SUMO_TAG_ROUTE_PROB_REROUTE, SUMO_ATTR_PROB, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain rerouter interval
        GNEAdditional* rerouterInterval = nullptr;
        // obtain parent depending if we're loading or creating it using GNEAdditionalFrame
        if (insertedAdditionals) {
            rerouterInterval = insertedAdditionals->retrieveParentAdditional(viewNet, SUMO_TAG_INTERVAL);
        } else {
            bool ok = true;
            rerouterInterval = viewNet->getNet()->retrieveAdditional(SUMO_TAG_INTERVAL, attrs.get<std::string>(GNE_ATTR_PARENT, "", ok));
        }
        // check that all elements are valid
        if (rerouterInterval != nullptr) {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildRouteProbReroute(viewNet, allowUndoRedo, rerouterInterval, routeID, probability);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildBusStop(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of bus stop
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_BUS_STOP, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_BUS_STOP, SUMO_ATTR_LANE, abort);
    std::string startPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_BUS_STOP, SUMO_ATTR_STARTPOS, abort);
    std::string endPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_BUS_STOP, SUMO_ATTR_ENDPOS, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_BUS_STOP, SUMO_ATTR_NAME, abort);
    std::vector<std::string> lines = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, id, SUMO_TAG_BUS_STOP, SUMO_ATTR_LINES, abort);
    const int personCapacity = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, id, SUMO_TAG_BUS_STOP, SUMO_ATTR_PERSON_CAPACITY, abort);
    const double parkingLength = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, SUMO_TAG_BUS_STOP, SUMO_ATTR_PARKING_LENGTH, abort);
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
        if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, id, false) != nullptr) {
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
            GNEAdditional* additionalCreated = buildBusStop(viewNet, allowUndoRedo, id, lane, startPosDouble, endPosDouble, parametersSet,
                                               name, lines, personCapacity, parkingLength, friendlyPosition, blockMovement);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildContainerStop(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of container stop
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_CONTAINER_STOP, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_CONTAINER_STOP, SUMO_ATTR_LANE, abort);
    std::string startPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_CONTAINER_STOP, SUMO_ATTR_STARTPOS, abort);
    std::string endPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_CONTAINER_STOP, SUMO_ATTR_ENDPOS, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_CONTAINER_STOP, SUMO_ATTR_NAME, abort);
    std::vector<std::string> lines = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, id, SUMO_TAG_CONTAINER_STOP, SUMO_ATTR_LINES, abort);
    bool friendlyPosition = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_CONTAINER_STOP, SUMO_ATTR_FRIENDLY_POS, abort);
    // parse Netedit attributes
    bool blockMovement = false;
    if (attrs.hasAttribute(GNE_ATTR_BLOCK_MOVEMENT)) {
        blockMovement = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_CONTAINER_STOP, GNE_ATTR_BLOCK_MOVEMENT, abort);
    }
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = viewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_CONTAINER_STOP) + " with the same ID='" + id + "'.");
        } else if (lane == nullptr) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(SUMO_TAG_CONTAINER_STOP) + " '" + id + "' is not known.");
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
                WRITE_WARNING("Invalid position for " + toString(SUMO_TAG_CONTAINER_STOP) + " with ID = '" + id + "'.");
                return false;
            }
            // save ID of last created element
            GNEAdditional* additionalCreated = buildContainerStop(viewNet, allowUndoRedo, id, lane, startPosDouble, endPosDouble, parametersSet,
                                               name, lines, friendlyPosition, blockMovement);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildAccess(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of Entry
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_ACCESS, SUMO_ATTR_LANE, abort);
    std::string position = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_ACCESS, SUMO_ATTR_POSITION, abort);
    std::string length = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_ACCESS, SUMO_ATTR_LENGTH, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, "", SUMO_TAG_ACCESS, SUMO_ATTR_FRIENDLY_POS, abort);
    // parse Netedit attributes
    bool blockMovement = false;
    if (attrs.hasAttribute(GNE_ATTR_BLOCK_MOVEMENT)) {
        blockMovement = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, "", SUMO_TAG_ACCESS, GNE_ATTR_BLOCK_MOVEMENT, abort);
    }
    // Check if parsing of parameters was correct
    if (!abort) {
        double posDouble = GNEAttributeCarrier::parse<double>(position);
        // get lane and busStop parent
        GNELane* lane = viewNet->getNet()->retrieveLane(laneId, false, true);
        GNEAdditional* busStop = nullptr;
        // obtain parent depending if we're loading or creating it using GNEAdditionalFrame
        if (insertedAdditionals) {
            busStop = insertedAdditionals->retrieveParentAdditional(viewNet, SUMO_TAG_BUS_STOP);
        } else {
            bool ok = true;
            busStop = viewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, attrs.get<std::string>(GNE_ATTR_PARENT, "", ok));
        }
        // check that all parameters are valid
        if (lane == nullptr) {
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(SUMO_TAG_ACCESS) + " is not known.");
        } else if (busStop == nullptr) {
            WRITE_WARNING("A " + toString(SUMO_TAG_ACCESS) + " must be declared within the definition of a " + toString(SUMO_TAG_BUS_STOP) + ".");
        } else if (!checkAndFixDetectorPosition(posDouble, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(SUMO_TAG_ACCESS) + ".");
        } else if (!accessCanBeCreated(busStop, lane->getParentEdge())) {
            WRITE_WARNING("Edge '" + lane->getParentEdge()->getID() + "' already has an Access for busStop '" + busStop->getID() + "'");
        } else {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildAccess(viewNet, allowUndoRedo, busStop, lane, posDouble, length, friendlyPos, blockMovement);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildChargingStation(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of charging station
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_CHARGING_STATION, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_CHARGING_STATION, SUMO_ATTR_LANE, abort);
    std::string startPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_CHARGING_STATION, SUMO_ATTR_STARTPOS, abort);
    std::string endPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_CHARGING_STATION, SUMO_ATTR_ENDPOS, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_CHARGING_STATION, SUMO_ATTR_NAME, abort);
    double chargingPower = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, SUMO_TAG_CHARGING_STATION, SUMO_ATTR_CHARGINGPOWER, abort);
    double efficiency = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, SUMO_TAG_CHARGING_STATION, SUMO_ATTR_EFFICIENCY, abort);
    bool chargeInTransit = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_CHARGING_STATION, SUMO_ATTR_CHARGEINTRANSIT, abort);
    SUMOTime chargeDelay = GNEAttributeCarrier::parseAttributeFromXML<SUMOTime>(attrs, id, SUMO_TAG_CHARGING_STATION, SUMO_ATTR_CHARGEDELAY, abort);
    bool friendlyPosition = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_CHARGING_STATION, SUMO_ATTR_FRIENDLY_POS, abort);
    // parse Netedit attributes
    bool blockMovement = false;
    if (attrs.hasAttribute(GNE_ATTR_BLOCK_MOVEMENT)) {
        blockMovement = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_CHARGING_STATION, GNE_ATTR_BLOCK_MOVEMENT, abort);
    }
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = viewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_CHARGING_STATION, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_CHARGING_STATION) + " with the same ID='" + id + "'.");
        } else if (lane == nullptr) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(SUMO_TAG_CHARGING_STATION) + " '" + id + "' is not known.");
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
                WRITE_WARNING("Invalid position for " + toString(SUMO_TAG_CHARGING_STATION) + " with ID = '" + id + "'.");
                return false;
            }
            // save ID of last created element
            GNEAdditional* additionalCreated = buildChargingStation(viewNet, allowUndoRedo, id, lane, startPosDouble, endPosDouble, parametersSet,
                                               name, chargingPower, efficiency, chargeInTransit, chargeDelay, friendlyPosition, blockMovement);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildParkingArea(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of charging station
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_PARKING_AREA, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_PARKING_AREA, SUMO_ATTR_LANE, abort);
    std::string startPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_PARKING_AREA, SUMO_ATTR_STARTPOS, abort);
    std::string endPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_PARKING_AREA, SUMO_ATTR_ENDPOS, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_PARKING_AREA, SUMO_ATTR_NAME, abort);
    bool friendlyPosition = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_PARKING_AREA, SUMO_ATTR_FRIENDLY_POS, abort);
    int roadSideCapacity = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, id, SUMO_TAG_PARKING_AREA, SUMO_ATTR_ROADSIDE_CAPACITY, abort);
    bool onRoad = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_PARKING_AREA, SUMO_ATTR_ONROAD, abort);
    double width = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, SUMO_TAG_PARKING_AREA, SUMO_ATTR_WIDTH, abort);
    std::string length = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_PARKING_AREA, SUMO_ATTR_LENGTH, abort);
    double angle = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, SUMO_TAG_PARKING_AREA, SUMO_ATTR_ANGLE, abort);
    // parse Netedit attributes
    bool blockMovement = false;
    if (attrs.hasAttribute(GNE_ATTR_BLOCK_MOVEMENT)) {
        blockMovement = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_PARKING_AREA, GNE_ATTR_BLOCK_MOVEMENT, abort);
    }
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = viewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_PARKING_AREA) + " with the same ID='" + id + "'.");
        } else if (lane == nullptr) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(SUMO_TAG_PARKING_AREA) + " '" + id + "' is not known.");
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
                WRITE_WARNING("Invalid position for " + toString(SUMO_TAG_PARKING_AREA) + " with ID = '" + id + "'.");
                return false;
            }
            // save ID of last created element
            GNEAdditional* additionalCreated = buildParkingArea(viewNet, allowUndoRedo, id, lane, startPosDouble, endPosDouble, parametersSet,
                                               name, friendlyPosition, roadSideCapacity, onRoad, width, length, angle, blockMovement);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildParkingSpace(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of Parking Spaces
    Position pos = GNEAttributeCarrier::parseAttributeFromXML<Position>(attrs, "", SUMO_TAG_PARKING_SPACE, SUMO_ATTR_POSITION, abort);
    double width = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", SUMO_TAG_PARKING_SPACE, SUMO_ATTR_WIDTH, abort);
    double length = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", SUMO_TAG_PARKING_SPACE, SUMO_ATTR_LENGTH, abort);
    double angle = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", SUMO_TAG_PARKING_SPACE, SUMO_ATTR_ANGLE, abort);
    // parse Netedit attributes
    bool blockMovement = false;
    if (attrs.hasAttribute(GNE_ATTR_BLOCK_MOVEMENT)) {
        blockMovement = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, "", SUMO_TAG_PARKING_SPACE, GNE_ATTR_BLOCK_MOVEMENT, abort);
    }
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get Parking Area Parent
        GNEAdditional* parkingAreaParent = nullptr;
        // obtain parent depending if we're loading or creating it using GNEAdditionalFrame
        if (insertedAdditionals) {
            parkingAreaParent = insertedAdditionals->retrieveParentAdditional(viewNet, SUMO_TAG_PARKING_AREA);
        } else {
            bool ok = true;
            parkingAreaParent = viewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, attrs.get<std::string>(GNE_ATTR_PARENT, "", ok));
        }
        // check that Parking Area Parent exists
        if (parkingAreaParent != nullptr) {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildParkingSpace(viewNet, allowUndoRedo, parkingAreaParent, pos, width, length, angle, blockMovement);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildCalibrator(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // due there is two differents calibrators, has to be parsed in a different way
    std::string edgeID, laneId, id;
    // change tag depending of XML parmeters
    if (attrs.hasAttribute(SUMO_ATTR_EDGE)) {
        id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_CALIBRATOR, SUMO_ATTR_ID, abort);
        edgeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_CALIBRATOR, SUMO_ATTR_EDGE, abort);
        std::string outfile = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_CALIBRATOR, SUMO_ATTR_OUTPUT, abort);
        double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, SUMO_TAG_CALIBRATOR, SUMO_ATTR_POSITION, abort);
        std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_CALIBRATOR, SUMO_ATTR_NAME, abort);
        SUMOTime freq = GNEAttributeCarrier::parseAttributeFromXML<SUMOTime>(attrs, id, SUMO_TAG_CALIBRATOR, SUMO_ATTR_FREQUENCY, abort);
        std::string routeProbe = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_CALIBRATOR, SUMO_ATTR_ROUTEPROBE, abort);
        // extra check for center element after creation
        bool centerAfterCreation = attrs.hasAttribute(GNE_ATTR_CENTER_AFTER_CREATION);
        // Continue if all parameters were sucesfully loaded
        if (!abort) {
            // get pointer and edge
            GNEEdge* edge = viewNet->getNet()->retrieveEdge(edgeID, false);
            // check that all elements are valid
            if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_CALIBRATOR, id, false) != nullptr) {
                WRITE_WARNING("There is another " + toString(SUMO_TAG_CALIBRATOR) + " with the same ID='" + id + "'.");
            } else if (edge == nullptr) {
                WRITE_WARNING("The  edge '" + edgeID + "' to use within the " + toString(SUMO_TAG_CALIBRATOR) + " '" + id + "' is not known.");
            } else {
                // save ID of last created element
                GNEAdditional* additionalCreated = buildCalibrator(viewNet, allowUndoRedo, id, edge, position, name, outfile, freq, routeProbe, centerAfterCreation);
                // check if insertion has to be commited
                if (insertedAdditionals) {
                    insertedAdditionals->commitElementInsertion(additionalCreated);
                }
                return true;
            }
        }
    } else if (attrs.hasAttribute(SUMO_ATTR_LANE)) {
        id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_LANECALIBRATOR, SUMO_ATTR_ID, abort);
        laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_LANECALIBRATOR, SUMO_ATTR_LANE, abort);
        std::string outfile = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_LANECALIBRATOR, SUMO_ATTR_OUTPUT, abort);
        double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, SUMO_TAG_LANECALIBRATOR, SUMO_ATTR_POSITION, abort);
        std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_LANECALIBRATOR, SUMO_ATTR_NAME, abort);
        SUMOTime freq = GNEAttributeCarrier::parseAttributeFromXML<SUMOTime>(attrs, id, SUMO_TAG_LANECALIBRATOR, SUMO_ATTR_FREQUENCY, abort);
        std::string routeProbe = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_LANECALIBRATOR, SUMO_ATTR_ROUTEPROBE, abort);
        // extra check for center element after creation
        bool centerAfterCreation = attrs.hasAttribute(GNE_ATTR_CENTER_AFTER_CREATION);
        // Continue if all parameters were sucesfully loaded
        if (!abort) {
            // get pointer to lane
            GNELane* lane = viewNet->getNet()->retrieveLane(laneId, false, true);
            // check that all elements are valid
            if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_LANECALIBRATOR, id, false) != nullptr) {
                WRITE_WARNING("There is another " + toString(SUMO_TAG_CALIBRATOR) + " with the same ID='" + id + "'.");
            } else if (lane == nullptr) {
                WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(SUMO_TAG_CALIBRATOR) + " '" + id + "' is not known.");
            } else {
                // save ID of last created element
                GNEAdditional* additionalCreated = buildCalibrator(viewNet, allowUndoRedo, id, lane, position, name, outfile, freq, routeProbe, centerAfterCreation);
                // check if insertion has to be commited
                if (insertedAdditionals) {
                    insertedAdditionals->commitElementInsertion(additionalCreated);
                }
                return true;
            }
        }
    } else {
        WRITE_WARNING("additional " + toString(SUMO_TAG_CALIBRATOR) + " must have either a lane or an edge attribute.");
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildDetectorE1(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of E1
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_E1DETECTOR, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_E1DETECTOR, SUMO_ATTR_LANE, abort);
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, SUMO_TAG_E1DETECTOR, SUMO_ATTR_POSITION, abort);
    SUMOTime frequency = GNEAttributeCarrier::parseAttributeFromXML<SUMOTime>(attrs, id, SUMO_TAG_E1DETECTOR, SUMO_ATTR_FREQUENCY, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_E1DETECTOR, SUMO_ATTR_FILE, abort);
    std::string vehicleTypes = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_E1DETECTOR, SUMO_ATTR_VTYPES, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_E1DETECTOR, SUMO_ATTR_NAME, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_E1DETECTOR, SUMO_ATTR_FRIENDLY_POS, abort);
    // parse Netedit attributes
    bool blockMovement = false;
    if (attrs.hasAttribute(GNE_ATTR_BLOCK_MOVEMENT)) {
        blockMovement = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_E1DETECTOR, GNE_ATTR_BLOCK_MOVEMENT, abort);
    }
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = viewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_E1DETECTOR, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_E1DETECTOR) + " with the same ID='" + id + "'.");
        } else if (lane == nullptr) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(SUMO_TAG_E1DETECTOR) + " '" + id + "' is not known.");
        } else if (!checkAndFixDetectorPosition(position, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(SUMO_TAG_E1DETECTOR) + " with ID = '" + id + "'.");
        } else {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildDetectorE1(viewNet, allowUndoRedo, id, lane, position, frequency, file, vehicleTypes, name, friendlyPos, blockMovement);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildDetectorE2(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    // Tag E2 detectors can build either E2 single lanes or E2 multilanes, depending of attribute "lanes"
    SumoXMLTag E2Tag = attrs.hasAttribute(SUMO_ATTR_LANES) ? SUMO_TAG_E2DETECTOR_MULTILANE : SUMO_TAG_E2DETECTOR;
    bool abort = false;
    // start parsing ID
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", E2Tag, SUMO_ATTR_ID, abort);
    // parse attributes of E2 SingleLanes
    std::string laneId = (E2Tag == SUMO_TAG_E2DETECTOR) ? GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, E2Tag, SUMO_ATTR_LANE, abort) : "";
    double length = (E2Tag == SUMO_TAG_E2DETECTOR) ? GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, E2Tag, SUMO_ATTR_LENGTH, abort) : 0;
    // parse attributes of E2 Multilanes
    std::string laneIds = (E2Tag == SUMO_TAG_E2DETECTOR_MULTILANE) ? GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, E2Tag, SUMO_ATTR_LANES, abort) : "";
    double endPos = (E2Tag == SUMO_TAG_E2DETECTOR_MULTILANE) ? GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, E2Tag, SUMO_ATTR_ENDPOS, abort) : 0;
    // parse common attributes
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, E2Tag, SUMO_ATTR_POSITION, abort);
    SUMOTime frequency = GNEAttributeCarrier::parseAttributeFromXML<SUMOTime>(attrs, id, E2Tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, E2Tag, SUMO_ATTR_FILE, abort);
    std::string vehicleTypes = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, E2Tag, SUMO_ATTR_VTYPES, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, E2Tag, SUMO_ATTR_NAME, abort);
    SUMOTime haltingTimeThreshold = GNEAttributeCarrier::parseAttributeFromXML<SUMOTime>(attrs, id, E2Tag, SUMO_ATTR_HALTING_TIME_THRESHOLD, abort);
    double haltingSpeedThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, E2Tag, SUMO_ATTR_HALTING_SPEED_THRESHOLD, abort);
    double jamDistThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, E2Tag, SUMO_ATTR_JAM_DIST_THRESHOLD, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, E2Tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // parse Netedit attributes
    bool blockMovement = false;
    if (attrs.hasAttribute(GNE_ATTR_BLOCK_MOVEMENT)) {
        blockMovement = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, E2Tag, GNE_ATTR_BLOCK_MOVEMENT, abort);
    }
    // cont attribute is deprecated
    GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, E2Tag, SUMO_ATTR_CONT, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // check if at leas lane or laneIDS are defined
        if (laneId.empty() && laneIds.empty()) {
            WRITE_WARNING("A " + toString(E2Tag) + " needs at least a lane or a list of lanes.");
        } else {
            // get pointer to lane
            GNELane* lane = viewNet->getNet()->retrieveLane(laneId, false, true);
            // get list of lanes
            std::vector<GNELane*> lanes;
            bool laneConsecutives = true;
            if (GNEAttributeCarrier::canParse<std::vector<GNELane*> >(viewNet->getNet(), laneIds, false)) {
                lanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(viewNet->getNet(), laneIds);
                // check if lanes are consecutives
                laneConsecutives = GNEAttributeCarrier::lanesConsecutives(lanes);
            }
            // check that all elements are valid
            if (viewNet->getNet()->retrieveAdditional(E2Tag, id, false) != nullptr) {
                // write error if neither lane nor lane aren't defined
                WRITE_WARNING("There is another " + toString(E2Tag) + " with the same ID='" + id + "'.");
            } else if (attrs.hasAttribute(SUMO_ATTR_LANE) && (lane == nullptr)) {
                // Write error if lane isn't valid
                WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(E2Tag) + " '" + id + "' is not known.");
            } else if (attrs.hasAttribute(SUMO_ATTR_LANES) && lanes.empty()) {
                // Write error if lane isn't valid
                WRITE_WARNING("The list of lanes cannot be empty.");
            } else if (attrs.hasAttribute(SUMO_ATTR_LANES) && lanes.empty()) {
                // Write error if lane isn't valid
                WRITE_WARNING("The list of lanes '" + laneIds + "' to use within the " + toString(E2Tag) + " '" + id + "' isn't valid.");
            } else if (!lanes.empty() && !laneConsecutives) {
                WRITE_WARNING("The lanes '" + laneIds + "' to use within the " + toString(E2Tag) + " '" + id + "' aren't consecutives.");
            } else if (lane && !fixE2DetectorPosition(position, length, lane->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPos)) {
                WRITE_WARNING("Invalid position for " + toString(E2Tag) + " with ID = '" + id + "'.");
            } else if (!lanes.empty() && !fixE2DetectorPosition(position, length, lanes.front()->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPos)) {
                WRITE_WARNING("Invalid position for " + toString(E2Tag) + " with ID = '" + id + "'.");
            } else if (!lanes.empty() && !fixE2DetectorPosition(endPos, length, lanes.back()->getParentEdge()->getNBEdge()->getFinalLength(), friendlyPos)) {
                WRITE_WARNING("Invalid end position for " + toString(E2Tag) + " with ID = '" + id + "'.");
            } else if (lane) {
                // save ID of last created element
                GNEAdditional* additionalCreated = buildSingleLaneDetectorE2(viewNet, allowUndoRedo, id, lane, position, length, frequency, file, vehicleTypes,
                                                   name, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold, friendlyPos, blockMovement);
                // check if insertion has to be commited
                if (insertedAdditionals) {
                    insertedAdditionals->commitElementInsertion(additionalCreated);
                }
                return true;
            } else {
                // save ID of last created element
                GNEAdditional* additionalCreated = buildMultiLaneDetectorE2(viewNet, allowUndoRedo, id, lanes, position, endPos, frequency, file, vehicleTypes,
                                                   name, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold, friendlyPos, blockMovement);
                // check if insertion has to be commited
                if (insertedAdditionals) {
                    insertedAdditionals->commitElementInsertion(additionalCreated);
                }
                return true;
            }
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildDetectorE3(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of E3
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_E3DETECTOR, SUMO_ATTR_ID, abort);
    SUMOTime frequency = GNEAttributeCarrier::parseAttributeFromXML<SUMOTime>(attrs, id, SUMO_TAG_E3DETECTOR, SUMO_ATTR_FREQUENCY, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_E3DETECTOR, SUMO_ATTR_FILE, abort);
    std::string vehicleTypes = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_E3DETECTOR, SUMO_ATTR_VTYPES, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_E3DETECTOR, SUMO_ATTR_NAME, abort);
    SUMOTime haltingTimeThreshold = GNEAttributeCarrier::parseAttributeFromXML<SUMOTime>(attrs, id, SUMO_TAG_E3DETECTOR, SUMO_ATTR_HALTING_TIME_THRESHOLD, abort);
    double haltingSpeedThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, SUMO_TAG_E3DETECTOR, SUMO_ATTR_HALTING_SPEED_THRESHOLD, abort);
    Position pos = GNEAttributeCarrier::parseAttributeFromXML<Position>(attrs, id, SUMO_TAG_E3DETECTOR, SUMO_ATTR_POSITION, abort);
    // parse Netedit attributes
    bool blockMovement = false;
    if (attrs.hasAttribute(GNE_ATTR_BLOCK_MOVEMENT)) {
        blockMovement = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_E3DETECTOR, GNE_ATTR_BLOCK_MOVEMENT, abort);
    }
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // check that all elements are valid
        if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_E3DETECTOR, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_E3DETECTOR) + " with the same ID='" + id + "'.");
        } else {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildDetectorE3(viewNet, allowUndoRedo, id, pos, frequency, file, vehicleTypes, name, haltingTimeThreshold, haltingSpeedThreshold, blockMovement);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildDetectorEntry(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of Entry
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_DET_ENTRY, SUMO_ATTR_LANE, abort);
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", SUMO_TAG_DET_ENTRY, SUMO_ATTR_POSITION, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, "", SUMO_TAG_DET_ENTRY, SUMO_ATTR_FRIENDLY_POS, abort);
    // parse Netedit attributes
    bool blockMovement = false;
    if (attrs.hasAttribute(GNE_ATTR_BLOCK_MOVEMENT)) {
        blockMovement = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, "", SUMO_TAG_DET_ENTRY, GNE_ATTR_BLOCK_MOVEMENT, abort);
    }
    // Check if parsing of parameters was correct
    if (!abort) {
        // get lane and E3 parent
        GNELane* lane = viewNet->getNet()->retrieveLane(laneId, false, true);
        GNEAdditional* E3Parent = nullptr;
        // obtain parent depending if we're loading or creating it using GNEAdditionalFrame
        if (insertedAdditionals) {
            E3Parent = insertedAdditionals->retrieveParentAdditional(viewNet, SUMO_TAG_E3DETECTOR);
        } else {
            bool ok = true;
            E3Parent = viewNet->getNet()->retrieveAdditional(SUMO_TAG_E3DETECTOR, attrs.get<std::string>(GNE_ATTR_PARENT, "", ok));
        }
        // check that all parameters are valid
        if (lane == nullptr) {
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(SUMO_TAG_DET_ENTRY) + " is not known.");
        } else if (!checkAndFixDetectorPosition(position, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(SUMO_TAG_DET_ENTRY) + ".");
        } else if (E3Parent) {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildDetectorEntry(viewNet, allowUndoRedo, E3Parent, lane, position, friendlyPos, blockMovement);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildDetectorExit(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of Exit
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_DET_EXIT, SUMO_ATTR_LANE, abort);
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", SUMO_TAG_DET_EXIT, SUMO_ATTR_POSITION, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, "", SUMO_TAG_DET_EXIT, SUMO_ATTR_FRIENDLY_POS, abort);
    // parse Netedit attributes
    bool blockMovement = false;
    if (attrs.hasAttribute(GNE_ATTR_BLOCK_MOVEMENT)) {
        blockMovement = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, "", SUMO_TAG_DET_EXIT, GNE_ATTR_BLOCK_MOVEMENT, abort);
    }
    // Check if parsing of parameters was correct
    if (!abort) {
        // get lane and E3 parent
        GNELane* lane = viewNet->getNet()->retrieveLane(laneId, false, true);
        GNEAdditional* E3Parent = nullptr;
        // obtain parent depending if we're loading or creating it using GNEAdditionalFrame
        if (insertedAdditionals) {
            E3Parent = insertedAdditionals->retrieveParentAdditional(viewNet, SUMO_TAG_E3DETECTOR);
        } else {
            bool ok = true;
            E3Parent = viewNet->getNet()->retrieveAdditional(SUMO_TAG_E3DETECTOR, attrs.get<std::string>(GNE_ATTR_PARENT, "", ok));
        }
        // check that all parameters are valid
        if (lane == nullptr) {
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(SUMO_TAG_DET_EXIT) + " is not known.");
        } else if (!checkAndFixDetectorPosition(position, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(SUMO_TAG_DET_EXIT) + ".");
        } else if (E3Parent) {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildDetectorExit(viewNet, allowUndoRedo, E3Parent, lane, position, friendlyPos, blockMovement);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}


bool
GNEAdditionalHandler::parseAndBuildDetectorE1Instant(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedAdditionals* insertedAdditionals) {
    bool abort = false;
    // parse attributes of E1Instant
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_INSTANT_INDUCTION_LOOP, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_INSTANT_INDUCTION_LOOP, SUMO_ATTR_LANE, abort);
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, SUMO_TAG_INSTANT_INDUCTION_LOOP, SUMO_ATTR_POSITION, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_INSTANT_INDUCTION_LOOP, SUMO_ATTR_FILE, abort);
    std::string vehicleTypes = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_INSTANT_INDUCTION_LOOP, SUMO_ATTR_VTYPES, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, SUMO_TAG_INSTANT_INDUCTION_LOOP, SUMO_ATTR_NAME, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_INSTANT_INDUCTION_LOOP, SUMO_ATTR_FRIENDLY_POS, abort);
    // parse Netedit attributes
    bool blockMovement = false;
    if (attrs.hasAttribute(GNE_ATTR_BLOCK_MOVEMENT)) {
        blockMovement = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, SUMO_TAG_INSTANT_INDUCTION_LOOP, GNE_ATTR_BLOCK_MOVEMENT, abort);
    }
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = viewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_INSTANT_INDUCTION_LOOP, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(SUMO_TAG_INSTANT_INDUCTION_LOOP) + " with the same ID='" + id + "'.");
        } else if (lane == nullptr) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(SUMO_TAG_INSTANT_INDUCTION_LOOP) + " '" + id + "' is not known.");
        } else if (!checkAndFixDetectorPosition(position, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(SUMO_TAG_INSTANT_INDUCTION_LOOP) + " with ID = '" + id + "'.");
        } else {
            // save ID of last created element
            GNEAdditional* additionalCreated = buildDetectorE1Instant(viewNet, allowUndoRedo, id, lane, position, file, vehicleTypes, name, friendlyPos, blockMovement);
            // check if insertion has to be commited
            if (insertedAdditionals) {
                insertedAdditionals->commitElementInsertion(additionalCreated);
            }
            return true;
        }
    }
    return false;
}

// ===========================================================================
// private method definitions
// ===========================================================================

void
GNEAdditionalHandler::parseAndBuildPOI(const SUMOSAXAttributes& attrs) {
    bool abort = false;
    // parse attributes of POIs
    std::string POIID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_POI, SUMO_ATTR_ID, abort);
    // POIs can be defined using a X,Y position,...
    Position pos = attrs.hasAttribute(SUMO_ATTR_X) ? GNEAttributeCarrier::parseAttributeFromXML<Position>(attrs, POIID, SUMO_TAG_POI, SUMO_ATTR_POSITION, abort) : Position::INVALID;
    // ... a Lon-Lat,...
    double lon = attrs.hasAttribute(SUMO_ATTR_LON) ? GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, POIID, SUMO_TAG_POI, SUMO_ATTR_LON, abort) : GNEAttributeCarrier::INVALID_POSITION;
    double lat = attrs.hasAttribute(SUMO_ATTR_LAT) ? GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, POIID, SUMO_TAG_POI, SUMO_ATTR_LAT, abort) : GNEAttributeCarrier::INVALID_POSITION;
    // .. or as Lane-PosLane
    std::string laneID = attrs.hasAttribute(SUMO_ATTR_LANE) ? GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, POIID, SUMO_TAG_POILANE, SUMO_ATTR_LANE, abort) : "";
    double lanePos = attrs.hasAttribute(SUMO_ATTR_POSITION) ? GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, POIID, SUMO_TAG_POILANE, SUMO_ATTR_POSITION, abort) : GNEAttributeCarrier::INVALID_POSITION;
    double lanePosLat = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, POIID, SUMO_TAG_POILANE, SUMO_ATTR_POSITION_LAT, abort);
    // continue with common parameters
    double layer = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, POIID, SUMO_TAG_POI, SUMO_ATTR_LAYER, abort);
    std::string type = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, POIID, SUMO_TAG_POI, SUMO_ATTR_TYPE, abort);
    RGBColor color = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, POIID, SUMO_TAG_POI, SUMO_ATTR_COLOR, abort);
    double angle = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, POIID, SUMO_TAG_POI, SUMO_ATTR_ANGLE, abort);
    std::string imgFile = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, POIID, SUMO_TAG_POI, SUMO_ATTR_IMGFILE, abort);
    bool relativePath = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, POIID, SUMO_TAG_POI, SUMO_ATTR_RELATIVEPATH, abort);
    double width = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, POIID, SUMO_TAG_POI, SUMO_ATTR_WIDTH, abort);
    double height = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, POIID, SUMO_TAG_POI, SUMO_ATTR_HEIGHT, abort);
    // check if ID is valid
    if (SUMOXMLDefinitions::isValidTypeID(POIID) == false) {
        WRITE_WARNING("Invalid characters for POI ID");
        abort = true;
    }
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // check if img file is absolute
        if (imgFile != "" && !FileHelpers::isAbsolute(imgFile)) {
            imgFile = FileHelpers::getConfigurationRelative(getFileName(), imgFile);
        }
        // check if lane exist
        if (laneID != "" && !myViewNet->getNet()->retrieveLane(laneID, false)) {
            WRITE_WARNING("The lane '" + laneID + "' to use within the PoI '" + POIID + "' is not known.");
            return;
        }
        // check position
        bool useGeo = false;
        // if position is invalid, then is either a POILane or a GEOPoi
        if (pos == Position::INVALID) {
            // try computing x,y from lane,pos
            if (laneID != "") {
                // if LaneID is defined, then is a POILane
                pos = getLanePos(POIID, laneID, lanePos, lanePosLat);
            } else {
                // try computing x,y from lon,lat
                if (lat == GNEAttributeCarrier::INVALID_POSITION || lon == GNEAttributeCarrier::INVALID_POSITION) {
                    WRITE_WARNING("Either (x, y), (lon, lat) or (lane, pos) must be specified for PoI '" + POIID + "'.");
                    return;
                } else if (!GeoConvHelper::getFinal().usingGeoProjection()) {
                    WRITE_WARNING("(lon, lat) is specified for PoI '" + POIID + "' but no geo-conversion is specified for the network.");
                    return;
                }
                // set GEO Position
                pos.set(lon, lat);
                useGeo = true;
                if (!GeoConvHelper::getFinal().x2cartesian_const(pos)) {
                    WRITE_WARNING("Unable to project coordinates for PoI '" + POIID + "'.");
                    return;
                }
            }
        }
        // create POI, or show an error if POI already exists
        if (!myViewNet->getNet()->getAttributeCarriers()->addPOI(POIID, type, color, pos, useGeo, laneID, lanePos, lanePosLat, layer, angle, imgFile, relativePath, width, height, false)) {
            WRITE_WARNING("POI with ID '" + POIID + "' already exists.");
        } else {
            // update myLastParameterised with the last inserted POI
            myLastParameterised = myViewNet->getNet()->getAttributeCarriers()->getPOIs().get(POIID);
        }
    }
}


void
GNEAdditionalHandler::parseParameter(const SUMOSAXAttributes& attrs) {
    // we have two cases: if we're parsing a Shape or we're parsing an Additional
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
        // set parameter in last inserted additional
        if (ok) {
            WRITE_DEBUG("Inserting parameter '" + key + "|" + val + "' into shape.");
            getLastParameterised()->setParameter(key, val);
        }
    } else if (myHierarchyInsertedAdditionals.getLastInsertedAdditional()) {
        // first check if given additional supports parameters
        if (myHierarchyInsertedAdditionals.getLastInsertedAdditional()->getTagProperty().hasParameters()) {
            bool ok = true;
            std::string key;
            if (attrs.hasAttribute(SUMO_ATTR_KEY)) {
                // obtain key
                key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, ok);
                if (key.empty()) {
                    WRITE_WARNING("Error parsing key from additional parameter. Key cannot be empty");
                    ok = false;
                }
                if (!SUMOXMLDefinitions::isValidTypeID(key)) {
                    WRITE_WARNING("Error parsing key from additional parameter. Key contains invalid characters");
                    ok = false;
                }
            } else {
                WRITE_WARNING("Error parsing key from additional parameter. Key doesn't exist");
                ok = false;
            }
            // circumventing empty string test
            const std::string val = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
            if (!SUMOXMLDefinitions::isValidAttribute(val)) {
                WRITE_WARNING("Error parsing value from additional parameter. Value contains invalid characters");
                ok = false;
            }
            // check double values
            if (myHierarchyInsertedAdditionals.getLastInsertedAdditional()->getTagProperty().hasDoubleParameters() && !GNEAttributeCarrier::canParse<double>(val)) {
                WRITE_WARNING("Error parsing value from additional float parameter. Value cannot be parsed to float");
                ok = false;
            }
            // set parameter in last inserted additional
            if (ok) {
                WRITE_DEBUG("Inserting parameter '" + key + "|" + val + "' into additional " + myHierarchyInsertedAdditionals.getLastInsertedAdditional()->getTagStr() + ".");
                myHierarchyInsertedAdditionals.getLastInsertedAdditional()->setParameter(key, val);
            }
        } else {
            WRITE_WARNING("Additionals of type '" + myHierarchyInsertedAdditionals.getLastInsertedAdditional()->getTagStr() + "' doesn't support parameters");
        }
    } else {
        WRITE_WARNING("Parameters has to be declared within the definition of an additional or a shape element");
    }
}

// ===========================================================================
// GNEAdditionalHandler::HierarchyInsertedAdditionals method definitions
// ===========================================================================

void
GNEAdditionalHandler::HierarchyInsertedAdditionals::insertElement(SumoXMLTag tag) {
    myInsertedElements.push_back(std::make_pair(tag, nullptr));
}


void
GNEAdditionalHandler::HierarchyInsertedAdditionals::commitElementInsertion(GNEAdditional* additional) {
    myInsertedElements.back().second = additional;
}


void
GNEAdditionalHandler::HierarchyInsertedAdditionals::popElement() {
    if (!myInsertedElements.empty()) {
        myInsertedElements.pop_back();
    }
}


GNEAdditional*
GNEAdditionalHandler::HierarchyInsertedAdditionals::retrieveParentAdditional(GNEViewNet* viewNet, SumoXMLTag expectedTag) const {
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
        GNEAdditional* retrievedAdditional = viewNet->getNet()->retrieveAdditional((myInsertedElements.end() - 2)->first, (myInsertedElements.end() - 2)->second->getID(), false);
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
    }
}


GNEAdditional*
GNEAdditionalHandler::HierarchyInsertedAdditionals::getLastInsertedAdditional() const {
    // ierate in reverse mode over myInsertedElements to obtain last inserted additional
    for (std::vector<std::pair<SumoXMLTag, GNEAdditional*> >::const_reverse_iterator i = myInsertedElements.rbegin(); i != myInsertedElements.rend(); i++) {
        // we need to avoid Tag Param because isn't an additional
        if (i->first != SUMO_TAG_PARAM) {
            return i->second;
        }
    }
    return nullptr;
}


/****************************************************************************/

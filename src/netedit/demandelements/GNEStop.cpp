/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEStop.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
/// @version $Id$
///
// Representation of Stops in NETEDIT
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <cmath>
#include <microsim/MSVehicle.h>
#include <microsim/devices/MSDevice_BTreceiver.h>
#include <netedit/additionals/GNEStoppingPlace.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/additionals/GNEAdditional.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIBaseVehicleHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEStop.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEStop::GNEStop(SumoXMLTag tag, GNEViewNet* viewNet, const SUMOVehicleParameter::Stop &stopParameter, GNEAdditional* stoppingPlace, GNEDemandElement* stopParent) :
    GNEDemandElement(stopParent, viewNet, GLO_STOP, tag, {}, {}, {stoppingPlace}, {stopParent}, {}, {}, {}, {}),
    SUMOVehicleParameter::Stop(stopParameter),
    myFriendlyPosition(false) {
}


GNEStop::GNEStop(GNEViewNet* viewNet, const SUMOVehicleParameter::Stop &stopParameter, GNELane* lane, const std::string &startPosition, const std::string &endPosition, bool friendlyPosition, GNEDemandElement* stopParent) :
    GNEDemandElement(stopParent, viewNet, GLO_STOP, SUMO_TAG_STOP_LANE, {}, {lane}, {}, {stopParent}, {}, {}, {}, {}),
    SUMOVehicleParameter::Stop(stopParameter),
    myStartPosition(startPosition),
    myEndPosition(endPosition), 
    myFriendlyPosition(friendlyPosition) { 
}


GNEStop::~GNEStop() {}


std::string 
GNEStop::getBegin() const {
    return "";
}


const RGBColor &
GNEStop::getColor() const {
    return RGBColor::BLACK;
}


void 
GNEStop::writeDemandElement(OutputDevice& device) const {
    write(device);
}


void
GNEStop::moveGeometry(const Position&) {
    // This demand element cannot be moved
}


void
GNEStop::commitGeometryMoving(GNEUndoList*) {
    // This demand element cannot be moved
}


void
GNEStop::updateGeometry(bool updateGrid) {
    //only update Stops over lanes, because other uses the geometry of stopping place parent
    if (myLaneParents.size() > 0) {
        // first check if object has to be removed from grid (SUMOTree)
        if (updateGrid) {
            myViewNet->getNet()->removeGLObjectFromGrid(this);
        }
        
        // Clear all containers
        myGeometry.clearGeometry();

        // Get value of option "lefthand"
        double offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;

        // Cut shape using as delimitators fixed start position and fixed end position
        myGeometry.shape = myLaneParents.front()->getShape().getSubpart(getStartGeometryPositionOverLane(), getEndGeometryPositionOverLane());

        // Get calculate lenghts and rotations
        myGeometry.calculateShapeRotationsAndLengths();

        // last step is to check if object has to be added into grid (SUMOTree) again
        if (updateGrid) {
            myViewNet->getNet()->addGLObjectIntoGrid(this);
        }
    }
}


Position
GNEStop::getPositionInView() const {
    if (myLaneParents.size() > 0) {
        if (myLaneParents.front()->getShape().length() < 2.5) {
            return myLaneParents.front()->getShape().front();
        } else {
            Position A = myLaneParents.front()->getShape().positionAtOffset(2.5);
            Position B = myLaneParents.front()->getShape().positionAtOffset(2.5);
            // return Middle point
            return Position((A.x() + B.x()) / 2, (A.y() + B.y()) / 2);
        }
    } else if (myDemandElementParents.size() > 0) {
        return myDemandElementParents.front()->getPositionInView();
    } else {
        throw ProcessError("Invalid Stop parent");
    }
}


std::string
GNEStop::getParentName() const {
    if (myDemandElementParents.size() > 0) {
        return myDemandElementParents.front()->getID();
    } else if (myAdditionalParents.size() > 0) {
        return myAdditionalParents.front()->getID();
    } else if (myLaneParents.size() > 0) {
        return myLaneParents.front()->getID();
    } else {
        throw ProcessError("Invalid parent");
    }
}


void
GNEStop::drawGL(const GUIVisualizationSettings& s) const {
    // only drawn in super mode demand
    if (myViewNet->getViewOptions().showDemandElements()) {
       
    }
}


void
GNEStop::selectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.select(dynamic_cast<GUIGlObject*>(this)->getGlID());
        // add object of list into selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->addedLockedObject(GLO_VEHICLE);
        if (changeFlag) {
            mySelected = true;
        }
    }
}


void
GNEStop::unselectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.deselect(dynamic_cast<GUIGlObject*>(this)->getGlID());
        // remove object of list of selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->removeLockedObject(GLO_VEHICLE);
        if (changeFlag) {
            mySelected = false;

        }
    }
}


std::string
GNEStop::getAttribute(SumoXMLAttr key) const {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            return getDemandElementID();
        case SUMO_ATTR_DURATION:
            return toString(duration);
        case SUMO_ATTR_UNTIL:
            return toString(until);
        case SUMO_ATTR_INDEX:
            return toString(index);
        case SUMO_ATTR_TRIGGERED:
            return toString(triggered);
        case SUMO_ATTR_EXPECTED:
            return toString(awaitedPersons);
        case SUMO_ATTR_EXPECTED_CONTAINERS:
            return toString(awaitedContainers);
        case SUMO_ATTR_PARKING:
            return toString(parking);
        case SUMO_ATTR_ACTTYPE:
            return "";  // CHECK
        // specific of Stops over stoppingPlaces
        case SUMO_ATTR_BUS_STOP:
        case SUMO_ATTR_CONTAINER_STOP:
        case SUMO_ATTR_CHARGING_STATION:
        case SUMO_ATTR_PARKING_AREA:
            return myAdditionalParents.front()->getID();
        // specific of stops over lanes
        case SUMO_ATTR_LANE:
            return myLaneParents.front()->getID();
        case SUMO_ATTR_STARTPOS:
            return myStartPosition;
        case SUMO_ATTR_ENDPOS:
            return myEndPosition;
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        //
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEStop::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_DURATION:
        case SUMO_ATTR_UNTIL:
        case SUMO_ATTR_INDEX:
        case SUMO_ATTR_TRIGGERED:
        case SUMO_ATTR_EXPECTED:
        case SUMO_ATTR_EXPECTED_CONTAINERS:
        case SUMO_ATTR_PARKING:
        case SUMO_ATTR_ACTTYPE:
        // specific of Stops over stoppingPlaces
        case SUMO_ATTR_BUS_STOP:
        case SUMO_ATTR_CONTAINER_STOP:
        case SUMO_ATTR_CHARGING_STATION:
        case SUMO_ATTR_PARKING_AREA:
        // specific of stops over lanes
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_STARTPOS:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_FRIENDLY_POS:
        //
        case GNE_ATTR_GENERIC:
        case GNE_ATTR_SELECTED:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEStop::isValid(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDemandElementID(value);
        case SUMO_ATTR_DURATION:
            return canParse<double>(value);
        case SUMO_ATTR_UNTIL:
            return canParse<double>(value);
        case SUMO_ATTR_INDEX:
            return canParse<int>(value);
        case SUMO_ATTR_TRIGGERED:
            return canParse<bool>(value);
        case SUMO_ATTR_EXPECTED:
        case SUMO_ATTR_EXPECTED_CONTAINERS:
            if (value.empty()) {
                return true;
            } else {
                return canParse<std::vector<std::string> >(value);
            }
        case SUMO_ATTR_PARKING:
            return canParse<bool>(value);
        case SUMO_ATTR_ACTTYPE:
            return false;  // CHECK
        // specific of Stops over stoppingPlaces
        case SUMO_ATTR_BUS_STOP:
            return (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, value, false) != nullptr);
        case SUMO_ATTR_CONTAINER_STOP:
            return (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, value, false) != nullptr);
        case SUMO_ATTR_CHARGING_STATION:
            return (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_CHARGING_STATION, value, false) != nullptr);
        case SUMO_ATTR_PARKING_AREA:
            return (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, value, false) != nullptr);
        // specific of stops over lanes
        case SUMO_ATTR_LANE:
            if (myViewNet->getNet()->retrieveLane(value, false) != nullptr) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_STARTPOS:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return GNEStoppingPlace::checkStoppinPlacePosition(value, myEndPosition, myLaneParents.front()->getParentEdge().getNBEdge()->getFinalLength(), myFriendlyPosition);
            } else {
                return false;
            }
        case SUMO_ATTR_ENDPOS:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return GNEStoppingPlace::checkStoppinPlacePosition(myStartPosition, value, myLaneParents.front()->getParentEdge().getNBEdge()->getFinalLength(), myFriendlyPosition);
            } else {
                return false;
            }
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        //
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool 
GNEStop::isDisjointAttributeSet(const SumoXMLAttr attr) const {
    switch (attr) {
        case SUMO_ATTR_END:
            return (parametersSet & VEHPARS_END_SET) != 0;
        case SUMO_ATTR_NUMBER:
            return (parametersSet & VEHPARS_NUMBER_SET) != 0;
        case SUMO_ATTR_VEHSPERHOUR:
            return (parametersSet & VEHPARS_VPH_SET) != 0;
        case SUMO_ATTR_PERIOD:
            return (parametersSet & VEHPARS_PERIOD_SET) != 0;
        case SUMO_ATTR_PROB:
            return (parametersSet & VEHPARS_PROB_SET) != 0;
        default:
            return true;
    };
}


void 
GNEStop::setDisjointAttribute(const SumoXMLAttr attr, GNEUndoList* undoList) {
    // obtain a copy of parameter sets
    int parametersSetCopy = parametersSet;
    // modify parametersSetCopy depending of attr
    switch (attr) {
        case SUMO_ATTR_END: {
            // give more priority to end
            parametersSetCopy = VEHPARS_END_SET | VEHPARS_NUMBER_SET;
            break;
        }
        case SUMO_ATTR_NUMBER:
            parametersSetCopy ^= VEHPARS_END_SET;
            parametersSetCopy |= VEHPARS_NUMBER_SET;
            break;
        case SUMO_ATTR_VEHSPERHOUR: {
            // give more priority to end
            if ((parametersSetCopy & VEHPARS_END_SET) && (parametersSetCopy & VEHPARS_NUMBER_SET)) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_END_SET) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_NUMBER_SET) {
                parametersSetCopy = VEHPARS_NUMBER_SET;
            }
            // set VehsPerHour
            parametersSetCopy |= VEHPARS_VPH_SET;
            break;
        }
        case SUMO_ATTR_PERIOD: {
            // give more priority to end
            if ((parametersSetCopy & VEHPARS_END_SET) && (parametersSetCopy & VEHPARS_NUMBER_SET)) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_END_SET) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_NUMBER_SET) {
                parametersSetCopy = VEHPARS_NUMBER_SET;
            }
            // set period
            parametersSetCopy |= VEHPARS_PERIOD_SET;
            break;
        }
        case SUMO_ATTR_PROB: {
            // give more priority to end
            if ((parametersSetCopy & VEHPARS_END_SET) && (parametersSetCopy & VEHPARS_NUMBER_SET)) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_END_SET) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_NUMBER_SET) {
                parametersSetCopy = VEHPARS_NUMBER_SET;
            }
            // set probability
            parametersSetCopy |= VEHPARS_PROB_SET;
            break;
        }
        default:
            break;
    }
    undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), parametersSet, parametersSetCopy));
}


std::string
GNEStop::getPopUpID() const {
    return getTagStr();
}


std::string
GNEStop::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) ;
}


double
GNEStop::getStartGeometryPositionOverLane() const {
    if (myStartPosition.empty()) {
        return 0;
    } else {
        double fixedPos = parse<double>(myStartPosition);
        const double len = myLaneParents.front()->getParentEdge().getNBEdge()->getFinalLength();
        if (fixedPos < 0) {
            fixedPos += len;
        }
        return fixedPos * myLaneParents.front()->getLengthGeometryFactor();
    }
}


double
GNEStop::getEndGeometryPositionOverLane() const {
    if (myEndPosition.empty()) {
        return myLaneParents.front()->getParentEdge().getNBEdge()->getFinalLength();
    } else {
        double fixedPos = parse<double>(myEndPosition);
        const double len = myLaneParents.front()->getParentEdge().getNBEdge()->getFinalLength();
        if (fixedPos < 0) {
            fixedPos += len;
        }
        return fixedPos * myLaneParents.front()->getLengthGeometryFactor();
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEStop::setAttribute(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            changeDemandElementID(value);
            break;
        case SUMO_ATTR_DURATION:
            duration = parse<double>(value);
            break;
        case SUMO_ATTR_UNTIL:
            until = parse<double>(value);
            break;
        case SUMO_ATTR_INDEX:
            index = parse<int>(value);
            break;
        case SUMO_ATTR_TRIGGERED:
            triggered = parse<bool>(value);
            break;
        case SUMO_ATTR_EXPECTED:
            awaitedPersons = parse<std::set<std::string> >(value);
            break;
        case SUMO_ATTR_EXPECTED_CONTAINERS:
            awaitedContainers = parse<std::set<std::string> >(value);
            break;
        case SUMO_ATTR_PARKING:
            parking = parse<bool>(value);
            break;
        case SUMO_ATTR_ACTTYPE:
            // CHECK
            break;
        // specific of Stops over stoppingPlaces
        /*
        case SUMO_ATTR_BUS_STOP:
            return (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, value, false) != nullptr);
        case SUMO_ATTR_CONTAINER_STOP:
            return (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, value, false) != nullptr);
        case SUMO_ATTR_CHARGING_STATION:
            return (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_CHARGING_STATION, value, false) != nullptr);
        case SUMO_ATTR_PARKING_AREA:
            return (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, value, false) != nullptr);
        */
        // specific of Stops over lanes
        case SUMO_ATTR_LANE:
            changeLaneParents(this, value);
            break;
        case SUMO_ATTR_STARTPOS:
            myStartPosition = value;
            break;
        case SUMO_ATTR_ENDPOS:
            myEndPosition = value;
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
            break;
        //
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // check if updated attribute requieres update geometry
    if (myTagProperty.hasAttribute(key) && myTagProperty.getAttributeProperties(key).requiereUpdateGeometry()) {
        updateGeometry(true);
    }
}

/****************************************************************************/

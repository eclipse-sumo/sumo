/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEVehicle.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// Representation of vehicles in NETEDIT
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <cmath>
#include <microsim/MSVehicle.h>
#include <microsim/devices/MSDevice_BTreceiver.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIBaseVehicleHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEVehicle.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVehicle::GNEVehicle(SumoXMLTag tag, GNEViewNet* viewNet, const std::string &vehicleID, GNEDemandElement* vehicleType, GNEDemandElement* route) :
    GNEDemandElement(vehicleID, viewNet, (tag == SUMO_TAG_FLOW)? GLO_FLOW : GLO_VEHICLE, tag, std::vector<GNEDemandElement*>{vehicleType, route}),
    SUMOVehicleParameter(),
    myFrom(nullptr),
    myTo(nullptr) {
    // SUMOVehicleParameter ID has to be set manually
    id = vehicleID;
    // set manually vtypeID (needed for saving)
    vtypeid = vehicleType->getID();
}


GNEVehicle::GNEVehicle(SumoXMLTag tag, GNEViewNet* viewNet, const SUMOVehicleParameter &vehicleParameter, GNEDemandElement* vehicleType, GNEDemandElement* route) :
    GNEDemandElement(vehicleParameter.id, viewNet, (tag == SUMO_TAG_FLOW)? GLO_FLOW : GLO_VEHICLE, tag, std::vector<GNEDemandElement*>{vehicleType, route}),
    SUMOVehicleParameter(vehicleParameter),
    myFrom(nullptr),
    myTo(nullptr) {
    // SUMOVehicleParameter ID has to be set manually
    id = vehicleParameter.id;
    // set manually vtypeID (needed for saving)
    vtypeid = vehicleType->getID();
}


GNEVehicle::GNEVehicle(GNEViewNet* viewNet, const std::string &tripID, GNEDemandElement* vehicleType, GNEEdge* from, GNEEdge* to, std::vector<GNEEdge*> viaEdges) : 
    GNEDemandElement(tripID, viewNet, GLO_TRIP, SUMO_TAG_TRIP, {vehicleType}),
    SUMOVehicleParameter(),
    myFrom(from),
    myTo(to),
    myVia(viaEdges) {
}


GNEVehicle::GNEVehicle(GNEViewNet* viewNet, const SUMOVehicleParameter &tripParameter, GNEDemandElement* vehicleType, GNEEdge* from, GNEEdge* to, std::vector<GNEEdge*> viaEdges) :
    GNEDemandElement(tripParameter.id, viewNet, GLO_TRIP, SUMO_TAG_TRIP, {vehicleType}),
    SUMOVehicleParameter(tripParameter),
    myFrom(from),
    myTo(to),
    myVia(viaEdges) {
}


GNEVehicle::~GNEVehicle() {}


std::string 
GNEVehicle::getBegin() const {
    // obtain depart depending if is a Vehicle, trip or flow
    std::string departStr;
    if (myTagProperty.getTag() == SUMO_TAG_FLOW) {
        departStr = toString(depart);
    } else {
        departStr = getDepart();
    }
    // we need to handle depart as a tuple of 20 numbers (format: 000000...00<departTime>)
    departStr.reserve(20 - departStr.size());
    // add 0s at the beginning of departStr until we have 20 numbers
    for (int i = (int)departStr.size(); i < 20; i++) {
        departStr.insert(departStr.begin(), '0');
    }
    return departStr;
}


const RGBColor &
GNEVehicle::getColor() const {
    return color;
}


void 
GNEVehicle::writeDemandElement(OutputDevice& device) const {
    write(device, OptionsCont::getOptions(), myTagProperty.getTag());
    // write specific attribute depeding of vehicle type
    if (myTagProperty.getTag() == SUMO_TAG_VEHICLE) {
        // write manually route
        device.writeAttr(SUMO_ATTR_ROUTE, myDemandElementParents.at(1)->getID());
    } else if (myTagProperty.getTag() == SUMO_TAG_FLOW) {
        // write flow values depending if it was set
        if (isDisjointAttributeSet(SUMO_ATTR_END)) {
            device.writeAttr(SUMO_ATTR_END,  time2string(repetitionEnd));
        }
        if (isDisjointAttributeSet(SUMO_ATTR_NUMBER)) {
            device.writeAttr(SUMO_ATTR_NUMBER , repetitionNumber);
        }
        if (isDisjointAttributeSet(SUMO_ATTR_VEHSPERHOUR)) {
            device.writeAttr(SUMO_ATTR_VEHSPERHOUR, 3600. / STEPS2TIME(repetitionOffset));
        }
        if (isDisjointAttributeSet(SUMO_ATTR_PERIOD)) {
            device.writeAttr(SUMO_ATTR_PERIOD, time2string(repetitionOffset));
        }
        if (isDisjointAttributeSet(SUMO_ATTR_PROB)) {
            device.writeAttr(SUMO_ATTR_PROB, repetitionProbability);
        }
    } else if (myTagProperty.getTag() == SUMO_TAG_TRIP) {
        // write manually from/to edges
        device.writeAttr(SUMO_ATTR_FROM, myFrom->getID());
        device.writeAttr(SUMO_ATTR_TO, myTo->getID());
        // write via only if there is edges
        if(myVia.size() > 0) {
            device.writeAttr(SUMO_ATTR_VIA, toString(myVia));
        }
    } else {
        throw ProcessError("Invalid vehicle tag");
    }
    // close vehicle tag
    device.closeTag();
}


void
GNEVehicle::moveGeometry(const Position&) {
    // This demand element cannot be moved
}


void
GNEVehicle::commitGeometryMoving(GNEUndoList*) {
    // This demand element cannot be moved
}


void
GNEVehicle::updateGeometry(bool updateGrid) {
    // first check if object has to be removed from grid (SUMOTree)
    if (updateGrid) {
        myViewNet->getNet()->removeGLObjectFromGrid(this);
    }

    // obtain lenght
    const double length = parse<double>(myDemandElementParents.at(0)->getAttribute(SUMO_ATTR_LENGTH)) ;

    // Clear geometry container
    myGeometry.clearGeometry();

    // get lanes of edge
    GNELane* vehicleLane;
    // check if vehicle is defined over a route
    if (myDemandElementParents.size() == 2) {
        vehicleLane = myDemandElementParents.at(1)->getGNEEdges().at(0)->getLanes().at(0);
    } else if (myFrom) {
        vehicleLane = myFrom->getLanes().at(0);
    } else {
        throw ProcessError("Invalid vehicle tag");
    }

    // Get shape of lane parent
    double offset = vehicleLane->getShape().length() < length ? vehicleLane->getShape().length() : length;
    myGeometry.shape.push_back(vehicleLane->getShape().positionAtOffset(offset));

    // Save rotation (angle)
    myGeometry.shapeRotations.push_back(vehicleLane->getShape().rotationDegreeAtOffset(offset) * -1);

    // calculate route for trip
    if (myTagProperty.getTag() == SUMO_TAG_TRIP) {
        // update temporal route
        myTemporalRoute = getRouteCalculatorInstance()->calculateDijkstraRoute(parse<SUMOVehicleClass>(myDemandElementParents.at(0)->getAttribute(SUMO_ATTR_VCLASS)), myFrom, myTo, myVia);

        if (myTemporalRoute.size() > 1) {
            // declare a vector of shapes
            std::vector<PositionVector> multiShape;

            // start with the first lane shape
            multiShape.push_back(myTemporalRoute.front()->getLanes().front().shape);

            // add first shape connection (if exist, in other case leave it empty)
            multiShape.push_back(PositionVector{myTemporalRoute.at(0)->getLanes().front().shape.back(), myTemporalRoute.at(1)->getLanes().front().shape.front()});

            // append shapes of intermediate lanes AND connections (if exist)
            for (int i = 1; i < ((int)myTemporalRoute.size() - 1); i++) {
                // add lane shape
                multiShape.push_back(myTemporalRoute.at(i)->getLanes().front().shape);
                // add empty shape for connection
                multiShape.push_back(PositionVector{myTemporalRoute.at(i)->getLanes().front().shape.back(), myTemporalRoute.at(i + 1)->getLanes().front().shape.front()});
            }

            // append last shape
            multiShape.push_back(myTemporalRoute.back()->getLanes().front().shape);

            // calculate unified shape
            for (auto i : multiShape) {
                myGeometry.shape.append(i);
            }
            myGeometry.shape.removeDoublePoints();
        }
    }

    // last step is to check if object has to be added into grid (SUMOTree) again
    if (updateGrid) {
        myViewNet->getNet()->addGLObjectIntoGrid(this);
    }
}


Position
GNEVehicle::getPositionInView() const {
    // obtain lane depending of edited vehicle type
    GNELane *lane = nullptr;
    if (myDemandElementParents.size() == 2) {
        lane = myDemandElementParents.at(1)->getGNEEdges().at(0)->getLanes().front();
    } else if (myFrom) {
        lane = myFrom->getLanes().front();
    } else {
        throw ProcessError("Invalid vehicle tag");
    }
    if (lane->getShape().length() < 2.5) {
        return lane->getShape().front();
    } else {
        Position A = lane->getShape().positionAtOffset(2.5);
        Position B = lane->getShape().positionAtOffset(2.5);
        // return Middle point
        return Position((A.x() + B.x()) / 2, (A.y() + B.y()) / 2);
    }
}


std::string
GNEVehicle::getParentName() const {
    if (myTagProperty.getTag() == SUMO_TAG_VEHICLE) {
        return myDemandElementParents.at(1)->getID();
    } else if (myTagProperty.getTag() == SUMO_TAG_FLOW) {
        return myDemandElementParents.at(1)->getID();
    } else if (myTagProperty.getTag() == SUMO_TAG_TRIP) {
        return myFrom->getID();
    } else {
        throw ProcessError("Invalid vehicle tag");
    }
}


void
GNEVehicle::drawGL(const GUIVisualizationSettings& s) const {
    // only drawn in super mode demand
    if (myViewNet->getViewOptions().showDemandElements()) {
        // declare common attributes
        const double width = parse<double>(myDemandElementParents.at(0)->getAttribute(SUMO_ATTR_WIDTH));
        const double length = parse<double>(myDemandElementParents.at(0)->getAttribute(SUMO_ATTR_LENGTH));
        SUMOVehicleShape shape = getVehicleShapeID(myDemandElementParents.at(0)->getAttribute(SUMO_ATTR_GUISHAPE));
        // first push name
        glPushName(getGlID());
        // push draw matrix
        glPushMatrix();
        // translate to drawing position
        glTranslated(myGeometry.shape.front().x(), myGeometry.shape.front().y(), getType());
        glRotated(myGeometry.shapeRotations.front(), 0, 0, 1);
        // set lane color
        setColor(s);
        // set scale
        const double upscale = s.vehicleSize.getExaggeration(s, this);
        double upscaleLength = upscale;
        if (upscale > 1 && length > 5) {
            // reduce the length/width ratio because this is not usefull at high zoom
            upscaleLength = MAX2(1.0, upscaleLength * (5 + sqrt(length - 5)) / length);
        }
        glScaled(upscale, upscaleLength, 1);
        // check if we're drawing in selecting mode
        if (s.drawForSelecting) {
            // draw vehicle as a box and don't draw the rest of details
            GUIBaseVehicleHelper::drawAction_drawVehicleAsBoxPlus(width, length);
        } else {
            // draw the vehicle
            switch (s.vehicleQuality) {
                case 0:
                    // in "normal" mode draw vehicle as poly
                    //GUIBaseVehicleHelper::drawAction_drawVehicleAsTrianglePlus(width, length);
                    GUIBaseVehicleHelper::drawAction_drawVehicleAsPoly(s, shape, width, length);
                    break;
                case 1:
                    GUIBaseVehicleHelper::drawAction_drawVehicleAsBoxPlus(width, length);
                    break;
                default:
                    GUIBaseVehicleHelper::drawAction_drawVehicleAsPoly(s, shape, width, length);
                    break;
            }
            // check if min gap has to be drawn
            if (s.drawMinGap) {
                const double minGap = -1 * parse<double>(myDemandElementParents.at(0)->getAttribute(SUMO_ATTR_MINGAP));
                glColor3d(0., 1., 0.);
                glBegin(GL_LINES);
                glVertex2d(0., 0);
                glVertex2d(0., minGap);
                glVertex2d(-.5, minGap);
                glVertex2d(.5, minGap);
                glEnd();
            }
             // drawing name at GLO_MAX fails unless translating z
            glTranslated(0, MIN2(length / 2, double(5)), -getType());
            glScaled(1 / upscale, 1 / upscaleLength, 1);
            glRotated(-1 * myGeometry.shapeRotations.front(), 0, 0, 1);
            drawName(Position(0, 0), s.scale, myDemandElementParents.at(0)->getAttribute(SUMO_ATTR_GUISHAPE) == "pedestrian" ? s.personName : s.vehicleName, s.angle);
            // draw line
            if (s.vehicleName.show && line != "") {
                glTranslated(0, 0.6 * s.vehicleName.scaledSize(s.scale), 0);
                GLHelper::drawTextSettings(s.vehicleName, "line:" + line, Position(0, 0), s.scale, s.angle);
            }
        }
        // pop draw matrix
        glPopMatrix();

        // check if dotted contour has to be drawn
        if (!s.drawForSelecting && (myViewNet->getDottedAC() == this)) {
            GLHelper::drawShapeDottedContour(getType(), myGeometry.shape.front(), width, length, myGeometry.shapeRotations.front(), 0, length/2);
            // check if draw temporal route must be drawed (only for Tags
            if (myTagProperty.getTag() == SUMO_TAG_TRIP) {
                if (myTemporalRoute.size() > 1) {
                    // Add a draw matrix
                    glPushMatrix();
                    // Start with the drawing of the area traslating matrix to origin
                    glTranslated(0, 0, getType() - 0.01);
                    // set orange color
                    GLHelper::setColor(RGBColor::ORANGE);
                    // set line width
                    glLineWidth(5);
                    // draw first line
                    GLHelper::drawLine(myTemporalRoute.at(0)->getLanes().front().shape.front(), 
                                        myTemporalRoute.at(0)->getLanes().front().shape.back());
                    // draw rest of lines
                    for (int i = 1; i < (int)myTemporalRoute.size(); i++) {
                        GLHelper::drawLine(myTemporalRoute.at(i-1)->getLanes().front().shape.back(), 
                                            myTemporalRoute.at(i)->getLanes().front().shape.front());
                        GLHelper::drawLine(myTemporalRoute.at(i)->getLanes().front().shape.front(), 
                                            myTemporalRoute.at(i)->getLanes().front().shape.back());
                    }
                    // Pop last matrix
                    glPopMatrix();
                } else {
                    // Add a draw matrix
                    glPushMatrix();
                    // Start with the drawing of the area traslating matrix to origin
                    glTranslated(0, 0, getType() - 0.01);
                    // set orange color
                    GLHelper::setColor(RGBColor::RED);
                    // set line width
                    glLineWidth(5);
                    // draw first line
                    GLHelper::drawLine(myFrom->getNBEdge()->getLanes().front().shape.front(), myTo->getNBEdge()->getLanes().front().shape.back());
                    // Pop last matrix
                    glPopMatrix();
                }
            }
        }
        // pop name
        glPopName();
    }
}


void
GNEVehicle::selectAttributeCarrier(bool changeFlag) {
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
GNEVehicle::unselectAttributeCarrier(bool changeFlag) {
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
GNEVehicle::getAttribute(SumoXMLAttr key) const {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            return getDemandElementID();
        case SUMO_ATTR_TYPE:
            return myDemandElementParents.at(0)->getID();
        case SUMO_ATTR_COLOR:
            return toString(color);
        case SUMO_ATTR_DEPARTLANE:
            return getDepartLane();
        case SUMO_ATTR_DEPARTPOS:
            return getDepartPos();
        case SUMO_ATTR_DEPARTSPEED:
            return getDepartSpeed();
        case SUMO_ATTR_ARRIVALLANE:
            return getArrivalLane();
        case SUMO_ATTR_ARRIVALPOS:
            return getArrivalPos();
        case SUMO_ATTR_ARRIVALSPEED:
            return getArrivalSpeed();
        case SUMO_ATTR_LINE:
            return line;
        case SUMO_ATTR_PERSON_NUMBER:
            return toString(personNumber);
        case SUMO_ATTR_CONTAINER_NUMBER:
            return toString(containerNumber);
        case SUMO_ATTR_REROUTE:
            return toString("false"); // check
        case SUMO_ATTR_VIA:
            return parseIDs(myVia);
        case SUMO_ATTR_DEPARTPOS_LAT:
            return getDepartPosLat();
        case SUMO_ATTR_ARRIVALPOS_LAT:
            return getArrivalPosLat();
        // Specific of vehicles
        case SUMO_ATTR_DEPART:
            return toString(depart);
        case SUMO_ATTR_ROUTE:
            return myDemandElementParents.at(1)->getID();
        // Specific of Trips
        case SUMO_ATTR_FROM:
            return myFrom->getID();
        case SUMO_ATTR_TO:
            return myTo->getID();
        // Specific of flows
        case SUMO_ATTR_BEGIN:
            return toString(depart);
        case SUMO_ATTR_END:
            return toString(repetitionEnd);
        case SUMO_ATTR_VEHSPERHOUR:
            return toString(repetitionOffset);
        case SUMO_ATTR_PERIOD:
            return toString(repetitionOffset);
        case SUMO_ATTR_PROB:
            return toString(repetitionProbability);
        case SUMO_ATTR_NUMBER:
            return toString(repetitionNumber);
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
GNEVehicle::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_DEPARTLANE:
        case SUMO_ATTR_DEPARTPOS:
        case SUMO_ATTR_DEPARTSPEED:
        case SUMO_ATTR_ARRIVALLANE:
        case SUMO_ATTR_ARRIVALPOS:
        case SUMO_ATTR_ARRIVALSPEED:
        case SUMO_ATTR_LINE:
        case SUMO_ATTR_PERSON_NUMBER:
        case SUMO_ATTR_CONTAINER_NUMBER:
        case SUMO_ATTR_REROUTE:
        case SUMO_ATTR_VIA:
        case SUMO_ATTR_DEPARTPOS_LAT:
        case SUMO_ATTR_ARRIVALPOS_LAT:
        // Specific of vehicles
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_ROUTE:
        // Specific of Trips
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
        //
        // Specific of flows
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
        case SUMO_ATTR_NUMBER:
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERIOD:
        case SUMO_ATTR_PROB:
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
GNEVehicle::isValid(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDemandElementID(value);
        case SUMO_ATTR_TYPE:
            return SUMOXMLDefinitions::isValidTypeID(value) && (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, value, false) != nullptr);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_DEPARTLANE: {
            int dummyDepartLane;
            DepartLaneDefinition dummyDepartLaneProcedure;
            parseDepartLane(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartLane, dummyDepartLaneProcedure, error); 
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_DEPARTPOS: {
            double dummyDepartPos;
            DepartPosDefinition dummyDepartPosProcedure;
            parseDepartPos(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartPos, dummyDepartPosProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_DEPARTSPEED: {
            double dummyDepartSpeed;
            DepartSpeedDefinition dummyDepartSpeedProcedure;
            parseDepartSpeed(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartSpeed, dummyDepartSpeedProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ARRIVALLANE: {
            int dummyArrivalLane;
            ArrivalLaneDefinition dummyArrivalLaneProcedure;
            parseArrivalLane(value, toString(SUMO_TAG_VEHICLE), id, dummyArrivalLane, dummyArrivalLaneProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ARRIVALPOS: {
            double dummyArrivalPos;
            ArrivalPosDefinition dummyArrivalPosProcedure;
            parseArrivalPos(value, toString(SUMO_TAG_VEHICLE), id, dummyArrivalPos, dummyArrivalPosProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ARRIVALSPEED: {
            double dummyArrivalSpeed;
            ArrivalSpeedDefinition dummyArrivalSpeedProcedure;
            parseArrivalSpeed(value, toString(SUMO_TAG_VEHICLE), id, dummyArrivalSpeed, dummyArrivalSpeedProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_LINE:
            return true;
        case SUMO_ATTR_PERSON_NUMBER:
            return canParse<int>(value) && parse<int>(value) >= 0;
        case SUMO_ATTR_CONTAINER_NUMBER:
            return canParse<int>(value) && parse<int>(value) >= 0;
        case SUMO_ATTR_REROUTE:
            return true;    // check
        case SUMO_ATTR_VIA:
            if (value.empty()) {
                return true;
            } else {
                return canParse<std::vector<GNEEdge*> >(myViewNet->getNet(), value, false);
            }
        case SUMO_ATTR_DEPARTPOS_LAT: {
            double dummyDepartPosLat;
            DepartPosLatDefinition dummyDepartPosLatProcedure;
            parseDepartPosLat(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartPosLat, dummyDepartPosLatProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ARRIVALPOS_LAT: {
            double dummyArrivalPosLat;
            ArrivalPosLatDefinition dummyArrivalPosLatProcedure;
            parseArrivalPosLat(value, toString(SUMO_TAG_VEHICLE), id, dummyArrivalPosLat, dummyArrivalPosLatProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        // Specific of vehicles
        case SUMO_ATTR_DEPART: {
            SUMOTime dummyDepart;
            DepartDefinition dummyDepartProcedure;
            parseDepart(value, toString(SUMO_TAG_VEHICLE), id, dummyDepart, dummyDepartProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ROUTE:
            return SUMOXMLDefinitions::isValidVehicleID(value) && (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, value, false) != nullptr);
        // Specific of Trips
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
            return SUMOXMLDefinitions::isValidNetID(value) && (myViewNet->getNet()->retrieveEdge(value, false) != nullptr);
        // Specific of flows
        case SUMO_ATTR_BEGIN:
            if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
        case SUMO_ATTR_END:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
        case SUMO_ATTR_VEHSPERHOUR:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return (parse<double>(value) > 0);
            } else {
                return false;
            }
        case SUMO_ATTR_PERIOD:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return (parse<double>(value) > 0);
            } else {
                return false;
            }
        case SUMO_ATTR_PROB:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
        case SUMO_ATTR_NUMBER:
             if (canParse<int>(value)) {
                return (parse<int>(value) >= 0);
            } else {
                return false;
            }
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
GNEVehicle::isDisjointAttributeSet(const SumoXMLAttr attr) const {
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
GNEVehicle::setDisjointAttribute(const SumoXMLAttr attr, GNEUndoList* undoList) {
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
GNEVehicle::getPopUpID() const {
    return getTagStr();
}


std::string
GNEVehicle::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) ;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEVehicle::setColor(const GUIVisualizationSettings& s) const {
    const GUIColorer& c = s.vehicleColorer;

    switch (c.getActive()) {
        case 0: {
            //test for emergency vehicle
            if (myDemandElementParents.at(0)->getAttribute(SUMO_ATTR_GUISHAPE) == "emergency") {
                GLHelper::setColor(RGBColor::WHITE);
                break;
            }
            //test for firebrigade
            if (myDemandElementParents.at(0)->getAttribute(SUMO_ATTR_GUISHAPE) == "firebrigade") {
                GLHelper::setColor(RGBColor::RED);
                break;
            }
            //test for police car
            if (myDemandElementParents.at(0)->getAttribute(SUMO_ATTR_GUISHAPE) == "police") {
                GLHelper::setColor(RGBColor::BLUE);
                break;
            }
            if (wasSet(VEHPARS_COLOR_SET)) {
                GLHelper::setColor(color);
                break;
            }
            if (myDemandElementParents.at(0)->isDisjointAttributeSet(SUMO_ATTR_COLOR)) {
                GLHelper::setColor(myDemandElementParents.at(0)->getColor());
                break;
            }
            if (&(myDemandElementParents.at(1)->getColor()) != &RGBColor::DEFAULT_COLOR) {
                GLHelper::setColor(myDemandElementParents.at(1)->getColor());
            } else {
                GLHelper::setColor(c.getScheme().getColor(0));
            }
            break;
        }
        case 2: {
            if (wasSet(VEHPARS_COLOR_SET)) {
                GLHelper::setColor(color);
            } else {
                GLHelper::setColor(c.getScheme().getColor(0));
            }
            break;
        }
        case 3: {
            if (myDemandElementParents.at(0)->isDisjointAttributeSet(SUMO_ATTR_COLOR)) {
                GLHelper::setColor(myDemandElementParents.at(0)->getColor());
            } else {
                GLHelper::setColor(c.getScheme().getColor(0));
            }
            break;
        }
        case 4: {
            if (myDemandElementParents.at(1)->getColor() != RGBColor::DEFAULT_COLOR) {
                GLHelper::setColor(myDemandElementParents.at(1)->getColor());
            } else {
                GLHelper::setColor(c.getScheme().getColor(0));
            }
            break;
        }
        case 5: {
            Position p = myDemandElementParents.at(1)->getGNEEdges().at(0)->getLanes().at(0)->getShape()[0];
            const Boundary& b = myViewNet->getNet()->getBoundary();
            Position center = b.getCenter();
            double hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / M_PI;
            double sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            break;
        }
        case 6: {
            Position p = myDemandElementParents.at(1)->getGNEEdges().back()->getLanes().at(0)->getShape()[-1];
            const Boundary& b = myViewNet->getNet()->getBoundary();
            Position center = b.getCenter();
            double hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / M_PI;
            double sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            break;
        }
        case 7: {
            Position pb = myDemandElementParents.at(1)->getGNEEdges().at(0)->getLanes().at(0)->getShape()[0];
            Position pe = myDemandElementParents.at(1)->getGNEEdges().back()->getLanes().at(0)->getShape()[-1];
            const Boundary& b = myViewNet->getNet()->getBoundary();
            double hue = 180. + atan2(pb.x() - pe.x(), pb.y() - pe.y()) * 180. / M_PI;
            Position minp(b.xmin(), b.ymin());
            Position maxp(b.xmax(), b.ymax());
            double sat = pb.distanceTo(pe) / minp.distanceTo(maxp);
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            break;
        }
        case 29: { // color randomly (by pointer hash)
            std::hash<const GNEVehicle*> ptr_hash;
            const double hue = (double)(ptr_hash(this) % 360); // [0-360]
            const double sat = ((ptr_hash(this) / 360) % 67) / 100.0 + 0.33; // [0.33-1]
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            break;
        }
        default: {
            GLHelper::setColor(c.getScheme().getColor(0));
        }
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEVehicle::setAttribute(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            changeDemandElementID(value);
            break;
        case SUMO_ATTR_TYPE:
            changeDemandElementParent(this, value, 0);
            // set manually vtypeID (needed for saving)
            vtypeid = value;
            break;
        case SUMO_ATTR_COLOR:
            color = parse<RGBColor>(value);
            break;  
        case SUMO_ATTR_DEPARTLANE:
            parseDepartLane(value, toString(SUMO_TAG_VEHICLE), id, departLane, departLaneProcedure, error); 
            break;       
        case SUMO_ATTR_DEPARTPOS:
            parseDepartPos(value, toString(SUMO_TAG_VEHICLE), id, departPos, departPosProcedure, error);
            break;
        case SUMO_ATTR_DEPARTSPEED:
            parseDepartSpeed(value, toString(SUMO_TAG_VEHICLE), id, departSpeed, departSpeedProcedure, error);
            break;
        case SUMO_ATTR_ARRIVALLANE:
            parseArrivalLane(value, toString(SUMO_TAG_VEHICLE), id, arrivalLane, arrivalLaneProcedure, error);
            break;
        case SUMO_ATTR_ARRIVALPOS:
            parseArrivalPos(value, toString(SUMO_TAG_VEHICLE), id, arrivalPos, arrivalPosProcedure, error);
            break;
        case SUMO_ATTR_ARRIVALSPEED:
            parseArrivalSpeed(value, toString(SUMO_TAG_VEHICLE), id, arrivalSpeed, arrivalSpeedProcedure, error);
            break;
        case SUMO_ATTR_LINE:
            line = value;
            break;
        case SUMO_ATTR_PERSON_NUMBER:
            personNumber = parse<int>(value);
            break;
        case SUMO_ATTR_CONTAINER_NUMBER:
            containerNumber = parse<int>(value);
            break;
        case SUMO_ATTR_REROUTE:
            // check
            break;
        case SUMO_ATTR_VIA:
            myVia = parse<std::vector<GNEEdge*> >(myViewNet->getNet(), value);
            myTemporalRoute = getRouteCalculatorInstance()->calculateDijkstraRoute(parse<SUMOVehicleClass>(myDemandElementParents.at(0)->getAttribute(SUMO_ATTR_VCLASS)), myFrom, myTo, myVia);
            break;
        case SUMO_ATTR_DEPARTPOS_LAT:
            parseDepartPosLat(value, toString(SUMO_TAG_VEHICLE), id, departPosLat, departPosLatProcedure, error);
            break;
        case SUMO_ATTR_ARRIVALPOS_LAT:
            parseArrivalPosLat(value, toString(SUMO_TAG_VEHICLE), id, arrivalPosLat, arrivalPosLatProcedure, error);
            break;
        // Specific of vehicles
        case SUMO_ATTR_DEPART: {
            std::string oldDepart = getBegin();
            parseDepart(value, toString(SUMO_TAG_VEHICLE), id, depart, departProcedure, error);
            myViewNet->getNet()->updateDemandElementBegin(oldDepart, this);
            break;
        }
        case SUMO_ATTR_ROUTE:
            changeDemandElementParent(this, value, 1);
            break;
        // Specific of Trips
        case SUMO_ATTR_FROM:
            myFrom = myViewNet->getNet()->retrieveEdge(value);
            break;
        case SUMO_ATTR_TO:
            myTo = myViewNet->getNet()->retrieveEdge(value);
            break;
        // Specific of flows
        case SUMO_ATTR_BEGIN: {
            std::string oldBegin = getBegin();
            depart = parse<SUMOTime>(value);
            myViewNet->getNet()->updateDemandElementBegin(oldBegin, this);
            break;
        }
        case SUMO_ATTR_END:
            repetitionEnd = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_VEHSPERHOUR:
            repetitionOffset = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_PERIOD:
            repetitionOffset = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_PROB:
            repetitionProbability = parse<double>(value);
            break;
        case SUMO_ATTR_NUMBER:
            repetitionNumber = parse<int>(value);
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

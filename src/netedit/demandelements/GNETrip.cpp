/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNETrip.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// Representation of Trips in NETEDIT
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

#include "GNETrip.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNETrip::GNETrip(GNEViewNet* viewNet, const std::string &tripID, GNEDemandElement* vehicleType, GNEEdge* from, GNEEdge* to, std::vector<GNEEdge*> viaEdges) : 
    GNEDemandElement(tripID, viewNet, GLO_TRIP, SUMO_TAG_TRIP),
    SUMOVehicleParameter(),
    myVehicleType(vehicleType),
    myFrom(from),
    myTo(to),
    myVia(viaEdges) {
    // calculate temporal route
    myTemporalRoute = getRouteCalculatorInstance()->calculateDijkstraRoute(parse<SUMOVehicleClass>(myVehicleType->getAttribute(SUMO_ATTR_VCLASS)), myFrom, myTo, myVia);
}


GNETrip::GNETrip(GNEViewNet* viewNet, const SUMOVehicleParameter &tripParameter, GNEDemandElement* vehicleType, GNEEdge* from, GNEEdge* to, std::vector<GNEEdge*> viaEdges) :
    GNEDemandElement(tripParameter.id, viewNet, GLO_TRIP, SUMO_TAG_TRIP),
    SUMOVehicleParameter(tripParameter),
    myVehicleType(vehicleType),
    myFrom(from),
    myTo(to),
    myVia(viaEdges) {
    // calculate temporal route
    myTemporalRoute = getRouteCalculatorInstance()->calculateDijkstraRoute(parse<SUMOVehicleClass>(myVehicleType->getAttribute(SUMO_ATTR_VCLASS)), myFrom, myTo, myVia);
}


GNETrip::~GNETrip() {}


std::string 
GNETrip::getBegin() const {
    // obtain begin
    std::string beginStr = toString(depart);
    // we need to handle begin as a tuple of 20 numbers (format: 000000...00<beginTime>)
    beginStr.reserve(20 - beginStr.size());
    // add 0s at the beginning of beginStr until we have 20 numbers
    for (int i = (int)beginStr.size(); i < 20; i++) {
        beginStr.insert(beginStr.begin(), '0');
    }
    return beginStr;
}


const RGBColor &
GNETrip::getColor() const {
    return color;
}


void 
GNETrip::writeDemandElement(OutputDevice& device) const {
    write(device, OptionsCont::getOptions(), SUMO_TAG_TRIP);
    // write manually from/to edges
    device.writeAttr(SUMO_ATTR_FROM, myFrom->getID());
    device.writeAttr(SUMO_ATTR_TO, myTo->getID());
    // write via only if there is edges
    if(myVia.size() > 0) {
        device.writeAttr(SUMO_ATTR_VIA, toString(myVia));
    }
    device.closeTag();
}


void
GNETrip::moveGeometry(const Position&) {
    // This demand element cannot be moved
}


void
GNETrip::commitGeometryMoving(GNEUndoList*) {
    // This demand element cannot be moved
}


void
GNETrip::updateGeometry(bool updateGrid) {
    // first check if object has to be removed from grid (SUMOTree)
    if (updateGrid) {
        myViewNet->getNet()->removeGLObjectFromGrid(this);
    }

    // obtain lenght
    const double length = parse<double>(myVehicleType->getAttribute(SUMO_ATTR_LENGTH)) ;

    // Clear geometry container
    myGeometry.clearGeometry();

    // get lanes of edge
    GNELane* vehicleLane = myFrom->getLanes().at(0);

    // Get shape of lane parent
    double offset = vehicleLane->getShape().length() < length ? vehicleLane->getShape().length() : length;
    myGeometry.shape.push_back(vehicleLane->getShape().positionAtOffset(offset));

    // Save rotation (angle)
    myGeometry.shapeRotations.push_back(vehicleLane->getShape().rotationDegreeAtOffset(offset) * -1);

    // start with the first lane shape
    myGeometry.multiShape.push_back(myTemporalRoute.front()->getLanes().front().shape);

    // add first shape connection (if exist, in other case leave it empty)
    myGeometry.multiShape.push_back(PositionVector{myTemporalRoute.at(0)->getLanes().front().shape.back(), myTemporalRoute.at(1)->getLanes().front().shape.front()});

    // append shapes of intermediate lanes AND connections (if exist)
    for (int i = 1; i < ((int)myTemporalRoute.size() - 1); i++) {
        // add lane shape
        myGeometry.multiShape.push_back(myTemporalRoute.at(i)->getLanes().front().shape);
        // add empty shape for connection
        myGeometry.multiShape.push_back(PositionVector{myTemporalRoute.at(i)->getLanes().front().shape.back(), myTemporalRoute.at(i + 1)->getLanes().front().shape.front()});
    }

    // append last shape
    myGeometry.multiShape.push_back(myTemporalRoute.back()->getLanes().front().shape);

    // calculate unified shape
    myGeometry.calculateMultiShapeUnified();

    // last step is to check if object has to be added into grid (SUMOTree) again
    if (updateGrid) {
        myViewNet->getNet()->addGLObjectIntoGrid(this);
    }
}


Position
GNETrip::getPositionInView() const {
    if (myFrom->getLanes().front()->getShape().length() < 2.5) {
        return myFrom->getLanes().front()->getShape().front();
    } else {
        Position A = myFrom->getLanes().front()->getShape().positionAtOffset(2.5);
        Position B = myFrom->getLanes().back()->getShape().positionAtOffset(2.5);
        // return Middle point
        return Position((A.x() + B.x()) / 2, (A.y() + B.y()) / 2);
    }
}


std::string
GNETrip::getParentName() const {
    return myFrom->getID();
}


void
GNETrip::drawGL(const GUIVisualizationSettings& s) const {
    // only drawn in super mode demand
    if (myViewNet->getViewOptions().showDemandElements()) {
        // declare common attributes
        const double width = parse<double>(myVehicleType->getAttribute(SUMO_ATTR_WIDTH));
        const double length = parse<double>(myVehicleType->getAttribute(SUMO_ATTR_LENGTH));
        SUMOVehicleShape shape = getVehicleShapeID(myVehicleType->getAttribute(SUMO_ATTR_GUISHAPE));
        // first push name
        glPushName(getGlID());
        // push draw matrix
        glPushMatrix();
        // translate to drawing position
        glTranslated(myGeometry.shape[0].x(), myGeometry.shape[0].y(), getType());
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
                const double minGap = -1 * parse<double>(myVehicleType->getAttribute(SUMO_ATTR_MINGAP));
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
            drawName(Position(0, 0), s.scale, myVehicleType->getAttribute(SUMO_ATTR_GUISHAPE) == "pedestrian" ? s.personName : s.vehicleName, s.angle);
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
            GLHelper::drawShapeDottedContour(getType(), myGeometry.shape[0], width, length, myGeometry.shapeRotations[0], 0, length/2);
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
        }
        // pop name
        glPopName();
    }
}


void
GNETrip::selectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.select(dynamic_cast<GUIGlObject*>(this)->getGlID());
        // add object of list into selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->addedLockedObject(GLO_TRIP);
        if (changeFlag) {
            mySelected = true;
        }
    }
}


void
GNETrip::unselectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.deselect(dynamic_cast<GUIGlObject*>(this)->getGlID());
        // remove object of list of selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->removeLockedObject(GLO_TRIP);
        if (changeFlag) {
            mySelected = false;

        }
    }
}


std::string
GNETrip::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getDemandElementID();
        case SUMO_ATTR_TYPE:
            return myVehicleType->getID();
        case SUMO_ATTR_FROM:
            return myFrom->getID();
        case SUMO_ATTR_TO:
            return myTo->getID();
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
            return toString("0"); // check
        case SUMO_ATTR_VIA:
            return parseIDs(myVia);
        case SUMO_ATTR_DEPARTPOS_LAT:
            return getDepartPosLat();
        case SUMO_ATTR_ARRIVALPOS_LAT:
            return getArrivalPosLat();
        // Specific of trips
        case SUMO_ATTR_DEPART:
            return toString(depart);
        //
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNETrip::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_ROUTE:
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
        // Specific of trips
        case SUMO_ATTR_DEPART:
        //
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNETrip::isValid(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDemandElementID(value);
        case SUMO_ATTR_TYPE:
            return SUMOXMLDefinitions::isValidTypeID(value) && (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, value, false) != nullptr);
        case SUMO_ATTR_FROM:
            return SUMOXMLDefinitions::isValidNetID(value) && (myViewNet->getNet()->retrieveEdge(value, false) != nullptr);
        case SUMO_ATTR_TO:
            return SUMOXMLDefinitions::isValidNetID(value) && (myViewNet->getNet()->retrieveEdge(value, false) != nullptr);
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
        // Specific of trips
        case SUMO_ATTR_DEPART: {
            SUMOTime dummyDepart;
            DepartDefinition dummyDepartProcedure;
            parseDepart(value, toString(SUMO_TAG_VEHICLE), id, dummyDepart, dummyDepartProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        //
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNETrip::getPopUpID() const {
    return getTagStr();
}


std::string
GNETrip::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) ;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNETrip::setColor(const GUIVisualizationSettings& s) const {
    const GUIColorer& c = s.vehicleColorer;

    switch (c.getActive()) {
        case 0: {
            //test for emergency vehicle
            if (myVehicleType->getAttribute(SUMO_ATTR_GUISHAPE) == "emergency") {
                GLHelper::setColor(RGBColor::WHITE);
                break;
            }
            //test for firebrigade
            if (myVehicleType->getAttribute(SUMO_ATTR_GUISHAPE) == "firebrigade") {
                GLHelper::setColor(RGBColor::RED);
                break;
            }
            //test for police car
            if (myVehicleType->getAttribute(SUMO_ATTR_GUISHAPE) == "police") {
                GLHelper::setColor(RGBColor::BLUE);
                break;
            }
            if (wasSet(VEHPARS_COLOR_SET)) {
                GLHelper::setColor(color);
                break;
            }
            if (myVehicleType->isDisjointAttributeSet(SUMO_ATTR_COLOR)) {
                GLHelper::setColor(myVehicleType->getColor());
                break;
            }
            GLHelper::setColor(c.getScheme().getColor(0));
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
            if (myVehicleType->isDisjointAttributeSet(SUMO_ATTR_COLOR)) {
                GLHelper::setColor(myVehicleType->getColor());
            } else {
                GLHelper::setColor(c.getScheme().getColor(0));
            }
            break;
        }
        case 4: {
            GLHelper::setColor(c.getScheme().getColor(0));
            break;
        }
        case 5: {
            Position p = myFrom->getLanes().at(0)->getShape()[0];
            const Boundary& b = myViewNet->getNet()->getBoundary();
            Position center = b.getCenter();
            double hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / M_PI;
            double sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            break;
        }
        case 6: {
            Position p = myFrom->getLanes().at(0)->getShape()[-1];
            const Boundary& b = myViewNet->getNet()->getBoundary();
            Position center = b.getCenter();
            double hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / M_PI;
            double sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            break;
        }
        case 7: {
            Position pb = myFrom->getLanes().at(0)->getShape()[0];
            Position pe = myFrom->getLanes().at(0)->getShape()[-1];
            const Boundary& b = myViewNet->getNet()->getBoundary();
            double hue = 180. + atan2(pb.x() - pe.x(), pb.y() - pe.y()) * 180. / M_PI;
            Position minp(b.xmin(), b.ymin());
            Position maxp(b.xmax(), b.ymax());
            double sat = pb.distanceTo(pe) / minp.distanceTo(maxp);
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            break;
        }
        case 29: { // color randomly (by pointer hash)
            std::hash<const GNETrip*> ptr_hash;
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
GNETrip::setAttribute(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            changeDemandElementID(value);
            break;
        case SUMO_ATTR_TYPE:
            myVehicleType = myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, value);
            break;
        case SUMO_ATTR_FROM:
            myFrom = myViewNet->getNet()->retrieveEdge(value);
            myTemporalRoute = getRouteCalculatorInstance()->calculateDijkstraRoute(parse<SUMOVehicleClass>(myVehicleType->getAttribute(SUMO_ATTR_VCLASS)), myFrom, myTo, myVia);
            break;
        case SUMO_ATTR_TO:
            myTo = myViewNet->getNet()->retrieveEdge(value);
            myTemporalRoute = getRouteCalculatorInstance()->calculateDijkstraRoute(parse<SUMOVehicleClass>(myVehicleType->getAttribute(SUMO_ATTR_VCLASS)), myFrom, myTo, myVia);
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
            myTemporalRoute = getRouteCalculatorInstance()->calculateDijkstraRoute(parse<SUMOVehicleClass>(myVehicleType->getAttribute(SUMO_ATTR_VCLASS)), myFrom, myTo, myVia);
            break;
        case SUMO_ATTR_DEPARTPOS_LAT:
            parseDepartPosLat(value, toString(SUMO_TAG_VEHICLE), id, departPosLat, departPosLatProcedure, error);
            break;
        case SUMO_ATTR_ARRIVALPOS_LAT:
            parseArrivalPosLat(value, toString(SUMO_TAG_VEHICLE), id, arrivalPosLat, arrivalPosLatProcedure, error);
            break;
        // Specific of trips
        case SUMO_ATTR_DEPART: {
            std::string oldDepart = getBegin();
            parseDepart(value, toString(SUMO_TAG_VEHICLE), id, depart, departProcedure, error);
            myViewNet->getNet()->updateDemandElementBegin(oldDepart, this);
            break;
        }
        //
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/

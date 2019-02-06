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
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIGlobals.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSJunction.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/devices/MSDevice_BTreceiver.h>
#include <microsim/devices/MSDevice_Transportable.h>
#include <microsim/devices/MSDevice_Vehroutes.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <netedit/GNENet.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <string>
#include <utils/common/StringUtils.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GLObjectValuePassConnector.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/options/OptionsCont.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <vector>

#include "GNEVehicle.h"

// ===========================================================================
// data definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * drawed shapes
 * ----------------------------------------------------------------------- */
double vehiclePoly_PassengerCarBody[] = { .5, 0,  0, 0,  0, .3,  0.08, .44,  0.25, .5,  0.95, .5,  1., .4,  1., -.4,  0.95, -.5,  0.25, -.5,  0.08, -.44,  0, -.3,  0, 0,  -10000 };
double vehiclePoly_PassengerCarBodyFront[] = { 0.1, 0,  0.025, 0,  0.025, 0.25,  0.27, 0.4,  0.27, -.4,  0.025, -0.25,  0.025, 0,  -10000 };
double vehiclePoly_PassengerFrontGlass[] = { 0.35, 0,  0.3, 0,  0.3, 0.4,  0.43, 0.3,  0.43, -0.3,  0.3, -0.4,  0.3, 0,  -10000 };
double vehiclePoly_PassengerSedanRightGlass[] = { 0.36, -.43,  0.34, -.47,  0.77, -.47,  0.67, -.37,  0.45, -.37,  0.34, -.47,  -10000 };
double vehiclePoly_PassengerSedanLeftGlass[] = { 0.36, .43,  0.34, .47,  0.77, .47,  0.67, .37,  0.45, .37,  0.34, .47,  -10000 };
double vehiclePoly_PassengerSedanBackGlass[] = { 0.80, 0,  0.70, 0,  0.70, 0.3,  0.83, 0.4,  0.83, -.4,  0.70, -.3,  0.70, 0,  -10000 };
double vehiclePoly_PassengerHatchbackRightGlass[] = { 0.36, -.43,  0.34, -.47,  0.94, -.47,  0.80, -.37,  0.45, -.37,  0.34, -.47,  -10000 };
double vehiclePoly_PassengerHatchbackLeftGlass[] = { 0.36, .43,  0.34, .47,  0.94, .47,  0.80, .37,  0.45, .37,  0.34, .47,  -10000 };
double vehiclePoly_PassengerHatchbackBackGlass[] = { 0.92, 0,  0.80, 0,  0.80, 0.3,  0.95, 0.4,  0.95, -.4,  0.80, -.3,  0.80, 0,  -10000 };
double vehiclePoly_PassengerWagonRightGlass[] = { 0.36, -.43,  0.34, -.47,  0.94, -.47,  0.87, -.37,  0.45, -.37,  0.34, -.47,  -10000 };
double vehiclePoly_PassengerWagonLeftGlass[] = { 0.36, .43,  0.34, .47,  0.94, .47,  0.87, .37,  0.45, .37,  0.34, .47,  -10000 };
double vehiclePoly_PassengerWagonBackGlass[] = { 0.92, 0,  0.90, 0,  0.90, 0.3,  0.95, 0.4,  0.95, -.4,  0.90, -.3,  0.90, 0,  -10000 };

double vehiclePoly_PassengerVanBody[] = { .5, 0,  0, 0,  0, .4,  0.1, .5,  0.97, .5,  1., .47,  1., -.47,  0.97, -.5,  0.1, -.5,  0, -.4,  0, 0,  -10000 };
double vehiclePoly_PassengerVanBodyFront[] = { 0.1, 0,  0.025, 0,  0.025, 0.25,  0.13, 0.4,  0.13, -.4,  0.025, -0.25,  0.025, 0,  -10000 };
double vehiclePoly_PassengerVanFrontGlass[] = { 0.21, 0,  0.16, 0,  0.16, 0.4,  0.29, 0.3,  0.29, -0.3,  0.16, -0.4,  0.16, 0,  -10000 };
double vehiclePoly_PassengerVanRightGlass[] = { 0.36, -.43,  0.20, -.47,  0.98, -.47,  0.91, -.37,  0.31, -.37,  0.20, -.47,  -10000 };
double vehiclePoly_PassengerVanLeftGlass[] = { 0.36, .43,  0.20, .47,  0.98, .47,  0.91, .37,  0.31, .37,  0.20, .47,  -10000 };
double vehiclePoly_PassengerVanBackGlass[] = { 0.95, 0,  0.94, 0,  0.94, 0.3,  0.98, 0.4,  0.98, -.4,  0.94, -.3,  0.94, 0,  -10000 };

double vehiclePoly_DeliveryMediumRightGlass[] = { 0.21, -.43,  0.20, -.47,  0.38, -.47,  0.38, -.37,  0.31, -.37,  0.20, -.47,  -10000 };
double vehiclePoly_DeliveryMediumLeftGlass[] = { 0.21, .43,  0.20, .47,  0.38, .47,  0.38, .37,  0.31, .37,  0.20, .47,  -10000 };

double vehiclePoly_TransportBody[] = { .5, 0,  0, 0,  0, .45,  0.05, .5,  2.25, .5,  2.25, -.5,  0.05, -.5,  0, -.45,  0, 0,  -10000 };
double vehiclePoly_TransportFrontGlass[] = { 0.1, 0,  0.05, 0,  0.05, 0.45,  0.25, 0.4,  0.25, -.4,  0.05, -0.45,  0.05, 0,  -10000 };
double vehiclePoly_TransportRightGlass[] = { 0.36, -.47,  0.10, -.48,  1.25, -.48,  1.25, -.4,  0.3, -.4,  0.10, -.48,  -10000 };
double vehiclePoly_TransportLeftGlass[] = { 0.36, .47,  0.10, .48,  1.25, .48,  1.25, .4,  0.3, .4,  0.10, .48,  -10000 };

double vehiclePoly_EVehicleBody[] = { .5, 0,  0, 0,  0, .3,  0.08, .44,  0.25, .5,  0.75, .5,  .92, .44,  1, .3,  1, -.3,  .92, -.44,  .75, -.5,  .25, -.5,  0.08, -.44,  0, -.3,  0, 0,  -1000 };
double vehiclePoly_EVehicleFrontGlass[] = { .5, 0,  0.05, .05,  0.05, .25,  0.13, .39,  0.3, .45,  0.70, .45,  .87, .39,  .95, .25,  .95, -.25,  .87, -.39,  .70, -.45,  .3, -.45,  0.13, -.39,  0.05, -.25,  0.05, 0.05,  -1000 };
//double vehiclePoly_EVehicleFrontGlass[] = { 0.35,0,  0.1,0,  0.1,0.4,  0.43,0.3,  0.43,-0.3,  0.1,-0.4,  0.1,0,  -10000 };
double vehiclePoly_EVehicleBackGlass[] =  { 0.65, 0,  0.9, 0,  0.9, 0.4,  0.57, 0.3,  0.57, -0.3,  0.9, -0.4,  0.9, 0,  -10000 };

double vehiclePoly_Ship[] =  { 0.25, 0,  0, 0,  0.1, 0.25, 0.2, 0.45, 0.25, 0.5,  0.95, 0.5, 1.0, 0.45,   1.0, -0.45, 0.95, -0.5,  0.25, -0.5, 0.2, -0.45,  0.1, -0.25, 0, 0,   -10000 };
double vehiclePoly_ShipDeck[] =  { 0.5, 0,  0.25, 0.4,  0.95, 0.4, 0.95, -0.4, 0.25, -0.4, 0.25, 0.4, -10000 };
double vehiclePoly_ShipSuperStructure[] =  { 0.8, 0,  0.5, 0.3,  0.85, 0.3,  0.85, -0.3, 0.5, -0.3,  0.5, 0.3,  -10000 };

double vehiclePoly_Cyclist[] =  { 0.5, 0,  0.25, 0.45,  0.25, 0.5, 0.8, 0.15,     0.8, -0.15, 0.25, -0.5, 0.25, -0.45,     -10000 };

double vehiclePoly_EmergencySign[] =   { .2, .5,  -.2, .5,  -.2, -.5,  .2, -.5, -10000 };
double vehiclePoly_Emergency[] =   { .1, .1,  -.1, .1,  -.1, -.1,  .1, -.1, -10000 };
double vehiclePoly_Emergency2[] =   { .04, .3,  -.04, .3,  -.04, -.3,  .04, -.3, -10000 };

double vehiclePoly_EmergencyLadder[] =   { -.5, .3, .5, .3, .5, .2, -.5, .2, -10000 };
double vehiclePoly_EmergencyLadder2[] =   { -.5, -.3, .5, -.3, .5, -.2, -.5, -.2, -10000 };
double vehiclePoly_EmergencyLadder3[] =   { -.45, .3, -.4, .3, -.4, -.3, -.45, -.3, -10000 };
double vehiclePoly_EmergencyLadder4[] =   { .45, .3, .4, .3, .4, -.3, .45, -.3, -10000 };
double vehiclePoly_EmergencyLadder5[] =   { .05, .3, .0, .3, .0, -.3, .05, -.3, -10000 };
double vehiclePoly_EmergencyLadder6[] =   { -.25, .3, -.2, .3, -.2, -.3, -.25, -.3, -10000 };
double vehiclePoly_EmergencyLadder7[] =   { .25, .3, .2, .3, .2, -.3, .25, -.3, -10000 };

double vehiclePoly_Rickshaw[] = { 0.5, 0,  0.25, 0.45,  0.25, 0.5, 0.8, 0.15,     0.8, -0.15, 0.25, -0.5, 0.25, -0.45,     -10000 };

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVehicle::GNEVehicle(GNEViewNet* viewNet, const std::string &vehicleID, GNEDemandElement* vehicleType, GNEDemandElement* route) :
    GNEDemandElement(vehicleID, viewNet, GLO_VEHICLE, SUMO_TAG_VEHICLE),
    SUMOVehicleParameter(),
    myVehicleType(vehicleType),
    myRoute(route) {
    // SUMOVehicleParameter ID has to be set manually
    id = vehicleID;
}


GNEVehicle::GNEVehicle(GNEViewNet* viewNet, const SUMOVehicleParameter &vehicleParameter, GNEDemandElement* vehicleType, GNEDemandElement* route) :
    GNEDemandElement(vehicleParameter.id, viewNet, GLO_VEHICLE, SUMO_TAG_VEHICLE),
    SUMOVehicleParameter(vehicleParameter),
    myVehicleType(vehicleType),
    myRoute(route) {
    // SUMOVehicleParameter ID has to be set manually
    id = vehicleParameter.id;
}


GNEVehicle::~GNEVehicle() {}


bool 
GNEVehicle::wasSet(int what) const {
    return (parametersSet & what) != 0;
}


const RGBColor &
GNEVehicle::getColor() const {
    return color;
}


void 
GNEVehicle::writeDemandElement(OutputDevice& device) const {
    write(device, OptionsCont::getOptions(), SUMO_TAG_VEHICLE);
    // write manually type and route
    device.writeAttr(SUMO_ATTR_TYPE, myVehicleType->getID());
    device.writeAttr(SUMO_ATTR_ROUTE, myRoute->getID());
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
GNEVehicle::updateGeometry(bool /*updateGrid*/) {
    // Currently this demand element doesn't own a Geometry
}


Position
GNEVehicle::getPositionInView() const {
    return myFirstDemandElementParent->getPositionInView();
}


std::string
GNEVehicle::getParentName() const {
    return myFirstDemandElementParent->getID();
}


void
GNEVehicle::drawGL(const GUIVisualizationSettings& s) const {
    glPushName(getGlID());
    glPushMatrix();
    Position p1(0, 0)/* = pos*/;
    const double degAngle = 0; // RAD2DEG(angle + M_PI / 2.);
    const double length = parse<double>(myVehicleType->getAttribute(SUMO_ATTR_LENGTH));
    glTranslated(p1.x(), p1.y(), getType());
    glRotated(degAngle, 0, 0, 1);
    // set lane color
    setColor(s);
    // scale
    const double upscale = s.vehicleSize.getExaggeration(s, this);
    double upscaleLength = upscale;
    if (upscale > 1 && length > 5) {
        // reduce the length/width ratio because this is not usefull at high zoom
        upscaleLength = MAX2(1.0, upscaleLength * (5 + sqrt(length - 5)) / length);
    }
    glScaled(upscale, upscaleLength, 1);
    // draw the vehicle
    bool drawCarriages = false;
    switch (s.vehicleQuality) {
        case 0:
            drawAction_drawVehicleAsTrianglePlus();
            break;
        case 1:
            drawAction_drawVehicleAsBoxPlus();
            break;
        default:
            drawCarriages = drawAction_drawVehicleAsPoly(s);
            break;
    }
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
    glTranslated(0, MIN2(length / 2, double(5)), -getType()); // drawing name at GLO_MAX fails unless translating z
    glScaled(1 / upscale, 1 / upscaleLength, 1);
    glRotated(-degAngle, 0, 0, 1);
    drawName(Position(0, 0), s.scale, myVehicleType->getAttribute(SUMO_ATTR_GUISHAPE) == "pedestrian" ? s.personName : s.vehicleName, s.angle);
    if (s.vehicleName.show && line != "") {
        glTranslated(0, 0.6 * s.vehicleName.scaledSize(s.scale), 0);
        GLHelper::drawTextSettings(s.vehicleName, "line:" + line, Position(0, 0), s.scale, s.angle);
    }
    glPopMatrix();
    glPopName();
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
            return myVehicleType->getID();
        case SUMO_ATTR_ROUTE:
            return myRoute->getID();
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
            return toString(""); // check
        case SUMO_ATTR_DEPARTPOS_LAT:
            return getDepartPosLat();
        case SUMO_ATTR_ARRIVALPOS_LAT:
            return getArrivalPosLat();
        // Specific of vehicles
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
GNEVehicle::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
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
        // Specific of vehicles
        case SUMO_ATTR_DEPART:
        //
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
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
        case SUMO_ATTR_ROUTE:
            return SUMOXMLDefinitions::isValidVehicleID(value) && (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, value, false) != nullptr);
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
            return true;    // check
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
        //
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNEVehicle::getPopUpID() const {
    return getTagStr();
}


std::string
GNEVehicle::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_BEGIN) + " -> " + getAttribute(SUMO_ATTR_END);
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
            if (myVehicleType->wasSet(VTYPEPARS_COLOR_SET)) {
                GLHelper::setColor(myVehicleType->getColor());
                break;
            }
            if (&(myRoute->getColor()) != &RGBColor::DEFAULT_COLOR) {
                GLHelper::setColor(myRoute->getColor());
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
            if (myVehicleType->wasSet(VTYPEPARS_COLOR_SET)) {
                GLHelper::setColor(myVehicleType->getColor());
            } else {
                GLHelper::setColor(c.getScheme().getColor(0));
            }
            break;
        }
        case 4: {
            if (myRoute->getColor() != RGBColor::DEFAULT_COLOR) {
                GLHelper::setColor(myRoute->getColor());
            } else {
                GLHelper::setColor(c.getScheme().getColor(0));
            }
            break;
        }
        case 5: {
            Position p = myRoute->getGNEEdges().at(0)->getLanes().at(0)->getShape()[0];
            const Boundary& b = myViewNet->getNet()->getBoundary();
            Position center = b.getCenter();
            double hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / M_PI;
            double sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            break;
        }
        case 6: {
            Position p = myRoute->getGNEEdges().back()->getLanes().at(0)->getShape()[-1];
            const Boundary& b = myViewNet->getNet()->getBoundary();
            Position center = b.getCenter();
            double hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / M_PI;
            double sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            break;
        }
        case 7: {
            Position pb = myRoute->getGNEEdges().at(0)->getLanes().at(0)->getShape()[0];
            Position pe = myRoute->getGNEEdges().back()->getLanes().at(0)->getShape()[-1];
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


void
GNEVehicle::drawPoly(double* poses, double offset) const {
    glPushMatrix();
    glTranslated(0, 0, offset * .1);
    glPolygonOffset(0, (GLfloat) - offset);
    glBegin(GL_TRIANGLE_FAN);
    int i = 0;
    while (poses[i] > -999) {
        glVertex2d(poses[i], poses[i + 1]);
        i = i + 2;
    }
    glEnd();
    glPopMatrix();
}


void
GNEVehicle::drawAction_drawVehicleAsBoxPlus() const {
    glPushMatrix();
    glScaled(parse<double>(myVehicleType->getAttribute(SUMO_ATTR_WIDTH)), parse<double>(myVehicleType->getAttribute(SUMO_ATTR_LENGTH)), 1.);
    glBegin(GL_TRIANGLE_STRIP);
    glVertex2d(0., 0.);
    glVertex2d(-.5, .15);
    glVertex2d(.5, .15);
    glVertex2d(-.5, 1.);
    glVertex2d(.5, 1.);
    glEnd();
    glPopMatrix();
}


void
GNEVehicle::drawAction_drawVehicleAsTrianglePlus() const {
    const double length = parse<double>(myVehicleType->getAttribute(SUMO_ATTR_LENGTH));
    if (length >= 8.) {
        drawAction_drawVehicleAsBoxPlus();
        return;
    }
    glPushMatrix();
    glScaled(parse<double>(myVehicleType->getAttribute(SUMO_ATTR_WIDTH)), length, 1.);
    glBegin(GL_TRIANGLES);
    glVertex2d(0., 0.);
    glVertex2d(-.5, 1.);
    glVertex2d(.5, 1.);
    glEnd();
    glPopMatrix();
}



bool
GNEVehicle::drawAction_drawVehicleAsPoly(const GUIVisualizationSettings& s) const {
    RGBColor current = GLHelper::getColor();
    RGBColor lighter = current.changedBrightness(51);
    RGBColor darker = current.changedBrightness(-51);

    const double length = parse<double>(myVehicleType->getAttribute(SUMO_ATTR_LENGTH));
    const double width = parse<double>(myVehicleType->getAttribute(SUMO_ATTR_WIDTH));
    glPushMatrix();
    glRotated(90, 0, 0, 1);
    glScaled(length, width, 1.);
    SUMOVehicleShape shape = getVehicleShapeID(myVehicleType->getAttribute(SUMO_ATTR_GUISHAPE));
    bool drawCarriages = false;

    // draw main body
    switch (shape) {
        case SVS_UNKNOWN:
            drawPoly(vehiclePoly_PassengerCarBody, 4);
            GLHelper::setColor(lighter);
            drawPoly(vehiclePoly_PassengerCarBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerFrontGlass, 4.5);
            break;
        case SVS_PEDESTRIAN:
            glTranslated(0, 0, .045);
            GLHelper::drawFilledCircle(0.25);
            glTranslated(0, 0, -.045);
            glScaled(.7, 2, 1);
            glTranslated(0, 0, .04);
            GLHelper::setColor(lighter);
            GLHelper::drawFilledCircle(0.25);
            glTranslated(0, 0, -.04);
            break;
        case SVS_BICYCLE:
        case SVS_MOPED:
        case SVS_MOTORCYCLE: {
            RGBColor darker = current.changedBrightness(-50);
            // body
            drawPoly(vehiclePoly_Cyclist, 4);
            // head
            glPushMatrix();
            glTranslated(0.4, 0, .5);
            glScaled(0.1, 0.2, 1);
            GLHelper::setColor(darker);
            GLHelper::drawFilledCircle(1);
            glPopMatrix();
            // bike frame
            GLHelper::setColor(RGBColor::GREY);
            glPushMatrix();
            glTranslated(0.5, 0, .3);
            glScaled(0.5, 0.05, 1);
            GLHelper::drawFilledCircle(1);
            glPopMatrix();
            // handle bar
            glPushMatrix();
            glTranslated(0.25, 0, .3);
            glScaled(0.02, 0.5, 1);
            GLHelper::drawFilledCircle(1);
            glPopMatrix();
        }
        break;
        case SVS_PASSENGER:
        case SVS_PASSENGER_SEDAN:
        case SVS_PASSENGER_HATCHBACK:
        case SVS_PASSENGER_WAGON:
            drawPoly(vehiclePoly_PassengerCarBody, 4);
            GLHelper::setColor(lighter);
            drawPoly(vehiclePoly_PassengerCarBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerFrontGlass, 4.5);
            break;
        case SVS_PASSENGER_VAN:
            drawPoly(vehiclePoly_PassengerVanBody, 4);
            GLHelper::setColor(lighter);
            drawPoly(vehiclePoly_PassengerVanBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerVanFrontGlass, 4.5);
            drawPoly(vehiclePoly_PassengerVanRightGlass, 4.5);
            drawPoly(vehiclePoly_PassengerVanLeftGlass, 4.5);
            drawPoly(vehiclePoly_PassengerVanBackGlass, 4.5);
            break;
        case SVS_DELIVERY:
            drawPoly(vehiclePoly_PassengerVanBody, 4);
            GLHelper::setColor(lighter);
            drawPoly(vehiclePoly_PassengerVanBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerVanFrontGlass, 4.5);
            drawPoly(vehiclePoly_DeliveryMediumRightGlass, 4.5);
            drawPoly(vehiclePoly_DeliveryMediumLeftGlass, 4.5);
            break;
        case SVS_TRUCK:
        case SVS_TRUCK_SEMITRAILER:
        case SVS_TRUCK_1TRAILER:
            glScaled(1. / (length), 1, 1.);
            drawPoly(vehiclePoly_TransportBody, 4);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_TransportFrontGlass, 4.5);
            drawPoly(vehiclePoly_TransportRightGlass, 4.5);
            drawPoly(vehiclePoly_TransportLeftGlass, 4.5);
            break;
        case SVS_BUS:
        case SVS_BUS_COACH:
        case SVS_BUS_TROLLEY: {
            double ml = length;
            glScaled(1. / (length), 1, 1.);
            glTranslated(0, 0, .04);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2d(ml / 2., 0);
            glVertex2d(0, 0);
            glVertex2d(0, -.45);
            glVertex2d(0 + .05, -.5);
            glVertex2d(ml - .05, -.5);
            glVertex2d(ml, -.45);
            glVertex2d(ml, .45);
            glVertex2d(ml - .05, .5);
            glVertex2d(0 + .05, .5);
            glVertex2d(0, .45);
            glVertex2d(0, 0);
            glEnd();
            glTranslated(0, 0, -.04);

            glTranslated(0, 0, .045);
            glColor3d(0, 0, 0);
            glBegin(GL_QUADS);
            glVertex2d(0 + .05, .48);
            glVertex2d(0 + .05, -.48);
            glVertex2d(0 + .15, -.48);
            glVertex2d(0 + .15, .48);

            glVertex2d(ml - .1, .45);
            glVertex2d(ml - .1, -.45);
            glVertex2d(ml - .05, -.45);
            glVertex2d(ml - .05, .45);

            glVertex2d(0 + .20, .49);
            glVertex2d(0 + .20, .45);
            glVertex2d(ml - .20, .45);
            glVertex2d(ml - .20, .49);

            glVertex2d(0 + .20, -.49);
            glVertex2d(0 + .20, -.45);
            glVertex2d(ml - .20, -.45);
            glVertex2d(ml - .20, -.49);

            glEnd();
            glTranslated(0, 0, -.045);
        }
        break;
        case SVS_BUS_FLEXIBLE:
        case SVS_RAIL:
        case SVS_RAIL_CAR:
        case SVS_RAIL_CARGO:
            //drawAction_drawCarriageClass(s, shape, false);
            drawCarriages = true;
            break;
        case SVS_E_VEHICLE:
            drawPoly(vehiclePoly_EVehicleBody, 4);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_EVehicleFrontGlass, 4.5);
            glTranslated(0, 0, .048);
            GLHelper::setColor(current);
            glBegin(GL_QUADS);
            glVertex2d(.3, .5);
            glVertex2d(.35, .5);
            glVertex2d(.35, -.5);
            glVertex2d(.3, -.5);

            glVertex2d(.3, -.05);
            glVertex2d(.7, -.05);
            glVertex2d(.7, .05);
            glVertex2d(.3, .05);

            glVertex2d(.7, .5);
            glVertex2d(.65, .5);
            glVertex2d(.65, -.5);
            glVertex2d(.7, -.5);
            glEnd();
            glTranslated(0, 0, -.048);
            //drawPoly(vehiclePoly_EVehicleBackGlass, 4.5);
            break;
        case SVS_ANT:
            glPushMatrix();
            // ant is stretched via vehicle length
            GLHelper::setColor(darker);
            // draw left side
            GLHelper::drawBoxLine(Position(-0.2, -.10), 350, 0.5, .02);
            GLHelper::drawBoxLine(Position(-0.3, -.50), 240, 0.4, .03);
            GLHelper::drawBoxLine(Position(0.3, -.10), 340, 0.8, .03);
            GLHelper::drawBoxLine(Position(0.05, -.80), 290, 0.6, .04);
            GLHelper::drawBoxLine(Position(0.4, -.10),  20, 0.8, .03);
            GLHelper::drawBoxLine(Position(0.65, -.80),  75, 0.6, .04);
            GLHelper::drawBoxLine(Position(0.5, -.10),  55, 0.8, .04);
            GLHelper::drawBoxLine(Position(1.1, -.55),  90, 0.6, .04);
            // draw right side
            GLHelper::drawBoxLine(Position(-0.2,  .10), 190, 0.5, .02);
            GLHelper::drawBoxLine(Position(-0.3,  .50), 300, 0.4, .03);
            GLHelper::drawBoxLine(Position(0.3,  .10), 200, 0.8, .03);
            GLHelper::drawBoxLine(Position(0.05, .80), 250, 0.6, .04);
            GLHelper::drawBoxLine(Position(0.4,  .10), 160, 0.8, .03);
            GLHelper::drawBoxLine(Position(0.65, .80), 105, 0.6, .04);
            GLHelper::drawBoxLine(Position(0.5,  .10), 125, 0.8, .04);
            GLHelper::drawBoxLine(Position(1.1,  .55),  90, 0.6, .04);
            // draw body
            GLHelper::setColor(current);
            glTranslated(0, 0, 0.1);
            GLHelper::drawFilledCircle(.25, 16);
            glTranslated(.4, 0, 0);
            GLHelper::drawFilledCircle(.2, 16);
            glTranslated(.4, 0, 0);
            GLHelper::drawFilledCircle(.3, 16);
            glPopMatrix();
            break;
        case SVS_SHIP: {
            RGBColor darker = current.changedBrightness(-30);
            RGBColor darker2 = current.changedBrightness(-70);
            drawPoly(vehiclePoly_Ship, 4);
            GLHelper::setColor(darker);
            drawPoly(vehiclePoly_ShipDeck, 5);
            GLHelper::setColor(darker2);
            drawPoly(vehiclePoly_ShipSuperStructure, 6);
            break;
        }
        case SVS_EMERGENCY: // similar to delivery
            drawPoly(vehiclePoly_PassengerVanBody, 4);
            GLHelper::setColor(darker);
            drawPoly(vehiclePoly_PassengerVanBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerVanFrontGlass, 4.5);
            drawPoly(vehiclePoly_DeliveryMediumRightGlass, 4.5);
            drawPoly(vehiclePoly_DeliveryMediumLeftGlass, 4.5);
            // first aid sign
            glTranslated(0.7, 0, 0);
            glColor3d(.18, .55, .34);
            drawPoly(vehiclePoly_EmergencySign, 4.5);
            glColor3d(1, 1, 1);
            drawPoly(vehiclePoly_Emergency, 5);
            drawPoly(vehiclePoly_Emergency2, 5);
            break;
        case SVS_FIREBRIGADE: // similar to delivery in red orange
            drawPoly(vehiclePoly_PassengerVanBody, 4);
            GLHelper::setColor(lighter);
            drawPoly(vehiclePoly_PassengerVanBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerVanFrontGlass, 4.5);
            drawPoly(vehiclePoly_DeliveryMediumRightGlass, 4.5);
            drawPoly(vehiclePoly_DeliveryMediumLeftGlass, 4.5);
            // draw ladder
            glTranslated(0.7, 0, 0);
            glColor3d(1, .5, 0);
            drawPoly(vehiclePoly_EmergencySign, 4.5);
            glColor3d(.5, .5, .5);
            drawPoly(vehiclePoly_EmergencyLadder, 5);
            drawPoly(vehiclePoly_EmergencyLadder2, 5);
            drawPoly(vehiclePoly_EmergencyLadder3, 5);
            drawPoly(vehiclePoly_EmergencyLadder4, 5);
            drawPoly(vehiclePoly_EmergencyLadder5, 5);
            drawPoly(vehiclePoly_EmergencyLadder6, 5);
            drawPoly(vehiclePoly_EmergencyLadder7, 5);
            break;
        case SVS_POLICE: // similar to passenger grey with blue
            drawPoly(vehiclePoly_PassengerCarBody, 4);
            GLHelper::setColor(lighter);
            drawPoly(vehiclePoly_PassengerCarBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerFrontGlass, 4.5);
            // first aid sign
            glTranslated(0.7, 0, 0);
            glColor3d(.5, .5, .5);
            drawPoly(vehiclePoly_EmergencySign, 4.5);
            glColor3d(0, 0, 1);
            drawPoly(vehiclePoly_Emergency, 5);
            drawPoly(vehiclePoly_Emergency2, 5);
            break;
        case SVS_RICKSHAW: // Rickshaw
            drawPoly(vehiclePoly_PassengerCarBody, 4);
            // wheels
            GLHelper::setColor(darker);
            glPushMatrix();
            glTranslated(.5, .5, -0.1);
            GLHelper::drawFilledCircle(.3, 6);
            glPopMatrix();
            //other wheel
            glPushMatrix();
            glTranslated(.5, -.5, -0.1);
            GLHelper::drawFilledCircle(.3, 6);
            glPopMatrix();
            break;
        default: // same as passenger
            drawPoly(vehiclePoly_PassengerCarBody, 4);
            glColor3d(1, 1, 1);
            drawPoly(vehiclePoly_PassengerCarBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerFrontGlass, 4.5);
            break;
    }

    // draw decorations
    switch (shape) {
        case SVS_PEDESTRIAN:
            break;
        case SVS_BICYCLE:
            //glScaled(length, 1, 1.);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2d(1 / 2., 0);
            glVertex2d(0, 0);
            glVertex2d(0, -.03);
            glVertex2d(0 + .05, -.05);
            glVertex2d(1 - .05, -.05);
            glVertex2d(1, -.03);
            glVertex2d(1, .03);
            glVertex2d(1 - .05, .05);
            glVertex2d(0 + .05, .05);
            glVertex2d(0, .03);
            glVertex2d(0, 0);
            glEnd();
            break;
        case SVS_MOPED:
        case SVS_MOTORCYCLE:
            //glScaled(length, 1, 1.);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2d(1 / 2., 0);
            glVertex2d(0, 0);
            glVertex2d(0, -.03);
            glVertex2d(0 + .05, -.2);
            glVertex2d(1 - .05, -.2);
            glVertex2d(1, -.03);
            glVertex2d(1, .03);
            glVertex2d(1 - .05, .2);
            glVertex2d(0 + .05, .2);
            glVertex2d(0, .03);
            glVertex2d(0, 0);
            glEnd();
            break;
        case SVS_PASSENGER:
        case SVS_PASSENGER_SEDAN:
            drawPoly(vehiclePoly_PassengerSedanRightGlass, 4.5);
            drawPoly(vehiclePoly_PassengerSedanLeftGlass, 4.5);
            drawPoly(vehiclePoly_PassengerSedanBackGlass, 4.5);
            break;
        case SVS_PASSENGER_HATCHBACK:
            drawPoly(vehiclePoly_PassengerHatchbackRightGlass, 4.5);
            drawPoly(vehiclePoly_PassengerHatchbackLeftGlass, 4.5);
            drawPoly(vehiclePoly_PassengerHatchbackBackGlass, 4.5);
            break;
        case SVS_PASSENGER_WAGON:
            drawPoly(vehiclePoly_PassengerWagonRightGlass, 4.5);
            drawPoly(vehiclePoly_PassengerWagonLeftGlass, 4.5);
            drawPoly(vehiclePoly_PassengerWagonBackGlass, 4.5);
            break;
        case SVS_PASSENGER_VAN:
        case SVS_DELIVERY:
            break;
        case SVS_TRUCK:
            GLHelper::setColor(current);
            GLHelper::drawBoxLine(Position(2.3, 0), 90., length - 2.3, .5);
            break;
        case SVS_TRUCK_SEMITRAILER:
            GLHelper::setColor(current);
            GLHelper::drawBoxLine(Position(2.8, 0), 90., length - 2.8, .5);
            break;
        case SVS_TRUCK_1TRAILER: {
            GLHelper::setColor(current);
            double l = length - 2.3;
            l = l / 2.;
            GLHelper::drawBoxLine(Position(2.3, 0), 90., l, .5);
            GLHelper::drawBoxLine(Position(2.3 + l + .5, 0), 90., l - .5, .5);
            break;
        }
        case SVS_BUS_TROLLEY:
            glPushMatrix();
            glTranslated(0, 0, .1);
            GLHelper::setColor(darker);
            GLHelper::drawBoxLine(Position(3.8, 0), 90., 1, .3);
            glTranslated(0, 0, .1);
            glColor3d(0, 0, 0);
            GLHelper::drawBoxLine(Position(4.3, .2), 90., 1, .06);
            GLHelper::drawBoxLine(Position(4.3, -.2), 90., 1, .06);
            GLHelper::drawBoxLine(Position(5.3, .2), 90., 3, .03);
            GLHelper::drawBoxLine(Position(5.3, -.2), 90., 3, .03);
            glPopMatrix();
            break;
        case SVS_BUS:
        case SVS_BUS_COACH:
        case SVS_BUS_FLEXIBLE:
        case SVS_RAIL:
        case SVS_RAIL_CAR:
        case SVS_RAIL_CARGO:
        case SVS_E_VEHICLE:
        case SVS_ANT:
        case SVS_SHIP:
        case SVS_EMERGENCY:
        case SVS_FIREBRIGADE:
        case SVS_POLICE:
        case SVS_RICKSHAW:
            break;
        default: // same as passenger/sedan
            drawPoly(vehiclePoly_PassengerSedanRightGlass, 4.5);
            drawPoly(vehiclePoly_PassengerSedanLeftGlass, 4.5);
            drawPoly(vehiclePoly_PassengerSedanBackGlass, 4.5);
            break;
    }
    /*
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(.5,.5); // center - strip begin
    glVertex2d(0,    .5); // center, front
    glVertex2d(0,    .8); // ... clockwise ... (vehicle right side)
    glVertex2d(0.08, .94);
    glVertex2d(0.25, 1.);
    glVertex2d(0.95, 1.);
    glVertex2d(1.,   .9);
    glVertex2d(1.,   .1); // (vehicle left side)
    glVertex2d(0.95, 0.);
    glVertex2d(0.25, 0.);
    glVertex2d(0.08, .06);
    glVertex2d(0,    .2); //
    glVertex2d(0,    .5); // center, front (close)
    glEnd();

    glPolygonOffset(0, -4.5);
    glColor3d(1, 1, 1); // front
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(0.1,0.5);
    glVertex2d(0.025,0.5);
    glVertex2d(0.025,0.75);
    glVertex2d(0.27,0.9);
    glVertex2d(0.27,0.1);
    glVertex2d(0.025,0.25);
    glVertex2d(0.025,0.5);
    glEnd();

    glColor3d(0, 0, 0); // front glass
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(0.35,0.5);
    glVertex2d(0.3,0.5);
    glVertex2d(0.3,0.9);
    glVertex2d(0.43,0.8);
    glVertex2d(0.43,0.2);
    glVertex2d(0.3,0.1);
    glVertex2d(0.3,0.5);
    glEnd();

    glBegin(GL_TRIANGLE_FAN); // back glass
    glVertex2d(0.92,0.5);
    glVertex2d(0.90,0.5);
    glVertex2d(0.90,0.8);
    glVertex2d(0.95,0.9);
    glVertex2d(0.95,0.1);
    glVertex2d(0.90,0.2);
    glVertex2d(0.90,0.5);
    glEnd();

    glBegin(GL_TRIANGLE_FAN); // right glass
    glVertex2d(0.36,0.07);
    glVertex2d(0.34,0.03);
    glVertex2d(0.94,0.03);
    glVertex2d(0.87,0.13);
    glVertex2d(0.45,0.13);
    glVertex2d(0.34,0.03);
    glEnd();

    glBegin(GL_TRIANGLE_FAN); // left glass
    glVertex2d(0.36,1.-0.07);
    glVertex2d(0.34,1.-0.03);
    glVertex2d(0.94,1.-0.03);
    glVertex2d(0.87,1.-0.13);
    glVertex2d(0.45,1.-0.13);
    glVertex2d(0.34,1.-0.03);
    glEnd();
    */

    glPopMatrix();
    return drawCarriages;
}


bool
GNEVehicle::drawAction_drawVehicleAsImage(const GUIVisualizationSettings& s, double length) const {
    const std::string& file = myVehicleType->getAttribute(SUMO_ATTR_IMGFILE);
    if (file != "") {
        int textureID = GUITexturesHelper::getTextureID(file);
        if (textureID > 0) {
            const double exaggeration = s.vehicleSize.getExaggeration(s, this);
            if (length < 0) {
                length = parse<double>(myVehicleType->getAttribute(SUMO_ATTR_LENGTH)) * exaggeration;
            }
            const double halfWidth = parse<double>(myVehicleType->getAttribute(SUMO_ATTR_WIDTH)) / 2.0 * exaggeration;
            GUITexturesHelper::drawTexturedBox(textureID, -halfWidth, 0, halfWidth, length);
            return true;
        }
    }
    return false;
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
            myVehicleType = myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, value);
            break;
        case SUMO_ATTR_ROUTE:
            myRoute = myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, value);
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
            // check
            break;
        case SUMO_ATTR_DEPARTPOS_LAT:
            parseDepartPosLat(value, toString(SUMO_TAG_VEHICLE), id, departPosLat, departPosLatProcedure, error);
            break;
        case SUMO_ATTR_ARRIVALPOS_LAT:
            parseArrivalPosLat(value, toString(SUMO_TAG_VEHICLE), id, arrivalPosLat, arrivalPosLatProcedure, error);
            break;        
        // Specific of vehicles
        case SUMO_ATTR_DEPART:
            parseDepart(value, toString(SUMO_TAG_VEHICLE), id, depart, departProcedure, error);
            break;     
        //
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/

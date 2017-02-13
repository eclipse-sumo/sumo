/****************************************************************************/
/// @file    GNECalibrator.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
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

#include <string>
#include <iostream>
#include <utility>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNECalibrator.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNERouteProbe.h"
#include "GNECalibratorDialog.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibrator::GNECalibrator(const std::string& id, GNEEdge* edge, GNEViewNet* viewNet, SUMOReal pos, SUMOReal frequency, const std::string& output, const std::vector<GNECalibrator::GNECalibratorFlow*>& flowValues) :
    GNEAdditional(id, viewNet, Position(pos, 0), SUMO_TAG_CALIBRATOR, ICON_CALIBRATOR),
    myFrequency(frequency),
    myOutput(output),
    myRouteProbe(NULL), /** change this in the future **/
    myFlowValues(flowValues) {
    // This additional belong to a edge
    myEdge = edge;
    // this additional ISN'T movable
    myMovable = false;
    // Update geometry;
    updateGeometry();
    // Set Colors
    myBaseColor = RGBColor(255, 255, 50, 0);
    myBaseColorSelected = RGBColor(255, 255, 125, 255);
    // Center view in the position of calibrator
    myViewNet->centerTo(getGlID(), false);
}


GNECalibrator::~GNECalibrator() {
    // delete all flows of calibrator
    for (std::vector<GNECalibratorFlow*>::iterator i = myFlowValues.begin(); i != myFlowValues.end(); i++) {
        delete(*i);
    }
}


void
GNECalibrator::moveAdditionalGeometry(SUMOReal, SUMOReal) {
    // This additional cannot be moved
}


void
GNECalibrator::commmitAdditionalGeometryMoved(SUMOReal, SUMOReal, GNEUndoList*) {
    // This additional cannot be moved
}


void
GNECalibrator::updateGeometry() {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();

    // clear Shape
    myShape.clear();

    // Iterate over lanes
    for (int i = 0; i < (int)myEdge->getLanes().size(); i++) {

        // Get shape of lane parent
        myShape.push_back(myEdge->getLanes().at(i)->getShape().positionAtOffset(myEdge->getLanes().at(i)->getPositionRelativeToParametricLenght(myPosition.x())));

        // Obtain first position
        Position f = myShape[i] - Position(1, 0);

        // Obtain next position
        Position s = myShape[i] + Position(1, 0);

        // Save rotation (angle) of the vector constructed by points f and s
        myShapeRotations.push_back(myEdge->getLanes().at(i)->getShape().rotationDegreeAtOffset(myEdge->getLanes().at(i)->getPositionRelativeToParametricLenght(myPosition.x())) * -1);
    }

    // Refresh element (neccesary to avoid grabbing problems)
    myViewNet->getNet()->refreshAdditional(this);
}


Position
GNECalibrator::getPositionInView() const {
    return myPosition;
}

void
GNECalibrator::openAdditionalDialog() {
    // Open calibrator dialog
    GNECalibratorDialog calibratorDialog(this);
}


void
GNECalibrator::writeAdditional(OutputDevice& device) const {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_LANE, myEdge->getLanes().at(0)->getID());
    device.writeAttr(SUMO_ATTR_POSITION, myPosition.x());
    device.writeAttr(SUMO_ATTR_FREQUENCY, myFrequency);
    device.writeAttr(SUMO_ATTR_OUTPUT, myOutput);
    // Write all flows of this calibrator
    for (std::vector<GNECalibrator::GNECalibratorFlow*>::const_iterator i = myFlowValues.begin(); i != myFlowValues.end(); ++i) {
        // Open flow tag
        device.openTag(SUMO_TAG_FLOW);
        // Write begin
        device.writeAttr(SUMO_ATTR_BEGIN, (*i)->getBegin());
        // Write nd
        device.writeAttr(SUMO_ATTR_END, (*i)->getEnd());
        // Write type
        device.writeAttr(SUMO_ATTR_TYPE, (*i)->getType());
        // Write route
        device.writeAttr(SUMO_ATTR_ROUTE, (*i)->getRoute());
        // Write color
        device.writeAttr(SUMO_ATTR_COLOR, (*i)->getColor());
        // Write depart lane
        device.writeAttr(SUMO_ATTR_DEPARTLANE, (*i)->getDepartLane());
        // Write depart pos
        device.writeAttr(SUMO_ATTR_DEPARTPOS, (*i)->getDepartPos());
        // Write depart speed
        device.writeAttr(SUMO_ATTR_DEPARTSPEED, (*i)->getDepartSpeed());
        // Write arrival lane
        device.writeAttr(SUMO_ATTR_ARRIVALLANE, (*i)->getArrivalLane());
        // Write arrival pos
        device.writeAttr(SUMO_ATTR_ARRIVALPOS, (*i)->getArrivalPos());
        // Write arrival speed
        device.writeAttr(SUMO_ATTR_ARRIVALSPEED, (*i)->getArrivalSpeed());
        // Write line
        device.writeAttr(SUMO_ATTR_LINE, (*i)->getLine());
        // Write person number
        device.writeAttr(SUMO_ATTR_PERSON_NUMBER, (*i)->getPersonNumber());
        // Write container number
        device.writeAttr(SUMO_ATTR_CONTAINER_NUMBER, (*i)->getContainerNumber());
        // Write vehsPerHour
        device.writeAttr(SUMO_ATTR_VEHSPERHOUR, (*i)->getVehsPerHour());
        // Write period
        device.writeAttr(SUMO_ATTR_PERIOD, (*i)->getPeriod());
        // Write probability
        device.writeAttr(SUMO_ATTR_PROB, (*i)->getProbability());
        // Write number
        device.writeAttr(SUMO_ATTR_NUMBER, (*i)->getNumber());
        // Close flow tag
        device.closeTag();
    }
    // Close tag
    device.closeTag();
}


std::vector<GNECalibrator::GNECalibratorFlow*>
GNECalibrator::getFlowValues() const {
    return myFlowValues;
}


void
GNECalibrator::setFlowValues(std::vector<GNECalibrator::GNECalibratorFlow*> calibratorFlowValues) {
    myFlowValues = calibratorFlowValues;
}


void
GNECalibrator::insertFlow(GNECalibratorFlow* flow) {
    std::vector<GNECalibratorFlow*>::iterator i = std::find(myFlowValues.begin(), myFlowValues.end(), flow);
    if (i == myFlowValues.end()) {
        myFlowValues.push_back(flow);
    } else {
        throw InvalidArgument("Flow duplicated in calibrator with id = '" + getID() + "'");
    }

}


void
GNECalibrator::removeFlow(GNECalibratorFlow* flow) {
    std::vector<GNECalibratorFlow*>::iterator i = std::find(myFlowValues.begin(), myFlowValues.end(), flow);
    if (i != myFlowValues.end()) {
        myFlowValues.erase(i);
    } else {
        throw InvalidArgument("Flow doesn't exitst in calibrator with id = '" + getID() + "'");
    }

}


const std::string&
GNECalibrator::getParentName() const {
    return myEdge->getMicrosimID();
}


void
GNECalibrator::drawGL(const GUIVisualizationSettings& s) const {
    // get values
    glPushName(getGlID());
    glLineWidth(1.0);
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);

    glPushName(getGlID());
    for (int i = 0; i < (int)myShape.size(); ++i) {
        const Position& pos = myShape[i];
        SUMOReal rot = myShapeRotations[i];
        glPushMatrix();
        glTranslated(pos.x(), pos.y(), getType());
        glRotated(rot, 0, 0, 1);
        glTranslated(0, 0, getType());
        glScaled(exaggeration, exaggeration, 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBegin(GL_TRIANGLES);
        glColor3d(1, .8f, 0);
        // base
        glVertex2d(0 - 1.4, 0);
        glVertex2d(0 - 1.4, 6);
        glVertex2d(0 + 1.4, 6);
        glVertex2d(0 + 1.4, 0);
        glVertex2d(0 - 1.4, 0);
        glVertex2d(0 + 1.4, 6);
        glEnd();

        // draw text
        if (s.scale * exaggeration >= 1.) {
            glTranslated(0, 0, .1);
            glColor3d(0, 0, 0);
            pfSetPosition(0, 0);
            pfSetScale(3.f);
            SUMOReal w = pfdkGetStringWidth("C");
            glRotated(180, 0, 1, 0);
            glTranslated(-w / 2., 2, 0);
            pfDrawString("C");
            glTranslated(w / 2., -2, 0);
        }
        glPopMatrix();
    }
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    glPopName();
}


std::string
GNECalibrator::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_LANE:
            return toString(myEdge->getLanes().at(0)->getAttribute(SUMO_ATTR_ID));
        case SUMO_ATTR_POSITION:
            return toString(myPosition.x());
        case SUMO_ATTR_FREQUENCY:
            return toString(myFrequency);
        case SUMO_ATTR_OUTPUT:
            return myOutput;
        case SUMO_ATTR_ROUTEPROBE:
            if (myRouteProbe) {
                return myRouteProbe->getID();
            } else {
                return "";
            }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNECalibrator::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_OUTPUT:
        case SUMO_ATTR_ROUTEPROBE:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            updateGeometry();
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }

}


bool
GNECalibrator::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            if (myViewNet->getNet()->getAdditional(getTag(), value) == NULL) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_LANE:
            if (myViewNet->getNet()->retrieveLane(value, false) != NULL) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_FREQUENCY:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0);
        case SUMO_ATTR_OUTPUT:
            return isValidFileValue(value);
        case SUMO_ATTR_ROUTEPROBE:
            if (myViewNet->getNet()->getAdditional(SUMO_TAG_ROUTEPROBE, value) != NULL) {
                return true;
            } else {
                return false;
            }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNECalibrator::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            setAdditionalID(value);
            break;
        case SUMO_ATTR_LANE:
            changeLane(value);
            break;
        case SUMO_ATTR_POSITION:
            myPosition = Position(parse<SUMOReal>(value), 0);
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_FREQUENCY:
            myFrequency = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_OUTPUT:
            myOutput = value;
            break;
        case SUMO_ATTR_ROUTEPROBE:
            myRouteProbe = dynamic_cast<GNERouteProbe*>(myViewNet->getNet()->getAdditional(SUMO_TAG_ROUTEPROBE, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


// ===========================================================================
// Calibrator Flow
// ===========================================================================


GNECalibrator::GNECalibratorFlow::GNECalibratorFlow(GNECalibrator* calibratorParent, std::string type, std::string route) :
    myCalibratorParent(calibratorParent), myType(type), myRoute(route), myColor(""), myDepartLane("first"),
    myDepartPos("base"), myDepartSpeed("0"), myArrivalLane("current"), myArrivalPos("max"), myArrivalSpeed("current"),
    myLine(""), myPersonNumber(0), myContainerNumber(0), myBegin(0), myEnd(0), myVehsPerHour(0), myPeriod(0), myProbability(0), myNumber(0) {}


GNECalibrator::GNECalibratorFlow::GNECalibratorFlow(GNECalibrator* calibratorParent, std::string type, std::string route,
        std::string color, std::string departLane, std::string departPos, std::string departSpeed, std::string arrivalLane,
        std::string arrivalPos, std::string arrivalSpeed, std::string line, int personNumber, int containerNumber,
        SUMOReal begin, SUMOReal end, SUMOReal vehsPerHour, SUMOReal period, SUMOReal probability, int number) :
    myCalibratorParent(calibratorParent), myType(type), myRoute(route), myColor(""), myDepartLane("first"),
    myDepartPos("base"), myDepartSpeed("0"), myArrivalLane("current"), myArrivalPos("max"), myArrivalSpeed("current"),
    myLine(""), myPersonNumber(0), myContainerNumber(0), myBegin(0), myEnd(0), myVehsPerHour(0), myPeriod(0), myProbability(0), myNumber(0) {
    // set parameters using the set functions, to avoid non valid values
    setColor(color);
    setDepartLane(departLane);
    setDepartPos(departPos);
    setDepartSpeed(departSpeed);
    setArrivalLane(arrivalLane);
    setArrivalPos(arrivalPos);
    setArrivalSpeed(arrivalSpeed);
    setLine(line);
    setPersonNumber(personNumber);
    setContainerNumber(containerNumber);
    setBegin(begin);
    setEnd(end);
    setVehsPerHour(vehsPerHour);
    setPeriod(period);
    setProbability(probability);
    setNumber(number);
}


GNECalibrator::GNECalibratorFlow::~GNECalibratorFlow() {}


GNECalibrator*
GNECalibrator::GNECalibratorFlow::getCalibratorParent() const {
    return myCalibratorParent;
}


const std::string&
GNECalibrator::GNECalibratorFlow::getType() const {
    return myType;
}


const std::string&
GNECalibrator::GNECalibratorFlow::getRoute() const {
    return myRoute;
}


const std::string&
GNECalibrator::GNECalibratorFlow::getColor() const {
    return myColor;
}


const std::string&
GNECalibrator::GNECalibratorFlow::getDepartLane() const {
    return myDepartLane;
}


const std::string&
GNECalibrator::GNECalibratorFlow::getDepartPos() const {
    return myDepartPos;
}


const std::string&
GNECalibrator::GNECalibratorFlow::getDepartSpeed() const {
    return myDepartSpeed;
}


const std::string&
GNECalibrator::GNECalibratorFlow::getArrivalLane() const {
    return myArrivalLane;
}


const std::string&
GNECalibrator::GNECalibratorFlow::getArrivalPos() const {
    return myArrivalPos;
}


const std::string&
GNECalibrator::GNECalibratorFlow::getArrivalSpeed() const {
    return myArrivalSpeed;
}


const std::string&
GNECalibrator::GNECalibratorFlow::getLine() const {
    return myLine;
}


int
GNECalibrator::GNECalibratorFlow::getPersonNumber() const {
    return myPersonNumber;
}


int
GNECalibrator::GNECalibratorFlow::getContainerNumber() const {
    return myContainerNumber;
}


SUMOReal
GNECalibrator::GNECalibratorFlow::getBegin() const {
    return myBegin;
}


SUMOReal
GNECalibrator::GNECalibratorFlow::getEnd() const {
    return myEnd;
}


SUMOReal
GNECalibrator::GNECalibratorFlow::getVehsPerHour() const {
    return myVehsPerHour;
}


SUMOReal
GNECalibrator::GNECalibratorFlow::getPeriod() const {
    return myPeriod;
}


SUMOReal
GNECalibrator::GNECalibratorFlow::getProbability() const {
    return myProbability;
}


int
GNECalibrator::GNECalibratorFlow::getNumber() const {
    return myNumber;
}


bool
GNECalibrator::GNECalibratorFlow::setType(std::string type) {
    if (type.empty()) {
        return false;
    } else {
        myType = type;
        return true;
    }
}


bool
GNECalibrator::GNECalibratorFlow::setRoute(std::string route) {
    if (route.empty()) {
        return false;
    } else {
        myRoute = route;
        return true;
    }
}


bool
GNECalibrator::GNECalibratorFlow::setColor(std::string color) {
    myColor = color;
    return true;
}


bool
GNECalibrator::GNECalibratorFlow::setDepartLane(std::string departLane) {
    int departLaneInt = -1;
    if (GNEAttributeCarrier::canParse<int>(departLane)) {
        departLaneInt = GNEAttributeCarrier::parse<int>(departLane);
    }
    if ((departLaneInt < 0) && (departLane != "random") && (departLane != "free") &&
            (departLane != "allowed") && (departLane != "best") && (departLane != "first")) {
        return false;
    } else {
        myDepartLane = departLane;
        return true;
    }
}


bool
GNECalibrator::GNECalibratorFlow::setDepartPos(std::string departPos) {
    SUMOReal departPosFloat = -1;
    if (GNEAttributeCarrier::canParse<SUMOReal>(departPos)) {
        departPosFloat = GNEAttributeCarrier::parse<SUMOReal>(departPos);
    }
    if ((departPosFloat < 0) && (departPos != "random") && (departPos != "free") &&
            (departPos != "random_free") && (departPos != "base") && (departPos != "last")) {
        return false;
    } else {
        myDepartPos = departPos;
        return true;
    }
}


bool
GNECalibrator::GNECalibratorFlow::setDepartSpeed(std::string departSpeed) {
    SUMOReal departSpeedDouble = -1;
    if (GNEAttributeCarrier::canParse<SUMOReal>(departSpeed)) {
        departSpeedDouble = GNEAttributeCarrier::parse<SUMOReal>(departSpeed);
    }
    if ((departSpeedDouble < 0) && (departSpeed != "random") && (departSpeed != "max")) {
        return false;
    } else {
        myDepartSpeed = departSpeed;
        return true;
    }
}


bool
GNECalibrator::GNECalibratorFlow::setArrivalLane(std::string arrivalLane) {
    int arrivalLaneInt = -1;
    if (GNEAttributeCarrier::canParse<int>(arrivalLane)) {
        arrivalLaneInt = GNEAttributeCarrier::parse<int>(arrivalLane);
    }
    if ((arrivalLaneInt < 0) && (arrivalLane != "current")) {
        return false;
    } else {
        myDepartLane = arrivalLane;
        return true;
    }
}


bool
GNECalibrator::GNECalibratorFlow::setArrivalPos(std::string arrivalPos) {
    SUMOReal arrivalPosFloat = -1;
    if (GNEAttributeCarrier::canParse<SUMOReal>(arrivalPos)) {
        arrivalPosFloat = GNEAttributeCarrier::parse<SUMOReal>(arrivalPos);
    }
    if ((arrivalPosFloat < 0) && (arrivalPos != "random") && (arrivalPos != "max")) {
        return false;
    } else {
        myDepartPos = arrivalPos;
        return true;
    }
}


bool
GNECalibrator::GNECalibratorFlow::setArrivalSpeed(std::string arrivalSpeed) {
    SUMOReal arrivalSpeedDouble = -1;
    if (GNEAttributeCarrier::canParse<SUMOReal>(arrivalSpeed)) {
        arrivalSpeedDouble = GNEAttributeCarrier::parse<SUMOReal>(arrivalSpeed);
    }
    if ((arrivalSpeedDouble < 0) && (arrivalSpeed != "current")) {
        return false;
    } else {
        myDepartSpeed = arrivalSpeed;
        return true;
    }
}


bool
GNECalibrator::GNECalibratorFlow::setLine(std::string line) {
    /// @todo check if line exists
    myLine = line;
    return true;
}


bool
GNECalibrator::GNECalibratorFlow::setPersonNumber(int personNumber) {
    if (personNumber < 0) {
        return false;
    } else {
        myPersonNumber = personNumber;
        return true;
    }
}


bool
GNECalibrator::GNECalibratorFlow::setContainerNumber(int containerNumber) {
    if (containerNumber < 0) {
        return false;
    } else {
        myContainerNumber = containerNumber;
        return true;
    }
}


bool
GNECalibrator::GNECalibratorFlow::setBegin(SUMOReal begin) {
    if (begin < 0) {
        return false;
    } else {
        myBegin = begin;
        return true;
    }
}


bool
GNECalibrator::GNECalibratorFlow::setEnd(SUMOReal end) {
    if (end < 0) {
        return false;
    } else {
        myEnd = end;
        return true;
    }
}


bool
GNECalibrator::GNECalibratorFlow::setVehsPerHour(SUMOReal vehsPerHour) {
    if (vehsPerHour < 0) {
        return false;
    } else {
        myVehsPerHour = vehsPerHour;
        return true;
    }
}


bool
GNECalibrator::GNECalibratorFlow::setPeriod(SUMOReal period) {
    if (period < 0) {
        return false;
    } else {
        myPeriod = period;
        return true;
    }
}

bool
GNECalibrator::GNECalibratorFlow::setProbability(SUMOReal probability) {
    if ((probability < 0) || (probability > 1)) {
        return false;
    } else {
        myProbability = probability;
        return true;
    }
}


bool
GNECalibrator::GNECalibratorFlow::setNumber(int number) {
    if (number < 0) {
        return false;
    } else {
        myNumber = number;
        return true;
    }
}


/****************************************************************************/

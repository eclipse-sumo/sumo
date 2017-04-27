/****************************************************************************/
/// @file    GNECalibratorFlow.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2016
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

#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNECalibratorFlow.h"
#include "GNECalibrator.h"


// ===========================================================================
// member method definitions
// ===========================================================================


GNECalibratorFlow::GNECalibratorFlow(GNECalibrator* calibratorParent) :
    myCalibratorParent(calibratorParent), myFlowID(calibratorParent->generateFlowID()), myVehicleType(""), myRoute(""), myColor(""), myDepartLane("first"),
    myDepartPos("base"), myDepartSpeed("0"), myArrivalLane("current"), myArrivalPos("max"), myArrivalSpeed("current"),
    myLine(""), myPersonNumber(0), myContainerNumber(0), myReroute(false), myDepartPosLat("center"), myArrivalPosLat(""),
    myBegin(0), myEnd(0), myVehsPerHour(0), myPeriod(0), myProbability(0), myNumber(0), myTypeOfFlow(GNE_CALIBRATORFLOW_VEHSPERHOUR) {
    if (myCalibratorParent->getCalibratorRoutes().size() > 0) {
        myRoute = myCalibratorParent->getCalibratorRoutes().front().getRouteID();
    }
    if (myCalibratorParent->getCalibratorVehicleTypes().size() > 0) {
        myVehicleType = myCalibratorParent->getCalibratorVehicleTypes().front().getVehicleTypeID();
    }
}


GNECalibratorFlow::GNECalibratorFlow(GNECalibrator* calibratorParent, std::string flowID, std::string vehicleType, std::string route,
                                     std::string color, std::string departLane, std::string departPos, std::string departSpeed, std::string arrivalLane, std::string arrivalPos,
                                     std::string arrivalSpeed, std::string line, int personNumber, int containerNumber, bool reroute, std::string departPosLat,
                                     std::string arrivalPosLat, double begin, double end, double vehsPerHour, double period, double probability, int number) :
    myCalibratorParent(calibratorParent), myFlowID(calibratorParent->generateFlowID()), myVehicleType(vehicleType), myRoute(route), myColor(""), myDepartLane("first"),
    myDepartPos("base"), myDepartSpeed("0"), myArrivalLane("current"), myArrivalPos("max"), myArrivalSpeed("current"),
    myLine(""), myPersonNumber(0), myContainerNumber(0), myReroute(false), myDepartPosLat("center"), myArrivalPosLat(""),
    myBegin(0), myEnd(0), myVehsPerHour(0), myPeriod(0), myProbability(0), myNumber(0), myTypeOfFlow(GNE_CALIBRATORFLOW_VEHSPERHOUR) {
    // set parameters using the set functions, to avoid non valid values
    setFlowID(flowID);
    setVehicleType(vehicleType);
    setRoute(route);
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
    setReroute(reroute);
    setDepartPosLat(departPosLat);
    setArrivalPosLat(arrivalPosLat);
    setBegin(begin);
    setEnd(end);
    setVehsPerHour(vehsPerHour);
    setPeriod(period);
    setProbability(probability);
    setNumber(number);
}


GNECalibratorFlow::~GNECalibratorFlow() {}


GNECalibrator*
GNECalibratorFlow::getCalibratorParent() const {
    return myCalibratorParent;
}


SumoXMLTag
GNECalibratorFlow::getTag() const {
    return SUMO_TAG_FLOW;
}


const std::string&
GNECalibratorFlow::getFlowID() const {
    return myFlowID;
}


const std::string&
GNECalibratorFlow::getVehicleType() const {
    return myVehicleType;
}


const std::string&
GNECalibratorFlow::getRoute() const {
    return myRoute;
}


const std::string&
GNECalibratorFlow::getColor() const {
    return myColor;
}


const std::string&
GNECalibratorFlow::getDepartLane() const {
    return myDepartLane;
}


const std::string&
GNECalibratorFlow::getDepartPos() const {
    return myDepartPos;
}


const std::string&
GNECalibratorFlow::getDepartSpeed() const {
    return myDepartSpeed;
}


const std::string&
GNECalibratorFlow::getArrivalLane() const {
    return myArrivalLane;
}


const std::string&
GNECalibratorFlow::getArrivalPos() const {
    return myArrivalPos;
}


const std::string&
GNECalibratorFlow::getArrivalSpeed() const {
    return myArrivalSpeed;
}


const std::string&
GNECalibratorFlow::getLine() const {
    return myLine;
}


int
GNECalibratorFlow::getPersonNumber() const {
    return myPersonNumber;
}


int
GNECalibratorFlow::getContainerNumber() const {
    return myContainerNumber;
}


bool
GNECalibratorFlow::getReroute() const {
    return myReroute;
}


std::string
GNECalibratorFlow::getDepartPosLat() const {
    return myDepartPosLat;
}


std::string
GNECalibratorFlow::getArrivalPosLat() const {
    return myArrivalPosLat;
}


double
GNECalibratorFlow::getBegin() const {
    return myBegin;
}


double
GNECalibratorFlow::getEnd() const {
    return myEnd;
}


double
GNECalibratorFlow::getVehsPerHour() const {
    return myVehsPerHour;
}


double
GNECalibratorFlow::getPeriod() const {
    return myPeriod;
}


double
GNECalibratorFlow::getProbability() const {
    return myProbability;
}


int
GNECalibratorFlow::getNumber() const {
    return myNumber;
}


GNECalibratorFlow::TypeOfFlow
GNECalibratorFlow::getFlowType() const {
    return myTypeOfFlow;
}


bool
GNECalibratorFlow::setFlowID(std::string flowID) {
    if (flowID.empty()) {
        return false;
    } else if (myCalibratorParent->getViewNet()->getNet()->flowExists(flowID) == true) {
        return false;
    } else {
        myFlowID = flowID;
        return true;
    }
}


bool
GNECalibratorFlow::setVehicleType(std::string vehicleType) {
    if (vehicleType.empty()) {
        return false;
    } else if (myCalibratorParent->getViewNet()->getNet()->vehicleTypeExists(vehicleType) == false) {
        return false;
    } else {
        myVehicleType = vehicleType;
        return true;
    }
}


bool
GNECalibratorFlow::setRoute(std::string route) {
    if (route.empty()) {
        return false;
    } else if (myCalibratorParent->getViewNet()->getNet()->routeExists(route) == false) {
        return false;
    } else {
        myRoute = route;
        return true;
    }
}


bool
GNECalibratorFlow::setColor(std::string color) {
    myColor = color;
    return true;
}


bool
GNECalibratorFlow::setDepartLane(std::string departLane) {
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
GNECalibratorFlow::setDepartPos(std::string departPos) {
    double departPosFloat = -1;
    if (GNEAttributeCarrier::canParse<double>(departPos)) {
        departPosFloat = GNEAttributeCarrier::parse<double>(departPos);
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
GNECalibratorFlow::setDepartSpeed(std::string departSpeed) {
    double departSpeedDouble = -1;
    if (GNEAttributeCarrier::canParse<double>(departSpeed)) {
        departSpeedDouble = GNEAttributeCarrier::parse<double>(departSpeed);
    }
    if ((departSpeedDouble < 0) && (departSpeed != "random") && (departSpeed != "max")) {
        return false;
    } else {
        myDepartSpeed = departSpeed;
        return true;
    }
}


bool
GNECalibratorFlow::setArrivalLane(std::string arrivalLane) {
    int arrivalLaneInt = -1;
    if (GNEAttributeCarrier::canParse<int>(arrivalLane)) {
        arrivalLaneInt = GNEAttributeCarrier::parse<int>(arrivalLane);
    }
    if ((arrivalLaneInt < 0) && (arrivalLane != "current")) {
        return false;
    } else {
        myArrivalLane = arrivalLane;
        return true;
    }
}


bool
GNECalibratorFlow::setArrivalPos(std::string arrivalPos) {
    double arrivalPosFloat = -1;
    if (GNEAttributeCarrier::canParse<double>(arrivalPos)) {
        arrivalPosFloat = GNEAttributeCarrier::parse<double>(arrivalPos);
    }
    if ((arrivalPosFloat < 0) && (arrivalPos != "random") && (arrivalPos != "max")) {
        return false;
    } else {
        myArrivalPos = arrivalPos;
        return true;
    }
}


bool
GNECalibratorFlow::setArrivalSpeed(std::string arrivalSpeed) {
    double arrivalSpeedDouble = -1;
    if (GNEAttributeCarrier::canParse<double>(arrivalSpeed)) {
        arrivalSpeedDouble = GNEAttributeCarrier::parse<double>(arrivalSpeed);
    }
    if ((arrivalSpeedDouble < 0) && (arrivalSpeed != "current")) {
        return false;
    } else {
        myArrivalSpeed = arrivalSpeed;
        return true;
    }
}


bool
GNECalibratorFlow::setLine(std::string line) {
    myLine = line;
    return true;
}


bool
GNECalibratorFlow::setPersonNumber(int personNumber) {
    if (personNumber < 0) {
        return false;
    } else {
        myPersonNumber = personNumber;
        return true;
    }
}


bool
GNECalibratorFlow::setPersonNumber(std::string personNumber) {
    if (GNEAttributeCarrier::canParse<int>(personNumber)) {
        return setPersonNumber(GNEAttributeCarrier::parse<int>(personNumber));
    } else if (GNEAttributeCarrier::canParse<double>(personNumber)) {
        double personNumberDouble = GNEAttributeCarrier::parse<double>(personNumber);
        // check if double is an integer
        if (fmod(personNumberDouble, 1) == 0) {
            return setPersonNumber((int)personNumberDouble);
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool
GNECalibratorFlow::setContainerNumber(int containerNumber) {
    if (containerNumber < 0) {
        return false;
    } else {
        myContainerNumber = containerNumber;
        return true;
    }
}


bool
GNECalibratorFlow::setContainerNumber(std::string ContainerNumber) {
    if (GNEAttributeCarrier::canParse<int>(ContainerNumber)) {
        return setContainerNumber(GNEAttributeCarrier::parse<int>(ContainerNumber));
    } else if (GNEAttributeCarrier::canParse<double>(ContainerNumber)) {
        double ContainerNumberDouble = GNEAttributeCarrier::parse<double>(ContainerNumber);
        // check if double is an integer
        if (fmod(ContainerNumberDouble, 1) == 0) {
            return setContainerNumber((int)ContainerNumberDouble);
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool
GNECalibratorFlow::setReroute(bool value) {
    myReroute = value;
    return true;
}


bool
GNECalibratorFlow::setReroute(std::string value) {
    if (GNEAttributeCarrier::canParse<bool>(value)) {
        return setReroute(GNEAttributeCarrier::parse<bool>(value));
    } else {
        return false;
    }
}


bool
GNECalibratorFlow::setDepartPosLat(std::string departPosLat) {
    double departPosLatFloat = -1;
    if (GNEAttributeCarrier::canParse<double>(departPosLat)) {
        departPosLatFloat = GNEAttributeCarrier::parse<double>(departPosLat);
    }
    if ((departPosLatFloat < 0) && (departPosLat != "random") && (departPosLat != "random_free") &&
            (departPosLat != "left") && (departPosLat != "right") && (departPosLat != "center")) {
        return false;
    } else {
        myDepartPosLat = departPosLat;
        return true;
    }
}


bool
GNECalibratorFlow::setArrivalPosLat(std::string arrivalPosLat) {
    double arrivalPosLatFloat = -1;
    if (GNEAttributeCarrier::canParse<double>(arrivalPosLat)) {
        arrivalPosLatFloat = GNEAttributeCarrier::parse<double>(arrivalPosLat);
    }
    if ((arrivalPosLatFloat < 0) && (arrivalPosLat != "left") && (arrivalPosLat != "right") && (arrivalPosLat != "center") && (arrivalPosLat != "")) {
        return false;
    } else {
        myArrivalPosLat = arrivalPosLat;
        return true;
    }
}


bool
GNECalibratorFlow::setBegin(double begin) {
    if (begin < 0) {
        return false;
    } else {
        myBegin = begin;
        return true;
    }
}


bool
GNECalibratorFlow::setBegin(std::string begin) {
    if (GNEAttributeCarrier::canParse<double>(begin)) {
        return setBegin(GNEAttributeCarrier::parse<double>(begin));
    } else {
        return false;
    }
}


bool
GNECalibratorFlow::setEnd(double end) {
    if (end < 0) {
        return false;
    } else {
        myEnd = end;
        return true;
    }
}


bool
GNECalibratorFlow::setEnd(std::string end) {
    if (GNEAttributeCarrier::canParse<double>(end)) {
        return setEnd(GNEAttributeCarrier::parse<double>(end));
    } else {
        return false;
    }
}


bool
GNECalibratorFlow::setVehsPerHour(double vehsPerHour) {
    if (vehsPerHour < 0) {
        return false;
    } else {
        myVehsPerHour = vehsPerHour;
        return true;
    }
}


bool
GNECalibratorFlow::setVehsPerHour(std::string vehsPerHour) {
    if (GNEAttributeCarrier::canParse<double>(vehsPerHour)) {
        return setVehsPerHour(GNEAttributeCarrier::parse<double>(vehsPerHour));
    } else {
        return false;
    }
}


bool
GNECalibratorFlow::setPeriod(double period) {
    if (period < 0) {
        return false;
    } else {
        myPeriod = period;
        return true;
    }
}


bool
GNECalibratorFlow::setPeriod(std::string period) {
    if (GNEAttributeCarrier::canParse<double>(period)) {
        return setPeriod(GNEAttributeCarrier::parse<double>(period));
    } else {
        return false;
    }
}

bool
GNECalibratorFlow::setProbability(double probability) {
    if ((probability < 0) || (probability > 1)) {
        return false;
    } else {
        myProbability = probability;
        return true;
    }
}


bool
GNECalibratorFlow::setProbability(std::string probability) {
    if (GNEAttributeCarrier::canParse<double>(probability)) {
        return setProbability(GNEAttributeCarrier::parse<double>(probability));
    } else {
        return false;
    }
}


bool
GNECalibratorFlow::setNumber(int number) {
    if (number < 0) {
        return false;
    } else {
        myNumber = number;
        return true;
    }
}


bool
GNECalibratorFlow::setNumber(std::string number) {
    if (GNEAttributeCarrier::canParse<int>(number)) {
        return setNumber(GNEAttributeCarrier::parse<int>(number));
    } else if (GNEAttributeCarrier::canParse<double>(number)) {
        double numberDouble = GNEAttributeCarrier::parse<double>(number);
        // check if double is an integer
        if (fmod(numberDouble, 1) == 0) {
            return setNumber((int)numberDouble);
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool
GNECalibratorFlow::setTypeOfFlow(TypeOfFlow type) {
    if ((type == GNE_CALIBRATORFLOW_VEHSPERHOUR) || (type == GNE_CALIBRATORFLOW_PERIOD) || (type == GNE_CALIBRATORFLOW_PROBABILITY)) {
        myTypeOfFlow = type;
        return true;
    } else {
        return false;
    }
}


bool
GNECalibratorFlow::operator==(const GNECalibratorFlow& calibratorFlow) const {
    return (myFlowID == calibratorFlow.getFlowID());
}

/****************************************************************************/

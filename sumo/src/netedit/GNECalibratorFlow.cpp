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

#include "GNECalibratorFlow.h"
#include "GNECalibrator.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// member method definitions
// ===========================================================================


GNECalibratorFlow::GNECalibratorFlow(GNECalibrator* calibratorParent, std::string type, std::string route) :
    myCalibratorParent(calibratorParent), myType(type), myRoute(route), myColor(""), myDepartLane("first"),
    myDepartPos("base"), myDepartSpeed("0"), myArrivalLane("current"), myArrivalPos("max"), myArrivalSpeed("current"),
    myLine(""), myPersonNumber(0), myContainerNumber(0), myBegin(0), myEnd(0), myVehsPerHour(0), myPeriod(0), myProbability(0), myNumber(0) {}

GNECalibratorFlow::GNECalibratorFlow(GNECalibrator* calibratorParent, std::string type, std::string route,
    std::string color, std::string departLane, std::string departPos, std::string departSpeed, std::string arrivalLane,
    std::string arrivalPos, std::string arrivalSpeed, std::string line, int personNumber, int containerNumber,
    double begin, double end, double vehsPerHour, double period, double probability, int number) :
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


GNECalibratorFlow::~GNECalibratorFlow() {}


GNECalibrator*
GNECalibratorFlow::getCalibratorParent() const {
    return myCalibratorParent;
}


const std::string&
GNECalibratorFlow::getType() const {
    return myType;
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


bool
GNECalibratorFlow::setType(std::string type) {
    if (type.empty()) {
        return false;
    } else {
        myType = type;
        return true;
    }
}


bool
GNECalibratorFlow::setRoute(std::string route) {
    if (route.empty()) {
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
        myDepartLane = arrivalLane;
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
        myDepartPos = arrivalPos;
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
        myDepartSpeed = arrivalSpeed;
        return true;
    }
}


bool
GNECalibratorFlow::setLine(std::string line) {
    /// @todo check if line exists
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
GNECalibratorFlow::setContainerNumber(int containerNumber) {
    if (containerNumber < 0) {
        return false;
    } else {
        myContainerNumber = containerNumber;
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
GNECalibratorFlow::setEnd(double end) {
    if (end < 0) {
        return false;
    } else {
        myEnd = end;
        return true;
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
GNECalibratorFlow::setPeriod(double period) {
    if (period < 0) {
        return false;
    } else {
        myPeriod = period;
        return true;
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
GNECalibratorFlow::setNumber(int number) {
    if (number < 0) {
        return false;
    } else {
        myNumber = number;
        return true;
    }
}


/****************************************************************************/

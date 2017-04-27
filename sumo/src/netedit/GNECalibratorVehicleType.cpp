/****************************************************************************/
/// @file    GNECalibratorVehicleType.cpp
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

#include "GNECalibratorVehicleType.h"
#include "GNECalibrator.h"
#include "GNEViewNet.h"
#include "GNENet.h"


// ===========================================================================
// member method definitions
// ===========================================================================


GNECalibratorVehicleType::GNECalibratorVehicleType(GNECalibrator* calibratorParent) :
    myCalibratorParent(calibratorParent), myVehicleTypeID(calibratorParent->generateVehicleTypeID()) {
    // setset default parameters
    setAccel();
    setDecel();
    setSigma();
    setTau();
    setLength();
    setMinGap();
    setMaxSpeed();
    setSpeedFactor();
    setSpeedDev();
    setColor();
    setVClass();
    setEmissionClass();
    setShape();
    setWidth();
    setFilename();
    setImpatience();
    setLaneChangeModel();
    setCarFollowModel();
    setPersonCapacity();
    setContainerCapacity();
    setBoardingDuration();
    setLoadingDuration();
    setLatAlignment();
    setMinGapLat();
    setMaxSpeedLat();
}


GNECalibratorVehicleType::GNECalibratorVehicleType(GNECalibrator* calibratorParent, std::string vehicleTypeID,
        double accel, double decel, double sigma, double tau, double length, double minGap, double maxSpeed,
        double speedFactor, double speedDev, const std::string& color, SUMOVehicleClass vClass, const std::string& emissionClass,
        SUMOVehicleShape shape, double width, const std::string& filename, double impatience, const std::string& laneChangeModel,
        const std::string& carFollowModel, int personCapacity, int containerCapacity, double boardingDuration,
        double loadingDuration, const std::string& latAlignment, double minGapLat, double maxSpeedLat) :
    myCalibratorParent(calibratorParent), myVehicleTypeID(calibratorParent->generateVehicleTypeID()) {
    // set parameters using the set functions, to avoid non valid values
    setVehicleTypeID(vehicleTypeID);
    setAccel(accel);
    setDecel(decel);
    setSigma(sigma);
    setTau(tau);
    setLength(length);
    setMinGap(minGap);
    setMaxSpeed(maxSpeed);
    setSpeedFactor(speedFactor);
    setSpeedDev(speedDev);
    setColor(color);
    setVClass(vClass);
    setEmissionClass(emissionClass);
    setShape(shape);
    setWidth(width);
    setFilename(filename);
    setImpatience(impatience);
    setLaneChangeModel(laneChangeModel);
    setCarFollowModel(carFollowModel);
    setPersonCapacity(personCapacity);
    setContainerCapacity(containerCapacity);
    setBoardingDuration(boardingDuration);
    setLoadingDuration(loadingDuration);
    setLatAlignment(latAlignment);
    setMinGapLat(minGapLat);
    setMaxSpeedLat(maxSpeedLat);
}


GNECalibratorVehicleType::~GNECalibratorVehicleType() {}


GNECalibrator*
GNECalibratorVehicleType::getCalibratorParent() const {
    return myCalibratorParent;
}


SumoXMLTag
GNECalibratorVehicleType::getTag() const {
    return SUMO_TAG_VTYPE;
}


std::string
GNECalibratorVehicleType::getVehicleTypeID() const {
    return myVehicleTypeID;
}


double
GNECalibratorVehicleType::getAccel() const {
    return myAccel;
}


double
GNECalibratorVehicleType::getDecel() const {
    return myDecel;
}


double
GNECalibratorVehicleType::getSigma() const {
    return mySigma;
}


double
GNECalibratorVehicleType::getTau() const {
    return myTau;
}


double
GNECalibratorVehicleType::getLength() const {
    return myLength;
}


double
GNECalibratorVehicleType::getMinGap() const {
    return myMinGap;
}


double
GNECalibratorVehicleType::getMaxSpeed() const {
    return myMaxSpeed;
}


double
GNECalibratorVehicleType::getSpeedFactor() const {
    return mySpeedFactor;
}


double
GNECalibratorVehicleType::getSpeedDev() const {
    return mySpeedDev;
}


std::string
GNECalibratorVehicleType::getColor() const {
    return myColor;
}


SUMOVehicleClass
GNECalibratorVehicleType::getVClass() const {
    return myVClass;
}


std::string
GNECalibratorVehicleType::getEmissionClass() const {
    return myEmissionClass;
}


SUMOVehicleShape
GNECalibratorVehicleType::getShape() const {
    return myShape;
}


double
GNECalibratorVehicleType::getWidth() const {
    return myWidth;
}


std::string
GNECalibratorVehicleType::getFilename() const {
    return myFilename;
}


double
GNECalibratorVehicleType::getImpatience() const {
    return myImpatience;
}


std::string
GNECalibratorVehicleType::getLaneChangeModel() const {
    return myLaneChangeModel;
}


std::string
GNECalibratorVehicleType::getCarFollowModel() const {
    return myCarFollowModel;
}


int
GNECalibratorVehicleType::getPersonCapacity() const {
    return myPersonCapacity;
}


int
GNECalibratorVehicleType::getContainerCapacity() const {
    return myContainerCapacity;
}


double
GNECalibratorVehicleType::getBoardingDuration() const {
    return myBoardingDuration;
}


double
GNECalibratorVehicleType::getLoadingDuration() const {
    return myLoadingDuration;
}


std::string
GNECalibratorVehicleType::getLatAlignment() const {
    return myLatAlignment;
}


double
GNECalibratorVehicleType::getMinGapLat() const {
    return myMinGapLat;
}


double
GNECalibratorVehicleType::getMaxSpeedLat() const {
    return myMaxSpeedLat;
}

bool
GNECalibratorVehicleType::setVehicleTypeID(std::string vehicleTypeID) {
    if (vehicleTypeID.empty()) {
        return false;
    } else if (myCalibratorParent->getViewNet()->getNet()->vehicleTypeExists(vehicleTypeID) == true) {
        return false;
    } else {
        myVehicleTypeID = vehicleTypeID;
        return true;
    }
}


bool
GNECalibratorVehicleType::setAccel(double accel) {
    if (accel < 0) {
        return false;
    } else {
        myAccel = accel;
        return true;
    }
}


bool
GNECalibratorVehicleType::setAccel(std::string accel) {
    if (GNEAttributeCarrier::canParse<double>(accel)) {
        return setAccel(GNEAttributeCarrier::parse<double>(accel));
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setDecel(double decel) {
    if (decel < 0) {
        return false;
    } else {
        myDecel = decel;
        return true;
    }
}


bool
GNECalibratorVehicleType::setDecel(std::string decel) {
    if (GNEAttributeCarrier::canParse<double>(decel)) {
        return setDecel(GNEAttributeCarrier::parse<double>(decel));
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setSigma(double sigma) {
    if (sigma < 0) {
        return false;
    } else {
        mySigma = sigma;
        return true;
    }
}


bool
GNECalibratorVehicleType::setSigma(std::string sigma) {
    if (GNEAttributeCarrier::canParse<double>(sigma)) {
        return setSigma(GNEAttributeCarrier::parse<double>(sigma));
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setTau(double tau) {
    if (tau < 0) {
        return false;
    } else {
        myTau = tau;
        return true;
    }
}


bool
GNECalibratorVehicleType::setTau(std::string tau) {
    if (GNEAttributeCarrier::canParse<double>(tau)) {
        return setTau(GNEAttributeCarrier::parse<double>(tau));
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setLength(double length) {
    if (length < 0) {
        return false;
    } else {
        myLength = length;
        return true;
    }
}


bool
GNECalibratorVehicleType::setLength(std::string length) {
    if (GNEAttributeCarrier::canParse<double>(length)) {
        return setLength(GNEAttributeCarrier::parse<double>(length));
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setMinGap(double minGap) {
    if (minGap < 0) {
        return false;
    } else {
        myMinGap = minGap;
        return true;
    }
}


bool
GNECalibratorVehicleType::setMinGap(std::string minGap) {
    if (GNEAttributeCarrier::canParse<double>(minGap)) {
        return setMinGap(GNEAttributeCarrier::parse<double>(minGap));
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setMaxSpeed(double maxSpeed) {
    if (maxSpeed < 0) {
        return false;
    } else {
        myMaxSpeed = maxSpeed;
        return true;
    }
}


bool
GNECalibratorVehicleType::setMaxSpeed(std::string maxSpeed) {
    if (GNEAttributeCarrier::canParse<double>(maxSpeed)) {
        return setMaxSpeed(GNEAttributeCarrier::parse<double>(maxSpeed));
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setSpeedFactor(double speedFactor) {
    if (speedFactor < 0) {
        return false;
    } else {
        mySpeedFactor = speedFactor;
        return true;
    }
}


bool
GNECalibratorVehicleType::setSpeedFactor(std::string speedFactor) {
    if (GNEAttributeCarrier::canParse<double>(speedFactor)) {
        return setSpeedFactor(GNEAttributeCarrier::parse<double>(speedFactor));
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setSpeedDev(double speedDev) {
    if (speedDev < 0) {
        return false;
    } else {
        mySpeedDev = speedDev;
        return true;
    }
}


bool
GNECalibratorVehicleType::setSpeedDev(std::string speedDev) {
    if (GNEAttributeCarrier::canParse<double>(speedDev)) {
        return setSpeedDev(GNEAttributeCarrier::parse<double>(speedDev));
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setColor(std::string color) {
    myColor = color;
    return true;
}


bool
GNECalibratorVehicleType::setVClass(SUMOVehicleClass vClass) {
    myVClass = vClass;
    return true;
}


bool
GNECalibratorVehicleType::setVClass(std::string vClass) {
    if (canParseVehicleClasses(vClass) == true) {
        return setVClass(getVehicleClassID(vClass));
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setEmissionClass(std::string emissionClass) {
    myEmissionClass = emissionClass;
    return true;
}


bool
GNECalibratorVehicleType::setShape(SUMOVehicleShape shape) {
    myShape = shape;
    return true;
}


bool
GNECalibratorVehicleType::setShape(std::string shape) {
    if (canParseVehicleShape(shape)) {
        return setShape(getVehicleShapeID(shape));
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setWidth(double width) {
    if (width < 0) {
        return false;
    } else {
        myWidth = width;
        return true;
    }
}


bool
GNECalibratorVehicleType::setWidth(std::string width) {
    if (GNEAttributeCarrier::canParse<double>(width)) {
        return setWidth(GNEAttributeCarrier::parse<double>(width));
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setFilename(std::string filename) {
    myFilename = filename;
    return true;
}


bool
GNECalibratorVehicleType::setImpatience(double impatience) {
    if (impatience < 0) {
        return false;
    } else {
        myImpatience = impatience;
        return true;
    }
}


bool
GNECalibratorVehicleType::setImpatience(std::string impatience) {
    if (GNEAttributeCarrier::canParse<double>(impatience)) {
        return setImpatience(GNEAttributeCarrier::parse<double>(impatience));
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setLaneChangeModel(std::string laneChangeModel) {
    myLaneChangeModel = laneChangeModel;
    return true;
}


bool
GNECalibratorVehicleType::setCarFollowModel(std::string carFollowModel) {
    myCarFollowModel = carFollowModel;
    return true;
}


bool
GNECalibratorVehicleType::setPersonCapacity(int personCapacity) {
    if (personCapacity < 0) {
        return false;
    } else {
        myPersonCapacity = personCapacity;
        return true;
    }
}


bool
GNECalibratorVehicleType::setPersonCapacity(std::string personCapacity) {
    if (GNEAttributeCarrier::canParse<double>(personCapacity)) {
        double personCapacityD = GNEAttributeCarrier::parse<double>(personCapacity);
        // Check that double doesn't have decimals
        if (fmod(personCapacityD, 1) == 0) {
            return setPersonCapacity((int)personCapacityD);
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setContainerCapacity(int containerCapacity) {
    if (containerCapacity < 0) {
        return false;
    } else {
        myContainerCapacity = containerCapacity;
        return true;
    }
}


bool
GNECalibratorVehicleType::setContainerCapacity(std::string containerCapacity) {
    if (GNEAttributeCarrier::canParse<double>(containerCapacity)) {
        double containerCapacityD = GNEAttributeCarrier::parse<double>(containerCapacity);
        // Check that double doesn't have decimals
        if (fmod(containerCapacityD, 1) == 0) {
            return setContainerCapacity((int)containerCapacityD);
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setBoardingDuration(double boardingDuration) {
    if (boardingDuration < 0) {
        return false;
    } else {
        myBoardingDuration = boardingDuration;
        return true;
    }
}


bool
GNECalibratorVehicleType::setBoardingDuration(std::string boardingDuration) {
    if (GNEAttributeCarrier::canParse<double>(boardingDuration)) {
        return setBoardingDuration(GNEAttributeCarrier::parse<double>(boardingDuration));
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setLoadingDuration(double loadingDuration) {
    if (loadingDuration < 0) {
        return false;
    } else {
        myLoadingDuration = loadingDuration;
        return true;
    }
}


bool
GNECalibratorVehicleType::setLoadingDuration(std::string loadingDuration) {
    if (GNEAttributeCarrier::canParse<double>(loadingDuration)) {
        return setLoadingDuration(GNEAttributeCarrier::parse<double>(loadingDuration));
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setLatAlignment(std::string latAlignment) {
    if ((latAlignment == "left") || (latAlignment == "right") || (latAlignment == "center") ||
            (latAlignment == "compact") || (latAlignment == "nice") || (latAlignment == "arbitrary")) {
        myLatAlignment = latAlignment;
        return true;
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setMinGapLat(double minGapLat) {
    if (minGapLat < 0) {
        return false;
    } else {
        myMinGapLat = minGapLat;
        return true;
    }
}


bool
GNECalibratorVehicleType::setMinGapLat(std::string minGapLat) {
    if (GNEAttributeCarrier::canParse<double>(minGapLat)) {
        return setMinGapLat(GNEAttributeCarrier::parse<double>(minGapLat));
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::setMaxSpeedLat(double maxSpeedLat) {
    if (maxSpeedLat < 0) {
        return false;
    } else {
        myMaxSpeedLat = maxSpeedLat;
        return true;
    }
}


bool
GNECalibratorVehicleType::setMaxSpeedLat(std::string maxSpeedLat) {
    if (GNEAttributeCarrier::canParse<double>(maxSpeedLat)) {
        return setMaxSpeedLat(GNEAttributeCarrier::parse<double>(maxSpeedLat));
    } else {
        return false;
    }
}


bool
GNECalibratorVehicleType::operator==(const GNECalibratorVehicleType& calibratorVehicleType) const {
    return (myVehicleTypeID == calibratorVehicleType.getVehicleTypeID());
}

/****************************************************************************/

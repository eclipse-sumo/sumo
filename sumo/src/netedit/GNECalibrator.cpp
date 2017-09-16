/****************************************************************************/
/// @file    GNECalibrator.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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


// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibrator::GNECalibrator(const std::string& id, GNEViewNet* viewNet, double pos, double frequency, const std::string& output, 
                             const std::vector<GNECalibratorRoute>& calibratorRoutes, const std::vector<GNECalibratorFlow>& calibratorFlows, 
                             const std::vector<GNECalibratorVehicleType>& calibratorVehicleTypes) :
    GNEAdditional(id, viewNet, SUMO_TAG_CALIBRATOR, ICON_CALIBRATOR),
    myPositionOverLane(/**pos / lane->getLaneParametricLength() **/ 0),
    myFrequency(frequency),
    myOutput(output),
    myRouteProbe(NULL), /** change this in the future **/
    myCalibratorRoutes(calibratorRoutes),
    myCalibratorFlows(calibratorFlows),
    myCalibratorVehicleTypes(calibratorVehicleTypes) {
    // this additional ISN'T movable
    myMovable = false;
    // Center view in the position of calibrator
    myViewNet->centerTo(getGlID(), false);
}


GNECalibrator::~GNECalibrator() {}


void
GNECalibrator::moveGeometry(const Position&) {
    // This additional cannot be moved currently
}


void
GNECalibrator::commitGeometryMoving(const Position&, GNEUndoList*) {
    // This additional cannot be moved currently
}


Position
GNECalibrator::getPositionInView() const {
    return myLane->getShape().positionAtOffset(myPositionOverLane * myLane->getShape().length());
}


void
GNECalibrator::openAdditionalDialog() {
    // Open calibrator dialog
    GNECalibratorDialog calibratorDialog(this);
}


void
GNECalibrator::addCalibratorVehicleType(const GNECalibratorVehicleType& vehicleType) {
    myCalibratorVehicleTypes.push_back(vehicleType);
}


void
GNECalibrator::addCalibratorFlow(const GNECalibratorFlow& flow) {
    myCalibratorFlows.push_back(flow);
}


void
GNECalibrator::addCalibratorRoute(const GNECalibratorRoute& route) {
    myCalibratorRoutes.push_back(route);
}


const std::vector<GNECalibratorVehicleType>&
GNECalibrator::getCalibratorVehicleTypes() const {
    return myCalibratorVehicleTypes;
}


const std::vector<GNECalibratorFlow>&
GNECalibrator::getCalibratorFlows() const {
    return myCalibratorFlows;
}


const std::vector<GNECalibratorRoute>&
GNECalibrator::getCalibratorRoutes() const {
    return myCalibratorRoutes;
}


void
GNECalibrator::setCalibratorVehicleTypes(const std::vector<GNECalibratorVehicleType>& calibratorVehicleTypes) {
    myCalibratorVehicleTypes = calibratorVehicleTypes;
}


void
GNECalibrator::setCalibratorFlows(const std::vector<GNECalibratorFlow>& calibratorFlows) {
    myCalibratorFlows = calibratorFlows;
}


void
GNECalibrator::setCalibratorRoutes(const std::vector<GNECalibratorRoute>& calibratorRoutes) {
    myCalibratorRoutes = calibratorRoutes;
}


std::string
GNECalibrator::generateVehicleTypeID() const {
    int counter = 0;
    while (myViewNet->getNet()->vehicleTypeExists(toString(SUMO_TAG_VTYPE) + toString(counter))) {
        counter++;
    }
    return (toString(SUMO_TAG_VTYPE) + toString(counter));
}


std::string
GNECalibrator::generateFlowID() const {
    int counter = 0;
    while (myViewNet->getNet()->flowExists(toString(SUMO_TAG_FLOW) + toString(counter))) {
        counter++;
    }
    return (toString(SUMO_TAG_FLOW) + toString(counter));
}


std::string
GNECalibrator::generateRouteID() const {
    int counter = 0;
    while (myViewNet->getNet()->routeExists(toString(SUMO_TAG_ROUTE) + toString(counter))) {
        counter++;
    }
    return (toString(SUMO_TAG_ROUTE) + toString(counter));
}


bool
GNECalibrator::vehicleTypeExists(std::string vehicleTypeID) const {
    for (std::vector<GNECalibratorVehicleType>::const_iterator i = myCalibratorVehicleTypes.begin(); i != myCalibratorVehicleTypes.end(); i++) {
        if (i->getVehicleTypeID() == vehicleTypeID) {
            return true;
        }
    }
    return false;
}


bool
GNECalibrator::flowExists(std::string flowID) const {
    for (std::vector<GNECalibratorFlow>::const_iterator i = myCalibratorFlows.begin(); i != myCalibratorFlows.end(); i++) {
        if (i->getFlowID() == flowID) {
            return true;
        }
    }
    return false;
}


bool
GNECalibrator::routeExists(std::string routeID) const {
    for (std::vector<GNECalibratorRoute>::const_iterator i = myCalibratorRoutes.begin(); i != myCalibratorRoutes.end(); i++) {
        if (i->getRouteID() == routeID) {
            return true;
        }
    }
    return false;
}


const GNECalibratorVehicleType&
GNECalibrator::getCalibratorVehicleType(const std::string& vehicleTypeID) {
    for (std::vector<GNECalibratorVehicleType>::iterator i = myCalibratorVehicleTypes.begin(); i != myCalibratorVehicleTypes.end(); i++) {
        if (i->getVehicleTypeID() == vehicleTypeID) {
            return (*i);
        }
    }
    throw InvalidArgument(toString(getTag()) + " " + getID() + " doesn't have a " + toString(SUMO_TAG_VTYPE) + " with id = '" + vehicleTypeID + "'");
}


const GNECalibratorFlow&
GNECalibrator::getCalibratorFlow(const std::string& flowID) {
    for (std::vector<GNECalibratorFlow>::iterator i = myCalibratorFlows.begin(); i != myCalibratorFlows.end(); i++) {
        if (i->getFlowID() == flowID) {
            return (*i);
        }
    }
    throw InvalidArgument(toString(getTag()) + " " + getID() + " doesn't have a " + toString(SUMO_TAG_FLOW) + " with id = '" + flowID + "'");
}


const GNECalibratorRoute&
GNECalibrator::getCalibratorRoute(const std::string& routeID) {
    for (std::vector<GNECalibratorRoute>::iterator i = myCalibratorRoutes.begin(); i != myCalibratorRoutes.end(); i++) {
        if (i->getRouteID() == routeID) {
            return (*i);
        }
    }
    throw InvalidArgument(toString(getTag()) + " " + getID() + " doesn't have a " + toString(SUMO_TAG_ROUTE) + " with id = '" + routeID + "'");
}

const std::string&
GNECalibrator::getParentName() const {
    return myLane->getMicrosimID();
}

/****************************************************************************/

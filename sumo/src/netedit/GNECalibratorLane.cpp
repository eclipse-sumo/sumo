/****************************************************************************/
/// @file    GNECalibratorLane.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2017
/// @version $Id: GNECalibratorLane.cpp 25918 2017-09-07 19:38:16Z behrisch $
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

#include "GNECalibratorLane.h"
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

GNECalibratorLane::GNECalibratorLane(const std::string& id, GNELane* lane, GNEViewNet* viewNet, double pos,
    double frequency, const std::string& output, const std::vector<GNECalibratorRoute>& calibratorRoutes,
    const std::vector<GNECalibratorFlow>& calibratorFlows, const std::vector<GNECalibratorVehicleType>& calibratorVehicleTypes) :
    GNECalibrator(id, viewNet, SUMO_TAG_CALIBRATOR_LANE, pos, frequency, output, calibratorRoutes, calibratorFlows, calibratorVehicleTypes)  {
    // This additional belong to a lane
    myLane = lane;
    // this additional ISN'T movable
    myMovable = false;
    // Update geometry;
    updateGeometry();
    // Center view in the position of calibrator
    myViewNet->centerTo(getGlID(), false);
}


GNECalibratorLane::~GNECalibratorLane() {}


std::string
GNECalibratorLane::getAttribute(SumoXMLAttr key) const {
    switch (key) {
    case SUMO_ATTR_ID:
        return getAdditionalID();
    case SUMO_ATTR_LANE:
        return toString(myLane->getID());
    case SUMO_ATTR_POSITION:
        return toString(myPositionOverLane * myLane->getLaneParametricLength());
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
GNECalibratorLane::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
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
GNECalibratorLane::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
    case SUMO_ATTR_ID:
        if (isValidID(value) && (myViewNet->getNet()->getAdditional(getTag(), value) == NULL)) {
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
        if (canParse<double>(value)) {
            // obtain relative new start position
            double newStartPos = parse<double>(value) / myLane->getLaneParametricLength();
            if ((newStartPos < 0) || (newStartPos > 1)) {
                return false;
            } else {
                return true;
            }
        } else {
            return false;
        }
    case SUMO_ATTR_FREQUENCY:
        return (canParse<double>(value) && parse<double>(value) >= 0);
    case SUMO_ATTR_OUTPUT:
        return isValidFilename(value);
    case SUMO_ATTR_ROUTEPROBE:
        if (isValidID(value) && (myViewNet->getNet()->getAdditional(SUMO_TAG_ROUTEPROBE, value) != NULL)) {
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
GNECalibratorLane::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
    case SUMO_ATTR_ID:
        setAdditionalID(value);
        break;
    case SUMO_ATTR_LANE:
        changeLane(value);
        break;
    case SUMO_ATTR_POSITION:
        myPositionOverLane = parse<double>(value) / myLane->getShape().length();
        updateGeometry();
        getViewNet()->update();
        break;
    case SUMO_ATTR_FREQUENCY:
        myFrequency = parse<double>(value);
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

/****************************************************************************/

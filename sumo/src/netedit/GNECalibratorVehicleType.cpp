/****************************************************************************/
/// @file    GNECalibratorVehicleType.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2016
/// @version $Id: GNECalibratorVehicleType.cpp 23530 2017-03-18 11:20:40Z palcraft $
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


// ===========================================================================
// member method definitions
// ===========================================================================


GNECalibratorVehicleType::GNECalibratorVehicleType(GNECalibrator* calibratorParent) :
    myCalibratorParent(calibratorParent), myVehicleTypeID(calibratorParent->generateVehicleTypeID()) {}


GNECalibratorVehicleType::GNECalibratorVehicleType(GNECalibrator* calibratorParent, std::string vehicleTypeID) :
    myCalibratorParent(calibratorParent), myVehicleTypeID(calibratorParent->generateVehicleTypeID()) {
    // set parameters using the set functions, to avoid non valid values
    setVehicleTypeID(vehicleTypeID);
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


bool 
GNECalibratorVehicleType::setVehicleTypeID(std::string vehicleTypeID) {
    if (vehicleTypeID.empty()) {
        return false;
    } else if(myCalibratorParent->vehicleTypeExists(vehicleTypeID) == true) {
        return false;
    } else {
        myVehicleTypeID = vehicleTypeID;
        return true;
    }
}

/****************************************************************************/

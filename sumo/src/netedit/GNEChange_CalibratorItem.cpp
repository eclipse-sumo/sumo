/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    GNEChange_Attribute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 1017
/// @version $Id$
///
// A change in the values of Calibrators in netedit
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>

#include "GNEChange_CalibratorItem.h"
#include "GNENet.h"
#include "GNEViewNet.h"
#include "GNECalibrator.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_CalibratorItem, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


/// @brief constructor for changing selection
GNEChange_CalibratorItem::GNEChange_CalibratorItem(GNECalibrator *calibrator, 
    const std::vector<GNECalibratorRoute>& newCalibratorRoutes, 
    const std::vector<GNECalibratorFlow>& newCalibratorFlows,
    const std::vector<GNECalibratorVehicleType>& newCalibratorVehicleTypes) :
    GNEChange(calibrator->getViewNet()->getNet(), true),
    myCalibrator(calibrator),
    myOldCalibratorRoutes(calibrator->getCalibratorRoutes()),
    myOldCalibratorFlows(calibrator->getCalibratorFlows()),
    myOldCalibratorVehicleTypes(calibrator->getCalibratorVehicleTypes()),
    myNewCalibratorRoutes(newCalibratorRoutes),
    myNewCalibratorFlows(newCalibratorFlows),
    myNewCalibratorVehicleTypes(newCalibratorVehicleTypes) {
}


GNEChange_CalibratorItem::~GNEChange_CalibratorItem() {
}


void
GNEChange_CalibratorItem::undo() {
    // show extra information for tests
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Setting previous " + 
                      toString(myOldCalibratorFlows.size() + myOldCalibratorRoutes.size() + myOldCalibratorVehicleTypes.size()) + 
                      " calibrator values into " + toString(myCalibrator->getTag()) + " '" + myCalibrator->getID() + "'");
    }
    // set original calibrators values
    myCalibrator->setCalibratorFlows(myOldCalibratorFlows);
    myCalibrator->setCalibratorRoutes(myOldCalibratorRoutes);
    myCalibrator->setCalibratorVehicleTypes(myOldCalibratorVehicleTypes);
    // enable save additionals
    myCalibrator->getViewNet()->getNet()->requiereSaveAdditionals();
}


void
GNEChange_CalibratorItem::redo() {
    // show extra information for tests
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Setting new " + 
            toString(myNewCalibratorFlows.size() + myNewCalibratorRoutes.size() + myNewCalibratorVehicleTypes.size()) + 
            " calibrator values into " + toString(myCalibrator->getTag()) + " '" + myCalibrator->getID() + "'");
    }
    // set new calibrators values
    myCalibrator->setCalibratorFlows(myNewCalibratorFlows);
    myCalibrator->setCalibratorRoutes(myNewCalibratorRoutes);
    myCalibrator->setCalibratorVehicleTypes(myNewCalibratorVehicleTypes);
    // enable save additionals
    myCalibrator->getViewNet()->getNet()->requiereSaveAdditionals();
}


FXString
GNEChange_CalibratorItem::undoName() const {
    return ("Undo change calibrator values");
}


FXString
GNEChange_CalibratorItem::redoName() const {
    return ("Redo change calibrator values");
}

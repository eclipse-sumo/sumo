/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_CalibratorItem.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2017
/// @version $Id$
///
// A change in the values of Calibrators in netedit
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/additionals/GNECalibrator.h>
#include <netedit/additionals/GNECalibratorFlow.h>
#include <netedit/additionals/GNECalibratorRoute.h>
#include <netedit/additionals/GNECalibratorVehicleType.h>

#include "GNEChange_CalibratorItem.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_CalibratorItem, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


GNEChange_CalibratorItem::GNEChange_CalibratorItem(GNECalibratorRoute* calibratorRoute, bool forward) :
    GNEChange(calibratorRoute->getViewNet()->getNet(), forward),
    myCalibratorRoute(calibratorRoute),
    myCalibratorVehicleType(nullptr) {
    myCalibratorRoute->incRef("GNEChange_CalibratorItem");
}


GNEChange_CalibratorItem::GNEChange_CalibratorItem(GNECalibratorVehicleType* calibratorVehicleType, bool forward) :
    GNEChange(calibratorVehicleType->getViewNet()->getNet(), forward),
    myCalibratorRoute(nullptr),
    myCalibratorVehicleType(calibratorVehicleType) {
    myCalibratorVehicleType->incRef("GNEChange_CalibratorItem");
}


GNEChange_CalibratorItem::~GNEChange_CalibratorItem() {
    if (myCalibratorRoute) {
        myCalibratorRoute->decRef("GNEChange_CalibratorItem");
        if (myCalibratorRoute->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting calibrator route");
            }
            // make sure that calibrator route isn't in net before removing
            if (myNet->retrieveCalibratorRoute(myCalibratorRoute->getID(), false)) {
                myNet->deleteCalibratorRoute(myCalibratorRoute);
            }
            delete myCalibratorRoute;
        }
    }
    // vehicle types can exist by themselves
}


void
GNEChange_CalibratorItem::undo() {
    if (myForward) {
        if (myCalibratorRoute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing calibrator route of calibrator '" + myCalibratorRoute->getCalibratorParent()->getID() + "'");
            }
            // remove calibrator route of calibrator and net
            myCalibratorRoute->getCalibratorParent()->removeCalibratorRoute(myCalibratorRoute);
            myNet->deleteCalibratorRoute(myCalibratorRoute);
        } else if (myCalibratorVehicleType) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing calibrator vehicle type '" + myCalibratorVehicleType->getID() + "'");
            }
            // remove calibrator vehicle type 
            myNet->deleteCalibratorVehicleType(myCalibratorVehicleType);
        } else {
            throw ProcessError("There isn't a defined Calibrator item");
        }
    } else {
        if (myCalibratorRoute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding calibrator route into calibrator '" + myCalibratorRoute->getCalibratorParent()->getID() + "'");
            }
            // add calibrator route into calibrator and net
            myCalibratorRoute->getCalibratorParent()->addCalibratorRoute(myCalibratorRoute);
            myNet->insertCalibratorRoute(myCalibratorRoute);
        } else if (myCalibratorVehicleType) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding calibrator vehicle type '" + myCalibratorVehicleType->getID() + "'");
            }
            // add calibrator vehicle type
            myNet->insertCalibratorVehicleType(myCalibratorVehicleType);
        } else {
            throw ProcessError("There isn't a defined Calibrator item");
        }
    }
    // enable save additionals
    myNet->requiereSaveAdditionals(true);
}


void
GNEChange_CalibratorItem::redo() {
    if (myForward) {
        if (myCalibratorRoute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding calibrator route into calibrator '" + myCalibratorRoute->getCalibratorParent()->getID() + "'");
            }
            // add calibrator route into calibrator and net
            myCalibratorRoute->getCalibratorParent()->addCalibratorRoute(myCalibratorRoute);
            myNet->insertCalibratorRoute(myCalibratorRoute);
        } else if (myCalibratorVehicleType) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding calibrator vehicle type '" + myCalibratorVehicleType->getID() + "'");
            }
            // add calibrator vehicle type
            myNet->insertCalibratorVehicleType(myCalibratorVehicleType);
        } else {
            throw ProcessError("There isn't a defined Calibrator item");
        }
    } else {
        if (myCalibratorRoute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing calibrator route of calibrator '" + myCalibratorRoute->getCalibratorParent()->getID() + "'");
            }
            // remove calibrator route of calibrator and net
            myCalibratorRoute->getCalibratorParent()->removeCalibratorRoute(myCalibratorRoute);
            myNet->deleteCalibratorRoute(myCalibratorRoute);
        } else if (myCalibratorVehicleType) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing calibrator vehicle type '" + myCalibratorVehicleType->getID() + "'");
            }
            // remove calibrator vehicle type
            myNet->deleteCalibratorVehicleType(myCalibratorVehicleType);
        } else {
            throw ProcessError("There isn't a defined Calibrator item");
        }
    }
    // enable save additionals
    myNet->requiereSaveAdditionals(true);
}


FXString
GNEChange_CalibratorItem::undoName() const {
    if (myCalibratorRoute) {
        return ("Undo change " + toString(myCalibratorRoute->getTag()) + " values").c_str();
    } else if (myCalibratorVehicleType) {
        return ("Undo change " + toString(myCalibratorVehicleType->getTag()) + " values").c_str();
    } else {
        throw ProcessError("There isn't a defined Calibrator item");
    }
}


FXString
GNEChange_CalibratorItem::redoName() const {
    if (myCalibratorRoute) {
        return ("Redo change " + toString(myCalibratorRoute->getTag()) + " values").c_str();
    } else if (myCalibratorVehicleType) {
        return ("Redo change " + toString(myCalibratorVehicleType->getTag()) + " values").c_str();
    } else {
        throw ProcessError("There isn't a defined Calibrator item");
    }
}

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
#include "GNECalibratorFlow.h"
#include "GNECalibratorRoute.h"
#include "GNECalibratorVehicleType.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_CalibratorItem, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


GNEChange_CalibratorItem::GNEChange_CalibratorItem(GNECalibratorFlow* calibratorFlow, bool forward) :
    GNEChange(calibratorFlow->getCalibratorParent()->getViewNet()->getNet(), forward),
    myCalibratorFlow(calibratorFlow),
    myCalibratorRoute(NULL),
    myCalibratorVehicleType(NULL) {
    myCalibratorFlow->incRef("GNEChange_CalibratorItem");
}


GNEChange_CalibratorItem::GNEChange_CalibratorItem(GNECalibratorRoute* calibratorRoute, bool forward) :
    GNEChange(calibratorRoute->getCalibratorParent()->getViewNet()->getNet(), forward),
    myCalibratorFlow(NULL),
    myCalibratorRoute(calibratorRoute),
    myCalibratorVehicleType(NULL) {
    myCalibratorRoute->incRef("GNEChange_CalibratorItem");
}


GNEChange_CalibratorItem::GNEChange_CalibratorItem(GNECalibratorVehicleType* calibratorVehicleType, bool forward) :
    GNEChange(calibratorVehicleType->getCalibratorParent()->getViewNet()->getNet(), forward),
    myCalibratorFlow(NULL),
    myCalibratorRoute(NULL),
    myCalibratorVehicleType(calibratorVehicleType) {
    myCalibratorVehicleType->incRef("GNEChange_CalibratorItem");
}


GNEChange_CalibratorItem::~GNEChange_CalibratorItem() {
    if (myCalibratorFlow) {
        myCalibratorFlow->decRef("GNEChange_CalibratorItem");
        if (myCalibratorFlow->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting calibrator flow of calibrator '" + myCalibratorFlow->getCalibratorParent()->getID() + "'");
            }
            // make sure that calibrator flow isn't in net before removing
            if (myNet->retrieveCalibratorFlow(myCalibratorFlow->getID(), false)) {
                myNet->deleteCalibratorFlow(myCalibratorFlow);
            }
            delete myCalibratorFlow;
        }
    } else if (myCalibratorRoute) {
        myCalibratorRoute->decRef("GNEChange_CalibratorItem");
        if (myCalibratorRoute->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting calibrator route of calibrator '" + myCalibratorRoute->getCalibratorParent()->getID() + "'");
            }
            // make sure that calibrator route isn't in net before removing
            if (myNet->retrieveCalibratorRoute(myCalibratorRoute->getID(), false)) {
                myNet->deleteCalibratorRoute(myCalibratorRoute);
            }
            delete myCalibratorRoute;
        }
    } else if (myCalibratorVehicleType) {
        myCalibratorVehicleType->decRef("GNEChange_CalibratorItem");
        if (myCalibratorVehicleType->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting calibrator vehicle type of calibrator '" + myCalibratorVehicleType->getCalibratorParent()->getID() + "'");
            }
            // make sure that calibrator Vehicle Type isn't in net before removing
            if (myNet->retrieveCalibratorVehicleType(myCalibratorVehicleType->getID(), false)) {
                myNet->deleteCalibratorVehicleType(myCalibratorVehicleType);
            }
            delete myCalibratorVehicleType;
        }
    }
}


void
GNEChange_CalibratorItem::undo() {
    if (myForward) {
        if (myCalibratorFlow) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing calibrator flow of calibrator '" + myCalibratorFlow->getCalibratorParent()->getID() + "'");
            }
            // remove calibrator flow of calibrator and net
            myCalibratorFlow->getCalibratorParent()->removeCalibratorFlow(myCalibratorFlow);
            myNet->deleteCalibratorFlow(myCalibratorFlow);
        } else if (myCalibratorRoute) {
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
                WRITE_WARNING("Removing calibrator vehicle type of calibrator '" + myCalibratorVehicleType->getCalibratorParent()->getID() + "'");
            }
            // remove calibrator vehicle type of calibrator and net
            myCalibratorVehicleType->getCalibratorParent()->removeCalibratorVehicleType(myCalibratorVehicleType);
            myNet->deleteCalibratorVehicleType(myCalibratorVehicleType);
        } else {
            throw ProcessError("There isn't a defined Calibrator item");
        }
    } else {
        if (myCalibratorFlow) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding calibrator flow into calibrator '" + myCalibratorFlow->getCalibratorParent()->getID() + "'");
            }
            // add calibrator flow into calibrator and net
            myCalibratorFlow->getCalibratorParent()->addCalibratorFlow(myCalibratorFlow);
            myNet->insertCalibratorFlow(myCalibratorFlow);
        } else if (myCalibratorRoute) {
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
                WRITE_WARNING("Adding calibrator vehicle type into calibrator '" + myCalibratorVehicleType->getCalibratorParent()->getID() + "'");
            }
            // add calibrator vehicle type into calibrator and net
            myCalibratorVehicleType->getCalibratorParent()->addCalibratorVehicleType(myCalibratorVehicleType);
            myNet->insertCalibratorVehicleType(myCalibratorVehicleType);
        } else {
            throw ProcessError("There isn't a defined Calibrator item");
        }
    }
    // enable save additionals
    myNet->requiereSaveAdditionals();
}


void
GNEChange_CalibratorItem::redo() {
    if (myForward) {
        if (myCalibratorFlow) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding calibrator flow into calibrator '" + myCalibratorFlow->getCalibratorParent()->getID() + "'");
            }
            // add calibrator flow into calibrator and net
            myCalibratorFlow->getCalibratorParent()->addCalibratorFlow(myCalibratorFlow);
            myNet->insertCalibratorFlow(myCalibratorFlow);
        } else if (myCalibratorRoute) {
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
                WRITE_WARNING("Adding calibrator vehicle type into calibrator '" + myCalibratorVehicleType->getCalibratorParent()->getID() + "'");
            }
            // add calibrator vehicle type into calibrator and net
            myCalibratorVehicleType->getCalibratorParent()->addCalibratorVehicleType(myCalibratorVehicleType);
            myNet->insertCalibratorVehicleType(myCalibratorVehicleType);
        } else {
            throw ProcessError("There isn't a defined Calibrator item");
        }
    } else {
        if (myCalibratorFlow) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing calibrator flow of calibrator '" + myCalibratorFlow->getCalibratorParent()->getID() + "'");
            }
            // remove calibrator flow of calibrator and net
            myCalibratorFlow->getCalibratorParent()->removeCalibratorFlow(myCalibratorFlow);
            myNet->deleteCalibratorFlow(myCalibratorFlow);
        } else if (myCalibratorRoute) {
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
                WRITE_WARNING("Removing calibrator vehicle type of calibrator '" + myCalibratorVehicleType->getCalibratorParent()->getID() + "'");
            }
            // remove calibrator vehicle type of calibrator and net
            myCalibratorVehicleType->getCalibratorParent()->removeCalibratorVehicleType(myCalibratorVehicleType);
            myNet->deleteCalibratorVehicleType(myCalibratorVehicleType);
        } else {
            throw ProcessError("There isn't a defined Calibrator item");
        }
    }
    // enable save additionals
    myNet->requiereSaveAdditionals();
}


FXString
GNEChange_CalibratorItem::undoName() const {
    if (myCalibratorFlow) {
        return ("Undo change " + toString(myCalibratorFlow->getTag()) + " values").c_str();
    } else if (myCalibratorRoute) {
        return ("Undo change " + toString(myCalibratorRoute->getTag()) + " values").c_str();
    } else if (myCalibratorVehicleType) {
        return ("Undo change " + toString(myCalibratorVehicleType->getTag()) + " values").c_str();
    } else {
        throw ProcessError("There isn't a defined Calibrator item");
    }
}


FXString
GNEChange_CalibratorItem::redoName() const {
    if (myCalibratorFlow) {
        return ("Redo change " + toString(myCalibratorFlow->getTag()) + " values").c_str();
    } else if (myCalibratorRoute) {
        return ("Redo change " + toString(myCalibratorRoute->getTag()) + " values").c_str();
    } else if (myCalibratorVehicleType) {
        return ("Redo change " + toString(myCalibratorVehicleType->getTag()) + " values").c_str();
    } else {
        throw ProcessError("There isn't a defined Calibrator item");
    }
}

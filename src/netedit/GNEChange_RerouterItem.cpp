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
/// @file    GNEChange_RerouterItem.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2017
/// @version $Id$
///
// A change in the values of Rerouters in netedit
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

#include "GNEChange_RerouterItem.h"
#include "GNENet.h"
#include "GNEViewNet.h"
#include "GNERerouter.h"
#include "GNERerouterInterval.h"
#include "GNEClosingReroute.h"
#include "GNEClosingLaneReroute.h"
#include "GNEDestProbReroute.h"
#include "GNERouteProbReroute.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_RerouterItem, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_RerouterItem::GNEChange_RerouterItem(GNERerouterInterval *rerouterInterval, bool forward) :
    GNEChange(rerouterInterval->getRerouterParent()->getViewNet()->getNet(), forward),
    myRerouterInterval(rerouterInterval),
    myClosingReroute(NULL),
    myClosingLaneReroute(NULL),
    myDestProbReroute(NULL),
    myRouteProbReroute(NULL) {
    myRerouterInterval->incRef("GNEChange_RerouterItem");
}


GNEChange_RerouterItem::GNEChange_RerouterItem(GNEClosingReroute *closingReroute, bool forward) :
    GNEChange(closingReroute->getRerouterIntervalParent()->getRerouterParent()->getViewNet()->getNet(), forward),
    myRerouterInterval(NULL),
    myClosingReroute(closingReroute),
    myClosingLaneReroute(NULL),
    myDestProbReroute(NULL),
    myRouteProbReroute(NULL) {
    myClosingReroute->incRef("GNEChange_RerouterItem");
}


GNEChange_RerouterItem::GNEChange_RerouterItem(GNEClosingLaneReroute *closingLaneReroute, bool forward) :
    GNEChange(closingLaneReroute->getRerouterIntervalParent()->getRerouterParent()->getViewNet()->getNet(), forward),
    myRerouterInterval(NULL),
    myClosingReroute(NULL),
    myClosingLaneReroute(closingLaneReroute),
    myDestProbReroute(NULL),
    myRouteProbReroute(NULL) {
    myClosingLaneReroute->incRef("GNEChange_RerouterItem");
}


GNEChange_RerouterItem::GNEChange_RerouterItem(GNEDestProbReroute *destProbReroute, bool forward) :
    GNEChange(destProbReroute->getRerouterIntervalParent()->getRerouterParent()->getViewNet()->getNet(), forward),
    myRerouterInterval(NULL),
    myClosingReroute(NULL),
    myClosingLaneReroute(NULL),
    myDestProbReroute(destProbReroute),
    myRouteProbReroute(NULL) {
    myDestProbReroute->incRef("GNEChange_RerouterItem");
}


GNEChange_RerouterItem::GNEChange_RerouterItem(GNERouteProbReroute *routeProbReroute, bool forward) :
    GNEChange(routeProbReroute->getRerouterIntervalParent()->getRerouterParent()->getViewNet()->getNet(), forward),
    myRerouterInterval(NULL),
    myClosingReroute(NULL),
    myClosingLaneReroute(NULL),
    myDestProbReroute(NULL),
    myRouteProbReroute(routeProbReroute) {
    myRouteProbReroute->incRef("GNEChange_RerouterItem");
}


GNEChange_RerouterItem::~GNEChange_RerouterItem() {
    if(myRerouterInterval) {
        myRerouterInterval->decRef("GNEChange_RerouterItem");
        if (myRerouterInterval->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting Interval of rerouter '" + myRerouterInterval->getRerouterParent()->getID() + "'");
            }
            delete myRerouterInterval;
        }
    } else if(myClosingReroute) {
        myClosingReroute->decRef("GNEChange_RerouterItem");
        if (myClosingReroute->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting Closing Reroute of rerouter '" + myClosingReroute->getRerouterIntervalParent()->getRerouterParent()->getID() + "'");
            }
            delete myClosingReroute;
        }
    } else if(myClosingLaneReroute) {
        myClosingLaneReroute->decRef("GNEChange_RerouterItem");
        if (myClosingLaneReroute->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting Closing lane Reroute type of rerouter '" + myClosingLaneReroute->getRerouterIntervalParent()->getRerouterParent()->getID() + "'");
            }
            delete myClosingLaneReroute;
        }
    } else if(myDestProbReroute) {
        myDestProbReroute->decRef("GNEChange_RerouterItem");
        if (myDestProbReroute->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting Destiny Probability Reroute of rerouter '" + myDestProbReroute->getRerouterIntervalParent()->getRerouterParent()->getID() + "'");
            }
            delete myDestProbReroute;
        }
    } else if(myRouteProbReroute) {
        myRouteProbReroute->decRef("GNEChange_RerouterItem");
        if (myRouteProbReroute->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting Route Probability Reroute of rerouter '" + myRouteProbReroute->getRerouterIntervalParent()->getRerouterParent()->getID() + "'");
            }
            delete myRouteProbReroute;
        }
    }
}


void
GNEChange_RerouterItem::undo() {
    /*
    if (myForward) {
        if(myRerouterFlow) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing rerouter flow of rerouter '" + myRerouterFlow->getRerouterParent()->getID() + "'");
            }
            // remove rerouter flow of rerouter and net
            myRerouterFlow->getRerouterParent()->removeRerouterFlow(myRerouterFlow);
            myNet->deleteRerouterFlow(myRerouterFlow);
        } else if (myRerouterRoute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing rerouter route of rerouter '" + myRerouterRoute->getRerouterParent()->getID() + "'");
            }
            // remove rerouter route of rerouter and net
            myRerouterRoute->getRerouterParent()->removeRerouterRoute(myRerouterRoute);
            myNet->deleteRerouterRoute(myRerouterRoute);
        } else if (myRerouterVehicleType) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing rerouter vehicle type of rerouter '" + myRerouterVehicleType->getRerouterParent()->getID() + "'");
            }
            // remove rerouter vehicle type of rerouter and net
            myRerouterVehicleType->getRerouterParent()->removeRerouterVehicleType(myRerouterVehicleType);
            myNet->deleteRerouterVehicleType(myRerouterVehicleType);
        } else {
            throw ProcessError("There isn't a defined Rerouter item");
        }
    } else {
        if(myRerouterFlow) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding rerouter flow into rerouter '" + myRerouterFlow->getRerouterParent()->getID() + "'");
            }
            // add rerouter flow into rerouter and net
            myRerouterFlow->getRerouterParent()->addRerouterFlow(myRerouterFlow);
            myNet->insertRerouterFlow(myRerouterFlow);
        } else if (myRerouterRoute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding rerouter route into rerouter '" + myRerouterRoute->getRerouterParent()->getID() + "'");
            }
            // add rerouter route into rerouter and net
            myRerouterRoute->getRerouterParent()->addRerouterRoute(myRerouterRoute);
            myNet->insertRerouterRoute(myRerouterRoute);
        } else if (myRerouterVehicleType) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding rerouter vehicle type into rerouter '" + myRerouterVehicleType->getRerouterParent()->getID() + "'");
            }
            // add rerouter vehicle type into rerouter and net
            myRerouterVehicleType->getRerouterParent()->addRerouterVehicleType(myRerouterVehicleType);
            myNet->insertRerouterVehicleType(myRerouterVehicleType);
        } else {
            throw ProcessError("There isn't a defined Rerouter item");
        }
    }
    */
    // enable save additionals
    myNet->requiereSaveAdditionals();
}


void
GNEChange_RerouterItem::redo() {
    /*
    if (myForward) {
        if(myRerouterFlow) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding rerouter flow into rerouter '" + myRerouterFlow->getRerouterParent()->getID() + "'");
            }
            // add rerouter flow into rerouter and net
            myRerouterFlow->getRerouterParent()->addRerouterFlow(myRerouterFlow);
            myNet->insertRerouterFlow(myRerouterFlow);
        } else if (myRerouterRoute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding rerouter route into rerouter '" + myRerouterRoute->getRerouterParent()->getID() + "'");
            }
            // add rerouter route into rerouter and net
            myRerouterRoute->getRerouterParent()->addRerouterRoute(myRerouterRoute);
            myNet->insertRerouterRoute(myRerouterRoute);
        } else if (myRerouterVehicleType) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding rerouter vehicle type into rerouter '" + myRerouterVehicleType->getRerouterParent()->getID() + "'");
            }
            // add rerouter vehicle type into rerouter and net
            myRerouterVehicleType->getRerouterParent()->addRerouterVehicleType(myRerouterVehicleType);
            myNet->insertRerouterVehicleType(myRerouterVehicleType);
        } else {
            throw ProcessError("There isn't a defined Rerouter item");
        }
    } else {
        if(myRerouterFlow) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing rerouter flow of rerouter '" + myRerouterFlow->getRerouterParent()->getID() + "'");
            }
            // remove rerouter flow of rerouter and net
            myRerouterFlow->getRerouterParent()->removeRerouterFlow(myRerouterFlow);
            myNet->deleteRerouterFlow(myRerouterFlow);
        } else if (myRerouterRoute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing rerouter route of rerouter '" + myRerouterRoute->getRerouterParent()->getID() + "'");
            }
            // remove rerouter route of rerouter and net
            myRerouterRoute->getRerouterParent()->removeRerouterRoute(myRerouterRoute);
            myNet->deleteRerouterRoute(myRerouterRoute);
        } else if (myRerouterVehicleType) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing rerouter vehicle type of rerouter '" + myRerouterVehicleType->getRerouterParent()->getID() + "'");
            }
            // remove rerouter vehicle type of rerouter and net
            myRerouterVehicleType->getRerouterParent()->removeRerouterVehicleType(myRerouterVehicleType);
            myNet->deleteRerouterVehicleType(myRerouterVehicleType);
        } else {
            throw ProcessError("There isn't a defined Rerouter item");
        }
    }
    */
    // enable save additionals
    myNet->requiereSaveAdditionals();
}


FXString
GNEChange_RerouterItem::undoName() const {
    if(myRerouterInterval) {
        return ("Undo change " + toString(myRerouterInterval->getTag()) + " values").c_str();
    } else if (myClosingReroute) {
        return ("Undo change " + toString(myClosingReroute->getTag()) + " values").c_str();
    } else if (myClosingLaneReroute) {
        return ("Undo change " + toString(myClosingLaneReroute->getTag()) + " values").c_str();
    } else if (myDestProbReroute) {
        return ("Undo change " + toString(myDestProbReroute->getTag()) + " values").c_str();
    } else if (myRouteProbReroute) {
        return ("Undo change " + toString(myRouteProbReroute->getTag()) + " values").c_str();
    } else {
        throw ProcessError("There isn't a defined Rerouter item");
    }
}


FXString
GNEChange_RerouterItem::redoName() const {
    if(myRerouterInterval) {
        return ("Redo change " + toString(myRerouterInterval->getTag()) + " values").c_str();
    } else if (myClosingReroute) {
        return ("Redo change " + toString(myClosingReroute->getTag()) + " values").c_str();
    } else if (myClosingLaneReroute) {
        return ("Redo change " + toString(myClosingLaneReroute->getTag()) + " values").c_str();
    } else if (myDestProbReroute) {
        return ("Redo change " + toString(myDestProbReroute->getTag()) + " values").c_str();
    } else if (myRouteProbReroute) {
        return ("Redo change " + toString(myRouteProbReroute->getTag()) + " values").c_str();
    } else {
        throw ProcessError("There isn't a defined Rerouter item");
    }
}

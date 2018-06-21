/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/additionals/GNERerouter.h>
#include <netedit/additionals/GNERerouterInterval.h>
#include <netedit/additionals/GNEClosingReroute.h>
#include <netedit/additionals/GNEClosingLaneReroute.h>
#include <netedit/additionals/GNEDestProbReroute.h>
#include <netedit/additionals/GNEParkingAreaReroute.h>
#include <netedit/additionals/GNERouteProbReroute.h>

#include "GNEChange_RerouterItem.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_RerouterItem, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_RerouterItem::GNEChange_RerouterItem(GNERerouterInterval* rerouterInterval, bool forward) :
    GNEChange(rerouterInterval->getRerouterParent()->getViewNet()->getNet(), forward),
    myRerouterInterval(rerouterInterval),
    myRouteProbReroute(nullptr) {
    myRerouterInterval->incRef("GNEChange_RerouterItem");
}


GNEChange_RerouterItem::GNEChange_RerouterItem(GNERouteProbReroute* routeProbReroute, bool forward) :
    GNEChange(routeProbReroute->getRerouterIntervalParent()->getRerouterParent()->getViewNet()->getNet(), forward),
    myRerouterInterval(nullptr),
    myRouteProbReroute(routeProbReroute) {
    myRouteProbReroute->incRef("GNEChange_RerouterItem");
}


GNEChange_RerouterItem::~GNEChange_RerouterItem() {
    if (myRerouterInterval) {
        myRerouterInterval->decRef("GNEChange_RerouterItem");
        if (myRerouterInterval->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting rerouter Interval");
            }
            delete myRerouterInterval;
        }
    } else if (myRouteProbReroute) {
        myRouteProbReroute->decRef("GNEChange_RerouterItem");
        if (myRouteProbReroute->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting Route Probability Reroute");
            }
            delete myRouteProbReroute;
        }
    }
}


void
GNEChange_RerouterItem::undo() {
    if (myForward) {
        if (myRerouterInterval) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing Rerouter Interval from Reroute '" + myRerouterInterval->getRerouterParent()->getID() + "'");
            }
            // remove rerouter interval from Rerouter
            myRerouterInterval->getRerouterParent()->removeRerouterInterval(myRerouterInterval);
        } else if (myRouteProbReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing Route Probability Reroute from Interval '" + myRouteProbReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // remove Route Probability Reroute from Interval
            myRouteProbReroute->getRerouterIntervalParent()->removeRouteProbReroute(myRouteProbReroute);
        } else {
            throw ProcessError("There isn't a defined Rerouter item");
        }
    } else {
        if (myRerouterInterval) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding Rerouter Interval to Reroute '" + myRerouterInterval->getRerouterParent()->getID() + "'");
            }
            // add rerouter interval to Rerouter
            myRerouterInterval->getRerouterParent()->addRerouterInterval(myRerouterInterval);
        } else if (myRouteProbReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding Route Probability Reroute to Interval '" + myRouteProbReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // add Route Probability Reroute to Interval
            myRouteProbReroute->getRerouterIntervalParent()->addRouteProbReroute(myRouteProbReroute);
        } else {
            throw ProcessError("There isn't a defined Rerouter item");
        }
    }
    // enable save additionals
    myNet->requiereSaveAdditionals(true);
}


void
GNEChange_RerouterItem::redo() {
    if (myForward) {
        if (myRerouterInterval) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding Rerouter Interval to Reroute '" + myRerouterInterval->getRerouterParent()->getID() + "'");
            }
            // add rerouter interval to Rerouter
            myRerouterInterval->getRerouterParent()->addRerouterInterval(myRerouterInterval);
        } else if (myRouteProbReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding Route Probability Reroute to Interval '" + myRouteProbReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // add Route Probability Reroute to Interval
            myRouteProbReroute->getRerouterIntervalParent()->addRouteProbReroute(myRouteProbReroute);
        } else {
            throw ProcessError("There isn't a defined Rerouter item");
        }
    } else {
        if (myRerouterInterval) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing Rerouter Interval from Reroute '" + myRerouterInterval->getRerouterParent()->getID() + "'");
            }
            // remove rerouter interval from Rerouter
            myRerouterInterval->getRerouterParent()->removeRerouterInterval(myRerouterInterval);
        } else if (myRouteProbReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing Route Probability Reroute from Interval '" + myRouteProbReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // remove Route Probability Reroute from Interval
            myRouteProbReroute->getRerouterIntervalParent()->removeRouteProbReroute(myRouteProbReroute);
        } else {
            throw ProcessError("There isn't a defined Rerouter item");
        }
    }
    // enable save additionals
    myNet->requiereSaveAdditionals(true);
}


FXString
GNEChange_RerouterItem::undoName() const {
    if (myRerouterInterval) {
        return ("Undo change " + toString(myRerouterInterval->getTag()) + " values").c_str();
    } else if (myRouteProbReroute) {
        return ("Undo change " + toString(myRouteProbReroute->getTag()) + " values").c_str();
    } else {
        throw ProcessError("There isn't a defined Rerouter item");
    }
}


FXString
GNEChange_RerouterItem::redoName() const {
    if (myRerouterInterval) {
        return ("Redo change " + toString(myRerouterInterval->getTag()) + " values").c_str();
    } else if (myRouteProbReroute) {
        return ("Redo change " + toString(myRouteProbReroute->getTag()) + " values").c_str();
    } else {
        throw ProcessError("There isn't a defined Rerouter item");
    }
}

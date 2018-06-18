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
    myClosingReroute(nullptr),
    myClosingLaneReroute(nullptr),
    myDestProbReroute(nullptr),
    myParkingAreaReroute(nullptr),
    myRouteProbReroute(nullptr) {
    myRerouterInterval->incRef("GNEChange_RerouterItem");
}


GNEChange_RerouterItem::GNEChange_RerouterItem(GNEClosingReroute* closingReroute, bool forward) :
    GNEChange(closingReroute->getRerouterIntervalParent()->getRerouterParent()->getViewNet()->getNet(), forward),
    myRerouterInterval(nullptr),
    myClosingReroute(closingReroute),
    myClosingLaneReroute(nullptr),
    myDestProbReroute(nullptr),
    myParkingAreaReroute(nullptr),
    myRouteProbReroute(nullptr) {
    myClosingReroute->incRef("GNEChange_RerouterItem");
}


GNEChange_RerouterItem::GNEChange_RerouterItem(GNEClosingLaneReroute* closingLaneReroute, bool forward) :
    GNEChange(closingLaneReroute->getRerouterIntervalParent()->getRerouterParent()->getViewNet()->getNet(), forward),
    myRerouterInterval(nullptr),
    myClosingReroute(nullptr),
    myClosingLaneReroute(closingLaneReroute),
    myDestProbReroute(nullptr),
    myParkingAreaReroute(nullptr),
    myRouteProbReroute(nullptr) {
    myClosingLaneReroute->incRef("GNEChange_RerouterItem");
}


GNEChange_RerouterItem::GNEChange_RerouterItem(GNEDestProbReroute* destProbReroute, bool forward) :
    GNEChange(destProbReroute->getRerouterIntervalParent()->getRerouterParent()->getViewNet()->getNet(), forward),
    myRerouterInterval(nullptr),
    myClosingReroute(nullptr),
    myClosingLaneReroute(nullptr),
    myDestProbReroute(destProbReroute),
    myParkingAreaReroute(nullptr),
    myRouteProbReroute(nullptr) {
    myDestProbReroute->incRef("GNEChange_RerouterItem");
}


GNEChange_RerouterItem::GNEChange_RerouterItem(GNERouteProbReroute* routeProbReroute, bool forward) :
    GNEChange(routeProbReroute->getRerouterIntervalParent()->getRerouterParent()->getViewNet()->getNet(), forward),
    myRerouterInterval(nullptr),
    myClosingReroute(nullptr),
    myClosingLaneReroute(nullptr),
    myDestProbReroute(nullptr),
    myParkingAreaReroute(nullptr),
    myRouteProbReroute(routeProbReroute) {
    myRouteProbReroute->incRef("GNEChange_RerouterItem");
}

GNEChange_RerouterItem::GNEChange_RerouterItem(GNEParkingAreaReroute* parkingAreaReroute, bool forward) :
    GNEChange(parkingAreaReroute->getRerouterIntervalParent()->getRerouterParent()->getViewNet()->getNet(), forward),
    myRerouterInterval(nullptr),
    myClosingReroute(nullptr),
    myClosingLaneReroute(nullptr),
    myDestProbReroute(nullptr),
    myParkingAreaReroute(parkingAreaReroute),
    myRouteProbReroute(nullptr) {
    myParkingAreaReroute->incRef("GNEChange_RerouterItem");
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
    } else if (myClosingReroute) {
        myClosingReroute->decRef("GNEChange_RerouterItem");
        if (myClosingReroute->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting Closing Reroute");
            }
            delete myClosingReroute;
        }
    } else if (myClosingLaneReroute) {
        myClosingLaneReroute->decRef("GNEChange_RerouterItem");
        if (myClosingLaneReroute->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting Closing lane Reroute");
            }
            delete myClosingLaneReroute;
        }
    } else if (myDestProbReroute) {
        myDestProbReroute->decRef("GNEChange_RerouterItem");
        if (myDestProbReroute->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting Destiny Probability Reroute");
            }
            delete myDestProbReroute;
        }
    } else if (myParkingAreaReroute) {
        myParkingAreaReroute->decRef("GNEChange_RerouterItem");
        if (myParkingAreaReroute->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting parkingAreaReroute");
            }
            delete myParkingAreaReroute;
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
        } else if (myClosingReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing Closing Reroute from Interval '" + myClosingReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // remove Closing Reroute from Interval
            myClosingReroute->getRerouterIntervalParent()->removeClosingReroute(myClosingReroute);
        } else if (myClosingLaneReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing Closing Lane Reroute from Interval '" + myClosingLaneReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // remove Closing Lane Reroute from Interval
            myClosingLaneReroute->getRerouterIntervalParent()->removeClosingLaneReroute(myClosingLaneReroute);
        } else if (myDestProbReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing Dest Probability Reroute from Interval '" + myDestProbReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // remove Destiny Probability Reroute from Interval
            myDestProbReroute->getRerouterIntervalParent()->removeDestProbReroute(myDestProbReroute);
        } else if (myParkingAreaReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing parkingAreaReroute from Interval '" + myParkingAreaReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // remove Destiny Probability Reroute from Interval
            myParkingAreaReroute->getRerouterIntervalParent()->removeParkingAreaReroute(myParkingAreaReroute);
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
        } else if (myClosingReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding Closing Reroute to Interval '" + myClosingReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // add Closing Reroute to Interval
            myClosingReroute->getRerouterIntervalParent()->addClosingReroute(myClosingReroute);
        } else if (myClosingLaneReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding Closing Lane Reroute to Interval '" + myClosingLaneReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // add Closing Lane Reroute to Interval
            myClosingLaneReroute->getRerouterIntervalParent()->addClosingLaneReroute(myClosingLaneReroute);
        } else if (myDestProbReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding Dest Probability Reroute to Interval '" + myDestProbReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // add Destiny Probability Reroute to Interval
            myDestProbReroute->getRerouterIntervalParent()->addDestProbReroute(myDestProbReroute);
        } else if (myParkingAreaReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding parkingAreaReroute to Interval '" + myParkingAreaReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // add Destiny Probability Reroute to Interval
            myParkingAreaReroute->getRerouterIntervalParent()->addParkingAreaReroute(myParkingAreaReroute);
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
        } else if (myClosingReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding Closing Reroute to Interval '" + myClosingReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // add Closing Reroute to Interval
            myClosingReroute->getRerouterIntervalParent()->addClosingReroute(myClosingReroute);
        } else if (myClosingLaneReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding Closing Lane Reroute to Interval '" + myClosingLaneReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // add Closing Lane Reroute to Interval
            myClosingLaneReroute->getRerouterIntervalParent()->addClosingLaneReroute(myClosingLaneReroute);
        } else if (myDestProbReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding Dest Probability Reroute to Interval '" + myDestProbReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // add Destiny Probability Reroute to Interval
            myDestProbReroute->getRerouterIntervalParent()->addDestProbReroute(myDestProbReroute);
        } else if (myParkingAreaReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Adding parkingAreaReroute to Interval '" + myParkingAreaReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // add Destiny Probability Reroute to Interval
            myParkingAreaReroute->getRerouterIntervalParent()->addParkingAreaReroute(myParkingAreaReroute);
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
        } else if (myClosingReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing Closing Reroute from Interval '" + myClosingReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // remove Closing Reroute from Interval
            myClosingReroute->getRerouterIntervalParent()->removeClosingReroute(myClosingReroute);
        } else if (myClosingLaneReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing Closing Lane Reroute from Interval '" + myClosingLaneReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // remove Closing Lane Reroute from Interval
            myClosingLaneReroute->getRerouterIntervalParent()->removeClosingLaneReroute(myClosingLaneReroute);
        } else if (myDestProbReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing Dest Probability Reroute from Interval '" + myDestProbReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // remove Destiny Probability Reroute from Interval
            myDestProbReroute->getRerouterIntervalParent()->removeDestProbReroute(myDestProbReroute);
        } else if (myParkingAreaReroute) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removing parkingAreaReroute from Interval '" + myParkingAreaReroute->getRerouterIntervalParent()->getID() + "'");
            }
            // remove Destiny Probability Reroute from Interval
            myParkingAreaReroute->getRerouterIntervalParent()->removeParkingAreaReroute(myParkingAreaReroute);
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
    } else if (myClosingReroute) {
        return ("Undo change " + toString(myClosingReroute->getTag()) + " values").c_str();
    } else if (myClosingLaneReroute) {
        return ("Undo change " + toString(myClosingLaneReroute->getTag()) + " values").c_str();
    } else if (myDestProbReroute) {
        return ("Undo change " + toString(myDestProbReroute->getTag()) + " values").c_str();
    } else if (myParkingAreaReroute) {
        return ("Undo change " + toString(myParkingAreaReroute->getTag()) + " values").c_str();
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
    } else if (myClosingReroute) {
        return ("Redo change " + toString(myClosingReroute->getTag()) + " values").c_str();
    } else if (myClosingLaneReroute) {
        return ("Redo change " + toString(myClosingLaneReroute->getTag()) + " values").c_str();
    } else if (myDestProbReroute) {
        return ("Redo change " + toString(myDestProbReroute->getTag()) + " values").c_str();
    } else if (myParkingAreaReroute) {
        return ("Redo change " + toString(myParkingAreaReroute->getTag()) + " values").c_str();
    } else if (myRouteProbReroute) {
        return ("Redo change " + toString(myRouteProbReroute->getTag()) + " values").c_str();
    } else {
        throw ProcessError("There isn't a defined Rerouter item");
    }
}

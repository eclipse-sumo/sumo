/****************************************************************************/
/// @file    GNEChange_TLS.cpp
/// @author  Jakob Erdmann
/// @date    July 2011
/// @version $Id$
///
// A network change in which a traffic light is created or deleted
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

#include <utils/common/MsgHandler.h>

#include <cassert>
#include <utils/options/OptionsCont.h>
#include <netbuild/NBTrafficLightDefinition.h>
#include <netbuild/NBOwnTLDef.h>
#include "GNEChange_TLS.h"
#include "GNEJunction.h"
#include "GNENet.h"
#include "GNEViewNet.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_TLS, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


/// @brief constructor for creating an edge
GNEChange_TLS::GNEChange_TLS(GNEJunction* junction, NBTrafficLightDefinition* tlDef, bool forward, bool forceInsert, const std::string tlID):
    GNEChange(junction->getNet(), forward),
    myJunction(junction),
    myTlDef(tlDef),
    myForceInsert(forceInsert) {
    assert(myNet);
    myJunction->incRef("GNEChange_TLS");
    if (myTlDef == 0) {
        assert(forward);
        // potential memory leak if this change is never executed
        TrafficLightType type = SUMOXMLDefinitions::TrafficLightTypes.get(OptionsCont::getOptions().getString("tls.default-type"));
        myTlDef = new NBOwnTLDef(tlID == "" ? myJunction->getMicrosimID() : tlID, 0, type);
    }
}


GNEChange_TLS::~GNEChange_TLS() {
    assert(myJunction);
    myJunction->decRef("GNEChange_TLS");
    if (myJunction->unreferenced()) {
        // show extra information for tests
        if (myNet->getViewNet()->isTestingModeEnabled()) {
            WRITE_WARNING("Deleting unreferenced " + toString(myJunction->getTag()) + " '" + myJunction->getID() + "'");
        }
        delete myJunction;
    }
}


void
GNEChange_TLS::undo() {
    if (myForward) {
        // show extra information for tests
        if (myJunction->getNet()->getViewNet()->isTestingModeEnabled()) {
            WRITE_WARNING("Deleting " + toString(myJunction->getTag()) + " '" + myJunction->getID() + "'");
        }
        // remove traffic light from junction
        myJunction->removeTrafficLight(myTlDef);
    } else {
        // show extra information for tests
        if (myJunction->getNet()->getViewNet()->isTestingModeEnabled()) {
            WRITE_WARNING("Adding " + toString(myJunction->getTag()) + " '" + myJunction->getID() + "'");
        }
        // add traffic light to junction
        myJunction->addTrafficLight(myTlDef, myForceInsert);
    }
}


void
GNEChange_TLS::redo() {
    if (myForward) {
        // show extra information for tests
        if (myJunction->getNet()->getViewNet()->isTestingModeEnabled()) {
            WRITE_WARNING("Adding " + toString(myJunction->getTag()) + " '" + myJunction->getID() + "'");
        }
        // add traffic light to junction
        myJunction->addTrafficLight(myTlDef, myForceInsert);
    } else {
        // show extra information for tests
        if (myJunction->getNet()->getViewNet()->isTestingModeEnabled()) {
            WRITE_WARNING("Deleting " + toString(myJunction->getTag()) + " '" + myJunction->getID() + "'");
        }
        // remove traffic light from junction
        myJunction->removeTrafficLight(myTlDef);
    }
}


FXString
GNEChange_TLS::undoName() const {
    if (myForward) {
        return ("Undo create " + toString(SUMO_TAG_TRAFFIC_LIGHT)).c_str();
    } else {
        return ("Undo delete " + toString(SUMO_TAG_TRAFFIC_LIGHT)).c_str();
    }
}


FXString
GNEChange_TLS::redoName() const {
    if (myForward) {
        return ("Redo create " + toString(SUMO_TAG_TRAFFIC_LIGHT)).c_str();
    } else {
        return ("Redo delete " + toString(SUMO_TAG_TRAFFIC_LIGHT)).c_str();
    }
}

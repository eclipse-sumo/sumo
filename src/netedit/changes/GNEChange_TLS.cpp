/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_TLS.cpp
/// @author  Jakob Erdmann
/// @date    July 2011
/// @version $Id$
///
// A network change in which a traffic light is created or deleted
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


#include <utils/options/OptionsCont.h>
#include <netbuild/NBOwnTLDef.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/GNENet.h>

#include "GNEChange_TLS.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_TLS, GNEChange, nullptr, 0)

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
    if (myTlDef == nullptr) {
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
        WRITE_DEBUG("Deleting unreferenced " + myJunction->getTagStr() + " '" + myJunction->getID() + "' in GNEChange_TLS");
        delete myJunction;
    }
}


void
GNEChange_TLS::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Removing TLS from " + myJunction->getTagStr() + " '" + myJunction->getID() + "'");
        // remove traffic light from junction
        myJunction->removeTrafficLight(myTlDef);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding TLS into " + myJunction->getTagStr() + " '" + myJunction->getID() + "'");
        // add traffic light to junction
        myJunction->addTrafficLight(myTlDef, myForceInsert);
    }
    // enable save netElements
    myNet->requiereSaveNet(true);
}


void
GNEChange_TLS::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding TLS into " + myJunction->getTagStr() + " '" + myJunction->getID() + "'");
        // add traffic light to junction
        myJunction->addTrafficLight(myTlDef, myForceInsert);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Deleting TLS from " + myJunction->getTagStr() + " '" + myJunction->getID() + "'");
        // remove traffic light from junction
        myJunction->removeTrafficLight(myTlDef);
    }
    // enable save netElements
    myNet->requiereSaveNet(true);
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

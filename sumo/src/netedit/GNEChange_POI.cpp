/****************************************************************************/
/// @file    GNEChange_POI.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2017
/// @version $Id$
///
// A network change in which a single POI is created or deleted
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
#include <utils/common/RGBColor.h>
#include <utils/geom/PositionVector.h>

#include "GNEChange_POI.h"
#include "GNEPOI.h"
#include "GNENet.h"
#include "GNEViewNet.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_POI, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


/// @brief constructor for creating a POI
GNEChange_POI::GNEChange_POI(GNENet *net, GNEPOI *POI, bool forward) :
    GNEChange(net, forward),
    myPOI(POI) {
    assert(myNet);
}


GNEChange_POI::~GNEChange_POI() {
    // show extra information for tests
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Removing " + toString(SUMO_TAG_POI) + " '" + myPOI->getID() + "' from net");
    }
    // remove POIgon of net
    myNet->removePOI(myPOI->getID());
}


void
GNEChange_POI::undo() {
    if (myForward) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing " + toString(SUMO_TAG_POI) + " '" + myPOI->getID() + "' from viewNet");
        }
        // remove POIgon of net
        myNet->removePOIOfView(myPOI);
    }
    else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Adding " + toString(SUMO_TAG_POI) + " '" + myPOI->getID() + "' into viewNet");
        }
        // Add POIgon to view
        myNet->insertPOIInView(myPOI);
    }
}


void
GNEChange_POI::redo() {
    if (myForward) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Adding " + toString(SUMO_TAG_POI) + " '" + myPOI->getID() + "' into viewNet");
        }
        // Add POIgon to view
        myNet->insertPOIInView(myPOI);
    }
    else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing " + toString(SUMO_TAG_POI) + " '" + myPOI->getID() + "' from viewNet");
        }
        // delete POI from view
        myNet->removePOIOfView(myPOI);
    }
}


FXString
GNEChange_POI::undoName() const {
    if (myForward) {
        return ("Undo create " + toString(SUMO_TAG_POI)).c_str();
    }
    else {
        return ("Undo delete " + toString(SUMO_TAG_POI)).c_str();
    }
}


FXString
GNEChange_POI::redoName() const {
    if (myForward) {
        return ("Redo create " + toString(SUMO_TAG_POI)).c_str();
    }
    else {
        return ("Redo delete " + toString(SUMO_TAG_POI)).c_str();
    }
}

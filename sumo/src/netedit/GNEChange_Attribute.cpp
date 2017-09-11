/****************************************************************************/
/// @file    GNEChange_Attribute.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A network change in which something is moved (for undo/redo)
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

#include <cassert>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>

#include "GNEChange_Attribute.h"
#include "GNEAttributeCarrier.h"
#include "GNENet.h"
#include "GNEViewNet.h"
#include "GNEAdditional.h"
#include "GNEShape.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Attribute, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_Attribute::GNEChange_Attribute(GNEAttributeCarrier* ac,
        SumoXMLAttr key, const std::string& value,
        bool customOrigValue, const std::string& origValue) :
    GNEChange(0, true),
    myAC(ac),
    myKey(key),
    myOrigValue(customOrigValue ? origValue : ac->getAttribute(key)),
    myNewValue(value),
    myAdditional(NULL),
    myShape(NULL) {
    myAC->incRef("GNEChange_Attribute " + toString(myKey));
    // try to cast AC as additional and Shape
    myAdditional = dynamic_cast<GNEAdditional*>(myAC);
    myShape = dynamic_cast<GNEShape*>(myAC);
}


GNEChange_Attribute::~GNEChange_Attribute() {
    assert(myAC);
    myAC->decRef("GNEChange_Attribute " + toString(myKey));
    if (myAC->unreferenced()) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Deleting unreferenced " + toString(myAC->getTag()) + " '" + myAC->getID() + "' in GNEChange_Attribute");
        }
        delete myAC;
    }
}


void
GNEChange_Attribute::undo() {
    // show extra information for tests
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Setting previous attribute " + toString(myKey) + " '" + myOrigValue + "' into " + toString(myAC->getTag()) + " '" + myAC->getID() + "'");
    }
    // set original value
    myAC->setAttribute(myKey, myOrigValue);
    // check if additional or shapes has to be saved
    if(myAdditional) {
        myAdditional->getViewNet()->getNet()->requiereSaveAdditionals();
    } else if(myShape) {
        myShape->getNet()->requiereSaveShapes();
    }
}


void
GNEChange_Attribute::redo() {
    // show extra information for tests
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Setting new attribute " + toString(myKey) + " '" + myNewValue + "' into " + toString(myAC->getTag()) + " '" + myAC->getID() + "'");
    }
    // set new value
    myAC->setAttribute(myKey, myNewValue);
    // check if additional or shapes has to be saved
    if(myAdditional) {
        myAdditional->getViewNet()->getNet()->requiereSaveAdditionals();
    } else if(myShape) {
        myShape->getNet()->requiereSaveShapes();
    }
}


bool
GNEChange_Attribute::trueChange() {
    return myOrigValue != myNewValue;
}


FXString
GNEChange_Attribute::undoName() const {
    return ("Undo change " + toString(myAC->getTag()) + " attribute").c_str();
}


FXString
GNEChange_Attribute::redoName() const {
    return ("Redo change " + toString(myAC->getTag()) + " attribute").c_str();
}


/****************************************************************************/

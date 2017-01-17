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
#include "GNEChange_Attribute.h"
#include "GNEAttributeCarrier.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


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
    myNewValue(value) {
    myAC->incRef("GNEChange_Attribute " + toString(myKey));
}


GNEChange_Attribute::~GNEChange_Attribute() {
    assert(myAC);
    myAC->decRef("GNEChange_Attribute " + toString(myKey));
    if (myAC->unreferenced()) {
        delete myAC;
    }
}


void
GNEChange_Attribute::undo() {
    myAC->setAttribute(myKey, myOrigValue);
}


void
GNEChange_Attribute::redo() {
    myAC->setAttribute(myKey, myNewValue);
}


bool
GNEChange_Attribute::trueChange() {
    return myOrigValue != myNewValue;
}


FXString
GNEChange_Attribute::undoName() const {
    return ("Undo change " + myAC->getDescription() + " attribute").c_str();
}


FXString
GNEChange_Attribute::redoName() const {
    return ("Redo change " + myAC->getDescription() + " attribute").c_str();
}


/****************************************************************************/

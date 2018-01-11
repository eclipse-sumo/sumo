/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_VariableSpeedSignItem.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2017
/// @version $Id$
///
// A change in the values of Variable Speed Signs in netedit
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

#include "GNEChange_VariableSpeedSignItem.h"
#include "GNEVariableSpeedSign.h"
#include "GNEVariableSpeedSignStep.h"
#include "GNENet.h"
#include "GNEViewNet.h"



// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_VariableSpeedSignItem, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_VariableSpeedSignItem::GNEChange_VariableSpeedSignItem(GNEVariableSpeedSignStep* variableSpeedSignStep, bool forward) :
    GNEChange(variableSpeedSignStep->getVariableSpeedSignParent()->getViewNet()->getNet(), forward),
    myVariableSpeedSignStep(variableSpeedSignStep) {
    myVariableSpeedSignStep->incRef("GNEChange_VariableSpeedSignItem");
}


GNEChange_VariableSpeedSignItem::~GNEChange_VariableSpeedSignItem() {
    myVariableSpeedSignStep->decRef("GNEChange_VariableSpeedSignItem");
    if (myVariableSpeedSignStep->unreferenced()) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Deleting Step of Variable Speed Sign '" + myVariableSpeedSignStep->getVariableSpeedSignParent()->getID() + "'");
        }
        delete myVariableSpeedSignStep;
    }
}


void
GNEChange_VariableSpeedSignItem::undo() {
    if (myForward) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing Step from Variable Speed Sign '" + myVariableSpeedSignStep->getVariableSpeedSignParent()->getID() + "'");
        }
        // remove step from Variable Speed Sign
        myVariableSpeedSignStep->getVariableSpeedSignParent()->removeVariableSpeedSignStep(myVariableSpeedSignStep);
    } else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Adding Step to Variable Speed Sign '" + myVariableSpeedSignStep->getVariableSpeedSignParent()->getID() + "'");
        }
        // add step to Variable Speed Sign
        myVariableSpeedSignStep->getVariableSpeedSignParent()->addVariableSpeedSignStep(myVariableSpeedSignStep);
    }
    // enable save additionals
    myNet->requiereSaveAdditionals();
}


void
GNEChange_VariableSpeedSignItem::redo() {
    if (myForward) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Adding Step to Variable Speed Sign '" + myVariableSpeedSignStep->getVariableSpeedSignParent()->getID() + "'");
        }
        // add step to Variable Speed Sign
        myVariableSpeedSignStep->getVariableSpeedSignParent()->addVariableSpeedSignStep(myVariableSpeedSignStep);
    } else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing  Step from Variable Speed Sign '" + myVariableSpeedSignStep->getVariableSpeedSignParent()->getID() + "'");
        }
        // remove step from Variable Speed Sign
        myVariableSpeedSignStep->getVariableSpeedSignParent()->removeVariableSpeedSignStep(myVariableSpeedSignStep);
    }
    // enable save additionals
    myNet->requiereSaveAdditionals();
}


FXString
GNEChange_VariableSpeedSignItem::undoName() const {
    return ("Undo change " + toString(myVariableSpeedSignStep->getTag()) + " values").c_str();
}


FXString
GNEChange_VariableSpeedSignItem::redoName() const {
    return ("Redo change " + toString(myVariableSpeedSignStep->getTag()) + " values").c_str();
}

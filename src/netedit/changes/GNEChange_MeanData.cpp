/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNEChange_MeanData.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2022
///
// A network change in which a mean data set is created or deleted
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/elements/data/GNEMeanData.h>

#include "GNEChange_MeanData.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_MeanData, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_MeanData::GNEChange_MeanData(GNEMeanData* meanData, bool forward) :
    GNEChange(Supermode::DATA, forward, meanData->isAttributeCarrierSelected()),
    myMeanData(meanData) {
    myMeanData->incRef("GNEChange_MeanData");
}


GNEChange_MeanData::~GNEChange_MeanData() {
    // only continue we have undo-redo mode enabled
    if (myMeanData->getNet()->getGNEApplicationWindow()->isUndoRedoAllowed()) {
        myMeanData->decRef("GNEChange_MeanData");
        if (myMeanData->unreferenced()) {
            // make sure that MeanData isn't in net before removing
            if (myMeanData->getNet()->getAttributeCarriers()->retrieveMeanData(myMeanData->getTagProperty()->getTag(), myMeanData->getID(), false)) {
                // delete MeanData from net
                myMeanData->getNet()->getAttributeCarriers()->deleteMeanData(myMeanData);
            }
            // delete mean data
            delete myMeanData;
        }
    }
}


void
GNEChange_MeanData::undo() {
    if (myForward) {
        // unselect if mySelectedElement is enabled
        if (mySelectedElement) {
            myMeanData->unselectAttributeCarrier();
        }
        // delete meanData from net
        myMeanData->getNet()->getAttributeCarriers()->deleteMeanData(myMeanData);
    } else {
        // select if mySelectedElement is enabled
        if (mySelectedElement) {
            myMeanData->selectAttributeCarrier();
        }
        // insert meanData into net
        myMeanData->getNet()->getAttributeCarriers()->insertMeanData(myMeanData);
    }
    // require always save elements
    myMeanData->getNet()->getSavingStatus()->requireSaveMeanDatas();
}


void
GNEChange_MeanData::redo() {
    if (myForward) {
        // select if mySelectedElement is enabled
        if (mySelectedElement) {
            myMeanData->selectAttributeCarrier();
        }
        // insert meanData into net
        myMeanData->getNet()->getAttributeCarriers()->insertMeanData(myMeanData);
    } else {
        // unselect if mySelectedElement is enabled
        if (mySelectedElement) {
            myMeanData->unselectAttributeCarrier();
        }
        // delete meanData from net
        myMeanData->getNet()->getAttributeCarriers()->deleteMeanData(myMeanData);
    }
    // require always save elements
    myMeanData->getNet()->getSavingStatus()->requireSaveMeanDatas();
}


std::string
GNEChange_MeanData::undoName() const {
    if (myForward) {
        return (TL("Undo create ") + myMeanData->getTagStr());
    } else {
        return (TL("Undo delete ") + myMeanData->getTagStr());
    }
}


std::string
GNEChange_MeanData::redoName() const {
    if (myForward) {
        return (TL("Redo create ") + myMeanData->getTagStr());
    } else {
        return (TL("Redo delete ") + myMeanData->getTagStr());
    }
}

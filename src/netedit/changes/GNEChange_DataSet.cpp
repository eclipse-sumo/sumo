/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_DataSet.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A network change in which a data set element is created or deleted
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/elements/data/GNEDataSet.h>
#include <netedit/GNEViewNet.h>

#include "GNEChange_DataSet.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_DataSet, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_DataSet::GNEChange_DataSet(GNEDataSet* dataSet, bool forward) :
    GNEChange(dataSet->getViewNet()->getNet(), forward),
    myDataSet(dataSet)
    /*
    myEdgePath(dataSet->getPathEdges()),
    myParentEdges(dataSet->getParentEdges()),
    myParentLanes(dataSet->getParentLanes()),
    myParentShapes(dataSet->getParentShapes()),
    myParentAdditionals(dataSet->getParentAdditionals()),
    myParentDataSets(dataSet->getParentDataSets()),
    myChildEdges(dataSet->getChildEdges()),
    myChildLanes(dataSet->getChildLanes()),
    myChildShapes(dataSet->getChildShapes()),
    myChildAdditionals(dataSet->getChildAdditionals()),
    myChildDataSets(dataSet->getChildDataSets())*/{
    myDataSet->incRef("GNEChange_DataSet");
}


GNEChange_DataSet::~GNEChange_DataSet() {
    assert(myDataSet);
    myDataSet->decRef("GNEChange_DataSet");
    if (myDataSet->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myDataSet->getTagStr() + " '" + myDataSet->getID() + "'");
        /*
        // make sure that element isn't in net before removing
        if (myNet->dataSetExist(myDataSet)) {
            myNet->deleteDataSet(myDataSet, false);
            // remove element from path
            for (const auto& i : myEdgePath) {
                i->removePathElement(myDataSet);
            }
            // Remove element from parent elements
            for (const auto& i : myParentEdges) {
                i->removeChildDataSet(myDataSet);
            }
            for (const auto& i : myParentLanes) {
                i->removeChildDataSet(myDataSet);
            }
            for (const auto& i : myParentShapes) {
                i->removeChildDataSet(myDataSet);
            }
            for (const auto& i : myParentAdditionals) {
                i->removeChildDataSet(myDataSet);
            }
            for (const auto& i : myParentDataSets) {
                i->removeChildDataSet(myDataSet);
            }
            // Remove element from child elements
            for (const auto& i : myChildEdges) {
                i->removeParentDataSet(myDataSet);
            }
            for (const auto& i : myChildLanes) {
                i->removeParentDataSet(myDataSet);
            }
            for (const auto& i : myChildShapes) {
                i->removeParentDataSet(myDataSet);
            }
            for (const auto& i : myChildAdditionals) {
                i->removeParentDataSet(myDataSet);
            }
            for (const auto& i : myChildDataSets) {
                i->removeParentDataSet(myDataSet);
            }
        }
        */
        delete myDataSet;
    }
}


void
GNEChange_DataSet::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myDataSet->getTagStr() + " '" + myDataSet->getID() + "' in GNEChange_DataSet");
        // delete data set from net
        //myNet->deleteDataSet(myDataSet, false);
        /*
        // remove element from path
        for (const auto& i : myEdgePath) {
            i->removePathElement(myDataSet);
        }
        // Remove element from parent elements
        for (const auto& i : myParentEdges) {
            i->removeChildDataSet(myDataSet);
        }
        for (const auto& i : myParentLanes) {
            i->removeChildDataSet(myDataSet);
        }
        for (const auto& i : myParentShapes) {
            i->removeChildDataSet(myDataSet);
        }
        for (const auto& i : myParentAdditionals) {
            i->removeChildDataSet(myDataSet);
        }
        for (const auto& i : myParentDataSets) {
            i->removeChildDataSet(myDataSet);
        }
        // Remove element from child elements
        for (const auto& i : myChildEdges) {
            i->removeParentDataSet(myDataSet);
        }
        for (const auto& i : myChildLanes) {
            i->removeParentDataSet(myDataSet);
        }
        for (const auto& i : myChildShapes) {
            i->removeParentDataSet(myDataSet);
        }
        for (const auto& i : myChildAdditionals) {
            i->removeParentDataSet(myDataSet);
        }
        for (const auto& i : myChildDataSets) {
            i->removeParentDataSet(myDataSet);
        }
        */
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myDataSet->getTagStr() + " '" + myDataSet->getID() + "' in GNEChange_DataSet");
        // insert data set into net
        //myNet->insertDataSet(myDataSet);
        /*
        // add element in parent elements
        for (const auto& i : myParentEdges) {
            i->addChildDataSet(myDataSet);
        }
        for (const auto& i : myParentLanes) {
            i->addChildDataSet(myDataSet);
        }
        for (const auto& i : myParentShapes) {
            i->addChildDataSet(myDataSet);
        }
        for (const auto& i : myParentAdditionals) {
            i->addChildDataSet(myDataSet);
        }
        for (const auto& i : myParentDataSets) {
            i->addChildDataSet(myDataSet);
        }
        // add element in child elements
        for (const auto& i : myChildEdges) {
            i->addParentDataSet(myDataSet);
        }
        for (const auto& i : myChildLanes) {
            i->addParentDataSet(myDataSet);
        }
        for (const auto& i : myChildShapes) {
            i->addParentDataSet(myDataSet);
        }
        for (const auto& i : myChildAdditionals) {
            i->addParentDataSet(myDataSet);
        }
        for (const auto& i : myChildDataSets) {
            i->addParentDataSet(myDataSet);
        }
        */
    }
    // Requiere always save elements
    myNet->requireSaveDataElements(true);
}


void
GNEChange_DataSet::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myDataSet->getTagStr() + " '" + myDataSet->getID() + "' in GNEChange_DataSet");
        // insert data set into net
        //myNet->insertDataSet(myDataSet);
        /*
        // add element in parent elements
        for (const auto& i : myParentEdges) {
            i->addChildDataSet(myDataSet);
        }
        for (const auto& i : myParentLanes) {
            i->addChildDataSet(myDataSet);
        }
        for (const auto& i : myParentShapes) {
            i->addChildDataSet(myDataSet);
        }
        for (const auto& i : myParentAdditionals) {
            i->addChildDataSet(myDataSet);
        }
        for (const auto& i : myParentDataSets) {
            i->addChildDataSet(myDataSet);
        }
        // add element in child elements
        for (const auto& i : myChildEdges) {
            i->addParentDataSet(myDataSet);
        }
        for (const auto& i : myChildLanes) {
            i->addParentDataSet(myDataSet);
        }
        for (const auto& i : myChildShapes) {
            i->addParentDataSet(myDataSet);
        }
        for (const auto& i : myChildAdditionals) {
            i->addParentDataSet(myDataSet);
        }
        for (const auto& i : myChildDataSets) {
            i->addParentDataSet(myDataSet);
        }
        */
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myDataSet->getTagStr() + " '" + myDataSet->getID() + "' in GNEChange_DataSet");
        // delete data set from net
        //myNet->deleteDataSet(myDataSet, false);
        /*
        // remove element from path
        for (const auto& i : myEdgePath) {
            i->removePathElement(myDataSet);
        }
        // Remove element from parent elements
        for (const auto& i : myParentEdges) {
            i->removeChildDataSet(myDataSet);
        }
        for (const auto& i : myParentLanes) {
            i->removeChildDataSet(myDataSet);
        }
        for (const auto& i : myParentShapes) {
            i->removeChildDataSet(myDataSet);
        }
        for (const auto& i : myParentAdditionals) {
            i->removeChildDataSet(myDataSet);
        }
        for (const auto& i : myParentDataSets) {
            i->removeChildDataSet(myDataSet);
        }
        // Remove element from child elements
        for (const auto& i : myChildEdges) {
            i->removeParentDataSet(myDataSet);
        }
        for (const auto& i : myChildLanes) {
            i->removeParentDataSet(myDataSet);
        }
        for (const auto& i : myChildShapes) {
            i->removeParentDataSet(myDataSet);
        }
        for (const auto& i : myChildAdditionals) {
            i->removeParentDataSet(myDataSet);
        }
        for (const auto& i : myChildDataSets) {
            i->removeParentDataSet(myDataSet);
        }
        */
    }
    // Requiere always save elements
    myNet->requireSaveDataElements(true);
}


FXString
GNEChange_DataSet::undoName() const {
    if (myForward) {
        return ("Undo create " + myDataSet->getTagStr()).c_str();
    } else {
        return ("Undo delete " + myDataSet->getTagStr()).c_str();
    }
}


FXString
GNEChange_DataSet::redoName() const {
    if (myForward) {
        return ("Redo create " + myDataSet->getTagStr()).c_str();
    } else {
        return ("Redo delete " + myDataSet->getTagStr()).c_str();
    }
}

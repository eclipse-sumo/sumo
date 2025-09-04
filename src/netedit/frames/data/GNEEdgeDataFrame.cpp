/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEEdgeDataFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// The Widget for add edgeData elements
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/data/GNEDataHandler.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/data/GNEEdgeData.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/frames/GNEAttributesEditor.h>

#include "GNEEdgeDataFrame.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEEdgeDataFrame::GNEEdgeDataFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEGenericDataFrame(viewParent, viewNet, GNE_TAG_EDGEREL_SINGLE, false) {
}


GNEEdgeDataFrame::~GNEEdgeDataFrame() {}


bool
GNEEdgeDataFrame::addEdgeData(const GNEViewNetHelper::ViewObjectsSelector& viewObjects, const GNEViewNetHelper::MouseButtonKeyPressed& /*mouseButtonKeyPressed*/) {
    // first check if we clicked over an edge
    if (viewObjects.getEdgeFront() && myDataSetSelector->getDataSet() && myIntervalSelector->getDataInterval()) {
        // first check if the given interval there is already a EdgeData for the given ID
        for (const auto& genericData : myIntervalSelector->getDataInterval()->getGenericDataChildren()) {
            if ((genericData->getTagProperty()->getTag() == GNE_TAG_EDGEREL_SINGLE) && (genericData->getParentEdges().front() == viewObjects.getEdgeFront())) {
                // write warning
                WRITE_WARNINGF(TL("There is already a % in edge '%'"), genericData->getTagStr(), viewObjects.getEdgeFront()->getID());
                // abort edge data creation
                return false;
            }
        }
        // check if parameters are valid
        if (myGenericDataAttributesEditor->checkAttributes(true)) {
            GNEDataHandler dataHandler(myViewNet->getNet(), "", myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed());
            // create interval base object
            CommonXMLStructure::SumoBaseObject* intervalBaseObject = new CommonXMLStructure::SumoBaseObject(nullptr);
            intervalBaseObject->addStringAttribute(SUMO_ATTR_ID, myIntervalSelector->getDataInterval()->getID());
            intervalBaseObject->addDoubleAttribute(SUMO_ATTR_BEGIN, myIntervalSelector->getDataInterval()->getAttributeDouble(SUMO_ATTR_BEGIN));
            intervalBaseObject->addDoubleAttribute(SUMO_ATTR_END, myIntervalSelector->getDataInterval()->getAttributeDouble(SUMO_ATTR_END));
            // create genericData base object
            CommonXMLStructure::SumoBaseObject* genericDataBaseObject = new CommonXMLStructure::SumoBaseObject(intervalBaseObject);
            // obtain parameters
            myGenericDataAttributesEditor->fillSumoBaseObject(genericDataBaseObject);
            // create edgeData
            dataHandler.buildEdgeData(genericDataBaseObject, viewObjects.getEdgeFront()->getID(), genericDataBaseObject->getParameters());
            // delete data interval object (and child)
            delete intervalBaseObject;
            return true;
        } else {
            return false;
        }
    } else {
        // invalid parent parameters
        return false;
    }
}


/****************************************************************************/

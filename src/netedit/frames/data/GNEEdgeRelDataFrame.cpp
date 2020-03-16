/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEEdgeRelDataFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2020
///
// The Widget for add EdgeRelationData elements
/****************************************************************************/
#include <config.h>

#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/data/GNEEdgeRelData.h>
#include <netedit/elements/data/GNEDataHandler.h>
#include <netedit/elements/network/GNEEdge.h>

#include "GNEEdgeRelDataFrame.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEEdgeRelDataFrame::GNEEdgeRelDataFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEGenericDataFrame(horizontalFrameParent, viewNet, "Edge relation datas") {
}


GNEEdgeRelDataFrame::~GNEEdgeRelDataFrame() {}


bool
GNEEdgeRelDataFrame::addEdgeRelationData(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // first check if we clicked over an edge
    if (objectsUnderCursor.getEdgeFront() && myDataSetSelector->getDataSet() && myIntervalSelector->getDataInterval()) {
        // first check if the given interval there is already a EdgeRelationData for the given ID
        for (const auto& genericData : myIntervalSelector->getDataInterval()->getGenericDataChildren()) {
            if ((genericData->getTagProperty().getTag() == SUMO_TAG_EDGEREL) && (genericData->getParentEdges().front() == objectsUnderCursor.getEdgeFront())) {
                // write warning
                WRITE_WARNING("There is already a " + genericData->getTagStr() + " in edge '" + objectsUnderCursor.getEdgeFront()->getID() + "'");
                // abort edge data creation
                return false;
            }
        }
        // finally create EdgeRelationData
        GNEDataHandler::buildEdgeRelationData(myViewNet, true, myIntervalSelector->getDataInterval(), objectsUnderCursor.getEdgeFront(), myParametersEditor->getParametersMap());
        // EdgeRelationData created, then return true
        return true;
    } else {
        // invalid parent parameters
        return false;
    }
}


/****************************************************************************/

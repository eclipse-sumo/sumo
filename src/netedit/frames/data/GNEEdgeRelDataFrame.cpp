/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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

#include <netedit/elements/data/GNEDataHandler.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/GNEViewNet.h>

#include "GNEEdgeRelDataFrame.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEEdgeRelDataFrame::GNEEdgeRelDataFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEGenericDataFrame(viewParent, viewNet, SUMO_TAG_EDGEREL, true) {
}


GNEEdgeRelDataFrame::~GNEEdgeRelDataFrame() {}


bool
GNEEdgeRelDataFrame::addEdgeRelationData(const GNEViewNetHelper::ViewObjectsSelector& viewObjects, const GNEViewNetHelper::MouseButtonKeyPressed& mouseButtonKeyPressed) {
    // first check if we clicked over an edge
    if (viewObjects.getEdgeFront() && myDataSetSelector->getDataSet() && myIntervalSelector->getDataInterval()) {
        return myPathCreator->addEdge(viewObjects.getEdgeFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else {
        // invalid parent parameters
        return false;
    }
}


bool
GNEEdgeRelDataFrame::createPath(const bool /*useLastRoute*/) {
    // first check that we have at least two edges and parameters are valid
    if ((myPathCreator->getSelectedEdges().size() > 1) && (myGenericDataAttributes->areAttributesValid())) {
        GNEDataHandler dataHandler(myViewNet->getNet(), "", true, false);
        // create data interval object and fill it
        CommonXMLStructure::SumoBaseObject* dataIntervalObject = new CommonXMLStructure::SumoBaseObject(nullptr);
        dataIntervalObject->addStringAttribute(SUMO_ATTR_ID, myIntervalSelector->getDataInterval()->getID());
        dataIntervalObject->addDoubleAttribute(SUMO_ATTR_BEGIN, myIntervalSelector->getDataInterval()->getAttributeDouble(SUMO_ATTR_BEGIN));
        dataIntervalObject->addDoubleAttribute(SUMO_ATTR_END, myIntervalSelector->getDataInterval()->getAttributeDouble(SUMO_ATTR_END));
        CommonXMLStructure::SumoBaseObject* edgeRelationData = new CommonXMLStructure::SumoBaseObject(dataIntervalObject);
        // create EdgeRelationData
        dataHandler.buildEdgeRelationData(edgeRelationData, myPathCreator->getSelectedEdges().front()->getID(),
                                          myPathCreator->getSelectedEdges().back()->getID(), myGenericDataAttributes->getParametersMap());
        // abort path creation
        myPathCreator->abortPathCreation();
        delete dataIntervalObject;
        return true;
    } else {
        return false;
    }
}

/****************************************************************************/

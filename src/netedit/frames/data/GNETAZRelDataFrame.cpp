/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNETAZRelDataFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2020
///
// The Widget for add TAZRelationData elements
/****************************************************************************/
#include <config.h>

#include <netedit/elements/data/GNEDataHandler.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/GNEViewNet.h>

#include "GNETAZRelDataFrame.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNETAZRelDataFrame::GNETAZRelDataFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEGenericDataFrame(horizontalFrameParent, viewNet, SUMO_TAG_TAZREL, false),
    myFirstTAZ(nullptr) {
}


GNETAZRelDataFrame::~GNETAZRelDataFrame() {}


bool
GNETAZRelDataFrame::addTAZRelationData(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, const GNEViewNetHelper::MouseButtonKeyPressed& /*mouseButtonKeyPressed*/) {
    // check if myFirstTAZElement is empty
    if (myFirstTAZ) {
        if (!objectsUnderCursor.getTAZElementFront()) {
            // only show warning if we clicked over another AC
            if (objectsUnderCursor.getAttributeCarrierFront()) {
                WRITE_WARNING("A " + toString(SUMO_TAG_TAZREL) + " must be defined between two TAZs.");
            }
            return false;
        } else if (!myIntervalSelector->getDataInterval()) {
            WRITE_WARNING("A " + toString(SUMO_TAG_TAZREL) + " must be defined within an interval.");
            return false;
        } else {
            // get second TAZ
            const auto secondTAZ = objectsUnderCursor.getTAZElementFront();
            // check conditions
            if (myFirstTAZ == secondTAZ) {
                WRITE_WARNING("A " + toString(SUMO_TAG_TAZREL) + " must be defined between two different TAZs.");
                return false;
            } else if (myIntervalSelector->getDataInterval()->TAZRelExists(myFirstTAZ, secondTAZ)) {
                WRITE_WARNING("There is already a " + toString(SUMO_TAG_TAZREL) + " defined between TAZ'" + myFirstTAZ->getID() + "' and '" + secondTAZ->getID() + "'.");
                return false;
            } else {
                // declare data handler
                GNEDataHandler dataHandler(myViewNet->getNet(), "", true);
                // build data interval object and fill it
                CommonXMLStructure::SumoBaseObject* dataIntervalObject = new CommonXMLStructure::SumoBaseObject(nullptr);
                dataIntervalObject->addStringAttribute(SUMO_ATTR_ID, myIntervalSelector->getDataInterval()->getID());
                dataIntervalObject->addDoubleAttribute(SUMO_ATTR_BEGIN, myIntervalSelector->getDataInterval()->getAttributeDouble(SUMO_ATTR_BEGIN));
                dataIntervalObject->addDoubleAttribute(SUMO_ATTR_END, myIntervalSelector->getDataInterval()->getAttributeDouble(SUMO_ATTR_END));
                // create TAZRelData
                CommonXMLStructure::SumoBaseObject* TAZRelData = new CommonXMLStructure::SumoBaseObject(dataIntervalObject);
                // finally create TAZRelationData
                dataHandler.buildTAZRelationData(TAZRelData, myFirstTAZ->getID(), secondTAZ->getID(), myParametersEditorCreator->getParametersMap());
                delete dataIntervalObject;
                // reset myFirstTAZElement
                myFirstTAZ = nullptr;
                // TAZRelationData created, then return true
                return true;
            }
        }
    } else if (objectsUnderCursor.getTAZElementFront()) {
        myFirstTAZ = objectsUnderCursor.getTAZElementFront();
        return true;
    } else {
        return false;
    }
}


void
GNETAZRelDataFrame::clearTAZSelection() {
    myFirstTAZ = nullptr;
}

/****************************************************************************/

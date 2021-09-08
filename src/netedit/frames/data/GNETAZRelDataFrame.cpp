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
#include <netedit/elements/additional/GNETAZElement.h>
#include <netedit/GNEViewNet.h>

#include "GNETAZRelDataFrame.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNETAZRelDataFrame::GNETAZRelDataFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEGenericDataFrame(horizontalFrameParent, viewNet, SUMO_TAG_TAZREL, false),
    myFirstTAZ(nullptr),
    mySecondTAZ(nullptr) {
}


GNETAZRelDataFrame::~GNETAZRelDataFrame() {}


bool
GNETAZRelDataFrame::setTAZ(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // check if myFirstTAZElement is empty
    if (myFirstTAZ) {
        if (mySecondTAZ) {
            // both already defined
            return false;
        } else {
            mySecondTAZ = objectsUnderCursor.getTAZElementFront();
            return true;
        }
    } else if (objectsUnderCursor.getTAZElementFront()) {
        myFirstTAZ = objectsUnderCursor.getTAZElementFront();
        return true;
    } else {
        return false;
    }
}


void 
GNETAZRelDataFrame::buildTAZRelationData() {
    // check conditions
    if (myFirstTAZ && mySecondTAZ) {
        if (myIntervalSelector->getDataInterval()->TAZRelExists(myFirstTAZ, mySecondTAZ)) {
            WRITE_WARNING("There is already a " + toString(SUMO_TAG_TAZREL) + " defined between TAZ'" + myFirstTAZ->getID() + "' and '" + mySecondTAZ->getID() + "'.");
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
            dataHandler.buildTAZRelationData(TAZRelData, myFirstTAZ->getID(), mySecondTAZ->getID(), myParametersEditorCreator->getParametersMap());
            delete dataIntervalObject;
            // reset both TAZs
            myFirstTAZ = nullptr;
            mySecondTAZ = nullptr;
        }
    }
}


GNETAZElement* 
GNETAZRelDataFrame::getFirstTAZ() const {
    return myFirstTAZ;
}


GNETAZElement* 
GNETAZRelDataFrame::getSecondTAZ() const {
    return mySecondTAZ;
}


void
GNETAZRelDataFrame::clearTAZSelection() {
    myFirstTAZ = nullptr;
    mySecondTAZ = nullptr;
}

/****************************************************************************/

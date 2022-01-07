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
/// @file    GNEGeneralHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2021
///
// General element handler for NETEDIT
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================

#include <utils/xml/XMLSubSys.h>

#include "GNEGeneralHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEGeneralHandler::GNEGeneralHandler(GNENet* net, const std::string& file, const bool allowUndoRedo) :
    GeneralHandler(file),
    myAdditionalHandler(net, allowUndoRedo),
    myDemandHandler(file, net, allowUndoRedo),
    myAdditionalTagFlag(false),
    myDemandTagFlag(false) {
}


GNEGeneralHandler::~GNEGeneralHandler() {}


void
GNEGeneralHandler::beginTag(SumoXMLTag tag, const SUMOSAXAttributes& attrs) {
    // reset both flags
    myAdditionalTagFlag = false;
    myDemandTagFlag = false;
    // try to parse additional or demand element
    if (myAdditionalHandler.beginParseAttributes(tag, attrs)) {
        myAdditionalTagFlag = true;
    } else if (myDemandHandler.beginParseAttributes(tag, attrs)) {
        myDemandTagFlag = true;
    }
}


void
GNEGeneralHandler::endTag() {
    // check flags
    if (myAdditionalTagFlag) {
        // end parse additional elements
        myAdditionalHandler.endParseAttributes();
    }
    if (myDemandTagFlag) {
        // end parse demand elements
        myDemandHandler.endParseAttributes();
    }
}

/****************************************************************************/

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
    SUMOSAXHandler(file),
    myAdditionalHandler(net, allowUndoRedo),
    myDemandHandler(file, net, allowUndoRedo) {
}


GNEGeneralHandler::~GNEGeneralHandler() {}


bool
GNEGeneralHandler::parse() {
    // run parser and return result
    return XMLSubSys::runParser(*this, getFileName());
}


void
GNEGeneralHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // obtain tag
    const SumoXMLTag tag = (element == 0)? SUMO_TAG_ROOTFILE : static_cast<SumoXMLTag>(element);
    // parse additional elements
    myAdditionalHandler.beginParseAttributes(tag, attrs);
    // parse demand elements
    myDemandHandler.beginParseAttributes(tag, attrs);
}


void
GNEGeneralHandler::myEndElement(int /*element*/) {
    // end parse additional elements
    myAdditionalHandler.endParseAttributes();
    // end parse demand elements
    myDemandHandler.endParseAttributes();
}

/****************************************************************************/

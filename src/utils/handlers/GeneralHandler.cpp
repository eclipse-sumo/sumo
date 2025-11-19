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
/// @file    GeneralHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2021
///
// General element handler
/****************************************************************************/

#include <utils/common/FileBucket.h>
#include <utils/xml/XMLSubSys.h>

#include "GeneralHandler.h"

// ===========================================================================
// method definitions
// ===========================================================================

GeneralHandler::GeneralHandler(FileBucket* fileBucket) :
    SUMOSAXHandler(fileBucket->getFilename()) {
}


GeneralHandler::~GeneralHandler() {}


bool
GeneralHandler::parse() {
    // run parser and postParser Task
    const bool parseResult = XMLSubSys::runParser(*this, getFileName());
    const bool postTaskResult = postParserTasks();
    return parseResult && postTaskResult;
}


void
GeneralHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // obtain tag
    const SumoXMLTag tag = (element == 0) ? SUMO_TAG_ROOTFILE : static_cast<SumoXMLTag>(element);
    // begin tag
    beginTag(tag, attrs);
}


void
GeneralHandler::myEndElement(int /*element*/) {
    // end tag
    endTag();
}

/****************************************************************************/

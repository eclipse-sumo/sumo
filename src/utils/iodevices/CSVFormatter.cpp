/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2025 German Aerospace Center (DLR) and others.
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
/// @file    CSVFormatter.cpp
/// @author  Michael Behrisch
/// @date    2025-06-12
///
// An output formatter for CSV files
/****************************************************************************/
#include <config.h>

#include <utils/common/ToString.h>
#include "CSVFormatter.h"


// ===========================================================================
// member method definitions
// ===========================================================================
CSVFormatter::CSVFormatter(const char separator)
    : OutputFormatter(OutputFormatterType::CSV), mySeparator(separator), myMaxDepth(0), myWroteHeader(false) {
}


bool
CSVFormatter::writeXMLHeader(std::ostream& /* into */, const std::string& /* rootElement */,
                             const std::map<SumoXMLAttr, std::string>& /* attrs */, bool /* includeConfig */) {
    return false;
}


void
CSVFormatter::openTag(std::ostream& /* into */, const std::string& /* xmlElement */) {
    myXMLStack.push_back(std::ostringstream());
}


void
CSVFormatter::openTag(std::ostream& /* into */, const SumoXMLTag& /* xmlElement */) {
    myXMLStack.push_back(std::ostringstream());
}


bool
CSVFormatter::closeTag(std::ostream& into, const std::string& /* comment */) {
    if (myMaxDepth == 0 || (myMaxDepth == (int)myXMLStack.size() && !myWroteHeader)) {
        into << myHeader << std::endl;
        if (myMaxDepth == 0) {
            myMaxDepth = (int)myXMLStack.size();
            myExpectedAttrs = mySeenAttrs;
        }
        myWroteHeader = true;
    }
    if ((int)myXMLStack.size() == myMaxDepth) {
        if (myExpectedAttrs != mySeenAttrs) {
            throw ProcessError(TLF("Incomplete attribute set '%', this file format does not support CSV output yet.", toString(mySeenAttrs)));
        }
        for (auto it = myXMLStack.begin(); it != myXMLStack.end() - 1; ++it) {
            into << it->str();
        }
        // remove the final separator
        std::string final = myXMLStack.back().str();
        final.pop_back();
        into << final << std::endl;
        mySeenAttrs.reset();
    }
    if (!myXMLStack.empty()) {
        myXMLStack.pop_back();
    }
    return false;
}


/****************************************************************************/

/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2026 German Aerospace Center (DLR) and others.
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

#ifdef HAVE_FMT
#include <fmt/ostream.h>
#endif
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "CSVFormatter.h"


// ===========================================================================
// member method definitions
// ===========================================================================
CSVFormatter::CSVFormatter(const std::string& columnNames, const char separator)
    : OutputFormatter(OutputFormatterType::CSV), myHeaderFormat(columnNames), mySeparator(separator) {
    if (myHeaderFormat == "none") {
        myWroteHeader = true;
    }
}


void
CSVFormatter::openTag(std::ostream& /* into */, const std::string& xmlElement) {
    myXMLStack.push_back((int)myValues.size());
    if (!myWroteHeader) {
        myCurrentTag = xmlElement;
    }
    if (myMaxDepth == (int)myXMLStack.size() && myWroteHeader && myCurrentTag != xmlElement) {
        WRITE_WARNINGF("Encountered mismatch in XML tags (expected % but got %). Column names may be incorrect.", myCurrentTag, xmlElement);
    }
}


void
CSVFormatter::openTag(std::ostream& /* into */, const SumoXMLTag& xmlElement) {
    myXMLStack.push_back((int)myValues.size());
    if (!myWroteHeader) {
        myCurrentTag = toString(xmlElement);
    }
    if (myMaxDepth == (int)myXMLStack.size() && myWroteHeader && myCurrentTag != toString(xmlElement)) {
        WRITE_WARNINGF("Encountered mismatch in XML tags (expected % but got %). Column names may be incorrect.", myCurrentTag, toString(xmlElement));
    }
}


bool
CSVFormatter::closeTag(std::ostream& into, const std::string& /* comment */) {
    if (myMaxDepth == 0) {
        // the auto detection case: the first closed tag determines the depth
        myMaxDepth = (int)myXMLStack.size();
    }
    if ((myMaxDepth == (int)myXMLStack.size() || myXMLStack.empty()) && !myWroteHeader) {
        // First complete row or EOF: write the header
        if (!myCheckColumns) {
            WRITE_WARNING("Column based formats are still experimental. Autodetection only works for homogeneous output.");
        }
#ifdef HAVE_FMT
        fmt::print(into, "{}\n", fmt::join(myHeader, std::string_view(&mySeparator, 1)));
#else
        into << joinToString(myHeader, mySeparator) << "\n";
#endif
        myWroteHeader = true;
    }
    if (myNeedsWrite) {
#ifdef HAVE_FMT
        const std::string row = fmt::format("{}", fmt::join(myValues, std::string_view(&mySeparator, 1)));
#else
        const std::string row = joinToString(myValues, mySeparator);
#endif
        myBufferedRows.emplace_back(row);
        mySeenAttrs.reset();
        myNeedsWrite = false;
    }
    if (myWroteHeader && !myBufferedRows.empty()) {
        for (const std::string& row : myBufferedRows) {
            into << row << '\n';
        }
        myBufferedRows.clear();
    }
    if (!myXMLStack.empty()) {
        if ((int)myValues.size() > myXMLStack.back()) {
            myValues.resize(myXMLStack.back());
        }
        myXMLStack.pop_back();
    }
    return false;
}


/****************************************************************************/

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
/// @file    PlainXMLFormatter.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2012
///
// Static storage of an output device and its base (abstract) implementation
/****************************************************************************/
#include <config.h>

#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include "PlainXMLFormatter.h"


// ===========================================================================
// member method definitions
// ===========================================================================
PlainXMLFormatter::PlainXMLFormatter(const int defaultIndentation)
    : myDefaultIndentation(defaultIndentation), myHavePendingOpener(false) {
}


bool
PlainXMLFormatter::writeHeader(StreamDevice& into, const SumoXMLTag& rootElement) {
    if (myXMLStack.empty()) {
        if (auto* ostream = into.getUnderlyingOStream()) {
            OptionsCont::getOptions().writeXMLHeader(*ostream);
        }
        else {
            std::ostringstream temp;
            OptionsCont::getOptions().writeXMLHeader(temp);
            into << temp.str();
        }        openTag(into, rootElement);
        return true;
    }
    return false;
}


bool
PlainXMLFormatter::writeXMLHeader(StreamDevice& into, const std::string& rootElement,
    const std::map<SumoXMLAttr, std::string>& attrs, bool includeConfig) {
    if (myXMLStack.empty()) {
        if (auto* ostream = into.getUnderlyingOStream()) {
            OptionsCont::getOptions().writeXMLHeader(*ostream, includeConfig);
        }
        else {
            std::ostringstream temp;
            OptionsCont::getOptions().writeXMLHeader(temp, includeConfig);
            into << temp.str();
        }
        openTag(into, rootElement);
        for (std::map<SumoXMLAttr, std::string>::const_iterator it = attrs.begin(); it != attrs.end(); ++it) {
            writeAttr(into, it->first, it->second);
        }
        into << ">\n";
        myHavePendingOpener = false;
        return true;
    }
    return false;
}


void
PlainXMLFormatter::openTag(StreamDevice& into, const std::string& xmlElement) {
    if (myHavePendingOpener) {
        into << ">\n";
    }
    myHavePendingOpener = true;
    into << std::string(4 * (myXMLStack.size() + myDefaultIndentation), ' ') << "<" << xmlElement;
    myXMLStack.push_back(xmlElement);
}


void
PlainXMLFormatter::openTag(StreamDevice& into, const SumoXMLTag& xmlElement) {
    openTag(into, toString(xmlElement));
}


bool
PlainXMLFormatter::closeTag(StreamDevice& into, const std::string& comment) {
    if (!myXMLStack.empty()) {
        if (myHavePendingOpener) {
            into << "/>" << comment << "\n";
            myHavePendingOpener = false;
        }
        else {
            const std::string indent(4 * (myXMLStack.size() + myDefaultIndentation - 1), ' ');
            into << indent << "</" << myXMLStack.back() << ">" << comment << "\n";
        }
        myXMLStack.pop_back();
        return true;
    }
    return false;
}


void
PlainXMLFormatter::writePreformattedTag(StreamDevice& into, const std::string& val) {
    if (myHavePendingOpener) {
        into << ">\n";
        myHavePendingOpener = false;
    }
    into << val;
}

void
PlainXMLFormatter::writePadding(StreamDevice& into, const std::string& val) {
    into << val;
}


/****************************************************************************/

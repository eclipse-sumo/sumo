/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    PlainXMLFormatter.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2012
/// @version $Id$
///
// Static storage of an output device and its base (abstract) implementation
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
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
PlainXMLFormatter::writeHeader(std::ostream& into, const SumoXMLTag& rootElement) {
    if (myXMLStack.empty()) {
        OptionsCont::getOptions().writeXMLHeader(into);
        openTag(into, rootElement);
        return true;
    }
    return false;
}


bool
PlainXMLFormatter::writeXMLHeader(std::ostream& into, const std::string& rootElement,
                                  const std::map<SumoXMLAttr, std::string>& attrs) {
    if (myXMLStack.empty()) {
        OptionsCont::getOptions().writeXMLHeader(into);
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
PlainXMLFormatter::openTag(std::ostream& into, const std::string& xmlElement) {
    if (myHavePendingOpener) {
        into << ">\n";
    }
    myHavePendingOpener = true;
    into << std::string(4 * (myXMLStack.size() + myDefaultIndentation), ' ') << "<" << xmlElement;
    myXMLStack.push_back(xmlElement);
}


void
PlainXMLFormatter::openTag(std::ostream& into, const SumoXMLTag& xmlElement) {
    openTag(into, toString(xmlElement));
}


bool
PlainXMLFormatter::closeTag(std::ostream& into, const std::string& comment) {
    if (!myXMLStack.empty()) {
        if (myHavePendingOpener) {
            into << "/>" << comment << "\n";
            myHavePendingOpener = false;
        } else {
            const std::string indent(4 * (myXMLStack.size() + myDefaultIndentation - 1), ' ');
            into << indent << "</" << myXMLStack.back() << ">" << comment << "\n";
        }
        myXMLStack.pop_back();
        return true;
    }
    return false;
}


void
PlainXMLFormatter::writePreformattedTag(std::ostream& into, const std::string& val) {
    if (myHavePendingOpener) {
        into << ">\n";
        myHavePendingOpener = false;
    }
    into << val;
}

void
PlainXMLFormatter::writePadding(std::ostream& into, const std::string& val) {
    into << val;
}

/****************************************************************************/


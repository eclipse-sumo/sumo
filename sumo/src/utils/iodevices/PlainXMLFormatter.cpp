/****************************************************************************/
/// @file    PlainXMLFormatter.cpp
/// @author  Michael Behrisch
/// @date    2012
/// @version $Id$
///
// Static storage of an output device and its base (abstract) implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include "PlainXMLFormatter.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
PlainXMLFormatter::PlainXMLFormatter(const unsigned int defaultIndentation)
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
                                  const std::string& attrs, const std::string& comment) {
    if (myXMLStack.empty()) {
        OptionsCont::getOptions().writeXMLHeader(into);
        if (comment != "") {
            into << comment << "\n";
        }
        openTag(into, rootElement);
        if (attrs != "") {
            into << " " << attrs;
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
PlainXMLFormatter::closeTag(std::ostream& into) {
    if (!myXMLStack.empty()) {
        if (myHavePendingOpener) {
            into << "/>\n";
            myHavePendingOpener = false;
        } else {
            const std::string indent(4 * (myXMLStack.size() + myDefaultIndentation - 1), ' ');
            into << indent << "</" << myXMLStack.back() << ">\n";
        }
        myXMLStack.pop_back();
        return true;
    }
    return false;
}


/****************************************************************************/


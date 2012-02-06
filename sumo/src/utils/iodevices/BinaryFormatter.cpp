/****************************************************************************/
/// @file    BinaryFormatter.cpp
/// @author  Michael Behrisch
/// @date    2012
/// @version $Id$
///
// Static storage of an output device and its base (abstract) implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/FileHelpers.h>
#include "BinaryFormatter.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
BinaryFormatter::BinaryFormatter() {
}


bool
BinaryFormatter::writeXMLHeader(std::ostream& into,
                                const std::string& rootElement,
								const std::string xmlParams,
                                const std::string& attrs,
								const std::string& comment) {
    if (myXMLStack.empty()) {
        return true;
    }
    return false;
}


void
BinaryFormatter::openTag(std::ostream& into, const std::string& xmlElement) {
}


void
BinaryFormatter::openTag(std::ostream& into, const SumoXMLTag& xmlElement) {
    myXMLStack.push_back(xmlElement);
}


void
BinaryFormatter::closeOpener(std::ostream& into) {
}


bool
BinaryFormatter::closeTag(std::ostream& into, bool abbreviated) {
    if (!myXMLStack.empty()) {
        myXMLStack.pop_back();
        return true;
    }
    return false;
}


void
BinaryFormatter::writeAttr(std::ostream& into, const std::string& attr, const std::string& val) {
}

/****************************************************************************/


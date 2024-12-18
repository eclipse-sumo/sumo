/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    CommonHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2024
///
// Collection of functions used in handlers
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/xml/XMLSubSys.h>

#include "CommonHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

CommonHandler::CommonHandler() {
}


CommonHandler::~CommonHandler() {}


bool
CommonHandler::isErrorCreatingElement() const {
    return myErrorCreatingElement;
}


bool
CommonHandler::writeError(const std::string& error) {
    WRITE_ERROR(error);
    myErrorCreatingElement = true;
    return false;
}


void
CommonHandler::writeErrorInvalidID(const SumoXMLTag tag, const std::string& id) {
    WRITE_ERRORF(TL("Could not build % with ID '%' in netedit; ID contains invalid characters."), toString(tag), id);
    myErrorCreatingElement = true;
}


void
CommonHandler::checkParent(const SumoXMLTag currentTag, const std::vector<SumoXMLTag>& parentTags, bool& ok) {
    // check that parent SUMOBaseObject's tag is the parentTag
    CommonXMLStructure::SumoBaseObject* const parent = myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject();
    if ((parent != nullptr) &&
            (parentTags.size() > 0) &&
            (std::find(parentTags.begin(), parentTags.end(), parent->getTag()) == parentTags.end())) {
        const std::string id = parent->hasStringAttribute(SUMO_ATTR_ID) ? ", id: '" + parent->getStringAttribute(SUMO_ATTR_ID) + "'" : "";
        if (id.empty()) {
            writeError(TLF("'%' must be defined within the definition of a '%'.", toString(currentTag), toString(parentTags.front())));
        } else {
            writeError(TLF("'%' must be defined within the definition of a '%' (found % '%').", toString(currentTag), toString(parentTags.front()), toString(parent->getTag()), id));
        }
        ok = false;
    }
}



/****************************************************************************/

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

CommonHandler::CommonHandler(const std::string& filename) :
    myFilename(filename) {
}


CommonHandler::~CommonHandler() {}


void
CommonHandler::forceOverwriteElements() {
    myOverwriteElements = true;
}


void
CommonHandler::forceRemainElements() {
    myRemainElements = true;
}


void
CommonHandler::abortLoading() {
    myAbortLoading = true;
}


bool
CommonHandler::isErrorCreatingElement() const {
    return myErrorCreatingElement;
}


bool
CommonHandler::isForceOverwriteElements() const {
    return myOverwriteElements;
}


bool
CommonHandler::isForceRemainElements() const {
    return myRemainElements;
}


bool
CommonHandler::isAbortLoading() const {
    return myAbortLoading;
}


void
CommonHandler::parseParameters(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // get key
    const std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, parsedOk);
    // get SumoBaseObject parent
    CommonXMLStructure::SumoBaseObject* SumoBaseObjectParent = myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject();
    // check parent
    if ((SumoBaseObjectParent == nullptr) || (SumoBaseObjectParent->getTag() == SUMO_TAG_ROOTFILE)) {
        writeError(TL("Parameters must be defined within an object"));
    } else if (SumoBaseObjectParent->getTag() == SUMO_TAG_PARAM) {
        writeError(TL("Parameters cannot be defined within another parameter."));
    } else if ((SumoBaseObjectParent->getTag() != SUMO_TAG_ERROR) && parsedOk) {
        // get tag str
        const std::string parentTagStr = toString(SumoBaseObjectParent->getTag());
        // circumventing empty string value
        const std::string value = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
        // show warnings if values are invalid
        if (key.empty()) {
            writeError(TLF("Error parsing key from % generic parameter. Key cannot be empty", parentTagStr));
        } else if (!SUMOXMLDefinitions::isValidParameterKey(key)) {
            writeError(TLF("Error parsing key from % generic parameter. Key contains invalid characters", parentTagStr));
        } else {
            // insert parameter in SumoBaseObjectParent
            SumoBaseObjectParent->addParameter(key, value);
        }
    }
}


CommonXMLStructure::SumoBaseObject*
CommonHandler::getEmbeddedRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const {
    // locate route in childrens
    for (const auto& embeddedRoute : sumoBaseObject->getSumoBaseObjectChildren()) {
        if ((embeddedRoute->getTag() == SUMO_TAG_ROUTE) && (!embeddedRoute->hasStringAttribute(SUMO_ATTR_ID))) {
            return embeddedRoute;
        }
    }
    return nullptr;
}


void
CommonHandler::checkParsedParent(const SumoXMLTag currentTag, const std::vector<SumoXMLTag>& parentTags, bool& ok) {
    if (parentTags.size() > 0) {
        std::string tagsStr;
        for (auto it = parentTags.begin(); it != parentTags.end(); it++) {
            tagsStr.append(toString(*it));
            if ((it + 1) != parentTags.end()) {
                if ((it + 2) != parentTags.end()) {
                    tagsStr.append(", ");
                } else {
                    tagsStr.append(" or ");
                }
            }
        }
        // obtain parent
        CommonXMLStructure::SumoBaseObject* const parent = myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject();
        if (parent == nullptr) {
            ok = writeError(TLF("'%' must be defined within the definition of a %.", toString(currentTag), tagsStr));
        } else if ((parent->getTag() != SUMO_TAG_ERROR) && std::find(parentTags.begin(), parentTags.end(), parent->getTag()) == parentTags.end()) {
            if (parent->hasStringAttribute(SUMO_ATTR_ID)) {
                ok = writeError(TLF("'%' must be defined within the definition of a '%' (found % '%').", toString(currentTag), tagsStr,
                                    toString(parent->getTag()), parent->getStringAttribute(SUMO_ATTR_ID)));
            } else {
                ok = writeError(TLF("'%' must be defined within the definition of a '%' (found %).", toString(currentTag), tagsStr,
                                    toString(parent->getTag())));
            }
        }
    }
}


bool
CommonHandler::checkListOfVehicleTypes(const SumoXMLTag tag, const std::string& id, const std::vector<std::string>& vTypeIDs) {
    for (const auto& vTypeID : vTypeIDs) {
        if (!SUMOXMLDefinitions::isValidTypeID(vTypeID)) {
            return writeError(TLF("Could not build % with ID '%' in netedit; '%' ist not a valid vType ID.", toString(tag), id, vTypeID));
        }
    }
    return true;
}


bool
CommonHandler::checkWithinDistribution(CommonXMLStructure::SumoBaseObject* obj) {
    if (obj->getParentSumoBaseObject() == nullptr) {
        return false;
    } else if (obj->getParentSumoBaseObject()->getTag() == SUMO_TAG_ROUTE_DISTRIBUTION) {
        return true;
    } else if (obj->getParentSumoBaseObject()->getTag() == SUMO_TAG_VTYPE_DISTRIBUTION) {
        return true;
    } else {
        return false;
    }
}


bool
CommonHandler::checkVehicleParents(CommonXMLStructure::SumoBaseObject* obj) {
    if (obj == nullptr) {
        return false;
    } else if (!obj->hasStringAttribute(SUMO_ATTR_ID)) {
        return false;
    } else {
        SumoXMLTag tag = obj->getTag();
        const std::string id = obj->getStringAttribute(SUMO_ATTR_ID);
        const bool hasRoute = obj->hasStringAttribute(SUMO_ATTR_ROUTE);
        const bool hasEmbeddedRoute = (getEmbeddedRoute(obj) != nullptr);
        const bool overEdges = obj->hasStringAttribute(SUMO_ATTR_FROM) && obj->hasStringAttribute(SUMO_ATTR_TO);
        const bool overJunctions = obj->hasStringAttribute(SUMO_ATTR_FROM_JUNCTION) && obj->hasStringAttribute(SUMO_ATTR_TO_JUNCTION);
        const bool overTAZs = obj->hasStringAttribute(SUMO_ATTR_FROM_TAZ) && obj->hasStringAttribute(SUMO_ATTR_TO_TAZ);
        if (hasRoute && hasEmbeddedRoute) {
            return writeError(TLF("Could not build % with ID '%' in netedit; Cannot have an external route and an embedded route in the same definition.", toString(tag), id));
        }
        if ((overEdges + overJunctions + overTAZs) > 1) {
            return writeError(TLF("Could not build % with ID '%' in netedit; Cannot have multiple from-to attributes.", toString(tag), id));
        }
        if ((hasRoute + hasEmbeddedRoute + overEdges + overJunctions + overTAZs) > 1) {
            return writeError(TLF("Could not build % with ID '%' in netedit; Cannot have from-to attributes and route attributes in the same definition.", toString(tag), id));
        }
        if ((hasRoute + hasEmbeddedRoute + overEdges + overJunctions + overTAZs) == 0) {
            return writeError(TLF("Could not build % with ID '%' in netedit; Requires either a route or an embedded route or a from-to attribute (Edges, junctions or TAZs).", toString(tag), id));
        }
        return true;
    }
}


bool
CommonHandler::checkPersonPlanParents(CommonXMLStructure::SumoBaseObject* obj) {
    const auto parent = obj->getParentSumoBaseObject();
    if (parent == nullptr) {
        return false;
    } else if (!parent->wasCreated()) {
        return false;
    } else if ((parent->getTag() == SUMO_TAG_PERSON) || (parent->getTag() == SUMO_TAG_PERSONFLOW)) {
        return true;
    } else {
        return false;
    }
}


bool
CommonHandler::checkContainerPlanParents(CommonXMLStructure::SumoBaseObject* obj) {
    const auto parent = obj->getParentSumoBaseObject();
    if (parent == nullptr) {
        return false;
    } else if (!parent->wasCreated()) {
        return false;
    } else if ((parent->getTag() == SUMO_TAG_CONTAINER) || (parent->getTag() == SUMO_TAG_CONTAINERFLOW)) {
        return true;
    } else {
        return false;
    }
}


bool
CommonHandler::checkStopParents(CommonXMLStructure::SumoBaseObject* obj) {
    const auto parent = obj->getParentSumoBaseObject();
    if (parent == nullptr) {
        return false;
    } else if (!parent->wasCreated()) {
        return false;
    } else if ((parent->getTag() == SUMO_TAG_ROUTE) || (parent->getTag() == SUMO_TAG_TRIP) ||
               (parent->getTag() == SUMO_TAG_VEHICLE) || (parent->getTag() == SUMO_TAG_FLOW) ||
               (parent->getTag() == SUMO_TAG_PERSON) || (parent->getTag() == SUMO_TAG_PERSONFLOW) ||
               (parent->getTag() == SUMO_TAG_CONTAINER) || (parent->getTag() == SUMO_TAG_CONTAINERFLOW)) {
        return true;
    } else {
        return false;
    }
}


bool
CommonHandler::checkNegative(const SumoXMLTag tag, const std::string& id, const SumoXMLAttr attribute, const int value, const bool canBeZero) {
    if (canBeZero) {
        if (value < 0) {
            return writeError(TLF("Could not build % with ID '%' in netedit; Attribute % cannot be negative.", toString(tag), id, toString(attribute)));
        } else {
            return true;
        }
    } else {
        if (value <= 0) {
            return writeError(TLF("Could not build % with ID '%' in netedit; Attribute % must be greather than zero.", toString(tag), id, toString(attribute)));
        } else {
            return true;
        }
    }
}


bool
CommonHandler::checkNegative(const SumoXMLTag tag, const std::string& id, const SumoXMLAttr attribute, const double value, const bool canBeZero) {
    if (canBeZero) {
        if (value < 0) {
            return writeError(TLF("Could not build % with ID '%' in netedit; Attribute % cannot be negative (%).", toString(tag), id, toString(attribute), toString(value)));
        } else {
            return true;
        }
    } else {
        if (value <= 0) {
            return writeError(TLF("Could not build % with ID '%' in netedit; Attribute % must be greather than zero (%).", toString(tag), id, toString(attribute), toString(value)));
        } else {
            return true;
        }
    }
}


bool
CommonHandler::checkNegative(const SumoXMLTag tag, const std::string& id, const SumoXMLAttr attribute, const SUMOTime value, const bool canBeZero) {
    if (canBeZero) {
        if (value < 0) {
            return writeError(TLF("Could not build % with ID '%' in netedit; Attribute % cannot be negative (%).", toString(tag), id, toString(attribute), time2string(value)));
        } else {
            return true;
        }
    } else {
        if (value <= 0) {
            return writeError(TLF("Could not build % with ID '%' in netedit; Attribute % must be greather than zero (%).", toString(tag), id, toString(attribute), time2string(value)));
        } else {
            return true;
        }
    }
}


bool
CommonHandler::checkFileName(const SumoXMLTag tag, const std::string& id, const SumoXMLAttr attribute, const std::string& value) {
    if (SUMOXMLDefinitions::isValidFilename(value)) {
        return true;
    } else {
        return writeError(TLF("Could not build % with ID '%' in netedit; % is invalid % ()", toString(tag), id, toString(attribute), value));
    }
}


bool
CommonHandler::checkValidAdditionalID(const SumoXMLTag tag, const std::string& value) {
    if (value.empty()) {
        return writeError(TLF("Could not build %; ID cannot be empty", toString(tag)));
    } else if (!SUMOXMLDefinitions::isValidVehicleID(value)) {
        return writeError(TLF("Could not build % with ID '%' in netedit; ID contains invalid characters.", toString(tag), value));
    } else {
        return true;
    }
}


bool
CommonHandler::checkValidDetectorID(const SumoXMLTag tag, const std::string& value) {
    if (value.empty()) {
        return writeError(TLF("Could not build %; ID cannot be empty", toString(tag)));
    } else if (!SUMOXMLDefinitions::isValidDetectorID(value)) {
        return writeError(TLF("Could not build % with ID '%' in netedit; detector ID contains invalid characters.", toString(tag), value));
    } else {
        return true;
    }
}


bool
CommonHandler::checkValidDemandElementID(const SumoXMLTag tag, const std::string& value) {
    if (value.empty()) {
        return writeError(TLF("Could not build %; ID cannot be empty", toString(tag)));
    } else if (!SUMOXMLDefinitions::isValidVehicleID(value)) {
        return writeError(TLF("Could not build % with ID '%' in netedit; ID contains invalid characters.", toString(tag), value));
    } else {
        return true;
    }
}


void
CommonHandler::writeWarningOverwriting(const SumoXMLTag tag, const std::string& id) {
    WRITE_WARNING(TLF("Overwriting % with ID '%'", toString(tag), id));
}


bool
CommonHandler::writeWarningDuplicated(const SumoXMLTag tag, const std::string& id, const SumoXMLTag checkedTag) {
    WRITE_WARNING(TLF("Could not build % with ID '%' in netedit; Found another % with the same ID.", toString(tag), id, toString(checkedTag)));
    return false;
}


bool
CommonHandler::writeError(const std::string& error) {
    WRITE_ERROR(error);
    myErrorCreatingElement = true;
    return false;
}


bool
CommonHandler::writeErrorInvalidPosition(const SumoXMLTag tag, const std::string& id) {
    return writeError(TLF("Could not build % with ID '%' in netedit; Invalid position over lane.", toString(tag), id));
}


bool
CommonHandler::writeErrorInvalidLanes(const SumoXMLTag tag, const std::string& id) {
    return writeError(TLF("Could not build % with ID '%' in netedit; List of lanes isn't valid.", toString(tag), id));
}


bool
CommonHandler::writeErrorInvalidParent(const SumoXMLTag tag, const std::string& id, const SumoXMLTag parentTag, const std::string& parentID) {
    return writeError(TLF("Could not build % with ID '%' in netedit; % parent with ID '%' doesn't exist.", toString(tag), id, toString(parentTag), parentID));
}


bool
CommonHandler::writeErrorInvalidParent(const SumoXMLTag tag, const SumoXMLTag parentTag, const std::string& parentID) {
    return writeError(TLF("Could not build % in netedit; % parent with ID '%' doesn't exist.", toString(tag), toString(parentTag), parentID));
}


bool
CommonHandler::writeErrorInvalidParent(const SumoXMLTag tag, const SumoXMLTag parentTag) {
    return writeError(TLF("Could not build % in netedit; % parent doesn't exist.", toString(tag), toString(parentTag)));
}

/****************************************************************************/

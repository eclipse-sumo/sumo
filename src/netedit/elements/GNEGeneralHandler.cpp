/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
// General element handler for netedit
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================

#include <utils/xml/XMLSubSys.h>

#include "GNEGeneralHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEGeneralHandler::GNEGeneralHandler(GNENet* net, const std::string& file, const bool allowUndoRedo, const bool overwrite) :
    GeneralHandler(file),
    myAdditionalHandler(net, allowUndoRedo, overwrite),
    myDemandHandler(file, net, allowUndoRedo, overwrite),
    myMeanDataHandler(net, allowUndoRedo, overwrite) {
}


GNEGeneralHandler::~GNEGeneralHandler() {}


bool
GNEGeneralHandler::isErrorCreatingElement() const {
    return (myAdditionalHandler.isErrorCreatingElement() ||
            myDemandHandler.isErrorCreatingElement() ||
            myMeanDataHandler.isErrorCreatingElement());
}


bool
GNEGeneralHandler::isAdditionalFile() const {
    return fileType == TagType::Type::ADDITIONAL;
}


bool
GNEGeneralHandler::isRouteFile() const {
    return fileType == TagType::Type::DEMAND;
}


bool
GNEGeneralHandler::isMeanDataFile() const {
    return fileType == TagType::Type::MEANDATA;
}


void
GNEGeneralHandler::beginTag(SumoXMLTag tag, const SUMOSAXAttributes& attrs) {
    switch (tag) {
        case SUMO_TAG_LOCATION:
            // process in Network handler
            myQueue.push_back(TagType(tag, TagType::Type::NETWORK));
            break;
        case SUMO_TAG_PARAM:
        case SUMO_TAG_INTERVAL:
            if (myQueue.size() > 0) {
                // try to parse additional or demand element depending of last inserted tag
                if (myQueue.back().isAdditional() && myAdditionalHandler.beginParseAttributes(tag, attrs)) {
                    myQueue.push_back(TagType(tag, TagType::Type::ADDITIONAL));
                } else if (myQueue.back().isDemand() && myDemandHandler.beginParseAttributes(tag, attrs)) {
                    myQueue.push_back(TagType(tag, TagType::Type::DEMAND));
                } else {
                    myQueue.push_back(TagType(tag, TagType::Type::NONE));
                }
            } else {
                myQueue.push_back(TagType(tag, TagType::Type::NONE));
            }
            break;
        case SUMO_TAG_FLOW:
            if (myQueue.size() > 0) {
                // try to parse additional or demand element depending of last inserted tag
                if (myQueue.back().isAdditional() && myAdditionalHandler.beginParseAttributes(tag, attrs)) {
                    myQueue.push_back(TagType(tag, TagType::Type::ADDITIONAL));
                } else if (myDemandHandler.beginParseAttributes(tag, attrs)) {
                    myQueue.push_back(TagType(tag, TagType::Type::DEMAND));
                } else {
                    myQueue.push_back(TagType(tag, TagType::Type::NONE));
                }
            } else {
                myQueue.push_back(TagType(tag, TagType::Type::NONE));
            }
            break;
        default:
            // try to parse additional or demand element
            if (myAdditionalHandler.beginParseAttributes(tag, attrs)) {
                myQueue.push_back(TagType(tag, TagType::Type::ADDITIONAL));
            } else if (myDemandHandler.beginParseAttributes(tag, attrs)) {
                myQueue.push_back(TagType(tag, TagType::Type::DEMAND));
            } else if (myMeanDataHandler.beginParseAttributes(tag, attrs)) {
                myQueue.push_back(TagType(tag, TagType::Type::MEANDATA));
            } else {
                myQueue.push_back(TagType(tag, TagType::Type::NONE));
            }
            break;
    }
    // maximum 10 tagTypes
    if (myQueue.size() > 10) {
        myQueue.pop_front();
    }
}


void
GNEGeneralHandler::endTag() {
    // check tagType
    if (myQueue.back().isNetwork()) {
        // currently ignored (will be implemented in the future)
    } else if (myQueue.back().isAdditional()) {
        // end parse additional elements
        myAdditionalHandler.endParseAttributes();
        // mark file as additional
        fileType = TagType::Type::ADDITIONAL;
    } else if (myQueue.back().isDemand()) {
        // end parse demand elements
        myDemandHandler.endParseAttributes();
        // mark file as demand
        fileType = TagType::Type::DEMAND;
    } else if (myQueue.back().isMeanData()) {
        // end parse meanData elements
        myMeanDataHandler.endParseAttributes();
        // mark file as mean data
        fileType = TagType::Type::MEANDATA;
    } else {
        // mark file as demand
        fileType = TagType::Type::NONE;
    }
}


GNEGeneralHandler::TagType::TagType(SumoXMLTag tag_, GNEGeneralHandler::TagType::Type type) :
    tag(tag_),
    myType(type) {
}


bool
GNEGeneralHandler::TagType::isNetwork() const {
    return (myType == Type::NETWORK);
}


bool
GNEGeneralHandler::TagType::isAdditional() const {
    return (myType == Type::ADDITIONAL);
}


bool
GNEGeneralHandler::TagType::isDemand() const {
    return (myType == Type::DEMAND);
}


bool
GNEGeneralHandler::TagType::isData() const {
    return (myType == Type::DATA);
}


bool
GNEGeneralHandler::TagType::isMeanData() const {
    return (myType == Type::MEANDATA);
}

/****************************************************************************/

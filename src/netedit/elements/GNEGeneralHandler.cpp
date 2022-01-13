/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
// General element handler for NETEDIT
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================

#include <utils/xml/XMLSubSys.h>

#include "GNEGeneralHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEGeneralHandler::GNEGeneralHandler(GNENet* net, const std::string& file, const bool allowUndoRedo) :
    GeneralHandler(file),
    myAdditionalHandler(net, allowUndoRedo),
    myDemandHandler(file, net, allowUndoRedo) {
}


GNEGeneralHandler::~GNEGeneralHandler() {}


void
GNEGeneralHandler::beginTag(SumoXMLTag tag, const SUMOSAXAttributes& attrs) {
    switch (tag) {
        case SUMO_TAG_PARAM:
        case SUMO_TAG_INTERVAL:
            if (myQueue.size() > 0) {
                // try to parse additional or demand element depending of last inserted tag
                if (myQueue.back().additional && myAdditionalHandler.beginParseAttributes(tag, attrs)) {
                    myQueue.push_back(TagType(tag, true, false));
                } else if (myQueue.back().demand && myDemandHandler.beginParseAttributes(tag, attrs)) {
                    myQueue.push_back(TagType(tag, false, true));
                } else {
                    myQueue.push_back(TagType(tag, false, false));
                }
            } else {
                myQueue.push_back(TagType(tag, false, false));
            }
            break;
        case SUMO_TAG_FLOW:
            if (myQueue.size() > 0) {
                // try to parse additional or demand element depending of last inserted tag
                if (myQueue.back().additional && myAdditionalHandler.beginParseAttributes(tag, attrs)) {
                    myQueue.push_back(TagType(tag, true, false));
                } else if (myDemandHandler.beginParseAttributes(tag, attrs)) {
                    myQueue.push_back(TagType(tag, false, true));
                } else {
                    myQueue.push_back(TagType(tag, false, false));
                }
            } else {
                myQueue.push_back(TagType(tag, false, false));
            }
            break;
        default:
            // try to parse additional or demand element
            if (myAdditionalHandler.beginParseAttributes(tag, attrs)) {
                myQueue.push_back(TagType(tag, true, false));
            } else if (myDemandHandler.beginParseAttributes(tag, attrs)) {
                myQueue.push_back(TagType(tag, false, true));
            } else {
                myQueue.push_back(TagType(tag, false, false));
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
    if (myQueue.back().additional) {
        // end parse additional elements
        myAdditionalHandler.endParseAttributes();
    } else if (myQueue.back().demand) {
        // end parse demand elements
        myDemandHandler.endParseAttributes();
    } else {
        WRITE_ERROR(toString(myQueue.back().tag) + " cannot be processed either with additional handler nor with demand handler");
    }
}


GNEGeneralHandler::TagType::TagType(SumoXMLTag tag_, const bool additional_, const bool demand_) :
    tag(tag_),
    additional(additional_),
    demand(demand_) {
}

/****************************************************************************/

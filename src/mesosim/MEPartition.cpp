/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    MEPartition.cpp
/// @author  Josh
/// @date    2026-11-26
///
// Load edge groups for mesoscopic simulation
/****************************************************************************/
#include <config.h>

#include <memory>
#include <microsim/MSEdge.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOSAXReader.h>
#include <utils/xml/XMLSubSys.h>
#include "MEPartition.h"

namespace {

class PartitionHandler : public SUMOSAXHandler {
public:
    PartitionHandler(const std::string& file, MEPartition::EdgeGroups& groups)
        : SUMOSAXHandler(file), myGroups(groups) {}

private:
    void myStartElement(int element, const SUMOSAXAttributes& attrs) override {
        switch (element) {
            case SUMO_TAG_TAZ:
                if (!myGroup.empty()) {
                    throw ProcessError(TLF("Nested 'taz' in mesoscopic partition file '%'.", getFileName()));
                }
                if (attrs.hasAttribute(SUMO_ATTR_ID)) {
                    myGroup = attrs.getString(SUMO_ATTR_ID);
                }
                if (myGroup.empty()) {
                    throw ProcessError(TLF("Missing 'taz' id in mesoscopic partition file '%'.", getFileName()));
                }
                if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
                    for (const std::string& id : StringTokenizer(attrs.getString(SUMO_ATTR_EDGES)).getVector()) {
                        addEdge(id);
                    }
                }
                break;
            case SUMO_TAG_TAZSOURCE:
            case SUMO_TAG_TAZSINK: {
                if (myGroup.empty()) {
                    throw ProcessError(TLF("'%' must be inside a 'taz' in mesoscopic partition file '%'.",
                                           toString(static_cast<SumoXMLTag>(element)), getFileName()));
                }
                const std::string id = attrs.hasAttribute(SUMO_ATTR_ID) ? attrs.getString(SUMO_ATTR_ID) : "";
                if (id.empty()) {
                    throw ProcessError(TLF("Missing edge id in mesoscopic partition '%' in file '%'.",
                                           myGroup, getFileName()));
                }
                addEdge(id);
                break;
            }
            default:
                break;
        }
    }

    void myEndElement(int element) override {
        if (element == SUMO_TAG_TAZ) {
            myGroup.clear();
        }
    }

    void addEdge(const std::string& id) {
        const MSEdge* edge = MSEdge::dictionary(id);
        if (edge == nullptr) {
            throw ProcessError(TLF("Unknown edge '%' in mesoscopic partition '%' in file '%'.",
                                   id, myGroup, getFileName()));
        }
        const auto [membership, inserted] = myGroups.emplace(edge, myGroup);
        if (!inserted && membership->second != myGroup) {
            throw ProcessError(TLF("Edge '%' belongs to mesoscopic partitions '%' and '%' in file '%'.",
                                   id, membership->second, myGroup, getFileName()));
        }
    }

    MEPartition::EdgeGroups& myGroups;
    std::string myGroup;
};

}


MEPartition::EdgeGroups
MEPartition::load(const std::string& file) {
    EdgeGroups groups;
    PartitionHandler handler(file, groups);
    std::unique_ptr<SUMOSAXReader> reader(XMLSubSys::getSAXReader(handler));
    reader->parse(file);
    return groups;
}

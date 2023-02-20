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
/// @file    NIXMLTypesHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    Tue, 20 Nov 2001
///
// Importer for edge type information stored in XML
/****************************************************************************/
#include <config.h>

#include <string>
#include <iostream>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/StringUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/SUMOVehicleClass.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBTypeCont.h>
#include "NIXMLTypesHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================
NIXMLTypesHandler::NIXMLTypesHandler(NBTypeCont& tc)
    : SUMOSAXHandler("xml-types - file"),
      myTypeCont(tc) {}


NIXMLTypesHandler::~NIXMLTypesHandler() {}


void
NIXMLTypesHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    switch (element) {
        case SUMO_TAG_TYPE: {
            bool ok = true;
            // get the id, report a warning if not given or empty...
            myCurrentTypeID = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
            const char* const id = myCurrentTypeID.c_str();
            const std::string defType = myTypeCont.knows(myCurrentTypeID) ? myCurrentTypeID : "";
            const int priority = attrs.getOpt<int>(SUMO_ATTR_PRIORITY, id, ok, myTypeCont.getEdgeTypePriority(defType));
            const int numLanes = attrs.getOpt<int>(SUMO_ATTR_NUMLANES, id, ok, myTypeCont.getEdgeTypeNumLanes(defType));
            const double speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, id, ok, myTypeCont.getEdgeTypeSpeed(defType));
            const std::string allowS = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, id, ok, "");
            const std::string disallowS = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, id, ok, "");
            const std::string spreadTypeS = attrs.getOpt<std::string>(SUMO_ATTR_SPREADTYPE, id, ok, "right");
            const bool oneway = attrs.getOpt<bool>(SUMO_ATTR_ONEWAY, id, ok, myTypeCont.getEdgeTypeIsOneWay(defType));
            const bool discard = attrs.getOpt<bool>(SUMO_ATTR_DISCARD, id, ok, false);
            const double width = attrs.getOpt<double>(SUMO_ATTR_WIDTH, id, ok, myTypeCont.getEdgeTypeWidth(defType));
            const double maxWidth = attrs.getOpt<double>(SUMO_ATTR_MAXWIDTH, id, ok, myTypeCont.getEdgeTypeMaxWidth(defType));
            const double minWidth = attrs.getOpt<double>(SUMO_ATTR_MINWIDTH, id, ok, myTypeCont.getEdgeTypeMinWidth(defType));
            const double widthResolution = attrs.getOpt<double>(SUMO_ATTR_WIDTHRESOLUTION, id, ok, myTypeCont.getEdgeTypeWidthResolution(defType));
            const double sidewalkWidth = attrs.getOpt<double>(SUMO_ATTR_SIDEWALKWIDTH, id, ok, myTypeCont.getEdgeTypeSidewalkWidth(defType));
            const double bikeLaneWidth = attrs.getOpt<double>(SUMO_ATTR_BIKELANEWIDTH, id, ok, myTypeCont.getEdgeTypeBikeLaneWidth(defType));
            // continue if parsing parameter was ok
            if (ok) {
                // build the type
                SVCPermissions permissions = myTypeCont.getEdgeTypePermissions(defType);
                if (allowS != "" || disallowS != "") {
                    permissions = parseVehicleClasses(allowS, disallowS);
                }
                // get spreadType
                LaneSpreadFunction spreadType = LaneSpreadFunction::RIGHT;
                // check if spreadType is valid
                if (SUMOXMLDefinitions::LaneSpreadFunctions.hasString(spreadTypeS)) {
                    spreadType = SUMOXMLDefinitions::LaneSpreadFunctions.get(spreadTypeS);
                } else {
                    WRITE_ERRORF(TL("Invalid lane spread type '%'. Using default 'right'"), spreadTypeS);
                }
                // insert edgeType in container
                myTypeCont.insertEdgeType(myCurrentTypeID, numLanes, speed, priority, permissions, spreadType, width,
                                          oneway, sidewalkWidth, bikeLaneWidth, widthResolution, maxWidth, minWidth);
                // check if mark edgeType as discard
                if (discard) {
                    myTypeCont.markEdgeTypeAsToDiscard(myCurrentTypeID);
                }
                // mark attributes as set
                SumoXMLAttr myAttrs[] = {SUMO_ATTR_PRIORITY, SUMO_ATTR_NUMLANES, SUMO_ATTR_SPEED, SUMO_ATTR_ALLOW,
                                         SUMO_ATTR_DISALLOW, SUMO_ATTR_SPREADTYPE, SUMO_ATTR_ONEWAY, SUMO_ATTR_DISCARD,
                                         SUMO_ATTR_WIDTH, SUMO_ATTR_SIDEWALKWIDTH, SUMO_ATTR_BIKELANEWIDTH
                                        };
                for (const auto& attr : myAttrs) {
                    if (attrs.hasAttribute(attr)) {
                        myTypeCont.markEdgeTypeAsSet(myCurrentTypeID, attr);
                    }
                }
            }
            break;
        }
        case SUMO_TAG_LANETYPE: {
            bool ok = true;
            // use id of last inserted edge
            const char* const edgeTypeId = myCurrentTypeID.c_str();
            const int index = attrs.get<int>(SUMO_ATTR_INDEX, edgeTypeId, ok);
            const std::string defType = myTypeCont.knows(myCurrentTypeID) ? myCurrentTypeID : "";
            if (index >= myTypeCont.getEdgeTypeNumLanes(defType)) {
                WRITE_ERRORF(TL("Invalid lane index % for edge type '%' with % lanes"), toString(index), defType, toString(myTypeCont.getEdgeTypeNumLanes(defType)));
                ok = false;
            }
            const double speed = attrs.getOpt<double>(SUMO_ATTR_SPEED, edgeTypeId, ok, myTypeCont.getEdgeTypeSpeed(edgeTypeId));
            const std::string allowS = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, edgeTypeId, ok, "");
            const std::string disallowS = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, edgeTypeId, ok, "");
            const double width = attrs.getOpt<double>(SUMO_ATTR_WIDTH, edgeTypeId, ok, myTypeCont.getEdgeTypeWidth(defType));
            // continue if parsing parameter was ok
            if (ok) {
                // build the type
                SVCPermissions permissions = myTypeCont.getEdgeTypePermissions(defType);
                if (allowS != "" || disallowS != "") {
                    permissions = parseVehicleClasses(allowS, disallowS);
                }

                // insert laneType in container
                myTypeCont.insertLaneType(myCurrentTypeID, index, speed, permissions, width, {});
                // mark attributes as set
                SumoXMLAttr myAttrs[] = {SUMO_ATTR_SPEED, SUMO_ATTR_ALLOW, SUMO_ATTR_DISALLOW, SUMO_ATTR_WIDTH};
                for (const auto& attr : myAttrs) {
                    if (attrs.hasAttribute(attr)) {
                        myTypeCont.markLaneTypeAsSet(myCurrentTypeID, index, attr);
                    }
                }
            }
            break;
        }
        case SUMO_TAG_RESTRICTION: {
            bool ok = true;
            const SUMOVehicleClass svc = getVehicleClassID(attrs.get<std::string>(SUMO_ATTR_VCLASS, myCurrentTypeID.c_str(), ok));
            const double speed = attrs.get<double>(SUMO_ATTR_SPEED, myCurrentTypeID.c_str(), ok);
            if (ok) {
                myTypeCont.addEdgeTypeRestriction(myCurrentTypeID, svc, speed);
            }
            break;
        }
        default:
            break;
    }
}


/****************************************************************************/

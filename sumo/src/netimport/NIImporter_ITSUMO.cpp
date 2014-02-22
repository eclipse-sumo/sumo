/****************************************************************************/
/// @file    NIImporter_ITSUMO.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    2011-09-16
/// @version $Id$
///
// Importer for networks stored in ITSUMO format
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
#include <set>
#include <functional>
#include <sstream>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/StringUtils.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/TplConvert.h>
#include <utils/xml/XMLSubSys.h>
#include "NILoader.h"
#include "NIImporter_ITSUMO.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



// ===========================================================================
// static variables
// ===========================================================================
StringBijection<int>::Entry NIImporter_ITSUMO::itsumoTags[] = {
    { "simulation",             NIImporter_ITSUMO::ITSUMO_TAG_SIMULATION },
    { "network_id",             NIImporter_ITSUMO::ITSUMO_TAG_NETWORK_ID },
    { "network_name",           NIImporter_ITSUMO::ITSUMO_TAG_NETWORK_NAME },
    { "nodes",                  NIImporter_ITSUMO::ITSUMO_TAG_NODES },
    { "node",                   NIImporter_ITSUMO::ITSUMO_TAG_NODE },
    { "node_id",                NIImporter_ITSUMO::ITSUMO_TAG_NODE_ID },
    { "node_name",              NIImporter_ITSUMO::ITSUMO_TAG_NODE_NAME },
    { "x_coord",                NIImporter_ITSUMO::ITSUMO_TAG_X_COORD },
    { "y_coord",                NIImporter_ITSUMO::ITSUMO_TAG_Y_COORD },
    { "sources",                NIImporter_ITSUMO::ITSUMO_TAG_SOURCES },
    { "sinks",                  NIImporter_ITSUMO::ITSUMO_TAG_SINKS },
    { "traffic_lights",         NIImporter_ITSUMO::ITSUMO_TAG_TRAFFIC_LIGHTS },
    { "streets",                NIImporter_ITSUMO::ITSUMO_TAG_STREETS },
    { "street",                 NIImporter_ITSUMO::ITSUMO_TAG_STREET },
    { "street_id",              NIImporter_ITSUMO::ITSUMO_TAG_STREET_ID },
    { "street_name",            NIImporter_ITSUMO::ITSUMO_TAG_STREET_NAME },
    { "sections",               NIImporter_ITSUMO::ITSUMO_TAG_SECTIONS },
    { "section",                NIImporter_ITSUMO::ITSUMO_TAG_SECTION },
    { "section_id",             NIImporter_ITSUMO::ITSUMO_TAG_SECTION_ID },
    { "section_name",           NIImporter_ITSUMO::ITSUMO_TAG_SECTION_NAME },
    { "is_preferencial",        NIImporter_ITSUMO::ITSUMO_TAG_IS_PREFERENCIAL },
    { "delimiting_node",        NIImporter_ITSUMO::ITSUMO_TAG_DELIMITING_NODE },
    { "lanesets",               NIImporter_ITSUMO::ITSUMO_TAG_LANESETS },
    { "laneset",                NIImporter_ITSUMO::ITSUMO_TAG_LANESET },
    { "laneset_id",             NIImporter_ITSUMO::ITSUMO_TAG_LANESET_ID },
    { "laneset_position",       NIImporter_ITSUMO::ITSUMO_TAG_LANESET_POSITION },
    { "start_node",             NIImporter_ITSUMO::ITSUMO_TAG_START_NODE },
    { "end_node",               NIImporter_ITSUMO::ITSUMO_TAG_END_NODE },
    { "turning_probabilities",  NIImporter_ITSUMO::ITSUMO_TAG_TURNING_PROBABILITIES },
    { "direction",              NIImporter_ITSUMO::ITSUMO_TAG_DIRECTION },
    { "destination_laneset",    NIImporter_ITSUMO::ITSUMO_TAG_DESTINATION_LANESET },
    { "probability",            NIImporter_ITSUMO::ITSUMO_TAG_PROBABILITY },
    { "lanes",                  NIImporter_ITSUMO::ITSUMO_TAG_LANES },
    { "lane",                   NIImporter_ITSUMO::ITSUMO_TAG_LANE },
    { "lane_id",                NIImporter_ITSUMO::ITSUMO_TAG_LANE_ID },
    { "lane_position",          NIImporter_ITSUMO::ITSUMO_TAG_LANE_POSITION },
    { "maximum_speed",          NIImporter_ITSUMO::ITSUMO_TAG_MAXIMUM_SPEED },
    { "deceleration_prob",      NIImporter_ITSUMO::ITSUMO_TAG_DECELERATION_PROB },
    { "",                       NIImporter_ITSUMO::ITSUMO_TAG_NOTHING }
};


StringBijection<int>::Entry NIImporter_ITSUMO::itsumoAttrs[] = {
    { "",               NIImporter_ITSUMO::ITSUMO_ATTR_NOTHING }
};


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods
// ---------------------------------------------------------------------------
void
NIImporter_ITSUMO::loadNetwork(const OptionsCont& oc, NBNetBuilder& nb) {
    // check whether the option is set (properly)
    if (!oc.isSet("itsumo-files")) {
        return;
    }
    /* Parse file(s)
     * Each file is parsed twice: first for nodes, second for edges. */
    std::vector<std::string> files = oc.getStringVector("itsumo-files");
    // load nodes, first
    Handler Handler(nb);
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        // nodes
        if (!FileHelpers::exists(*file)) {
            WRITE_ERROR("Could not open itsumo-file '" + *file + "'.");
            return;
        }
        Handler.setFileName(*file);
        PROGRESS_BEGIN_MESSAGE("Parsing nodes from itsumo-file '" + *file + "'");
        if (!XMLSubSys::runParser(Handler, *file)) {
            return;
        }
        PROGRESS_DONE_MESSAGE();
    }
}


// ---------------------------------------------------------------------------
// definitions of NIImporter_ITSUMO::Handler-methods
// ---------------------------------------------------------------------------
NIImporter_ITSUMO::Handler::Handler(NBNetBuilder& toFill)
    : GenericSAXHandler(itsumoTags, ITSUMO_TAG_NOTHING, itsumoAttrs, ITSUMO_ATTR_NOTHING, "itsumo - file"), myNetBuilder(toFill) {
}


NIImporter_ITSUMO::Handler::~Handler() {}


void
NIImporter_ITSUMO::Handler::myStartElement(int element, const SUMOSAXAttributes& /* attrs */) {
    switch (element) {
        case ITSUMO_TAG_NODE:
            myParameter.clear();
            break;
        case ITSUMO_TAG_LANESET:
            myParameter.clear();
            break;
        default:
            break;
    }
}


void
NIImporter_ITSUMO::Handler::myCharacters(int element, const std::string& chars) {
    std::string mc = StringUtils::prune(chars);
    switch (element) {
            // node parsing
        case ITSUMO_TAG_NODE_ID:
            myParameter["id"] = mc;
            break;
        case ITSUMO_TAG_NODE_NAME:
            myParameter["name"] = mc;
            break;
        case ITSUMO_TAG_X_COORD:
            myParameter["x"] = mc;
            break;
        case ITSUMO_TAG_Y_COORD:
            myParameter["y"] = mc;
            break;
            // section parsing
        case ITSUMO_TAG_SECTION_ID:
            myParameter["sectionID"] = mc;
            break;
            // laneset parsing
        case ITSUMO_TAG_LANESET_ID:
            myParameter["lanesetID"] = mc;
            break;
        case ITSUMO_TAG_LANESET_POSITION:
            myParameter["pos"] = mc;
            break;
        case ITSUMO_TAG_START_NODE:
            myParameter["from"] = mc;
            break;
        case ITSUMO_TAG_END_NODE:
            myParameter["to"] = mc;
            break;
            // lane parsing
        case ITSUMO_TAG_LANE_ID:
            myParameter["laneID"] = mc;
            break;
        case ITSUMO_TAG_LANE_POSITION:
            myParameter["i"] = mc;
            break;
        case ITSUMO_TAG_MAXIMUM_SPEED:
            myParameter["v"] = mc;
            break;
        default:
            break;
    }
}


void
NIImporter_ITSUMO::Handler::myEndElement(int element) {
    switch (element) {
        case ITSUMO_TAG_SIMULATION: {
            for (std::vector<Section*>::iterator i = mySections.begin(); i != mySections.end(); ++i) {
                for (std::vector<LaneSet*>::iterator j = (*i)->laneSets.begin(); j != (*i)->laneSets.end(); ++j) {
                    LaneSet* ls = (*j);
                    NBEdge* edge = new NBEdge(ls->id, ls->from, ls->to, "", ls->v, (unsigned int)ls->lanes.size(), -1, NBEdge::UNSPECIFIED_WIDTH, NBEdge::UNSPECIFIED_OFFSET);
                    if (!myNetBuilder.getEdgeCont().insert(edge)) {
                        delete edge;
                        WRITE_ERROR("Could not add edge '" + ls->id + "'. Probably declared twice.");
                    }
                    delete ls;
                }
                delete *i;
            }
        }
        break;
        case ITSUMO_TAG_NODE: {
            try {
                std::string id = myParameter["id"];
                SUMOReal x = TplConvert::_2SUMOReal(myParameter["x"].c_str());
                SUMOReal y = TplConvert::_2SUMOReal(myParameter["y"].c_str());
                Position pos(x, y);
                if (!NBNetBuilder::transformCoordinates(pos)) {
                    WRITE_ERROR("Unable to project coordinates for node '" + id + "'.");
                }
                NBNode* node = new NBNode(id, pos);
                if (!myNetBuilder.getNodeCont().insert(node)) {
                    delete node;
                    WRITE_ERROR("Could not add node '" + id + "'. Probably declared twice.");
                }
            } catch (NumberFormatException&) {
                WRITE_ERROR("Not numeric position information for node '" + myParameter["id"] + "'.");
            } catch (EmptyData&) {
                WRITE_ERROR("Missing data in node '" + myParameter["id"] + "'.");
            }
        }
        break;
        case ITSUMO_TAG_SECTION: {
            mySections.push_back(new Section(myParameter["sectionID"], myCurrentLaneSets));
            myCurrentLaneSets.clear();
        }
        break;
        case ITSUMO_TAG_LANESET: {
            try {
                std::string id = myParameter["lanesetID"];
                int i = TplConvert::_2int(myParameter["i"].c_str());
                std::string fromID = myParameter["from"];
                std::string toID = myParameter["to"];
                NBNode* from = myNetBuilder.getNodeCont().retrieve(fromID);
                NBNode* to = myNetBuilder.getNodeCont().retrieve(toID);
                if (from == 0 || to == 0) {
                    WRITE_ERROR("Missing node in laneset '" + myParameter["lanesetID"] + "'.");
                } else {
                    if (myLaneSets.find(id) != myLaneSets.end()) {
                        WRITE_ERROR("Fond laneset-id '" + id + "' twice.");
                    } else {
                        SUMOReal vSum = 0;
                        for (std::vector<Lane>::iterator j = myCurrentLanes.begin(); j != myCurrentLanes.end(); ++j) {
                            vSum += (*j).v;
                        }
                        vSum /= (SUMOReal) myCurrentLanes.size();
                        LaneSet* ls = new LaneSet(id, myCurrentLanes, vSum, i, from, to);
                        myLaneSets[id] = ls;
                        myCurrentLaneSets.push_back(ls);
                        myCurrentLanes.clear();
                    }
                }
            } catch (NumberFormatException&) {
                WRITE_ERROR("Not numeric value in laneset '" + myParameter["lanesetID"] + "'.");
            } catch (EmptyData&) {
                WRITE_ERROR("Missing data in laneset '" + myParameter["lanesetID"] + "'.");
            }
        }
        break;
        case ITSUMO_TAG_LANE: {
            try {
                std::string id = myParameter["laneID"];
                int i = TplConvert::_2int(myParameter["i"].c_str());
                SUMOReal v = TplConvert::_2SUMOReal(myParameter["v"].c_str());
                myCurrentLanes.push_back(Lane(id, (unsigned int) i, v));
            } catch (NumberFormatException&) {
                WRITE_ERROR("Not numeric value in lane '" + myParameter["laneID"] + "'.");
            } catch (EmptyData&) {
                WRITE_ERROR("Missing data in lane '" + myParameter["laneID"] + "'.");
            }
        }
        break;
        default:
            break;
    }
}


/****************************************************************************/


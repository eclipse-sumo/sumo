/****************************************************************************/
/// @file    PCLoaderOSM.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Christoph Sommer
/// @author  Michael Behrisch
/// @date    Wed, 19.11.2008
/// @version $Id$
///
// A reader of pois and polygons stored in OSM-format
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

#include <string>
#include <map>
#include <fstream>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/StdDefs.h>
#include <polyconvert/PCPolyContainer.h>
#include "PCLoaderOSM.h"
#include <utils/common/RGBColor.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/common/FileHelpers.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static interface
// ---------------------------------------------------------------------------
void
PCLoaderOSM::loadIfSet(OptionsCont& oc, PCPolyContainer& toFill,
                       PCTypeMap& tm) {
    if (!oc.isSet("osm-files")) {
        return;
    }
    // parse file(s)
    std::vector<std::string> files = oc.getStringVector("osm-files");
    // load nodes, first
    std::map<SUMOLong, PCOSMNode*> nodes;
    NodesHandler nodesHandler(nodes);
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        // nodes
        if (!FileHelpers::exists(*file)) {
            WRITE_ERROR("Could not open osm-file '" + *file + "'.");
            return;
        }
        PROGRESS_BEGIN_MESSAGE("Parsing nodes from osm-file '" + *file + "'");
        if (!XMLSubSys::runParser(nodesHandler, *file)) {
            throw ProcessError();
        }
        PROGRESS_DONE_MESSAGE();
    }
    // load edges, then
    std::map<std::string, PCOSMEdge*> edges;
    EdgesHandler edgesHandler(nodes, edges);
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        // edges
        PROGRESS_BEGIN_MESSAGE("Parsing edges from osm-file '" + *file + "'");
        XMLSubSys::runParser(edgesHandler, *file);
        PROGRESS_DONE_MESSAGE();
    }
    // build all
    RGBColor c = RGBColor::parseColor(oc.getString("color"));
    // instatiate polygons
    for (std::map<std::string, PCOSMEdge*>::iterator i = edges.begin(); i != edges.end(); ++i) {
        PCOSMEdge* e = (*i).second;
        if (!e->myIsAdditional) {
            continue;
        }
        // compute shape
        PositionVector vec;
        for (std::vector<SUMOLong>::iterator j = e->myCurrentNodes.begin(); j != e->myCurrentNodes.end(); ++j) {
            PCOSMNode* n = nodes.find(*j)->second;
            Position pos(n->lon, n->lat);
            if (!GeoConvHelper::getProcessing().x2cartesian(pos)) {
                WRITE_WARNING("Unable to project coordinates for polygon '" + e->id + "'.");
            }
            vec.push_back_noDoublePos(pos);
        }
        // set type etc.
        std::string name = oc.getBool("osm.use-name") && e->name != "" ? e->name : e->id;
        std::string type;
        RGBColor color;
        bool fill = vec.getBegin() == vec.getEnd();
        bool discard = oc.getBool("discard");
        int layer = oc.getInt("layer");
        if (tm.has(e->myType)) {
            const PCTypeMap::TypeDef& def = tm.get(e->myType);
            name = def.prefix + name;
            type = def.id;
            color = RGBColor::parseColor(def.color);
            fill = fill && def.allowFill;
            discard = def.discard;
            layer = def.layer;
        } else if (e->myType.find(".") != std::string::npos && tm.has(e->myType.substr(0, e->myType.find(".")))) {
            const PCTypeMap::TypeDef& def = tm.get(e->myType.substr(0, e->myType.find(".")));
            name = def.prefix + name;
            type = def.id;
            color = RGBColor::parseColor(def.color);
            fill = fill && def.allowFill;
            discard = def.discard;
            layer = def.layer;
        } else {
            name = oc.getString("prefix") + name;
            type = oc.getString("type");
            color = c;
        }
        if (!discard) {
            if (oc.getBool("osm.keep-full-type")) {
                type = e->myType;
            }
            name = StringUtils::escapeXML(name);
            type = StringUtils::escapeXML(type);
            Polygon* poly = new Polygon(name, type, color, vec, fill, (SUMOReal)layer);
            if (!toFill.insert(name, poly, layer)) {
                WRITE_ERROR("Polygon '" + name + "' could not be added.");
                delete poly;
            }
        }
    }
    // instantiate pois
    for (std::map<SUMOLong, PCOSMNode*>::iterator i = nodes.begin(); i != nodes.end(); ++i) {
        PCOSMNode* n = (*i).second;
        if (!n->myIsAdditional) {
            continue;
        }

        // patch the values
        bool discard = oc.getBool("discard");
        int layer = oc.getInt("layer");
        std::string name = toString(n->id);
        std::string type;
        RGBColor color;
        if (tm.has(n->myType)) {
            const PCTypeMap::TypeDef& def = tm.get(n->myType);
            name = def.prefix + name;
            type = def.id;
            color = RGBColor::parseColor(def.color);
            discard = def.discard;
            layer = def.layer;
        } else if (type.find(".") != std::string::npos && tm.has(type.substr(0, type.find(".")))) {
            const PCTypeMap::TypeDef& def = tm.get(type.substr(0, type.find(".")));
            name = def.prefix + name;
            type = def.id;
            color = RGBColor::parseColor(def.color);
            discard = def.discard;
            layer = def.layer;
        } else {
            name = oc.getString("prefix") + name;
            type = oc.getString("type");
            color = c;
        }
        if (!discard) {
            if (oc.getBool("osm.keep-full-type")) {
                type = n->myType;
            }
            bool ignorePrunning = false;
            if (OptionsCont::getOptions().isInStringVector("prune.keep-list", name)) {
                ignorePrunning = true;
            }
            Position pos(n->lon, n->lat);
            if (!GeoConvHelper::getProcessing().x2cartesian(pos)) {
                WRITE_WARNING("Unable to project coordinates for POI '" + name + "'.");
            }
            name = StringUtils::escapeXML(name);
            type = StringUtils::escapeXML(type);
            PointOfInterest* poi = new PointOfInterest(name, type, color, pos, (SUMOReal)layer);
            if (!toFill.insert(name, poi, layer, ignorePrunning)) {
                WRITE_ERROR("POI '" + name + "' could not be added.");
                delete poi;
            }
        }
    }


    // delete nodes
    for (std::map<SUMOLong, PCOSMNode*>::const_iterator i = nodes.begin(); i != nodes.end(); ++i) {
        delete(*i).second;
    }
    // delete edges
    for (std::map<std::string, PCOSMEdge*>::iterator i = edges.begin(); i != edges.end(); ++i) {
        delete(*i).second;
    }
}



// ---------------------------------------------------------------------------
// definitions of PCLoaderOSM::NodesHandler-methods
// ---------------------------------------------------------------------------
PCLoaderOSM::NodesHandler::NodesHandler(std::map<SUMOLong, PCOSMNode*>& toFill)
    : SUMOSAXHandler("osm - file"), myToFill(toFill), myLastNodeID(-1) {}


PCLoaderOSM::NodesHandler::~NodesHandler() {}


void
PCLoaderOSM::NodesHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    myParentElements.push_back(element);
    if (element == SUMO_TAG_NODE) {
        bool ok = true;
        SUMOLong id = attrs.getLongReporting(SUMO_ATTR_ID, 0, ok);
        if (!ok) {
            return;
        }
        myLastNodeID = -1;
        if (myToFill.find(id) == myToFill.end()) {
            myLastNodeID = id;
            // assume we are loading multiple files...
            //  ... so we won't report duplicate nodes
            PCOSMNode* toAdd = new PCOSMNode();
            toAdd->id = id;
            toAdd->myIsAdditional = false;
            bool ok = true;
            toAdd->lon = attrs.getSUMORealReporting(SUMO_ATTR_LON, toString(id).c_str(), ok);
            toAdd->lat = attrs.getSUMORealReporting(SUMO_ATTR_LAT, toString(id).c_str(), ok);
            if (!ok) {
                delete toAdd;
                return;
            }
            myToFill[toAdd->id] = toAdd;
        }
    }
    if (element == SUMO_TAG_TAG && myParentElements.size() > 2 && myParentElements[myParentElements.size() - 2] == SUMO_TAG_NODE) {
        bool ok = true;
        std::string key = attrs.getStringReporting(SUMO_ATTR_K, toString(myLastNodeID).c_str(), ok);
        std::string value = attrs.getStringReporting(SUMO_ATTR_V, toString(myLastNodeID).c_str(), ok, false);
        if (!ok) {
            return;
        }
        if (key == "waterway" || key == "aeroway" || key == "aerialway" || key == "power" || key == "man_made" || key == "building"
                || key == "leisure" || key == "amenity" || key == "shop" || key == "tourism" || key == "historic" || key == "landuse"
                || key == "natural" || key == "military" || key == "boundary" || key == "sport" || key == "polygon") {
            if (myLastNodeID >= 0) {
                myToFill[myLastNodeID]->myType = key + "." + value;
                myToFill[myLastNodeID]->myIsAdditional = true;
            }
        }
        if (key == "name" && myLastNodeID != -1) {
            myToFill[myLastNodeID]->myType = key + "." + value;
        }
    }
}


void
PCLoaderOSM::NodesHandler::myEndElement(int element) {
    if (element == SUMO_TAG_NODE) {
        myLastNodeID = -1;
    }
    myParentElements.pop_back();
}


// ---------------------------------------------------------------------------
// definitions of PCLoaderOSM::EdgesHandler-methods
// ---------------------------------------------------------------------------
PCLoaderOSM::EdgesHandler::EdgesHandler(
    const std::map<SUMOLong, PCOSMNode*>& osmNodes,
    std::map<std::string, PCOSMEdge*>& toFill)
    : SUMOSAXHandler("osm - file"),
      myOSMNodes(osmNodes), myEdgeMap(toFill) {
}


PCLoaderOSM::EdgesHandler::~EdgesHandler() {
}


void
PCLoaderOSM::EdgesHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    myParentElements.push_back(element);
    // parse "way" elements
    if (element == SUMO_TAG_WAY) {
        bool ok = true;
        std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
        if (!ok) {
            return;
        }
        myCurrentEdge = new PCOSMEdge();
        myCurrentEdge->id = id;
        myCurrentEdge->myIsAdditional = false;
        myCurrentEdge->myIsClosed = false;
    }
    // parse "nd" (node) elements
    if (element == SUMO_TAG_ND) {
        bool ok = true;
        SUMOLong ref = attrs.getLongReporting(SUMO_ATTR_REF, 0, ok);
        if (ok) {
            if (myOSMNodes.find(ref) == myOSMNodes.end()) {
                WRITE_WARNING("The referenced geometry information (ref='" + toString(ref) + "') is not known");
                return;
            }
            myCurrentEdge->myCurrentNodes.push_back(ref);
        }
    }
    // parse values
    if (element == SUMO_TAG_TAG && myParentElements.size() > 2 && myParentElements[myParentElements.size() - 2] == SUMO_TAG_WAY) {
        bool ok = true;
        std::string key = attrs.getStringReporting(SUMO_ATTR_K, toString(myCurrentEdge->id).c_str(), ok);
        std::string value = attrs.getStringReporting(SUMO_ATTR_V, toString(myCurrentEdge->id).c_str(), ok, false);
        if (!ok) {
            return;
        }
        if (key == "waterway" || key == "aeroway" || key == "aerialway" || key == "power" || key == "man_made"
                || key == "building" || key == "leisure" || key == "amenity" || key == "shop" || key == "tourism"
                || key == "historic" || key == "landuse" || key == "natural" || key == "military" || key == "boundary"
                || key == "sport" || key == "polygon") {
            myCurrentEdge->myType = key + "." + value;
            myCurrentEdge->myIsAdditional = true;
        } else if (key == "name") {
            myCurrentEdge->name = value;
        }
    }
}


void
PCLoaderOSM::EdgesHandler::myEndElement(int element) {
    myParentElements.pop_back();
    if (element == SUMO_TAG_WAY) {
        if (myCurrentEdge->myIsAdditional) {
            myEdgeMap[myCurrentEdge->id] = myCurrentEdge;
        } else {
            delete myCurrentEdge;
        }
        myCurrentEdge = 0;
    }
}


/****************************************************************************/


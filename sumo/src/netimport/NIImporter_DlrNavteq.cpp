/****************************************************************************/
/// @file    NIImporter_DlrNavteq.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in Elmar's format
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

#include <string>
#include <sstream>
#include <limits>
#include <utils/importio/LineHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/importio/LineReader.h>
#include <utils/geom/GeoConvHelper.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBOwnTLDef.h>
#include <netimport/NINavTeqHelper.h>
#include "NILoader.h"
#include "NIImporter_DlrNavteq.h"


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ---------------------------------------------------------------------------
// static members
// ---------------------------------------------------------------------------
const int NIImporter_DlrNavteq::GEO_SCALE = 5;
const int NIImporter_DlrNavteq::EdgesHandler::MISSING_COLUMN = std::numeric_limits<int>::max();

// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods
// ---------------------------------------------------------------------------
void
NIImporter_DlrNavteq::loadNetwork(const OptionsCont& oc, NBNetBuilder& nb) {
    // check whether the option is set (properly)
    if (!oc.isSet("dlr-navteq-prefix")) {
        return;
    }
    // parse file(s)
    LineReader lr;
    // load nodes
    std::map<std::string, PositionVector> myGeoms;
    PROGRESS_BEGIN_MESSAGE("Loading nodes");
    std::string file = oc.getString("dlr-navteq-prefix") + "_nodes_unsplitted.txt";
    NodesHandler handler1(nb.getNodeCont(), file, myGeoms);
    if (!lr.setFile(file)) {
        throw ProcessError("The file '" + file + "' could not be opened.");
    }
    lr.readAll(handler1);
    PROGRESS_DONE_MESSAGE();

    // load street names if given and wished
    std::map<std::string, std::string> streetNames; // nameID : name
    if (oc.getBool("output.street-names")) {
        file = oc.getString("dlr-navteq-prefix") + "_names.txt";
        if (lr.setFile(file)) {
            PROGRESS_BEGIN_MESSAGE("Loading Street Names");
            NamesHandler handler4(file, streetNames);
            lr.readAll(handler4);
            PROGRESS_DONE_MESSAGE();
        } else {
            WRITE_WARNING("Output will not contain street names because the file '" + file + "' was not found");
        }
    }

    // load edges
    PROGRESS_BEGIN_MESSAGE("Loading edges");
    file = oc.getString("dlr-navteq-prefix") + "_links_unsplitted.txt";
    // parse the file
    EdgesHandler handler2(nb.getNodeCont(), nb.getEdgeCont(), file, myGeoms, streetNames);
    if (!lr.setFile(file)) {
        throw ProcessError("The file '" + file + "' could not be opened.");
    }
    lr.readAll(handler2);
    nb.getEdgeCont().recheckLaneSpread();
    PROGRESS_DONE_MESSAGE();

    // load traffic lights if given
    file = oc.getString("dlr-navteq-prefix") + "_traffic_signals.txt";
    if (lr.setFile(file)) {
        PROGRESS_BEGIN_MESSAGE("Loading traffic lights");
        TrafficlightsHandler handler3(nb.getNodeCont(), nb.getTLLogicCont(), nb.getEdgeCont(), file);
        lr.readAll(handler3);
        PROGRESS_DONE_MESSAGE();
    }
}


// ---------------------------------------------------------------------------
// definitions of NIImporter_DlrNavteq::NodesHandler-methods
// ---------------------------------------------------------------------------
NIImporter_DlrNavteq::NodesHandler::NodesHandler(NBNodeCont& nc,
        const std::string& file,
        std::map<std::string, PositionVector>& geoms)
    : myNodeCont(nc), myGeoms(geoms) {
    UNUSED_PARAMETER(file);
}


NIImporter_DlrNavteq::NodesHandler::~NodesHandler() {}


bool
NIImporter_DlrNavteq::NodesHandler::report(const std::string& result) {
    if (result[0] == '#') {
        return true;
    }
    std::string id;
    double x, y;
    int no_geoms, intermediate;
    // parse
    std::istringstream stream(result);
    // id
    stream >> id;
    if (stream.fail()) {
        throw ProcessError("Something is wrong with the following data line\n" + result);
    }
    // intermediate?
    stream >> intermediate;
    if (stream.fail()) {
        if (myNodeCont.size() == 0) { // be generous with extra data at beginning of file
            return true;
        }
        throw ProcessError("Non-numerical value for intermediate status in node " + id + ".");
    }
    // number of geometrical information
    stream >> no_geoms;
    if (stream.fail()) {
        throw ProcessError("Non-numerical value for number of geometries in node " + id + ".");
    }
    // geometrical information
    PositionVector geoms;
    for (int i = 0; i < no_geoms; i++) {
        stream >> x;
        if (stream.fail()) {
            throw ProcessError("Non-numerical value for x-position in node " + id + ".");
        }
        stream >> y;
        if (stream.fail()) {
            throw ProcessError("Non-numerical value for y-position in node " + id + ".");
        }
        Position pos(x, y);
        if (!NBNetBuilder::transformCoordinates(pos, true)) {
            throw ProcessError("Unable to project coordinates for node " + id + ".");
        }
        geoms.push_back(pos);
    }

    if (intermediate == 0) {
        NBNode* n = new NBNode(id, geoms[0]);
        if (!myNodeCont.insert(n)) {
            delete n;
            throw ProcessError("Could not add node '" + id + "'.");
        }
    } else {
        myGeoms[id] = geoms;
    }
    return true;
}


// ---------------------------------------------------------------------------
// definitions of NIImporter_DlrNavteq::EdgesHandler-methods
// ---------------------------------------------------------------------------
NIImporter_DlrNavteq::EdgesHandler::EdgesHandler(NBNodeCont& nc, NBEdgeCont& ec,
        const std::string& file,
        std::map<std::string, PositionVector>& geoms,
        std::map<std::string, std::string>& streetNames):
    myNodeCont(nc),
    myEdgeCont(ec),
    myGeoms(geoms),
    myStreetNames(streetNames),
    myVersion(0),
    myFile(file)
{ }


NIImporter_DlrNavteq::EdgesHandler::~EdgesHandler() {}


bool
NIImporter_DlrNavteq::EdgesHandler::report(const std::string& result) {
    // parse version number from first comment line and initialize column definitions
    if (result[0] == '#') {
        if (!myColumns.empty()) {
            return true;
        }
        const std::string marker = "Extraction version: V";
        size_t vStart = result.find(marker);
        if (vStart == std::string::npos) {
            return true;
        }
        vStart += marker.size();
        const size_t vEnd = result.find(" ", vStart);
        try {
            myVersion = TplConvert::_2SUMOReal(result.substr(vStart, vEnd - vStart).c_str());
            if (myVersion < 0) {
                throw ProcessError("Invalid version number '" + toString(myVersion) + "' in file '" + myFile + "'.");
            }
            // init columns
            const size_t NUM_COLUMNS = 25; // @note arrays must match this size!
            const int MC = MISSING_COLUMN;
            if (myVersion < 3) {
                const int columns[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, MC, 12, 13, 14, 15, 16, 17, 18, 19, 20, MC, MC, -21};
                myColumns = std::vector<int>(columns, columns + NUM_COLUMNS);
            } else if (myVersion < 6) {
                const int columns[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, MC, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, -23};
                myColumns = std::vector<int>(columns, columns + NUM_COLUMNS);
            } else {
                const int columns[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
                myColumns = std::vector<int>(columns, columns + NUM_COLUMNS);
            }
        } catch (NumberFormatException&) {
            throw ProcessError("Non-numerical value for version string in file '" + myFile + "'.");
        }
        return true;
    }
    if (myColumns.empty()) {
        throw ProcessError("Missing version string in file '" + myFile + "'.");
    }
    // interpret link attributes
    StringTokenizer st(result, StringTokenizer::WHITECHARS);
    const std::string id = getColumn(st, LINK_ID);
    // form of way (for priority and permissions)
    int form_of_way;
    try {
        form_of_way = TplConvert::_2int(getColumn(st, FORM_OF_WAY).c_str());
    } catch (NumberFormatException&) {
        throw ProcessError("Non-numerical value for form_of_way of link '" + id + "'.");
    }
    // priority based on street_type / frc
    int priority;
    try {
        priority = -TplConvert::_2int(getColumn(st, FUNCTIONAL_ROAD_CLASS).c_str());
        // lower priority using form_of_way
        if (form_of_way == 11) {
            priority -= 1; // frontage road, very often with lowered curb
        } else if (form_of_way > 11) {
            priority -= 2; // parking/service access assume lowered curb
        }
    } catch (NumberFormatException&) {
        throw ProcessError("Non-numerical value for street_type of link '" + id + "').");
    }
    // street name
    std::string streetName = getStreetNameFromIDs(
                                 getColumn(st, NAME_ID1_REGIONAL),
                                 getColumn(st, NAME_ID2_LOCAL));
    // try to get the nodes
    const std::string fromID = getColumn(st, NODE_ID_FROM);
    const std::string toID = getColumn(st, NODE_ID_TO);
    NBNode* from = myNodeCont.retrieve(fromID);
    NBNode* to = myNodeCont.retrieve(toID);
    if (from == 0) {
        throw ProcessError("The from-node '" + fromID + "' of link '" + id + "' could not be found");
    }
    if (to == 0) {
        throw ProcessError("The to-node '" + toID + "' of link '" + id + "' could not be found");
    }
    // speed
    SUMOReal speed;
    try {
        speed = TplConvert::_2int(getColumn(st, SPEED_RESTRICTION, "-1").c_str()) / 3.6;
    } catch (NumberFormatException) {
        throw ProcessError("Non-numerical value for the SPEED_RESTRICTION of link '" + id + "'.");
    }
    if (speed < 0) {
        // speed category as fallback
        speed = NINavTeqHelper::getSpeed(id, getColumn(st, SPEED_CATEGORY));
    }
    // number of lanes
    int numLanes;
    try {
        // EXTENDED_NUMBER_OF_LANES is prefered but may not be defined
        numLanes = TplConvert::_2int(getColumn(st, EXTENDED_NUMBER_OF_LANES, "-1").c_str());
        if (numLanes == -1) {
            numLanes = NINavTeqHelper::getLaneNumber(id, getColumn(st, NUMBER_OF_LANES), speed);
        }
    } catch (NumberFormatException&) {
        throw ProcessError("Non-numerical value for the number of lanes of link '" + id + "'.");
    }
    // build the edge
    NBEdge* e = 0;
    const std::string interID = getColumn(st, BETWEEN_NODE_ID);
    if (interID == "-1") {
        e = new NBEdge(id, from, to, "", speed, numLanes, priority,
                       NBEdge::UNSPECIFIED_WIDTH, NBEdge::UNSPECIFIED_OFFSET, streetName);
    } else {
        PositionVector geoms = myGeoms[interID];
        if (getColumn(st, CONNECTION, "0") == "1") {
            geoms = geoms.reverse();
        }
        geoms.push_front(from->getPosition());
        geoms.push_back(to->getPosition());
        e = new NBEdge(id, from, to, "", speed, numLanes, priority,
                       NBEdge::UNSPECIFIED_WIDTH, NBEdge::UNSPECIFIED_OFFSET, geoms, streetName, LANESPREAD_CENTER);
    }
    // add vehicle type information to the edge
    if (myVersion < 6.0) {
        NINavTeqHelper::addVehicleClasses(*e, getColumn(st, VEHICLE_TYPE));
    } else {
        NINavTeqHelper::addVehicleClassesV6(*e, getColumn(st, VEHICLE_TYPE));
    }
    // permission modifications based on form_of_way
    if (form_of_way == 14) { // pedestrian area (fussgaengerzone)
        // unfortunately, the veh_type string is misleading in this case
        e->disallowVehicleClass(-1, SVC_PASSENGER);
    }
    // insert the edge to the network
    if (!myEdgeCont.insert(e)) {
        delete e;
        throw ProcessError("Could not add edge '" + id + "'.");
    }
    return true;
}


std::string
NIImporter_DlrNavteq::EdgesHandler::getColumn(const StringTokenizer& st, ColumnName name, const std::string fallback) {
    assert(!myColumns.empty());
    if (myColumns[name] == MISSING_COLUMN) {
        if (fallback == "") {
            throw ProcessError("Missing column " + toString(name) + ".");
        } else {
            return fallback;
        }
    } else if (myColumns[name] >= 0) {
        return st.get((size_t)(myColumns[name]));
    } else {
        // negative column number implies an optional column
        if ((int) st.size() <= -myColumns[name]) {
            // the column is not present
            if (fallback == "") {
                throw ProcessError("Missing optional column " + toString(name) + " without default value.");
            } else {
                return fallback;
            }
        } else {
            return st.get((size_t)(-myColumns[name]));
        }
    }
}


std::string
NIImporter_DlrNavteq::EdgesHandler::getStreetNameFromIDs(
    const std::string& regionalID, const std::string& localID) const {
    std::string result = "";
    bool hadRegional = false;
    if (myStreetNames.count(regionalID) > 0) {
        hadRegional = true;
        result += myStreetNames[regionalID];
    }
    if (myStreetNames.count(localID) > 0) {
        if (hadRegional) {
            result += " / ";
        }
        result += myStreetNames[localID];
    }
    return result;
}

// ---------------------------------------------------------------------------
// definitions of NIImporter_DlrNavteq::TrafficlightsHandler-methods
// ---------------------------------------------------------------------------
NIImporter_DlrNavteq::TrafficlightsHandler::TrafficlightsHandler(NBNodeCont& nc,
        NBTrafficLightLogicCont& tlc,
        NBEdgeCont& ne,
        const std::string& file) :
    myNodeCont(nc),
    myTLLogicCont(tlc),
    myEdgeCont(ne) {
    UNUSED_PARAMETER(file);
}


NIImporter_DlrNavteq::TrafficlightsHandler::~TrafficlightsHandler() {}


bool
NIImporter_DlrNavteq::TrafficlightsHandler::report(const std::string& result) {
// #ID     POICOL-TYPE     DESCRIPTION     LONGITUDE       LATITUDE        NAVTEQ_LINK_ID  NODEID

    if (result[0] == '#') {
        return true;
    }
    StringTokenizer st(result, StringTokenizer::WHITECHARS);
    const std::string edgeID = st.get(5);
    NBEdge* edge = myEdgeCont.retrieve(edgeID);
    if (edge == 0) {
        WRITE_WARNING("The traffic light edge '" + edgeID + "' could not be found");
    } else {
        NBNode* node = edge->getToNode();
        if (node->getType() != NODETYPE_TRAFFIC_LIGHT) {
            node->reinit(node->getPosition(), NODETYPE_TRAFFIC_LIGHT);
            // @note. There may be additional information somewhere in the GDF files about traffic light type ...
            TrafficLightType type = SUMOXMLDefinitions::TrafficLightTypes.get(OptionsCont::getOptions().getString("tls.default-type"));
            // @note actually we could use the navteq node ID here
            NBTrafficLightDefinition* tlDef = new NBOwnTLDef(node->getID(), node, 0, type);
            if (!myTLLogicCont.insert(tlDef)) {
                // actually, nothing should fail here
                delete tlDef;
                throw ProcessError("Could not allocate tls for '" + node->getID() + "'.");
            }
        }
    }
    return true;
}


// ---------------------------------------------------------------------------
// definitions of NIImporter_DlrNavteq::NamesHandler-methods
// ---------------------------------------------------------------------------
NIImporter_DlrNavteq::NamesHandler::NamesHandler(
    const std::string& file, std::map<std::string, std::string>& streetNames) :
    myStreetNames(streetNames) {
    UNUSED_PARAMETER(file);
}


NIImporter_DlrNavteq::NamesHandler::~NamesHandler() {}


bool
NIImporter_DlrNavteq::NamesHandler::report(const std::string& result) {
// # NAME_ID    Name
    if (result[0] == '#') {
        return true;
    }
    StringTokenizer st(result, StringTokenizer::WHITECHARS);
    if (st.size() == 1) {
        return true; // one line with the number of data containing lines in it
    }
    assert(st.size() >= 2);
    const std::string id = st.next();
    myStreetNames[id] = joinToString(st.getVector(), " ");
    return true;
}
/****************************************************************************/

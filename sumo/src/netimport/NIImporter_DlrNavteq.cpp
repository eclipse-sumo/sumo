/****************************************************************************/
/// @file    NIImporter_DlrNavteq.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 14.04.2008
/// @version $Id: NIImporter_DlrNavteq.cpp 7703 2009-09-10 13:45:09Z simsiem $
///
// Importer for networks stored in Elmar's format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <utils/importio/LineHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <utils/options/OptionsCont.h>
#include <utils/importio/LineReader.h>
#include <utils/geom/GeoConvHelper.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBTypeCont.h>
#include <netimport/NINavTeqHelper.h>
#include "NIImporter_DlrNavteq.h"


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;



// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods
// ---------------------------------------------------------------------------
void
NIImporter_DlrNavteq::loadNetwork(const OptionsCont &oc, NBNetBuilder &nb) {
    // check whether the option is set (properly)
    if (!oc.isSet("dlr-navteq")) {
        return;
    }
    // parse file(s)
    LineReader lr;
    // load nodes
    std::map<std::string, Position2DVector> myGeoms;
    MsgHandler::getMessageInstance()->beginProcessMsg("Loading nodes...");
    std::string file = oc.getString("dlr-navteq") + "_nodes_unsplitted.txt";
    NodesHandler handler1(nb.getNodeCont(), file, myGeoms);
    if (!lr.setFile(file)) {
        throw ProcessError("The file '" + file + "' could not be opened.");
    }
    lr.readAll(handler1);
    MsgHandler::getMessageInstance()->endProcessMsg("done.");

    // load edges
    MsgHandler::getMessageInstance()->beginProcessMsg("Loading edges...");
    file = oc.getString("dlr-navteq") + "_links_unsplitted.txt";
    // parse the file
    EdgesHandler handler2(nb.getNodeCont(), nb.getEdgeCont(), file, myGeoms);
    if (!lr.setFile(file)) {
        throw ProcessError("The file '" + file + "' could not be opened.");
    }
    lr.readAll(handler2);
    nb.getEdgeCont().recheckLaneSpread();
    MsgHandler::getMessageInstance()->endProcessMsg("done.");
}


// ---------------------------------------------------------------------------
// definitions of NIImporter_DlrNavteq::NodesHandler-methods
// ---------------------------------------------------------------------------
NIImporter_DlrNavteq::NodesHandler::NodesHandler(NBNodeCont &nc,
        const std::string &file,
        std::map<std::string, Position2DVector> &geoms) throw()
        : myNodeCont(nc), myGeoms(geoms) {}


NIImporter_DlrNavteq::NodesHandler::~NodesHandler() throw() {}


bool
NIImporter_DlrNavteq::NodesHandler::report(const std::string &result) throw(ProcessError) {
    if (result[0]=='#') {
        return true;
    }
    string id;
    SUMOReal x, y;
    int no_geoms, intermediate;
    StringTokenizer st(result, StringTokenizer::WHITECHARS);
    // check
    if (st.size()<5) {
        throw ProcessError("Something is wrong with the following data line\n" + result);
    }
    // parse
    // id
    id = st.next();
    // intermediate?
    try {
        intermediate = TplConvert<char>::_2int(st.next().c_str());
    } catch (NumberFormatException &) {
        throw ProcessError("Non-numerical value for intermediate status in node " + id + ".");
    }
    // number of geometrical information
    try {
        no_geoms = TplConvert<char>::_2int(st.next().c_str());
    } catch (NumberFormatException &) {
        throw ProcessError("Non-numerical value for number of geometries in node " + id + ".");
    }
    // geometrical information
    Position2DVector geoms;
    for (int i=0; i<no_geoms; i++) {
        try {
            x = (SUMOReal) TplConvert<char>::_2SUMOReal(st.next().c_str());
        } catch (NumberFormatException &) {
            throw ProcessError("Non-numerical value for x-position in node " + id + ".");
        }
        try {
            y = (SUMOReal) TplConvert<char>::_2SUMOReal(st.next().c_str());
        } catch (NumberFormatException &) {
            throw ProcessError("Non-numerical value for y-position in node " + id + ".");
        }

        Position2D pos(x, y);
        if (!GeoConvHelper::x2cartesian(pos)) {
            throw ProcessError("Unable to project coordinates for node " + id + ".");
        }
        geoms.push_back(pos);
    }

    if (intermediate==0) {
        NBNode *n = new NBNode(id, geoms[0]);
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
NIImporter_DlrNavteq::EdgesHandler::EdgesHandler(NBNodeCont &nc, NBEdgeCont &ec,
        const std::string &file,
        std::map<std::string,
        Position2DVector> &geoms) throw()
        : myNodeCont(nc), myEdgeCont(ec), myGeoms(geoms) {}


NIImporter_DlrNavteq::EdgesHandler::~EdgesHandler() throw() {}


bool
NIImporter_DlrNavteq::EdgesHandler::report(const std::string &result) throw(ProcessError) {
//	0: LINK_ID	NODE_ID_FROM	NODE_ID_TO	BETWEEN_NODE_ID
//  4: length	vehicle_type	form_of_way	brunnel_type
//  7: street_type	speed_category	number_of_lanes	average_speed
//  10: NAME_ID1	NAME_ID2	housenumbers_right	housenumbers_left
//  ZIP_CODE	AREA_ID	SUBAREA_ID	through_traffic	special_restrictions
//  extended_number_of_lanes  isRamp    (these two only exist in networks extracted since 05/2009)
//  connection (this may be omitted)

    if (result[0]=='#') {
        return true;
    }
    string id, fromID, toID, interID;
    SUMOReal length;
    SUMOReal speed = (SUMOReal) 30.0 / (SUMOReal) 3.6;
    int nolanes = 1;
    int priority = -1;
    // parse
    StringTokenizer st(result, StringTokenizer::WHITECHARS);
    // id
    id = st.next();
    // from node id
    fromID = st.next();
    // to node id
    toID = st.next();
    // intermediate node id
    interID = st.next();
    // length
    try {
        length = TplConvert<char>::_2SUMOReal(st.next().c_str());
    } catch (NumberFormatException &) {
        throw ProcessError("Non-numerical value for an edge's length occured (edge '" + id + "'.");
    }
    // vehicle_type
    string veh_type = st.next();
    // form_of_way
    string form_of_way = st.next();
    // brunnel_type
    string brunnel_type = st.next();
    // street_type
    string street_type = st.next();
    speed = NINavTeqHelper::getSpeed(id, st.next());
    // number of lanes
    nolanes = NINavTeqHelper::getLaneNumber(id, st.next(), speed);
    std::vector<std::string> theRest = st.getVector();
    bool connection = (theRest.size() == 11) && (theRest[10] == "1");
    if (theRest.size() > 11) {
        // post 05/2009 network
        if (theRest[11] != "-1") {
            try {
                nolanes = TplConvert<char>::_2int(theRest[11].c_str());
            } catch (NumberFormatException &) {
                throw ProcessError("Non-numerical value for the extended number of lanes (edge '" + id + "'.");
            }
        }
        connection = (theRest.size() == 13) && (theRest[12] == "1");
    }
    // try to get the nodes
    NBNode *from = myNodeCont.retrieve(fromID);
    NBNode *to = myNodeCont.retrieve(toID);
    if (from==0) {
        throw ProcessError("The from-node '" + fromID + "' of edge '" + id + "' could not be found");
    }
    if (to==0) {
        throw ProcessError("The to-node '" + toID + "' of edge '" + id + "' could not be found");
    }
    // build the edge
    NBEdge *e = 0;
    if (interID=="-1") {
        e = new NBEdge(id, from, to, "DEFAULT", speed, nolanes, priority);
    } else {
        Position2DVector geoms = myGeoms[interID];
        if (connection) {
            geoms = geoms.reverse();
            geoms.push_front(from->getPosition());
            geoms.push_back(to->getPosition());
            e = new NBEdge(id, from, to, "DEFAULT", speed, nolanes, priority, geoms, NBEdge::LANESPREAD_CENTER);
        } else {
            geoms.push_front(from->getPosition());
            geoms.push_back(to->getPosition());
            e = new NBEdge(id, from, to, "DEFAULT", speed, nolanes, priority, geoms, NBEdge::LANESPREAD_CENTER);
        }
    }
    // add vehicle type information to the edge
    NINavTeqHelper::addVehicleClasses(*e, veh_type);
    // insert the edge to the network
    if (!myEdgeCont.insert(e)) {
        delete e;
        throw ProcessError("Could not add edge '" + id + "'.");
    }
    return true;
}



/****************************************************************************/


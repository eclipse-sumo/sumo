/****************************************************************************/
/// @file    NIElmar2EdgesHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sun, 16 May 2004
/// @version $Id$
///
// Importer of edges stored in unsplit elmar format
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
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBTypeCont.h>
#include "NIElmar2EdgesHandler.h"
#include <netimport/NINavTeqHelper.h>

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
NIElmar2EdgesHandler::NIElmar2EdgesHandler(NBNodeCont &nc, NBEdgeCont &ec,
        const std::string &file,
        std::map<std::string,
        Position2DVector> &geoms, bool tryIgnoreNodePositions) throw()
        : myNodeCont(nc), myEdgeCont(ec), myGeoms(geoms),
        myTryIgnoreNodePositions(tryIgnoreNodePositions) {}


NIElmar2EdgesHandler::~NIElmar2EdgesHandler() throw() {}


bool
NIElmar2EdgesHandler::report(const std::string &result) throw(ProcessError) {
//	0: LINK_ID	NODE_ID_FROM	NODE_ID_TO	BETWEEN_NODE_ID
//  4: length	vehicle_type	form_of_way	brunnel_type
//  7: street_type	speed_category	number_of_lanes	average_speed
//  10: NAME_ID1	NAME_ID2	housenumebrs_right	housenumbers_left
//  ZIP_CODE	AREA_ID	SUBAREA_ID	through_traffic	special_restrictions
//  connection

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
    // skip some
    st.next(); // average_speed
    st.next(); // NAME_ID1
    st.next(); // NAME_ID2
    st.next(); // housenumebrs_right
    st.next(); // housenumbers_left
    st.next(); // ZIP_CODE
    st.next(); // AREA_ID
    st.next(); // SUBAREA_ID
    st.next(); // through_traffic
    st.next(); // special_restrictions

    bool connection = !st.hasNext()
                      ? 0
                      : st.next()[0] == '1';
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
            e = new NBEdge(id, from, to, "DEFAULT", speed, nolanes, priority, geoms, myTryIgnoreNodePositions, NBEdge::LANESPREAD_CENTER);
        } else {
            geoms.push_front(from->getPosition());
            geoms.push_back(to->getPosition());
            e = new NBEdge(id, from, to, "DEFAULT", speed, nolanes, priority, geoms, myTryIgnoreNodePositions, NBEdge::LANESPREAD_CENTER);
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


/****************************************************************************/
/// @file    NIElmarEdgesHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sun, 16 May 2004
/// @version $Id:NIElmarEdgesHandler.cpp 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer of edges stored in split elmar format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include "NIElmarEdgesHandler.h"
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
NIElmarEdgesHandler::NIElmarEdgesHandler(NBNodeCont &nc, NBEdgeCont &ec,
        const std::string &file) throw()
        : myNodeCont(nc), myEdgeCont(ec)
{}

NIElmarEdgesHandler::~NIElmarEdgesHandler() throw()
{}

bool
NIElmarEdgesHandler::report(const std::string &result) throw(ProcessError)
{
// 00: KANTEN_ID Knoten_ID_FROM Knoten_ID_TO length vehicle_type
// 05: form_of_way brunnel_type street_type speed_category number_of_lanes
// average_speed    Namens_ID1  Namens_ID2  Hausnummern_rechts  Hausnummern_links   Postleitzahl    Gebiets_ID  Teilgebiets_ID  through_traffic special_restrictions    direction_of_flow_in_validity_period    direction_of_flow_through_traffic   direction_of_flow_vehicle_type  direction_of_flow_validity_period   construction_status_in_validity_period  construction_status_through_traffic construction_status_validity_period construction_status_vehicle_type
    if (result[0]=='#') {
        return true;
    }

    string id, fromID, toID;
    SUMOReal length;
    // parse
    StringTokenizer st(result, StringTokenizer::WHITECHARS);
    id = st.next();
    fromID = st.next();
    toID = st.next();
    SUMOReal speed = (SUMOReal) 30.0 / (SUMOReal) 3.6;
    int nolanes = 1;
    int priority = -1;
    try {
        length = TplConvert<char>::_2SUMOReal(st.next().c_str());
    } catch (NumberFormatException &) {
        throw ProcessError("Non-numerical value for an edge's length occured (edge '" + id + "'.");
    }
    string veh_type = st.next();
    string form_of_way = st.next();
    string brunnel_type = st.next();
    string street_type = st.next();
    speed = NINavTeqHelper::getSpeed(id, st.next());
    nolanes = NINavTeqHelper::getLaneNumber(id, st.next(), speed);
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
    NBEdge *e =
        new NBEdge(id, from, to, "DEFAULT", speed, nolanes, priority, NBEdge::LANESPREAD_CENTER);
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


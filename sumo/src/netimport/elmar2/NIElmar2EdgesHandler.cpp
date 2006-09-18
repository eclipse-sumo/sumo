/***************************************************************************
                          NIElmar2EdgesHandler.cpp
             A LineHandler-derivate to load edges form a elmar-edges-file
                             -------------------
    project              : SUMO
    begin                : Sun, 16 May 2004
    copyright            : (C) 2004 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.9  2006/09/18 10:12:57  dkrajzew
// added import of vclasses
//
// Revision 1.8  2006/04/18 08:05:45  dkrajzew
// beautifying: output consolidation
//
// Revision 1.7  2006/03/08 13:02:27  dkrajzew
// some further work on converting geo-coordinates
//
// Revision 1.6  2006/01/31 10:59:35  dkrajzew
// extracted common used methods; optional usage of old lane number information in navteq-networks import added
//
// Revision 1.5  2006/01/19 09:26:04  dkrajzew
// adapted to the current version
//
// Revision 1.4  2005/10/07 11:39:36  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.3  2005/09/23 06:02:24  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.2  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2005/07/14 11:05:28  dkrajzew
// elmar unsplitted import added
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/importio/LineHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBCapacity2Lanes.h>
#include "NIElmar2EdgesHandler.h"
#include <netimport/NINavTeqHelper.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIElmar2EdgesHandler::NIElmar2EdgesHandler(NBNodeCont &nc, NBEdgeCont &ec,
                                           const std::string &file,
                                           std::map<std::string,
										   Position2DVector> &geoms,
										   bool useNewLaneNumberInfoPlain)
    : FileErrorReporter("elmar-edges", file),
    myNodeCont(nc), myEdgeCont(ec), myGeoms(geoms),
	myUseNewLaneNumberInfoPlain(useNewLaneNumberInfoPlain)
{
}

NIElmar2EdgesHandler::~NIElmar2EdgesHandler()
{
}

bool
NIElmar2EdgesHandler::report(const std::string &result)
{
//	0: LINK_ID	NODE_ID_FROM	NODE_ID_TO	BETWEEN_NODE_ID
//  4: length	vehicle_type	form_of_way	brunnel_type
//  7: street_type	speed_category	number_of_lanes	average_speed
//  10: NAME_ID1	NAME_ID2	housenumebrs_right	housenumbers_left
//  ZIP_CODE	AREA_ID	SUBAREA_ID	through_traffic	special_restrictions
//  connection

    if(result[0]=='#') {
        return true;
    }
    string id, fromID, toID, interID;
    SUMOReal length;
    SUMOReal speed = (SUMOReal) 30.0 / (SUMOReal) 3.6;
    int nolanes = 1;
    int priority = 0;
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
        MsgHandler::getErrorInstance()->inform("Non-numerical value for an edge's length occured (edge '" + id + "'.");
        throw ProcessError();
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
	nolanes =
		NINavTeqHelper::getLaneNumber(id, st.next(), speed, myUseNewLaneNumberInfoPlain);
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
    if(from==0) {
        MsgHandler::getErrorInstance()->inform("The from-node '" + fromID + "' of edge '" + id + "' could not be found");
        throw ProcessError();
    }
    if(to==0) {
        MsgHandler::getErrorInstance()->inform("The to-node '" + toID + "' of edge '" + id + "' could not be found");
        throw ProcessError();
    }

    NBEdge *e = 0;
    if(interID=="-1") {
        e = new NBEdge(id, id, from, to, "DEFAULT",
            speed, nolanes, length, priority);
    } else {
        Position2DVector geoms = myGeoms[interID];
        if(connection) {
            geoms = geoms.reverse();
            geoms.push_front(from->getPosition());
            geoms.push_back(to->getPosition());
            e = new NBEdge(id, id, from, to, "DEFAULT",
                speed, nolanes, length, priority, geoms, NBEdge::LANESPREAD_CENTER);
        } else {
            geoms.push_front(from->getPosition());
            geoms.push_back(to->getPosition());
            e = new NBEdge(id, id, from, to, "DEFAULT",
                speed, nolanes, length, priority, geoms, NBEdge::LANESPREAD_CENTER);
        }
    }
    // insert the edge to the network
    if(!myEdgeCont.insert(e)) {
        delete e;
        MsgHandler::getErrorInstance()->inform("Could not add edge '" + id + "'.");
        throw ProcessError();
    }
    // add vehicle type information to the edge
    NINavTeqHelper::addVehicleClasses(*e, veh_type);
    return true;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



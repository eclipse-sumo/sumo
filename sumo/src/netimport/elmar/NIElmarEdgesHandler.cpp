/***************************************************************************
                          NIElmarEdgesHandler.cpp
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
// Revision 1.9  2006/01/31 10:59:35  dkrajzew
// extracted common used methods; optional usage of old lane number information in navteq-networks import added
//
// Revision 1.8  2005/11/09 06:42:07  dkrajzew
// complete geometry building rework (unfinished)
//
// Revision 1.7  2005/10/07 11:39:26  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:02:15  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/07/12 12:35:22  dkrajzew
// elmar2 importer included; debugging
//
// Revision 1.3  2005/04/27 12:24:35  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.2  2004/11/23 10:23:52  dkrajzew
// debugging
//
// Revision 1.1  2004/07/02 09:34:38  dkrajzew
// elmar and tiger import added
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
#include "NIElmarEdgesHandler.h"
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
NIElmarEdgesHandler::NIElmarEdgesHandler(NBNodeCont &nc, NBEdgeCont &ec,
                                         const std::string &file,
										 bool useNewLaneNumberInfoPlain)
    : FileErrorReporter("elmar-edges", file),
    myNodeCont(nc), myEdgeCont(ec),
	myUseNewLaneNumberInfoPlain(useNewLaneNumberInfoPlain)
{
}

NIElmarEdgesHandler::~NIElmarEdgesHandler()
{
}

bool
NIElmarEdgesHandler::report(const std::string &result)
{
// 00: KANTEN_ID Knoten_ID_FROM Knoten_ID_TO length vehicle_type
// 05: form_of_way brunnel_type street_type speed_category number_of_lanes
// average_speed    Namens_ID1  Namens_ID2  Hausnummern_rechts  Hausnummern_links   Postleitzahl    Gebiets_ID  Teilgebiets_ID  through_traffic special_restrictions    direction_of_flow_in_validity_period    direction_of_flow_through_traffic   direction_of_flow_vehicle_type  direction_of_flow_validity_period   construction_status_in_validity_period  construction_status_through_traffic construction_status_validity_period construction_status_vehicle_type
    if(result[0]=='#') {
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
    int priority = 0;
    try {
        length = TplConvert<char>::_2SUMOReal(st.next().c_str());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Non-numerical value for an edge's length occured (edge '" + id + "'.");
        throw ProcessError();
    }
    string veh_type = st.next();
    string form_of_way = st.next();
    string brunnel_type = st.next();
    string street_type = st.next();
	speed = NINavTeqHelper::getSpeed(id, st.next());
	nolanes =
		NINavTeqHelper::getLaneNumber(id, st.next(), speed, myUseNewLaneNumberInfoPlain);
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

    NBEdge *e =
        new NBEdge(id, id, from, to, "DEFAULT",
            speed, nolanes, length, priority,
            NBEdge::LANESPREAD_CENTER);

    if(!myEdgeCont.insert(e)) {
        delete e;
        MsgHandler::getErrorInstance()->inform("Could not add edge '" + id + "'.");
        throw ProcessError();
    }
    return true;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



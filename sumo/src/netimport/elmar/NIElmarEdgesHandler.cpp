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
#include <config.h>
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
                                         const std::string &file)
    : FileErrorReporter("elmar-edges", file),
    myNodeCont(nc), myEdgeCont(ec)
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
        MsgHandler::getErrorInstance()->inform(
            string("Non-numerical value for an edge's length occured (edge '") + id
            + string("'."));
        throw ProcessError();
    }
    string veh_type = st.next();
    string form_of_way = st.next();
    string brunnel_type = st.next();
    string street_type = st.next();
    try {
        int speed_class = TplConvert<char>::_2int(st.next().c_str());
        switch(speed_class) {
        case -1:
            speed = (SUMOReal) 1.0 / (SUMOReal) 3.6;
            break;
        case 1:
            speed = (SUMOReal) 200 / (SUMOReal) 3.6; //> 130 KPH / > 80 MPH
            break;
        case 2:
            speed = (SUMOReal) 115 / (SUMOReal) 3.6; //101-130 KPH / 65-80 MPH
            break;
        case 3:
            speed = (SUMOReal) 95 / (SUMOReal) 3.6; // 91-100 KPH / 55-64 MPH
            break;
        case 4:
            speed = (SUMOReal) 80 / (SUMOReal) 3.6; // 71-90 KPH / 41-54 MPH
            break;
        case 5:
            speed = (SUMOReal) 60 / (SUMOReal) 3.6; // 51-70 KPH / 31-40 MPH
            break;
        case 6:
            speed = (SUMOReal) 40 / (SUMOReal) 3.6; // 31-50 KPH / 21-30 MPH
            break;
        case 7:
            speed = (SUMOReal) 20 / (SUMOReal) 3.6; // 11-30 KPH / 6-20 MPH
            break;
        case 8:
            speed = (SUMOReal) 5 / (SUMOReal) 3.6; //< 11 KPH / < 6 MPH
            break;
        default:
            MsgHandler::getErrorInstance()->inform(
                string("Invalid speed code (edge '") + id
                + string("'."));
            throw ProcessError();
        }
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform(
            string("Non-numerical value for an edge's speed type occured (edge '") + id
            + string("')."));
        throw ProcessError();
    }
    try {
        nolanes = TplConvert<char>::_2int(st.next().c_str());
        if(nolanes<0) {
            nolanes = 1;
        } else if(nolanes/10>0) {
                nolanes = nolanes / 10;
        } else {
            switch(nolanes%10) {
            case 1:
                nolanes = 1;
                break;
            case 2:
                nolanes = 2;
                if(speed>78.0/3.6) {
                    nolanes = 3;
                }
                break;
            case 3:
                nolanes = 4;
                break;
            default:
                MsgHandler::getErrorInstance()->inform(
                    string("Invalid lane number (edge '") + id
                    + string("')."));
                throw ProcessError();
            }
        }
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform(
            string("Non-numerical value for an edge's lane number occured (edge '") + id
            + string("'."));
        throw ProcessError();
    }

    // try to get the nodes
    NBNode *from = myNodeCont.retrieve(fromID);
    NBNode *to = myNodeCont.retrieve(toID);
    if(from==0) {
        MsgHandler::getErrorInstance()->inform(
            string("The from-node '") + fromID + string("' of edge '")
            + id + string("' could not be found"));
        throw ProcessError();
    }
    if(to==0) {
        MsgHandler::getErrorInstance()->inform(
            string("The to-node '") + toID + string("' of edge '")
            + id + string("' could not be found"));
        throw ProcessError();
    }

    NBEdge *e =
        new NBEdge(id, id, from, to, "DEFAULT",
            speed, nolanes, length, priority);

    if(!myEdgeCont.insert(e)) {
        delete e;
        MsgHandler::getErrorInstance()->inform(
            string("Could not add edge '") + id + string("'."));
        throw ProcessError();
    }
    return true;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



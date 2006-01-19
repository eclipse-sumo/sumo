/***************************************************************************
                          NIElmar2NodesHandler.cpp
             A LineHandler-derivate to load nodes form a elmar-nodes-file
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
#include <utils/geom/GeomHelper.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIElmar2NodesHandler.h"

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
NIElmar2NodesHandler::NIElmar2NodesHandler(NBNodeCont &nc,
                                           const std::string &file,
                                           SUMOReal centerX, SUMOReal centerY,
                                           std::map<std::string, Position2DVector> &geoms)
    : FileErrorReporter("elmar-nodes", file),
    myInitX(centerX), myInitY(centerY),
    myNodeCont(nc), myGeoms(geoms)
{
    myInitX /= 100000.0;
    myInitY /= 100000.0;
}


NIElmar2NodesHandler::~NIElmar2NodesHandler()
{
}


bool
NIElmar2NodesHandler::report(const std::string &result)
{
    if(result[0]=='#') {
        return true;
    }
    string id;
    SUMOReal x, y;
    int no_geoms, intermediate;
    StringTokenizer st(result, StringTokenizer::WHITECHARS);
    // check
    if(st.size()<5) {
        MsgHandler::getErrorInstance()->inform(
            "Something is wrong with the following data line");
        MsgHandler::getErrorInstance()->inform(
            result);
        throw ProcessError();
    }
    // parse
        // id
    id = st.next();
        // intermediate?
    try {
        intermediate = TplConvert<char>::_2int(st.next().c_str());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform(
            "Non-numerical value for internmediate y/n occured.");
        throw ProcessError();
    }
        // number of geometrical information
    try {
        no_geoms = TplConvert<char>::_2int(st.next().c_str());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform(
            "Non-numerical value for number of nodes occured.");
        throw ProcessError();
    }
        // geometrical information
    Position2DVector geoms;
    for(int i=0; i<no_geoms; i++) {
        try {
            x = (SUMOReal) TplConvert<char>::_2SUMOReal(st.next().c_str());
        } catch (NumberFormatException &) {
            MsgHandler::getErrorInstance()->inform(
                "Non-numerical value for node-x-position occured.");
            throw ProcessError();
        }
        try {
            y = (SUMOReal) TplConvert<char>::_2SUMOReal(st.next().c_str());
        } catch (NumberFormatException &) {
            MsgHandler::getErrorInstance()->inform(
                "Non-numerical value for node-y-position occured.");
            throw ProcessError();
        }
        x = (SUMOReal) (x / 100000.0);
        y = (SUMOReal) (y / 100000.0);
        SUMOReal ys = y;
        x = (x-myInitX);
        y = (y-myInitY);
        SUMOReal x1 = (SUMOReal) (x * 111.320*1000.);
        SUMOReal y1 = (SUMOReal) (y * 111.136*1000.);
        x1 *= (SUMOReal) cos(ys*PI/180.0);
        geoms.push_back(Position2D(x1, y1));
    }
    // geo->metric
//    x = -1.0 * (x-myInitX)
//        * (SUMOReal) 111.320 * /*(SUMOReal) 1000.0 * */cos(y / (SUMOReal) 10000.0*PI/180.0)
//        / (SUMOReal) 10.0; // 10000.0
//    y = (y-myInitY)
//        * (SUMOReal) 111.136 /* * (SUMOReal) 1000.0*/
//        / (SUMOReal) 10.0; // 10000.0

//    y1 *= 4.0/2.0;

    if(intermediate==0) {
        NBNode *n = new NBNode(id, geoms.at(0));
        if(!myNodeCont.insert(n)) {
            delete n;
            MsgHandler::getErrorInstance()->inform(
                string("Could not add node '") + id + string("'."));
            throw ProcessError();
        }
    } else {
        myGeoms[id] = geoms;
    }
    return true;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



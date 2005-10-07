/***************************************************************************
                          NIElmarNodesHandler.cpp
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
// Revision 1.7  2005/10/07 11:39:26  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:02:15  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/04/27 12:24:35  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.3  2004/11/23 10:23:52  dkrajzew
// debugging
//
// Revision 1.2  2004/08/02 12:44:12  dkrajzew
// using Position2D instead of two SUMOReals
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
#include <utils/geom/GeomHelper.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIElmarNodesHandler.h"

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
NIElmarNodesHandler::NIElmarNodesHandler(NBNodeCont &nc,
                                         const std::string &file,
                                         SUMOReal centerX, SUMOReal centerY)
    : FileErrorReporter("elmar-nodes", file),
    myCurrentLine(0), myInitX(centerX), myInitY(centerY),
    myNodeCont(nc)
{
    myInitX /= 100000.0;
    myInitY /= 100000.0;
}


NIElmarNodesHandler::~NIElmarNodesHandler()
{
}


bool
NIElmarNodesHandler::report(const std::string &result)
{
    if(result[0]=='#') {
        return true;
    }
    // skip previous information
    while(++myCurrentLine<6) {
        return true;
    }
    string id;
    SUMOReal x, y;
    StringTokenizer st(result, StringTokenizer::WHITECHARS);
    // check
    if(st.size()<3) {
        MsgHandler::getErrorInstance()->inform(
            "Something is wrong with the following data line");
        MsgHandler::getErrorInstance()->inform(
            result);
        throw ProcessError();
    }
    // parse
    id = st.next();
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
    // geo->metric
//    x = -1.0 * (x-myInitX)
//        * (SUMOReal) 111.320 * /*(SUMOReal) 1000.0 * */cos(y / (SUMOReal) 10000.0*PI/180.0)
//        / (SUMOReal) 10.0; // 10000.0
//    y = (y-myInitY)
//        * (SUMOReal) 111.136 /* * (SUMOReal) 1000.0*/
//        / (SUMOReal) 10.0; // 10000.0
    x = x / (SUMOReal) 100000.0;
    y = y / (SUMOReal) 100000.0;
    SUMOReal ys = y;
    x = (x-myInitX);
    y = (y-myInitY);
    SUMOReal x1 = (SUMOReal) (x * 111.320*1000.);
    SUMOReal y1 = (SUMOReal) (y * 111.136*1000.);
    x1 *= (SUMOReal) cos(ys*PI/180.0);
//    y1 *= 4.0/2.0;

    NBNode *n = new NBNode(id, Position2D(x1, y1));
    if(!myNodeCont.insert(n)) {
        delete n;
        MsgHandler::getErrorInstance()->inform(
            string("Could not add node '") + id + string("'."));
        throw ProcessError();
    }
    return true;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



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
// Revision 1.1  2004/07/02 09:34:38  dkrajzew
// elmar and tiger import added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <utils/importio/LineHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/convert/TplConvert.h>
#include <utils/geom/GeomHelper.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIElmarNodesHandler.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NIElmarNodesHandler::NIElmarNodesHandler(const std::string &file,
                                         double centerX, double centerY)
    : FileErrorReporter("elmar-nodes", file),
    myCurrentLine(0), myInitX(centerX), myInitY(centerY)
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
    // skip previous information
    while(++myCurrentLine<6) {
        return true;
    }
    string id;
    double x, y;
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
        x = (double) TplConvert<char>::_2float(st.next().c_str());
    } catch (NumberFormatException &e) {
        MsgHandler::getErrorInstance()->inform(
            "Non-numerical value for node-x-position occured.");
        throw ProcessError();
    }
    try {
        y = (double) TplConvert<char>::_2float(st.next().c_str());
    } catch (NumberFormatException &e) {
        MsgHandler::getErrorInstance()->inform(
            "Non-numerical value for node-y-position occured.");
        throw ProcessError();
    }
    // geo->metric
//    x = -1.0 * (x-myInitX)
//        * (double) 111.320 * /*(double) 1000.0 * */cos(y / (double) 10000.0*PI/180.0)
//        / (double) 10.0; // 10000.0
//    y = (y-myInitY)
//        * (double) 111.136 /* * (double) 1000.0*/
//        / (double) 10.0; // 10000.0
    x = x / 100000.0;
    y = y / 100000.0;
    double ys = y;
    x = (x-myInitX);
    y = (y-myInitY);
    double x1 = x * 111.320*1000;
    double y1 = y * 111.136*1000;
    x1 *= cos(ys*PI/180.0);
//    y1 *= 4.0/2.0;

    NBNode *n = new NBNode(id, x1, y1);
    if(!NBNodeCont::insert(n)) {
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



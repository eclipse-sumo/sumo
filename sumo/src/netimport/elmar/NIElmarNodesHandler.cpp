/****************************************************************************/
/// @file    NIElmarNodesHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sun, 16 May 2004
/// @version $Id$
///
// A LineHandler-derivate to load nodes form a elmar-nodes-file
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
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
#include <utils/geom/GeomHelper.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIElmarNodesHandler.h"
#include <utils/geoconv/GeoConvHelper.h>

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
NIElmarNodesHandler::NIElmarNodesHandler(NBNodeCont &nc,
        const std::string &file)
        : FileErrorReporter("elmar-nodes", file),
        myInitX(-1), myInitY(-1),
        myNodeCont(nc)
{}


NIElmarNodesHandler::~NIElmarNodesHandler()
{}


bool
NIElmarNodesHandler::report(const std::string &result)
{
    if (result[0]=='#') {
        return true;
    }
    // skip previous information
    string id;
    SUMOReal x, y;
    StringTokenizer st(result, StringTokenizer::WHITECHARS);
    // check
    if (st.size()<3) {
        MsgHandler::getErrorInstance()->inform("Something is wrong with the following data line\n" + result);
        throw ProcessError();
    }
    // parse
    id = st.next();
    try {
        x = (SUMOReal) TplConvert<char>::_2SUMOReal(st.next().c_str());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Non-numerical value for node-x-position occured.");
        throw ProcessError();
    }
    try {
        y = (SUMOReal) TplConvert<char>::_2SUMOReal(st.next().c_str());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Non-numerical value for node-y-position occured.");
        throw ProcessError();
    }
    // geo->metric
    myNodeCont.addGeoreference(Position2D((SUMOReal)(x / 100000.0), (SUMOReal)(y / 100000.0)));
    Position2D pos(x, y);
    GeoConvHelper::remap(pos);
    NBNode *n = new NBNode(id, pos);
    if (!myNodeCont.insert(n)) {
        delete n;
        MsgHandler::getErrorInstance()->inform("Could not add node '" + id + "'.");
        throw ProcessError();
    }
    return true;
}



/****************************************************************************/


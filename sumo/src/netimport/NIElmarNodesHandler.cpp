/****************************************************************************/
/// @file    NIElmarNodesHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sun, 16 May 2004
/// @version $Id:NIElmarNodesHandler.cpp 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer of nodes stored in split elmar format
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
#include <utils/geom/GeomHelper.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include "NIElmarNodesHandler.h"
#include <utils/geom/GeoConvHelper.h>

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
        const std::string &file) throw()
        : myNodeCont(nc)
{}


NIElmarNodesHandler::~NIElmarNodesHandler() throw()
{}


bool
NIElmarNodesHandler::report(const std::string &result) throw(ProcessError)
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
        throw ProcessError("Something is wrong with the following data line\n" + result);
    }
    // parse
    id = st.next();
    try {
        x = (SUMOReal) TplConvert<char>::_2SUMOReal(st.next().c_str());
    } catch (NumberFormatException &) {
        throw ProcessError("Non-numerical value for node-x-position occured.");
    }
    try {
        y = (SUMOReal) TplConvert<char>::_2SUMOReal(st.next().c_str());
    } catch (NumberFormatException &) {
        throw ProcessError("Non-numerical value for node-y-position occured.");
    }
    // geo->metric
    Position2D pos(x, y);
    GeoConvHelper::x2cartesian(pos);
    NBNode *n = new NBNode(id, pos);
    if (!myNodeCont.insert(n)) {
        delete n;
        throw ProcessError("Could not add node '" + id + "'.");
    }
    return true;
}



/****************************************************************************/


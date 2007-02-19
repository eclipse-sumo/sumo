/****************************************************************************/
/// @file    NIElmar2NodesHandler.cpp
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
#include "NIElmar2NodesHandler.h"
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
NIElmar2NodesHandler::NIElmar2NodesHandler(NBNodeCont &nc,
        const std::string &file,
        std::map<std::string, Position2DVector> &geoms)
        : FileErrorReporter("elmar-nodes", file),
        myInitX(-1), myInitY(-1),
        myNodeCont(nc), myGeoms(geoms)
{}


NIElmar2NodesHandler::~NIElmar2NodesHandler()
{}


bool
NIElmar2NodesHandler::report(const std::string &result)
{
    if (result[0]=='#') {
        return true;
    }
    string id;
    SUMOReal x, y;
    int no_geoms, intermediate;
    StringTokenizer st(result, StringTokenizer::WHITECHARS);
    // check
    if (st.size()<5) {
        MsgHandler::getErrorInstance()->inform("Something is wrong with the following data line\n" + result);
        throw ProcessError();
    }
    // parse
    // id
    id = st.next();
    // intermediate?
    try {
        intermediate = TplConvert<char>::_2int(st.next().c_str());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Non-numerical value for internmediate y/n occured.");
        throw ProcessError();
    }
    // number of geometrical information
    try {
        no_geoms = TplConvert<char>::_2int(st.next().c_str());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Non-numerical value for number of nodes occured.");
        throw ProcessError();
    }
    // geometrical information
    Position2DVector geoms;
    for (int i=0; i<no_geoms; i++) {
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

        myNodeCont.addGeoreference(Position2D((SUMOReal)(x / 100000.0), (SUMOReal)(y / 100000.0)));
        Position2D pos(x, y);
        GeoConvHelper::remap(pos);
        geoms.push_back(pos);
    }

    if (intermediate==0) {
        NBNode *n = new NBNode(id, geoms[0]);
        if (!myNodeCont.insert(n)) {
            delete n;
            MsgHandler::getErrorInstance()->inform("Could not add node '" + id + "'.");
            throw ProcessError();
        }
    } else {
        myGeoms[id] = geoms;
    }
    return true;
}



/****************************************************************************/


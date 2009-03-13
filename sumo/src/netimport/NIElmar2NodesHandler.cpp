/****************************************************************************/
/// @file    NIElmar2NodesHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sun, 16 May 2004
/// @version $Id$
///
// Importer of nodes stored in unsplit elmar format
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
#include "NIElmar2NodesHandler.h"
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
NIElmar2NodesHandler::NIElmar2NodesHandler(NBNodeCont &nc,
        const std::string &file,
        std::map<std::string, Position2DVector> &geoms) throw()
        : myNodeCont(nc), myGeoms(geoms) {}


NIElmar2NodesHandler::~NIElmar2NodesHandler() throw() {}


bool
NIElmar2NodesHandler::report(const std::string &result) throw(ProcessError) {
    if (result[0]=='#') {
        return true;
    }
    string id;
    SUMOReal x, y;
    int no_geoms, intermediate;
    StringTokenizer st(result, StringTokenizer::WHITECHARS);
    // check
    if (st.size()<5) {
        throw ProcessError("Something is wrong with the following data line\n" + result);
    }
    // parse
    // id
    id = st.next();
    // intermediate?
    try {
        intermediate = TplConvert<char>::_2int(st.next().c_str());
    } catch (NumberFormatException &) {
        throw ProcessError("Non-numerical value for internmediate y/n occured.");
    }
    // number of geometrical information
    try {
        no_geoms = TplConvert<char>::_2int(st.next().c_str());
    } catch (NumberFormatException &) {
        throw ProcessError("Non-numerical value for number of nodes occured.");
    }
    // geometrical information
    Position2DVector geoms;
    for (int i=0; i<no_geoms; i++) {
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

        Position2D pos(x, y);
        GeoConvHelper::x2cartesian(pos);
        geoms.push_back(pos);
    }

    if (intermediate==0) {
        NBNode *n = new NBNode(id, geoms[0]);
        if (!myNodeCont.insert(n)) {
            delete n;
            throw ProcessError("Could not add node '" + id + "'.");
        }
    } else {
        myGeoms[id] = geoms;
    }
    return true;
}



/****************************************************************************/


/****************************************************************************/
/// @file    NIVisumParser_Nodes.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 14 Nov 2002
/// @version $Id$
///
// Parser for visum-nodes
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/TplConvert.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_Nodes.h"
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
NIVisumParser_Nodes::NIVisumParser_Nodes(NIVisumLoader &parent,
        NBNodeCont &nc, const std::string &dataName)
        : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
        myNodeCont(nc)
{}


NIVisumParser_Nodes::~NIVisumParser_Nodes()
{}


void
NIVisumParser_Nodes::myDependentReport()
{
    string id;
    try {
        // get the id
        id = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
        // get the position
        SUMOReal x = getNamedFloat("XKoord");
        SUMOReal y = getNamedFloat("YKoord");
        Position2D pos(x, y);
        GeoConvHelper::x2cartesian(pos);
        // add to the list
        if (!myNodeCont.insert(id, pos)) {
            addError(" Duplicate node occured ('" + id + "').");
        }
    } catch (OutOfBoundsException &) {
        addError2("KNOTEN", id, "OutOfBounds");
    } catch (NumberFormatException &) {
        addError2("KNOTEN", id, "NumberFormat");
    } catch (UnknownElement &) {
        addError2("KNOTEN", id, "UnknownElement");
    }
}



/****************************************************************************/


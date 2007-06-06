/****************************************************************************/
/// @file    NIArtemisParser_Links.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10 Feb 2003
/// @version $Id$
///
// -------------------
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

#include <utils/common/UtilExceptions.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <utils/common/TplConvert.h>
#include "NIArtemisLoader.h"
#include "NIArtemisParser_Links.h"

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
NIArtemisParser_Links::NIArtemisParser_Links(
    NBNodeCont &nc, NBEdgeCont &ec, NIArtemisLoader &parent,
    const std::string &dataName)
        : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName),
        myNodeCont(nc), myEdgeCont(ec)
{}


NIArtemisParser_Links::~NIArtemisParser_Links()
{}


void
NIArtemisParser_Links::myDependentReport()
{
    string from = myLineParser.get("From");
    string to = myLineParser.get("To");
    string id = myLineParser.get("ID");
    string name = myLineParser.get("Name");
    SUMOReal length =
        TplConvert<char>::_2SUMOReal(myLineParser.get("Length").c_str());
    SUMOReal speed =
        TplConvert<char>::_2SUMOReal(myLineParser.get("Speed").c_str());
    // costfact omitted
    int laneno =
        TplConvert<char>::_2int(myLineParser.get("NumLanes").c_str());
    // rest omitted (by now)
// !!! right-of-way
// !!! prio
    // get nodes
    NBNode *fromNode = myNodeCont.retrieve(from);
    NBNode *toNode = myNodeCont.retrieve(to);
    // build if both nodes were found
    if (fromNode!=0&&toNode!=0) {
        NBEdge *edge = new NBEdge(id, name, fromNode, toNode,
                                  "", speed, laneno, length, -1,
                                  NBEdge::LANESPREAD_RIGHT, NBEdge::EDGEFUNCTION_NORMAL); // !!! validate spread
        myEdgeCont.insert(edge);
    }
}



/****************************************************************************/


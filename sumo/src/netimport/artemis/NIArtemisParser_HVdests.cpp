/****************************************************************************/
/// @file    NIArtemisParser_HVdests.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 13 Mar 2003
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
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIArtemisLoader.h"
#include "NIArtemisParser_HVdests.h"

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
NIArtemisParser_HVdests::NIArtemisParser_HVdests(NBNodeCont &nc,
        NBEdgeCont &ec,
        NIArtemisLoader &parent,
        const std::string &dataName)
        : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName),
        myNodeCont(nc), myEdgeCont(ec)
{}


NIArtemisParser_HVdests::~NIArtemisParser_HVdests()
{}


void
NIArtemisParser_HVdests::myDependentReport()
{
    string origid = myLineParser.get("NodeID");
    string destid = myLineParser.get("DestID");
    // retrieve nodes
    NBNode *node1 = myNodeCont.retrieve(origid);
    NBNode *node2 = myNodeCont.retrieve(destid);
    // check
    if (node1==0) {
        MsgHandler::getErrorInstance()->inform("Could not find node '" + origid + "'");
        return;
    }
    if (node2==0) {
        MsgHandler::getErrorInstance()->inform("Could not find node '" + destid + "'");
        return;
    }
    // try to build a source into node1
    Position2D dir1 = node1->getEmptyDir();
    Position2D dir2 = node2->getEmptyDir();
    if (myEdgeCont.retrieve(origid + "SOURCE")==0) {
        dir1.mul(10.0);
        dir1.add(node1->getPosition());
        NBNode *tmp = new NBNode(origid + "SOURCENode", dir1);
        myNodeCont.insert(tmp); // !!! check
        NBEdge *edge = new NBEdge(origid + "SOURCE", origid + "SOURCE",
                                  tmp, node1, "", 20.0, 2, -1, NBEdge::LANESPREAD_RIGHT,
                                  NBEdge::EDGEFUNCTION_SOURCE);
        myEdgeCont.insert(edge); // !!! check
    }
    // try to build a sink from node2
    if (myEdgeCont.retrieve(destid + "SINK")==0) {
        dir2.mul(10.0);
        dir2.add(node2->getPosition());
        NBNode *tmp = new NBNode(destid + "SINKNode", dir2);
        myNodeCont.insert(tmp); // !!! check
        NBEdge *edge = new NBEdge(destid + "SINK", destid + "SINK",
                                  node2, tmp, "", 20.0, 2, -1, NBEdge::LANESPREAD_RIGHT,
                                  NBEdge::EDGEFUNCTION_SINK);
        myEdgeCont.insert(edge); // !!! check
    }
}



/****************************************************************************/


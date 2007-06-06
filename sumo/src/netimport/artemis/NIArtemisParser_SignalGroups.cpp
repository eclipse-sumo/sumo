/****************************************************************************/
/// @file    NIArtemisParser_SignalGroups.cpp
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

#include <string>
#include <map>
#include <netbuild/NBConnectionDefs.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include "NIArtemisLoader.h"
#include "NIArtemisTempSignal.h"
#include "NIArtemisParser_SignalGroups.h"

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
NIArtemisParser_SignalGroups::NIArtemisParser_SignalGroups(
    NBNodeCont &nc,
    NIArtemisLoader &parent,
    const std::string &dataName)
        : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName),
        myNodeCont(nc)
{}


NIArtemisParser_SignalGroups::~NIArtemisParser_SignalGroups()
{}


void
NIArtemisParser_SignalGroups::myDependentReport()
{
    string nodeid = myLineParser.get("NodeID");
    string group = myLineParser.get("Group");
    string from = myLineParser.get("From");
    string to = myLineParser.get("To");
    // "isArrow" omitted
    char startPhase = myLineParser.get("StartPhase").at(0); // !!! insecure when string empty
    char endPhase = myLineParser.get("EndPhase").at(0); // !!! insecure when string empty
    // "satflow" omitted
    // get the according structures
    NBNode *node = myNodeCont.retrieve(nodeid);
    NBNode *fromNode = myNodeCont.retrieve(from);
    NBNode *toNode = myNodeCont.retrieve(to);
    // check whether the node is valid
    if (node==0) {
        MsgHandler::getErrorInstance()->inform("The node '" + nodeid + "' is not known within a signal group.");
        return;
    }
    // check whether the incoming edge is valid
    if (fromNode==0) {
        MsgHandler::getErrorInstance()->inform("The from node '" + from + "' is not known within a signal group.");
        return;
    }
    // check whether the outgoing edge is valid
    if (toNode==0) {
        MsgHandler::getErrorInstance()->inform("The to node '" + to + "' is not known within a signal group.");
        return;
    }
    NBEdge *fromEdge =
        NBContHelper::findConnectingEdge(
            node->getIncomingEdges(), fromNode, node);
    NBEdge *toEdge =
        NBContHelper::findConnectingEdge(
            node->getOutgoingEdges(), node, toNode);
    if (fromEdge==0) {
        MsgHandler::getErrorInstance()->inform("Could not find connection between '" + from + "' and '" + nodeid + "' within a signal group.");
        return;
    }
    if (toEdge==0) {
        MsgHandler::getErrorInstance()->inform("Could not find connection between '" + nodeid + "' and '" + to + "' within a signal group.");
        return;
    }
    // compute the phases
    NBConnection c(fromEdge, toEdge);
    NIArtemisTempSignal::addConnectionPhases(c, startPhase, endPhase, group);
}

void
NIArtemisParser_SignalGroups::myClose()
{
    NIArtemisTempSignal::close();
}



/****************************************************************************/


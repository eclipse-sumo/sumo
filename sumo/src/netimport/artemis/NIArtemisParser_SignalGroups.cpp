/***************************************************************************
                          NIArtemisParser_SignalGroups.cpp
                             -------------------
    project              : SUMO
    begin                : Mon, 10 Feb 2003
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.5  2003/06/18 11:14:13  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.4  2003/05/20 09:37:13  dkrajzew
// patch to current netbuilder-API; still not working properly
//
// Revision 1.3  2003/03/17 14:18:58  dkrajzew
// Windows eol removed
//
// Revision 1.2  2003/03/12 16:44:47  dkrajzew
// further work on artemis-import
//
// Revision 1.1  2003/03/03 15:00:31  dkrajzew
// initial commit for artemis-import files
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <map>
#include <netbuild/NBConnectionDefs.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/convert/TplConvert.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include "NIArtemisLoader.h"
#include "NIArtemisTempSignal.h"
#include "NIArtemisParser_SignalGroups.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIArtemisParser_SignalGroups::NIArtemisParser_SignalGroups(NIArtemisLoader &parent,
        const std::string &dataName)
    : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName)
{
}


NIArtemisParser_SignalGroups::~NIArtemisParser_SignalGroups()
{
}


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
    NBNode *node = NBNodeCont::retrieve(nodeid);
    NBNode *fromNode = NBNodeCont::retrieve(from);
    NBNode *toNode = NBNodeCont::retrieve(to);
        // check whether the node is valid
    if(node==0) {
        MsgHandler::getErrorInstance()->inform(
            string("The node '") + nodeid + string("' is not known within a signal group."));
        return;
    }
        // check whether the incoming edge is valid
    if(fromNode==0) {
        MsgHandler::getErrorInstance()->inform(
            string("The from node '") + from + string("' is not known within a signal group."));
        return;
    }
        // check whether the outgoing edge is valid
    if(toNode==0) {
        MsgHandler::getErrorInstance()->inform(
            string("The to node '") + to + string("' is not known within a signal group."));
        return;
    }
    NBEdge *fromEdge =
        NBContHelper::findConnectingEdge(
            node->getIncomingEdges(), fromNode, node);
    NBEdge *toEdge =
        NBContHelper::findConnectingEdge(
            node->getOutgoingEdges(), node, toNode);
    if(fromEdge==0) {
        MsgHandler::getErrorInstance()->inform(
            string("Could not find connection between '") + from
            + string("' and '") + nodeid + string("' within a signal group."));
        return;
    }
    if(toEdge==0) {
        MsgHandler::getErrorInstance()->inform(
            string("Could not find connection between '") + nodeid
            + string("' and '") + to + string("' within a signal group."));
        return;
    }
    // compute the phases
    NBConnection c(fromEdge, toEdge);
    NIArtemisTempSignal::addConnectionPhases(c, startPhase, endPhase,
        group);
}

void
NIArtemisParser_SignalGroups::myClose()
{
    NIArtemisTempSignal::close();
}





/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIArtemisParser_SignalGroups.icc"
//#endif

// Local Variables:
// mode:C++
// End:




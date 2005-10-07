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
// Revision 1.10  2005/10/07 11:39:05  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.9  2005/09/23 06:01:53  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.8  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.7  2005/04/27 12:24:25  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.6  2004/01/12 15:30:31  dkrajzew
// node-building classes are now lying in an own folder
//
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

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

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIArtemisParser_SignalGroups::NIArtemisParser_SignalGroups(
        NBNodeCont &nc,
        NIArtemisLoader &parent,
        const std::string &dataName)
    : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName),
    myNodeCont(nc)
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
    NBNode *node = myNodeCont.retrieve(nodeid);
    NBNode *fromNode = myNodeCont.retrieve(from);
    NBNode *toNode = myNodeCont.retrieve(to);
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

// Local Variables:
// mode:C++
// End:




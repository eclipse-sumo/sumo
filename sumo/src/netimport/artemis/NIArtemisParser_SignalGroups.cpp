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
#include <utils/common/SErrorHandler.h>
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
    NBEdge *fromEdge = NBEdgeCont::retrieve(from);
    NBEdge *toEdge = NBEdgeCont::retrieve(to);
        // check whether the node is valid
    if(node==0) {
        SErrorHandler::add(
            string("The node '") + from + string("' is not known within a signal group."));
        return;
    }
        // check whether the incoming edge is valid
    if(fromEdge==0) {
        SErrorHandler::add(
            string("The from edge '") + from + string("' is not known within a signal group."));
        return;
    }
        // check whether the outgoing edge is valid
    if(toEdge==0) {
        SErrorHandler::add(
            string("The to edge '") + from + string("' is not known within a signal group."));
        return;
    }
    // compute the phases
    Connection c(fromEdge, toEdge);
    NIArtemisTempSignal::addConnectionPhases(c, startPhase, endPhase,
        group);
//    !!! insert
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIArtemisParser_SignalGroups.icc"
//#endif

// Local Variables:
// mode:C++
// End:




/***************************************************************************
                          NIArtemisParser_HVdests.cpp
                             -------------------
    project              : SUMO
    begin                : Wed, 13 Mar 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.5  2003/11/11 08:29:48  dkrajzew
// consequent geometry usage
//
// Revision 1.4  2003/07/07 08:25:33  dkrajzew
// adapted the importer to the lane geometry description
//
// Revision 1.3  2003/06/18 11:14:13  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/03/17 14:18:58  dkrajzew
// Windows eol removed
//
// Revision 1.1  2003/03/12 16:44:45  dkrajzew
// further work on artemis-import
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/convert/TplConvert.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include "NIArtemisLoader.h"
#include "NIArtemisParser_HVdests.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIArtemisParser_HVdests::NIArtemisParser_HVdests(NIArtemisLoader &parent,
        const std::string &dataName)
    : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName)
{
}


NIArtemisParser_HVdests::~NIArtemisParser_HVdests()
{
}


void
NIArtemisParser_HVdests::myDependentReport()
{
    string origid = myLineParser.get("NodeID");
    string destid = myLineParser.get("DestID");
    // retrieve nodes
    NBNode *node1 = NBNodeCont::retrieve(origid);
    NBNode *node2 = NBNodeCont::retrieve(destid);
    // check
    if(node1==0) {
        MsgHandler::getErrorInstance()->inform(
            string("Could not find node '") + origid + string("'"));
        return;
    }
    if(node2==0) {
        MsgHandler::getErrorInstance()->inform(
            string("Could not find node '") + destid + string("'"));
        return;
    }
    // try to build a source into node1
    Position2D dir1 = node1->getEmptyDir();
    Position2D dir2 = node2->getEmptyDir();
    if(NBEdgeCont::retrieve(origid + "SOURCE")==0) {
        dir1.mul(10.0);
        dir1.add(node1->getPosition());
        NBNode *tmp = new NBNode(origid + "SOURCENode", dir1.x(), dir1.y());
        NBNodeCont::insert(tmp); // !!! check
        NBEdge *edge = new NBEdge(origid + "SOURCE", origid + "SOURCE",
            tmp, node1, "", 20.0, 2, -1, 0, NBEdge::LANESPREAD_RIGHT,
            NBEdge::EDGEFUNCTION_SOURCE);
        NBEdgeCont::insert(edge); // !!! check
    }
    // try to build a sink from node2
    if(NBEdgeCont::retrieve(destid + "SINK")==0) {
        dir2.mul(10.0);
        dir2.add(node2->getPosition());
        NBNode *tmp = new NBNode(destid + "SINKNode", dir2.x(), dir2.y());
        NBNodeCont::insert(tmp); // !!! check
        NBEdge *edge = new NBEdge(destid + "SINK", destid + "SINK",
            node2, tmp, "", 20.0, 2, -1, 0, NBEdge::LANESPREAD_RIGHT,
            NBEdge::EDGEFUNCTION_SINK);
        NBEdgeCont::insert(edge); // !!! check
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIArtemisParser_HVdests.icc"
//#endif

// Local Variables:
// mode:C++
// End:



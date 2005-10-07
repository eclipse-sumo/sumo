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
// Revision 1.11  2005/10/07 11:39:05  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.10  2005/09/23 06:01:53  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.9  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/04/27 12:24:25  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.7  2004/08/02 12:44:11  dkrajzew
// using Position2D instead of two SUMOReals
//
// Revision 1.6  2004/01/12 15:30:31  dkrajzew
// node-building classes are now lying in an own folder
//
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

#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIArtemisLoader.h"
#include "NIArtemisParser_HVdests.h"

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
NIArtemisParser_HVdests::NIArtemisParser_HVdests(NBNodeCont &nc,
                                                 NBEdgeCont &ec,
                                                 NIArtemisLoader &parent,
                                                 const std::string &dataName)
    : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName),
    myNodeCont(nc), myEdgeCont(ec)
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
    NBNode *node1 = myNodeCont.retrieve(origid);
    NBNode *node2 = myNodeCont.retrieve(destid);
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
    if(myEdgeCont.retrieve(origid + "SOURCE")==0) {
        dir1.mul(10.0);
        dir1.add(node1->getPosition());
        NBNode *tmp = new NBNode(origid + "SOURCENode", dir1);
        myNodeCont.insert(tmp); // !!! check
        NBEdge *edge = new NBEdge(origid + "SOURCE", origid + "SOURCE",
            tmp, node1, "", 20.0, 2, -1, 0, NBEdge::LANESPREAD_RIGHT,
            NBEdge::EDGEFUNCTION_SOURCE);
        myEdgeCont.insert(edge); // !!! check
    }
    // try to build a sink from node2
    if(myEdgeCont.retrieve(destid + "SINK")==0) {
        dir2.mul(10.0);
        dir2.add(node2->getPosition());
        NBNode *tmp = new NBNode(destid + "SINKNode", dir2);
        myNodeCont.insert(tmp); // !!! check
        NBEdge *edge = new NBEdge(destid + "SINK", destid + "SINK",
            node2, tmp, "", 20.0, 2, -1, 0, NBEdge::LANESPREAD_RIGHT,
            NBEdge::EDGEFUNCTION_SINK);
        myEdgeCont.insert(edge); // !!! check
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



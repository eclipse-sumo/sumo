/***************************************************************************
                          NIArtemisParser_Links.cpp
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
// Revision 1.7  2005/09/23 06:01:53  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/04/27 12:24:25  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.4  2004/01/12 15:30:31  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.3  2003/07/07 08:25:33  dkrajzew
// adapted the importer to the lane geometry description
//
// Revision 1.2  2003/03/17 14:18:58  dkrajzew
// Windows eol removed
//
// Revision 1.1  2003/03/03 15:00:29  dkrajzew
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include <utils/common/UtilExceptions.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <utils/common/TplConvert.h>
#include "NIArtemisLoader.h"
#include "NIArtemisParser_Links.h"

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
NIArtemisParser_Links::NIArtemisParser_Links(
        NBNodeCont &nc, NBEdgeCont &ec, NIArtemisLoader &parent,
        const std::string &dataName)
    : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName),
    myNodeCont(nc), myEdgeCont(ec)
{
}


NIArtemisParser_Links::~NIArtemisParser_Links()
{
}


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
    if(fromNode!=0&&toNode!=0) {
        NBEdge *edge = new NBEdge(id, name, fromNode, toNode,
            "", speed, laneno, length, 0,
            NBEdge::LANESPREAD_RIGHT, NBEdge::EDGEFUNCTION_NORMAL); // !!! validate spread
        myEdgeCont.insert(edge);
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/common/UtilExceptions.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <utils/convert/TplConvert.h>
#include "NIArtemisLoader.h"
#include "NIArtemisParser_Links.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIArtemisParser_Links::NIArtemisParser_Links(NIArtemisLoader &parent,
        const std::string &dataName)
    : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName)
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
    double length =
        TplConvert<char>::_2float(myLineParser.get("Length").c_str());
    double speed =
        TplConvert<char>::_2float(myLineParser.get("Speed").c_str());
    // costfact omitted
    int laneno =
        TplConvert<char>::_2int(myLineParser.get("NumLanes").c_str());
    // rest omitted (by now)
// !!! right-of-way
// !!! prio
    // get nodes
    NBNode *fromNode = NBNodeCont::retrieve(from);
    NBNode *toNode = NBNodeCont::retrieve(to);
    // build if both nodes were found
    if(fromNode!=0&&toNode!=0) {
        NBEdge *edge = new NBEdge(id, name, fromNode, toNode,
            "", speed, laneno, length, 0,
            NBEdge::LANESPREAD_RIGHT, NBEdge::EDGEFUNCTION_NORMAL); // !!! validate spread
        NBEdgeCont::insert(edge);
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIArtemisParser_Links.icc"
//#endif

// Local Variables:
// mode:C++
// End:



/***************************************************************************
                          NIArtemisParser_Nodes.cpp
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
// Revision 1.2  2003/03/06 17:14:39  dkrajzew
// more stringent usage of insertion into containers; y-direction flipped
//
// Revision 1.1  2003/03/03 15:00:31  dkrajzew
// initial commit for artemis-import files
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/common/UtilExceptions.h>
#include <utils/common/SErrorHandler.h>
#include <utils/convert/TplConvert.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include "NIArtemisLoader.h"
#include "NIArtemisParser_Nodes.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIArtemisParser_Nodes::NIArtemisParser_Nodes(NIArtemisLoader &parent,
        const std::string &dataName)
    : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName)
{
}


NIArtemisParser_Nodes::~NIArtemisParser_Nodes()
{
}


void
NIArtemisParser_Nodes::myDependentReport()
{
    string id = myLineParser.get("IDnum");
    string name = myLineParser.get("Name");
    double x = TplConvert<char>::_2float(myLineParser.get("X").c_str());
    double y = TplConvert<char>::_2float(myLineParser.get("Y").c_str());
    // size omitted
    int type = TplConvert<char>::_2int(myLineParser.get("Type").c_str());
    string myType;
    // radius omitted
    switch(type) {
    case 0:
        // no function (virtual one-way)
        myType = "no_junction";
        break;
    case 1:
        // no function (virtual two-way)
        myType = "no_junction";
        break;
    case 2:
        // origin/generator
//        !!!
        break;
    case 3:
        // destination
//        !!!
        break;
    case 4:
        // origin/destination
//        !!!
        break;
    case 5:
        // signalised
        myType = "traffic_light";
        break;
    case 6:
        // unsignalised
        myType = "priority_junction";
        break;
    case 7:
        // roundabout
        myType = "priority_junction"; // !!!
        break;
    case 8:
        // merge
        myType = "priority_junction"; // !!!
        break;
    case 9:
        // diverge
        myType = "priority_junction"; // !!!
        break;
    case 10:
        // signalised roundabout
        myType = "traffic_light"; // !!!
        break;
    default:
        addError("Unsupported junction type.");
        break;
    }
    // build if ok
    if(!SErrorHandler::errorOccured()) {
        NBNode *node = new NBNode(id, x, y, myType);
        if(!NBNodeCont::insert(node)) {
            delete node;
        }
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIArtemisParser_Nodes.icc"
//#endif

// Local Variables:
// mode:C++
// End:



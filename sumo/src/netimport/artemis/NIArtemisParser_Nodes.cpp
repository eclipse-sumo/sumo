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
// Revision 1.8  2004/08/02 12:44:11  dkrajzew
// using Position2D instead of two doubles
//
// Revision 1.7  2004/01/12 15:30:31  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.6  2003/07/22 15:11:24  dkrajzew
// removed warnings
//
// Revision 1.5  2003/07/07 08:26:33  dkrajzew
// adapted the importer to the new node type description
//
// Revision 1.4  2003/06/18 11:14:13  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.3  2003/03/17 14:19:48  dkrajzew
// node type assignment corrected
//
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
#include <utils/common/MsgHandler.h>
#include <utils/convert/TplConvert.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBOwnTLDef.h>
#include <netbuild/NBTrafficLightDefinition.h>
#include <netbuild/NBTrafficLightLogicCont.h>
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
    // return if an error occured
    if(MsgHandler::getErrorInstance()->wasInformed()) {
        return;
    }
    NBNode::BasicNodeType myType = NBNode::NODETYPE_NOJUNCTION;
    // radius omitted
    switch(type) {
    case 0: // no function (virtual one-way)
    case 1: // no function (virtual two-way)
    case 2: // origin (generator)
    case 3: // destination
    case 4: // origin/destination
        // we assume, that no other "normal" links are participating
        //  and allow all cars to go through
        myType = NBNode::NODETYPE_NOJUNCTION;
        break;
    case 5: // signalised
    case 10: // signalised roundabout
        myType = NBNode::NODETYPE_PRIORITY_JUNCTION; // !!! (should be a roundabout, in fact)
        break;
    case 6: // unsignalised
    case 7: // roundabout // !!! (should be a roundabout, in fact)
    case 8: // merge
    case 9: // diverge
        myType = NBNode::NODETYPE_PRIORITY_JUNCTION;
        break;
    default:
        addError("Unsupported junction type.");
        break;
    }
    // build if ok
    NBNode *node = new NBNode(id, Position2D(x, y), myType);
    if(!NBNodeCont::insert(node)) {
        // should never happen
        delete node;
        return;
    }
    // check traffic light junctions)
    if(type==5||type==10) {
        NBTrafficLightDefinition *tlDef =
            new NBOwnTLDef(id, node);
        if(!NBTrafficLightLogicCont::insert(id, tlDef)) {
            // actually, nothing should fail here
            delete tlDef;
            throw ProcessError();
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



/****************************************************************************/
/// @file    NIArtemisParser_Nodes.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10 Feb 2003
/// @version $Id: $
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBOwnTLDef.h>
#include <netbuild/NBTrafficLightDefinition.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include "NIArtemisLoader.h"
#include "NIArtemisParser_Nodes.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NIArtemisParser_Nodes::NIArtemisParser_Nodes(
    NBNodeCont &nc, NBTrafficLightLogicCont &tlc,
    NIArtemisLoader &parent,
    const std::string &dataName)
        : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName),
        myNodeCont(nc), myTLLogicCont(tlc)
{}


NIArtemisParser_Nodes::~NIArtemisParser_Nodes()
{}


void
NIArtemisParser_Nodes::myDependentReport()
{
    string id = myLineParser.get("IDnum");
    string name = myLineParser.get("Name");
    SUMOReal x = TplConvert<char>::_2SUMOReal(myLineParser.get("X").c_str());
    SUMOReal y = TplConvert<char>::_2SUMOReal(myLineParser.get("Y").c_str());
    // size omitted
    int type = TplConvert<char>::_2int(myLineParser.get("Type").c_str());
    // return if an error occured
    if (MsgHandler::getErrorInstance()->wasInformed()) {
        return;
    }
    NBNode::BasicNodeType myType = NBNode::NODETYPE_NOJUNCTION;
    // radius omitted
    switch (type) {
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
    if (!myNodeCont.insert(node)) {
        // should never happen
        delete node;
        return;
    }
    // check traffic light junctions)
    if (type==5||type==10) {
        NBTrafficLightDefinition *tlDef =
            new NBOwnTLDef(id, node);
        if (!myTLLogicCont.insert(id, tlDef)) {
            // actually, nothing should fail here
            delete tlDef;
            throw ProcessError();
        }
    }
}



/****************************************************************************/


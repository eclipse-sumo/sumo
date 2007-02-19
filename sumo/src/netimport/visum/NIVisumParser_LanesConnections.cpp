/****************************************************************************/
/// @file    NIVisumParser_LanesConnections.cpp
/// @author  Daniel Krajzewicz
/// @date    TThu, 23 Mar 2006
/// @version $Id$
///
// Parser for visum-lanes
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

#include <netbuild/NBHelpers.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/nodes/NBNode.h>
#include <utils/common/TplConvert.h>
#include <utils/common/TplConvertSec.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_LanesConnections.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NIVisumParser_LanesConnections::NIVisumParser_LanesConnections(NIVisumLoader &parent,
        NBNodeCont &nc, NBEdgeCont &ec, const std::string &dataName)
        : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
        myNodeCont(nc), myEdgeCont(ec)
{}


NIVisumParser_LanesConnections::~NIVisumParser_LanesConnections()
{}


void
NIVisumParser_LanesConnections::myDependentReport()
{
    try {
        // get the node
        NBNode *node = getNamedNode(myNodeCont, "FAHRSTREIFENABBIEGER", "KNOTNR", "KNOT");
        if (node==0) {
            return;
        }
        // get the from-edge
        NBEdge *fromEdge = getNamedEdgeContinuating(myEdgeCont, "FAHRSTREIFENABBIEGER", "VONSTRNR", "VONSTR", node);
        NBEdge *toEdge = getNamedEdgeContinuating(myEdgeCont, "FAHRSTREIFENABBIEGER", "NACHSTRNR", "NACHSTR", node);
        if (fromEdge==0||toEdge==0) {
            return;
        }

        int fromLaneOffset = 0;//fromEdge->getNoLanes();
        if (!node->hasIncoming(fromEdge)) {
            fromLaneOffset = fromEdge->getNoLanes();
            fromEdge = getReversedContinuating(myEdgeCont, fromEdge, node);
        } else {
            fromEdge = getReversedContinuating(myEdgeCont, fromEdge, node);
            NBEdge *tmp = myEdgeCont.retrieve(fromEdge->getID().substr(0, fromEdge->getID().find('_')));
            fromLaneOffset = tmp->getNoLanes();
        }

        int toLaneOffset = 0;//toEdge->getNoLanes();
        if (!node->hasOutgoing(toEdge)) {
            toLaneOffset = toEdge->getNoLanes();
            toEdge = getReversedContinuating(myEdgeCont, toEdge, node);
        } else {
            NBEdge *tmp = myEdgeCont.retrieve(toEdge->getID().substr(0, toEdge->getID().find('_')));
            toLaneOffset = tmp->getNoLanes();
        }
        //
        // get the from-lane
        string fromLaneS =
            NBHelpers::normalIDRepresentation(myLineParser.get("VONFSNR"));

        int fromLane = -1;
        try {
            fromLane = TplConvert<char>::_2int(fromLaneS.c_str());
        } catch (NumberFormatException) {
            addError("A from-lane number for edge '" + fromEdge->getID() + "' is not numeric (" + fromLaneS + ").");
            return;
        }
        fromLane -= 1;
        if (fromLane<0) {
            addError("A from-lane number for edge '" + fromEdge->getID() + "' is not positive (" + fromLaneS + ").");
            return;
        }
        // get the from-lane
        string toLaneS =
            NBHelpers::normalIDRepresentation(myLineParser.get("NACHFSNR"));
        int toLane = -1;
        try {
            toLane = TplConvert<char>::_2int(toLaneS.c_str());
        } catch (NumberFormatException) {
            addError("A to-lane number for edge '" + toEdge->getID() + "' is not numeric (" + toLaneS + ").");
            return;
        }
        toLane -= 1;
        if (toLane<0) {
            addError("A to-lane number for edge '" + toEdge->getID() + "' is not positive (" + toLaneS + ").");
            return;
        }

        // !!! the next is probably a hack
        if (fromLane-fromLaneOffset<0) {
            fromLaneOffset = 0;
        } else {
            fromLane = fromEdge->getNoLanes() - (fromLane-fromLaneOffset) - 1;
        }
        if (toLane-toLaneOffset<0) {
            toLaneOffset = 0;
        } else {
            toLane = toEdge->getNoLanes() - (toLane-toLaneOffset) - 1;
        }


        //
        if ((int) fromEdge->getNoLanes()<=fromLane) {
            addError("A from-lane number for edge '" + fromEdge->getID() + "' is larger than the edge's lane number (" + fromLaneS + ").");
            return;
        }
        if ((int) toEdge->getNoLanes()<=toLane) {
            addError("A to-lane number for edge '" + toEdge->getID() + "' is larger than the edge's lane number (" + toLaneS + ").");
            return;
        }


        //
        fromEdge->addLane2LaneConnection(fromLane, toEdge, toLane, false);
    } catch (OutOfBoundsException) {
        addError2("FAHRSTREIFENABBIEGER", "", "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("FAHRSTREIFENABBIEGER", "", "NumberFormat");
    } catch (UnknownElement) {
        addError2("FAHRSTREIFENABBIEGER", "", "UnknownElement");
    }
}



/****************************************************************************/


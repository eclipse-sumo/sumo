/****************************************************************************/
/// @file    NIVisumParser_Lanes.cpp
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
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_Lanes.h"

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
NIVisumParser_Lanes::NIVisumParser_Lanes(NIVisumLoader &parent,
        NBNodeCont &nc, NBEdgeCont &ec, NBDistrictCont &dc,
        const std::string &dataName)
        : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
        myNodeCont(nc), myEdgeCont(ec), myDistrictCont(dc)
{}


NIVisumParser_Lanes::~NIVisumParser_Lanes()
{}


void
NIVisumParser_Lanes::myDependentReport()
{
    try {
        // get the node
        NBNode *node = getNamedNode(myNodeCont, "FAHRSTREIFEN", "KNOTNR");
        // get the edge
        NBEdge *baseEdge = getNamedEdge(myEdgeCont, "FAHRSTREIFEN", "STRNR");
        NBEdge *edge = getNamedEdgeContinuating(myEdgeCont, "FAHRSTREIFEN", "STRNR", node);
        // check
        if (node==0||edge==0) {
            return;
        }
        // get the lane
        string laneS = NBHelpers::normalIDRepresentation(myLineParser.get("FSNR"));
        int lane = -1;
        try {
            lane = TplConvert<char>::_2int(laneS.c_str());
        } catch (NumberFormatException &) {
            addError("A lane number for edge '" + edge->getID() + "' is not numeric (" + laneS + ").");
            return;
        }
        lane -= 1;
        if (lane<0) {
            addError("A lane number for edge '" + edge->getID() + "' is not positive (" + laneS + ").");
            return;
        }
        // get the direction
        string dirS =
            NBHelpers::normalIDRepresentation(myLineParser.get("RICHTTYP"));
        int prevLaneNo = baseEdge->getNoLanes();
        if ((dirS=="1"&&!(node->hasIncoming(edge))) || (dirS=="0"&&!(node->hasOutgoing(edge)))) {
            // get the last part of the turnaround direction
            edge = getReversedContinuating(myEdgeCont, edge, node);
        }
        // get the length
        string lengthS = NBHelpers::normalIDRepresentation(myLineParser.get("LAENGE"));
        SUMOReal length = -1;
        try {
            length = TplConvert<char>::_2SUMOReal(lengthS.c_str());
        } catch (NumberFormatException &) {
            addError("A lane length for edge '" + edge->getID() + "' is not numeric (" + lengthS + ").");
            return;
        }
        if (length<0) {
            addError("A lane length for edge '" + edge->getID() + "' is not positive (" + lengthS + ").");
            return;
        }

        //
        if (dirS=="1") {
            lane -= prevLaneNo;
        }

        //
        if (length==0) {
            if ((int) edge->getNoLanes()>lane) {
                // ok, we know this already...
                return;
            }
            // increment by one
            edge->incLaneNo(1);
        } else {
            // check whether this edge already has been created
            if (edge->getID().substr(edge->getID().length()-node->getID().length()-1)=="_" + node->getID()) {
                if (edge->getID().substr(edge->getID().find('_'))=="_" + toString(length) + "_" + node->getID()) {
                    if ((int) edge->getNoLanes()>lane) {
                        // ok, we know this already...
                        return;
                    }
                    // increment by one
                    edge->incLaneNo(1);
                    return;
                }
            }
            // nope, we have to split the edge...
            //  maybe it is not the proper edge to split - VISUM seems not to sort the splits...
            bool mustRecheck = true;
            NBNode *nextNode = node;
            SUMOReal seenLength = 0;
            while (mustRecheck) {
                if (edge->getID().substr(edge->getID().length()-node->getID().length()-1)=="_" + node->getID()) {
                    // ok, we have a previously created edge here
                    string sub = edge->getID();
                    sub = sub.substr(sub.rfind('_', sub.rfind('_')-1));
                    sub = sub.substr(1, sub.find('_', 1)-1);
                    SUMOReal dist = TplConvert<char>::_2SUMOReal(sub.c_str());
                    if (dist<length) {
                        seenLength += edge->getLength();
                        if (dirS=="1") {
                            // incoming -> move back
                            edge = edge->getFromNode()->getIncomingEdges()[0];
                            nextNode = edge->getToNode();
                            nextNode = edge->getFromNode();
                        } else {
                            // outgoing -> move forward
                            edge = edge->getToNode()->getOutgoingEdges()[0];
                            nextNode = edge->getFromNode();
                            nextNode = edge->getToNode();
                        }
                    } else {
                        mustRecheck = false;
                    }
                } else {
                    // we have the center edge - do not continue...
                    mustRecheck = false;
                }
            }
            // compute position
            Position2D p;
            SUMOReal useLength = length - seenLength;//length - (seenLength - edge->getLength());
            useLength = edge->getLength()-useLength;
            string edgeID = edge->getID();
            p = edge->getGeometry().positionAtLengthPosition(useLength);
            if (edgeID.substr(edgeID.length()-node->getID().length()-1)=="_" + node->getID()) {
                edgeID = edgeID.substr(0, edgeID.find('_'));
            }
            NBNode *rn = new NBNode(edgeID + "_" +  toString((size_t) length) + "_" + node->getID(), p);
            if (!myNodeCont.insert(rn)) {
                MsgHandler::getErrorInstance()->inform("Ups - could not insert node!");
                throw ProcessError();
            }
            string nid = edgeID + "_" +  toString((size_t) length) + "_" + node->getID();
            myEdgeCont.splitAt(myDistrictCont, edge, useLength/*edge->getLength()-length*/, rn,
                               edge->getID(), nid,
                               edge->getNoLanes()+0, edge->getNoLanes()+1);
            NBEdge *nedge = myEdgeCont.retrieve(nid);
            nedge = nedge->getToNode()->getOutgoingEdges()[0];
            while (nedge->getID().substr(nedge->getID().length()-node->getID().length()-1)=="_" + node->getID()) {
                assert(nedge->getToNode()->getOutgoingEdges().size()>0);
                nedge->incLaneNo(1);
                nedge = nedge->getToNode()->getOutgoingEdges()[0];
            }

        }
    } catch (OutOfBoundsException &) {
        addError2("FAHRSTREIFEN", "", "OutOfBounds");
    } catch (NumberFormatException &) {
        addError2("FAHRSTREIFEN", "", "NumberFormat");
    } catch (UnknownElement &) {
        addError2("FAHRSTREIFEN", "", "UnknownElement");
    }
}



/****************************************************************************/


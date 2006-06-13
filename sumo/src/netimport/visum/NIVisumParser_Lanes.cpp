/***************************************************************************
                          NIVisumParser_Lanes.cpp
              Parser for visum-lanes
                             -------------------
    project              : SUMO
    begin                : TThu, 23 Mar 2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.6  2006/06/13 13:16:00  dkrajzew
// patching problems on loading split lanes and tls
//
// Revision 1.5  2006/04/07 10:41:47  dkrajzew
// code beautifying: embedding string in strings removed
//
// Revision 1.4  2006/04/07 05:28:50  dkrajzew
// finished lane-2-lane connections setting
//
// Revision 1.3  2006/04/05 05:33:04  dkrajzew
// further work on lane connection import
//
// Revision 1.2  2006/03/28 09:12:43  dkrajzew
// lane connections for unsplitted lanes implemented, further refactoring
//
// Revision 1.1  2006/03/28 06:15:49  dkrajzew
// refactoring and extending the Visum-import
//
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

#include <netbuild/NBHelpers.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/nodes/NBNode.h>
#include <utils/common/TplConvert.h>
#include <utils/common/TplConvertSec.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_Lanes.h"

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
NIVisumParser_Lanes::NIVisumParser_Lanes(NIVisumLoader &parent,
        NBNodeCont &nc, NBEdgeCont &ec, NBDistrictCont &dc,
        const std::string &dataName)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
    myNodeCont(nc), myEdgeCont(ec), myDistrictCont(dc)
{
}


NIVisumParser_Lanes::~NIVisumParser_Lanes()
{
}


void
NIVisumParser_Lanes::myDependentReport()
{
    try {
        // get the node
        NBNode *node = getNamedNode(myNodeCont, "FAHRSTREIFEN", "KNOTNR");
        // get the edge
        NBEdge *edge = getNamedEdgeContinuating(myEdgeCont, "FAHRSTREIFEN", "STRNR", node);
        // check
        if(node==0||edge==0) {
            return;
        }
        // get the lane
        string laneS = NBHelpers::normalIDRepresentation(myLineParser.get("FSNR"));
        int lane = -1;
        try {
            lane = TplConvert<char>::_2int(laneS.c_str());
        } catch (NumberFormatException) {
            addError("A lane number for edge '" + edge->getID() + "' is not numeric (" + laneS + ").");
            return;
        }
        lane -= 1;
        if(lane<0) {
            addError("A lane number for edge '" + edge->getID() + "' is not positive (" + laneS + ").");
            return;
        }
        // get the direction
        string dirS =
            NBHelpers::normalIDRepresentation(myLineParser.get("RICHTTYP"));
        int prevLaneNo = edge->getNoLanes();
        if( (dirS=="1"&&!(node->hasIncoming(edge))) || (dirS=="0"&&!(node->hasOutgoing(edge))) ) {
            // get the last part of the turnaround direction
            edge = getReversedContinuating(myEdgeCont, edge, node);
        }
        // get the length
        string lengthS = NBHelpers::normalIDRepresentation(myLineParser.get("LAENGE"));
        SUMOReal length = -1;
        try {
            length = TplConvert<char>::_2SUMOReal(lengthS.c_str());
        } catch (NumberFormatException) {
            addError("A lane length for edge '" + edge->getID() + "' is not numeric (" + lengthS + ").");
            return;
        }
        if(length<0) {
            addError("A lane length for edge '" + edge->getID() + "' is not positive (" + lengthS + ").");
            return;
        }

        //
        if(dirS=="1") {
            lane -= prevLaneNo;
        }

        //
        if(length==0) {
            if((int) edge->getNoLanes()>lane) {
                // ok, we know this already...
                return;
            }
            // increment by one
            edge->incLaneNo(1);
        } else {
            // check whether this edge already has been created
            if(edge->getID().substr(edge->getID().length()-node->getID().length()-1)=="_" + node->getID()) {
                if(edge->getID().substr(edge->getID().find('_'))=="_" + toString(length) + "_" + node->getID()) {
                    edge->incLaneNo(1);
                    return;
                }
            }
            // nope, we have to split the edge...
            //  maybe it is not the proper edge to split - VISUM seems not to sort the splits...
            bool mustRecheck = true;
            while(mustRecheck) {
                if(edge->getID().substr(edge->getID().length()-node->getID().length()-1)=="_" + node->getID()) {
                    // ok, we have a previously created edge here
                    string sub = edge->getID();
                    sub = sub.substr(sub.rfind('_', sub.rfind('_')-1));
                    sub = sub.substr(1, sub.find('_', 1)-1);
                    SUMOReal dist = TplConvert<char>::_2SUMOReal(sub.c_str());
                    if(dist>length) {
                        if(dirS=="1") {
                            // incoming -> move back
                            edge = edge->getFromNode()->getIncomingEdges()[0];
                        } else {
                            // outgoing -> move forward
                            edge = edge->getToNode()->getOutgoingEdges()[0];
                        }
                    }
                } else {
                    // we have the center edge - do not continue...
                    mustRecheck = false;
                }
            }
            // compute position
            Position2D p;
            if(dirS!="1") {
                p =
                    node->hasOutgoing(edge)
                    ? edge->getGeometry().positionAtLengthPosition(edge->getLength()-length)
                    : edge->getGeometry().positionAtLengthPosition(length);
            } else {
                p =
                    node->hasIncoming(edge)
                    ? edge->getGeometry().positionAtLengthPosition(edge->getLength()-length)
                    : edge->getGeometry().positionAtLengthPosition(length);
            }
            NBNode *rn = new NBNode(edge->getID() + "_" +  toString(length) + "_" + node->getID(), p);
            if(!myNodeCont.insert(rn)) {
                MsgHandler::getErrorInstance()->inform("Ups - could not insert node!");
                throw ProcessError();
            }
            myEdgeCont.splitAt(myDistrictCont, edge, edge->getLength()-length, rn,
                edge->getID(), edge->getID() + "_" +  toString(length) + "_" + node->getID(),
                edge->getNoLanes(), edge->getNoLanes()+1);
        }
    } catch (OutOfBoundsException) {
        addError2("FAHRSTREIFEN", "", "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("FAHRSTREIFEN", "", "NumberFormat");
    } catch (UnknownElement) {
        addError2("FAHRSTREIFEN", "", "UnknownElement");
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



/***************************************************************************
                          NIVisumParser_LanesConnections.cpp
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
#include "NIVisumLoader.h"
#include "NIVisumParser_LanesConnections.h"

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
NIVisumParser_LanesConnections::NIVisumParser_LanesConnections(NIVisumLoader &parent,
        NBNodeCont &nc, NBEdgeCont &ec, const std::string &dataName)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
    myNodeCont(nc), myEdgeCont(ec)
{
}


NIVisumParser_LanesConnections::~NIVisumParser_LanesConnections()
{
}


void
NIVisumParser_LanesConnections::myDependentReport()
{
    try {
        // get the node
        NBNode *node = getNamedNode(myNodeCont, "FAHRSTREIFENABBIEGER", "KNOTNR", "KNOT");
        if(node==0) {
            return;
        }
        // get the from-edge
        NBEdge *fromEdge = getNamedEdge(myEdgeCont, "FAHRSTREIFENABBIEGER", "VONSTRNR", "VONSTR");
        NBEdge *toEdge = getNamedEdge(myEdgeCont, "FAHRSTREIFENABBIEGER", "NACHSTRNR", "NACHSTR");
        if(fromEdge==0||toEdge==0) {
            return;
        }

        int fromLaneOffset = 0;//fromEdge->getNoLanes();
        if(!node->hasIncoming(fromEdge)) {
            fromLaneOffset = fromEdge->getNoLanes();
            string sid;
            if(fromEdge->getID()[0]=='-') {
                sid = fromEdge->getID().substr(1);
            } else {
                sid = "-" + fromEdge->getID();
            }
            fromEdge = myEdgeCont.retrieve(sid);
            /*
            if(fromEdge==0) {
                if(sid.find("_s")!=string::npos) {
                    sid = sid.substr(0, sid.length()-2);
                } else {
                    sid = sid + "_s";
                }
                fromEdge = myEdgeCont.retrieve(sid);
            }
            */
        } else {
            string sid;
            if(fromEdge->getID()[0]=='-') {
                sid = fromEdge->getID().substr(1);
            } else {
                sid = "-" + fromEdge->getID();
            }
            NBEdge *tmp = myEdgeCont.retrieve(sid);
            fromLaneOffset = tmp->getNoLanes();
        }

        int toLaneOffset = 0;//toEdge->getNoLanes();
        if(!node->hasOutgoing(toEdge)) {
            int toLaneOffset = toEdge->getNoLanes();
            string sid;
            if(toEdge->getID()[0]=='-') {
                sid = toEdge->getID().substr(1);
            } else {
                sid = "-" + toEdge->getID();
            }
            toEdge = myEdgeCont.retrieve(sid);
            /*
            if(toEdge==0) {
                if(sid.find("_s")!=string::npos) {
                    sid = sid.substr(0, sid.length()-2);
                } else {
                    sid = sid + "_s";
                }
                toEdge = myEdgeCont.retrieve(sid);
            }
            */
        } else {
            int toLaneOffset = toEdge->getNoLanes();
            string sid;
            if(toEdge->getID()[0]=='-') {
                sid = toEdge->getID().substr(1);
            } else {
                sid = "-" + toEdge->getID();
            }
            NBEdge *tmp = myEdgeCont.retrieve(sid);
            toLaneOffset = tmp->getNoLanes();
        }
        //
        if(myEdgeCont.retrieve(fromEdge->getID() + "_s")!=0) {
            fromEdge = myEdgeCont.retrieve(fromEdge->getID() + "_s");
            assert(fromEdge!=0);
        }
        if(myEdgeCont.retrieve(toEdge->getID() + "_s")!=0) {
            toEdge = myEdgeCont.retrieve(toEdge->getID() + "_s");
            assert(toEdge!=0);
        }


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
        if(fromLane<0) {
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
        if(toLane<0) {
            addError("A to-lane number for edge '" + toEdge->getID() + "' is not positive (" + toLaneS + ").");
            return;
        }

        // !!! the next is probably a hack
        if(fromLane-fromLaneOffset<0) {
            fromLaneOffset = 0;
        } else {
            fromLane = fromEdge->getNoLanes() - (fromLane-fromLaneOffset) - 1;
        }
        if(toLane-toLaneOffset<0) {
            toLaneOffset = 0;
        } else {
            toLane = toEdge->getNoLanes() - (toLane-toLaneOffset) - 1;
        }


        //
        if(fromEdge->getNoLanes()<=fromLane) {
            addError("A from-lane number for edge '" + fromEdge->getID() + "' is larger than the edge's lane number (" + fromLaneS + ").");
            return;
        }
        if(toEdge->getNoLanes()<=toLane) {
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



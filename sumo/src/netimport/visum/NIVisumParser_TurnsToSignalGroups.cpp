/***************************************************************************
                          NIVisumParser_TurnsToSignalGroups.cpp
			  Parser for visum-TurnsToSignalGroups
                             -------------------
    project              : SUMO
    begin                : Fri, 09 May 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
    author               : Markus Hartinger
    email                : Markus.Hartinger@dlr.de
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
// Revision 1.8  2006/06/13 13:16:00  dkrajzew
// patching problems on loading split lanes and tls
//
// Revision 1.7  2006/03/28 09:12:43  dkrajzew
// lane connections for unsplitted lanes implemented, further refactoring
//
// Revision 1.6  2005/10/07 11:41:01  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/23 06:03:50  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.4  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/04/27 12:24:42  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.2  2004/01/12 15:36:08  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.1  2003/05/20 09:39:14  dkrajzew
// Visum traffic light import added (by Markus Hartinger)
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

#include <utils/common/TplConvert.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_TurnsToSignalGroups.h"
#include "NIVisumTL.h"

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
NIVisumParser_TurnsToSignalGroups::NIVisumParser_TurnsToSignalGroups(
    NIVisumLoader &parent, NBNodeCont &nc, NBEdgeCont &ec,
	const std::string &dataName, NIVisumLoader::NIVisumTL_Map &NIVisumTLs)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
	myNIVisumTLs(NIVisumTLs), myNodeCont(nc), myEdgeCont(ec)
{
}


NIVisumParser_TurnsToSignalGroups::~NIVisumParser_TurnsToSignalGroups()
{
}


NBEdge* NIVisumParser_TurnsToSignalGroups::getEdge(NBNode *FromNode, NBNode *ToNode)
{
	EdgeVector::const_iterator i;
	for (i = FromNode->getOutgoingEdges().begin(); i != FromNode->getOutgoingEdges().end(); i++) {
		if (ToNode == (*i)->getToNode()) return (*i);
	}
    //!!!
	return 0;
}

void
NIVisumParser_TurnsToSignalGroups::myDependentReport()
{
	std::string SGid;
	std::string LSAid;
    try {
        // get the id
        SGid = getNamedString("SGNR", "SIGNALGRUPPENNR");
        LSAid = getNamedString("LsaNr");
        // nodes
        NBNode *from = myLineParser.know("VonKnot") ? getNamedNode(myNodeCont, "TurnsToSignalGroups", "VonKnot") : 0;
        NBNode *via = getNamedNode(myNodeCont, "TurnsToSignalGroups", "UeberKnot", "UeberKnotNr");
        NBNode *to = myLineParser.know("NachKnot") ? getNamedNode(myNodeCont, "TurnsToSignalGroups", "NachKnot") : 0;
        // edges
		NBEdge *edg1 = 0;
		NBEdge *edg2 = 0;
        if(from==0&&to==0) {
            edg1 = getNamedEdgeContinuating(myEdgeCont, "TurnsToSignalGroups", "VONSTRNR", via);
            edg2 = getNamedEdgeContinuating(myEdgeCont, "TurnsToSignalGroups", "NACHSTRNR", via);
        } else {
		    edg1 = getEdge(from, via);
		    edg2 = getEdge(via, to);
        }
		// add to the list
		NIVisumTL::SignalGroup *SG;
		SG = (*myNIVisumTLs.find(LSAid)).second->GetSignalGroup(SGid);
		if (edg1!=0 && edg2!=0) {

            if(!via->hasIncoming(edg1)) {
                string sid;
                if(edg1->getID()[0]=='-') {
                    sid = edg1->getID().substr(1);
                } else {
                    sid = "-" + edg1->getID();
                }
                if(sid.find('_')!=string::npos) {
                    sid = sid.substr(0, sid.find('_'));
                }
                edg1 = getNamedEdgeContinuating(myEdgeCont, sid,  via);
            }
            if(!via->hasOutgoing(edg2)) {
                string sid;
                if(edg2->getID()[0]=='-') {
                    sid = edg2->getID().substr(1);
                } else {
                    sid = "-" + edg2->getID();
                }
                if(sid.find('_')!=string::npos) {
                    sid = sid.substr(0, sid.find('_'));
                }
                edg2 = getNamedEdgeContinuating(myEdgeCont, sid,  via);
            }


			SG->GetConnections()->push_back(NBConnection(edg1, edg2));
		}
    } catch (OutOfBoundsException) {
        addError2("TurnsToSignalGroups", "LSA:" + LSAid + " SignalGroup:" + SGid, "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("TurnsToSignalGroups", "LSA:" + LSAid + " SignalGroup:" + SGid, "NumberFormat");
    } catch (UnknownElement) {
        addError2("TurnsToSignalGroups", "LSA:" + LSAid + " SignalGroup:" + SGid, "UnknownElement");
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

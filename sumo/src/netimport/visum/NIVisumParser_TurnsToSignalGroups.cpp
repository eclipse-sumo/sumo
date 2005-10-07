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
    NIVisumLoader &parent, NBNodeCont &nc,
	const std::string &dataName, NIVisumLoader::NIVisumTL_Map &NIVisumTLs)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
	myNIVisumTLs(NIVisumTLs), myNodeCont(nc)
{
}


NIVisumParser_TurnsToSignalGroups::~NIVisumParser_TurnsToSignalGroups()
{
}


NBEdge* NIVisumParser_TurnsToSignalGroups::getEdge(NBNode *FromNode, NBNode *ToNode)
{
	EdgeVector::const_iterator i;
	for (i = FromNode->getOutgoingEdges().begin();
	i != FromNode->getOutgoingEdges().end(); i++) {
		if (ToNode == (*i)->getToNode()) return (*i);
	}
	return 0;
}

void
NIVisumParser_TurnsToSignalGroups::myDependentReport()
{
	std::string SGid;
	std::string LSAid;
    try {
        // get the id
        SGid = NBHelpers::normalIDRepresentation(myLineParser.get("SGNR"));
        LSAid = NBHelpers::normalIDRepresentation(myLineParser.get("LsaNr"));
        // nodes
        NBNode *FromNode = myNodeCont.retrieve(myLineParser.get("VonKnot").c_str());
        NBNode *OverNode = myNodeCont.retrieve(myLineParser.get("UeberKnot").c_str());
        NBNode *ToNode = myNodeCont.retrieve(myLineParser.get("NachKnot").c_str());
		// add to the list
		NIVisumTL::SignalGroup *SG;
		SG = (*myNIVisumTLs.find(LSAid)).second->GetSignalGroup(SGid);
		NBEdge *Edg1 = getEdge(FromNode, OverNode);
		NBEdge *Edg2 = getEdge(OverNode, ToNode);
		if ( (Edg1 != 0) && (Edg2!=0)) {
			SG->GetConnections()->push_back(NBConnection(Edg1, Edg2));
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

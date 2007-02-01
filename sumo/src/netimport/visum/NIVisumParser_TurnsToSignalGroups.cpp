/****************************************************************************/
/// @file    NIVisumParser_TurnsToSignalGroups.cpp
/// @author  unknown_author
/// @date    Fri, 09 May 2003
/// @version $Id: $
///
// Parser for visum-TurnsToSignalGroups
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

#include <utils/common/TplConvert.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_TurnsToSignalGroups.h"
#include "NIVisumTL.h"

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
NIVisumParser_TurnsToSignalGroups::NIVisumParser_TurnsToSignalGroups(
    NIVisumLoader &parent, NBNodeCont &nc, NBEdgeCont &ec,
    const std::string &dataName, NIVisumLoader::NIVisumTL_Map &NIVisumTLs)
        : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
        myNIVisumTLs(NIVisumTLs), myNodeCont(nc), myEdgeCont(ec)
{}


NIVisumParser_TurnsToSignalGroups::~NIVisumParser_TurnsToSignalGroups()
{}


NBEdge* NIVisumParser_TurnsToSignalGroups::getEdge(NBNode *FromNode, NBNode *ToNode)
{
    EdgeVector::const_iterator i;
    for (i = FromNode->getOutgoingEdges().begin(); i != FromNode->getOutgoingEdges().end(); i++) {
        if (ToNode == (*i)->getToNode()) return(*i);
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
        if (from==0&&to==0) {
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

            if (!via->hasIncoming(edg1)) {
                string sid;
                if (edg1->getID()[0]=='-') {
                    sid = edg1->getID().substr(1);
                } else {
                    sid = "-" + edg1->getID();
                }
                if (sid.find('_')!=string::npos) {
                    sid = sid.substr(0, sid.find('_'));
                }
                edg1 = getNamedEdgeContinuating(myEdgeCont, sid,  via);
            }
            if (!via->hasOutgoing(edg2)) {
                string sid;
                if (edg2->getID()[0]=='-') {
                    sid = edg2->getID().substr(1);
                } else {
                    sid = "-" + edg2->getID();
                }
                if (sid.find('_')!=string::npos) {
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



/****************************************************************************/


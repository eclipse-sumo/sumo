/****************************************************************************/
/// @file    NIVisumParser_Edges.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 14 Nov 2002
/// @version $Id: $
///
// Parser for visum-edges
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

#include <utils/common/TplConvertSec.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBTypeCont.h>
#include <utils/geom/GeomHelper.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_Edges.h"

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
NIVisumParser_Edges::NIVisumParser_Edges(NIVisumLoader &parent,
        NBNodeCont &nc, NBEdgeCont &ec, NBTypeCont &tc,
        const std::string &dataName)
        : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
        myNodeCont(nc), myEdgeCont(ec), myTypeCont(tc)
{}


NIVisumParser_Edges::~NIVisumParser_Edges()
{}


void
NIVisumParser_Edges::myDependentReport()
{
    string id;
    try {
        // get the id
        id = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
        // get the from- & to-node and validate them
        NBNode *from = getNamedNode(myNodeCont, "STRECKE", "VonKnot", "VonKnotNr");
        NBNode *to = getNamedNode(myNodeCont, "STRECKE", "NachKnot", "NachKnotNr");
        if (from==0||to==0||!checkNodes(from, to)) {
            return;
        }
        // get the type
        string type = myLineParser.know("Typ") ? myLineParser.get("Typ") : myLineParser.get("TypNr");
        // get the street length
        SUMOReal length = -1;//getLength(from, to); !!! we have to recompute the length!
        // get the speed
        SUMOReal speed = getSpeed(type);
        // get the information whether the edge is a one-way
        bool oneway =
            myLineParser.know("Einbahn")
            ? TplConvert<char>::_2bool(myLineParser.get("Einbahn").c_str())
            : true;
        // get the number of lanes
        int nolanes = getNoLanes(type);
        // check whether the id is already used
        //  (should be the opposite direction)
        bool oneway_checked = oneway;
        NBEdge *previous = myEdgeCont.retrieve(id);
        if (previous!=0) {
            id = '-' + id;
            previous->setLaneSpreadFunction(NBEdge::LANESPREAD_RIGHT);
            oneway_checked = false;
        }
        if (find(myTouchedEdges.begin(), myTouchedEdges.end(), id)!=myTouchedEdges.end()) {
            oneway_checked = false;
        }
        string tmpid = '-' + id;
        if (find(myTouchedEdges.begin(), myTouchedEdges.end(), tmpid)!=myTouchedEdges.end()) {
            previous = myEdgeCont.retrieve(tmpid);
            if (previous!=0) {
                previous->setLaneSpreadFunction(NBEdge::LANESPREAD_RIGHT);
            }
            oneway_checked = false;
        }
        // add the edge
        int prio = myTypeCont.getPriority(type);
        if (nolanes!=0) {
            insertEdge(id, from, to, type, speed, nolanes, length, prio, oneway_checked);
        }
        myTouchedEdges.push_back(id);
        // nothing more to do, when the edge is a one-way street
        if (oneway) {
            return;
        }
        // add the opposite edge
        id = '-' + id;
        if (nolanes!=0) {
            insertEdge(id, to, from, type, speed, nolanes, length, prio, false);
        }
        myTouchedEdges.push_back(id);
    } catch (OutOfBoundsException) {
        addError2("STRECKE", id, "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("STRECKE", id, "NumberFormat");
    } catch (UnknownElement) {
        addError2("STRECKE", id, "UnknownElement");
    }
}


bool
NIVisumParser_Edges::checkNodes(NBNode *from, NBNode *to) const
{
    if (from==0) {
        addError(" The from-node was not found within the net");
    }
    if (to==0) {
        addError(" The to-node was not found within the net");
    }
    if (from==to) {
        addError(" Both nodes are the same");
    }
    return from!=0&&to!=0&&from!=to;
}


SUMOReal
NIVisumParser_Edges::getLength(NBNode *from, NBNode *to) const
{
    SUMOReal length = 0;
    try {
        length = TplConvertSec<char>::_2SUMORealSec(
                     myLineParser.get("Laenge").c_str(), 0);
    } catch (OutOfBoundsException) {}
    // compute when the street's length is not available
    if (length==0) {
        length = GeomHelper::distance(from->getPosition(), to->getPosition());
    }
    return length;
}


SUMOReal
NIVisumParser_Edges::getSpeed(const std::string &type) const
{
    SUMOReal speed = 0;
    try {
        speed = myLineParser.know("v0-IV")
                ? TplConvertSec<char>::_2SUMORealSec(myLineParser.get("v0-IV").c_str(), -1)
                : TplConvertSec<char>::_2SUMORealSec(myLineParser.get("V0IV").c_str(), -1);
    } catch (OutOfBoundsException) {}
    if (speed<=0) {
        speed = myTypeCont.getSpeed(type);
    } else {
        speed = speed / (SUMOReal) 3.6;
    }
    return speed;
}


int
NIVisumParser_Edges::getNoLanes(const std::string &type) const
{
    int nolanes = 0;
    try {
        nolanes = myLineParser.know("Fahrstreifen")
                  ? TplConvertSec<char>::_2intSec(myLineParser.get("Fahrstreifen").c_str(), 0)
                  : TplConvertSec<char>::_2intSec(myLineParser.get("ANZFAHRSTREIFEN").c_str(), 0);
    } catch (UnknownElement) {
        nolanes = myTypeCont.getNoLanes(type);
    }
    return nolanes;
}


void
NIVisumParser_Edges::insertEdge(const std::string &id,
                                NBNode *from, NBNode *to,
                                const std::string &type,
                                SUMOReal speed, int nolanes, SUMOReal length,
                                int prio, bool oneway) const
{
    NBEdge::LaneSpreadFunction lsf = oneway
                                     ? NBEdge::LANESPREAD_CENTER
                                     : NBEdge::LANESPREAD_RIGHT;
    NBEdge *e = new NBEdge(id, id, from, to, type, speed, nolanes, length, prio, lsf);
    if (!myEdgeCont.insert(e)) {
        delete e;
        addError(" Duplicate edge occured ('" + id + "').");
    }
}



/****************************************************************************/


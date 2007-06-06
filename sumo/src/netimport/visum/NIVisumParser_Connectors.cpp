/****************************************************************************/
/// @file    NIVisumParser_Connectors.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 14 Nov 2002
/// @version $Id$
///
// Parser for visum-connectors
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <netbuild/NBHelpers.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBDistrictCont.h>
#include <netbuild/NBDistrict.h>
#include <netbuild/nodes/NBNode.h>
#include <utils/common/TplConvert.h>
#include <utils/common/TplConvertSec.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_Connectors.h"

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
NIVisumParser_Connectors::NIVisumParser_Connectors(NIVisumLoader &parent,
        NBNodeCont &nc, NBEdgeCont &ec, NBTypeCont &tc, NBDistrictCont &dc,
        const std::string &dataName)
        : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
        myNodeCont(nc), myEdgeCont(ec), myTypeCont(tc), myDistrictCont(dc)
{}


NIVisumParser_Connectors::~NIVisumParser_Connectors()
{}


void
NIVisumParser_Connectors::myDependentReport()
{
    try {
        // get the source district
        string bez = NBHelpers::normalIDRepresentation(myLineParser.get("BezNr"));
        // get the destination node
        NBNode *dest = getNamedNode(myNodeCont, "ANBINDUNG", "KnotNr");
        if (dest==0) {
            return;
        }
        // get the weight of the connection
        SUMOReal proz = getWeightedFloat("Proz");
        if (proz>0) {
            proz /= 100.;
        } else {
            proz = 1;
        }
        // get the duration to wait
        SUMOReal retard = -1;
        if (myLineParser.know("t0-IV")) {
            retard = getNamedFloat("t0-IV", -1);
        }
        // get the type;
        //  use a standard type with a large speed when a type is not given
        string type =
            myLineParser.know("Typ")
            ? NBHelpers::normalIDRepresentation(myLineParser.get("Typ"))
            : "";
        // add the connectors as an edge
        string id = bez + "-" + dest->getID();
        // get the information whether this is a sink or a source
        string dir = myLineParser.get("Richtung");
        if (dir.length()==0) {
            dir = "QZ";
        }
        // build the source when needed
        if (dir.find('Q')!=string::npos) {
            NBNode *src = buildDistrictNode(bez, dest, NBEdge::EDGEFUNCTION_SOURCE);
            if (src==0) {
                addError("The district '" + bez + "' could not be built.");
                return;
            }
            NBEdge *edge = new NBEdge(id, id, src, dest, "VisumConnector",
                                      100, 3/*nolanes*/, 2000.0, -1, NBEdge::LANESPREAD_RIGHT, NBEdge::EDGEFUNCTION_SOURCE);
            if (!myEdgeCont.insert(edge)) {
                addError("A duplicate edge id occured (ID='" + id + "').");
            } else {
                myDistrictCont.addSource(bez, edge, proz);
            }
        }
        // build the sink when needed
        if (dir.find('Z')!=string::npos) {
            NBNode *src = buildDistrictNode(bez, dest, NBEdge::EDGEFUNCTION_SINK);
            if (src==0) {
                addError("The district '" + bez + "' could not be built.");
                return;
            }
            id = "-" + id;
            NBEdge *edge = new NBEdge(id, id, dest, src, "VisumConnector",
                                      100, 3/*nolanes*/, 2000.0, -1, NBEdge::LANESPREAD_RIGHT, NBEdge::EDGEFUNCTION_SINK);
            if (!myEdgeCont.insert(edge)) {
                addError("A duplicate edge id occured (ID='" + id + "').");
            } else {
                myDistrictCont.addSink(bez, edge, proz);
            }
        }
    } catch (OutOfBoundsException &) {
        addError2("ANBINDUNG", "", "OutOfBounds");
    } catch (NumberFormatException &) {
        addError2("ANBINDUNG", "", "NumberFormat");
    } catch (UnknownElement &) {
        addError2("ANBINDUNG", "", "UnknownElement");
    }
}


NBNode *
NIVisumParser_Connectors::buildDistrictNode(const std::string &id,
        NBNode *dest,
        NBEdge::EdgeBasicFunction dir)
{
    // get the district
    NBDistrict *dist = myDistrictCont.retrieve(id);
    if (dist==0) {
        return 0;
    }
    // get the coordinates of the new node
    SUMOReal x = dest->getPosition().x()
                 + (dist->getPosition().x() - dest->getPosition().x()) / 10;
    SUMOReal y = dest->getPosition().y()
                 + (dist->getPosition().y() - dest->getPosition().y()) / 10;
    // translate in dependence to the type
    if (dir==NBEdge::EDGEFUNCTION_SINK) {
        x += (dist->getPosition().y() - dest->getPosition().y()) / 100;
        y -= (dist->getPosition().x() - dest->getPosition().x()) / 100;
    } else {
        x -= (dist->getPosition().y() - dest->getPosition().y()) / 100;
        y += (dist->getPosition().x() - dest->getPosition().x()) / 100;
    }
    // build the id
    string nid;
    nid = id + "-" + dest->getID();
    if (dir==NBEdge::EDGEFUNCTION_SINK) {
        nid = "-" + nid;
    }
    // insert the node
    if (!myNodeCont.insert(nid, Position2D(x, y))) {
        x += (SUMOReal) 0.1;
        y -= (SUMOReal) 0.1;
        if (!myNodeCont.insert(nid, Position2D(x, y), dist)) {
            addError("Ups, this should not happen: A district lies on a node.");
            return 0;
        }
    }
    // return the node
    return myNodeCont.retrieve(nid);
}



/****************************************************************************/


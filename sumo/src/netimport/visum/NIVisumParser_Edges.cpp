/***************************************************************************
                          NIVisumParser_Edges.cpp
			  Parser for visum-edges
                             -------------------
    project              : SUMO
    begin                : Thu, 14 Nov 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.4  2004/01/12 15:36:08  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.3  2003/11/11 08:19:46  dkrajzew
// made the code a little bit more pretty
//
// Revision 1.2  2003/05/20 09:39:14  dkrajzew
// Visum traffic light import added (by Markus Hartinger)
//
// Revision 1.1  2003/02/07 11:14:54  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/convert/TplConvertSec.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBTypeCont.h>
#include <utils/geom/GeomHelper.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_Edges.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVisumParser_Edges::NIVisumParser_Edges(NIVisumLoader &parent,
        const std::string &dataName)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName)
{
}


NIVisumParser_Edges::~NIVisumParser_Edges()
{
}


void
NIVisumParser_Edges::myDependentReport()
{
    string id;
    try {
        // get the id
        id = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
        // get the from- & to-node and validate them
        NBNode *from = NBNodeCont::retrieve(
            NBHelpers::normalIDRepresentation(myLineParser.get("VonKnot")));
        NBNode *to = NBNodeCont::retrieve(
            NBHelpers::normalIDRepresentation(myLineParser.get("NachKnot")));
		if(!checkNodes(from, to)) {
			return;
		}
        // get the type
        string type = myLineParser.get("Typ");
        // get the street length
		float length = getLength(from, to);
        // get the speed
		float speed = getSpeed(type);
        // get the information whether the edge is a one-way
        bool oneway =
            TplConvert<char>::_2bool(myLineParser.get("Einbahn").c_str());
		// get the number of lanes
		int nolanes = getNoLanes(type);
        // check whether the id is already used
        //  (should be the opposite direction)
        if(NBEdgeCont::retrieve(id)!=0) {
            id = '-' + id;
        }
        // add the edge
		int prio = NBTypeCont::getPriority(type);
		insertEdge(id, from, to, type, speed, nolanes, length, prio);
        // nothing more to do, when the edge is a one-way street
        if(oneway) {
            return;
        }
        // add the opposite edge
        id = '-' + id;
		insertEdge(id, to, from, type, speed, nolanes, length, prio);
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
	if(from==0) {
		addError(" The from-node was not found within the net");
	}
	if(to==0) {
		addError(" The to-node was not found within the net");
	}
	if(from==to) {
		addError(" Both nodes are the same");
	}
	return from!=0&&to!=0&&from!=to;
}


float
NIVisumParser_Edges::getLength(NBNode *from, NBNode *to) const
{
	float length = 0;
	try {
		length = TplConvertSec<char>::_2floatSec(
			myLineParser.get("Laenge").c_str(), 0);
	} catch (OutOfBoundsException) {
	}
	// compute when the street's length is not available
	if(length==0) {
        length = GeomHelper::distance(from->getPosition(), to->getPosition());
	}
	return length;
}


float
NIVisumParser_Edges::getSpeed(const std::string &type) const
{
	float speed = 0;
	try {
		speed =
			TplConvertSec<char>::_2floatSec(
				myLineParser.get("v0-IV").c_str(), -1);
	} catch (OutOfBoundsException) {
	}
	if(speed<=0) {
		speed = NBTypeCont::getSpeed(type);
	} else {
		speed = speed / 3.6;
	}
	return speed;
}


int
NIVisumParser_Edges::getNoLanes(const std::string &type) const
{
	int nolanes = 0;
	try {
		nolanes =
			TplConvertSec<char>::_2intSec(
				myLineParser.get("Fahrstreifen").c_str(), 0);
	} catch (UnknownElement) {
		nolanes = NBTypeCont::getNoLanes(type);
	}
	return nolanes;
}


void
NIVisumParser_Edges::insertEdge(const std::string &id,
								NBNode *from, NBNode *to,
								const std::string &type,
								float speed, int nolanes, float length,
								int prio) const
{
	NBEdge *e = new NBEdge(id, id, from, to, type, speed, nolanes, length, prio);
	if( !NBEdgeCont::insert(e)) {
		delete e;
		addError(
			string(" Duplicate edge occured ('")
			+ id + string("')."));
	}
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVisumParser_Edges.icc"
//#endif

// Local Variables:
// mode:C++
// End:

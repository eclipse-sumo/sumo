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
// Revision 1.1  2003/02/07 11:14:54  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/convert/TplConvertSec.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBTypeCont.h>
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
        if(from==0||to==0) {
            if(from==0) {
                addError(
                    " The from-node was not found within the net");
            } else {
                addError(
                    " The to-node was not found within the net");
            }
        }
        // get the type
        string type = myLineParser.get("Typ");
        // get the street length
        float length =
            TplConvertSec<char>::_2floatSec(
                myLineParser.get("Laenge").c_str(), 0);
        // compute when the street's length is not available
        if(length==0) {
            double xb = from->getXCoordinate();
            double xe = to->getXCoordinate();
            double yb = from->getYCoordinate();
            double ye = to->getYCoordinate();
            length = sqrt((xb-xe)*(xb-xe) + (yb-ye)*(yb-ye));
        }
        // get the speed
        float speed = TplConvertSec<char>::_2floatSec(myLineParser.get("v0-IV").c_str(), -1);
        if(speed<=0) {
            speed = NBTypeCont::getSpeed(type);
        } else {
            speed = speed / 3.6;
        }
        // get the information whether the edge is a one-way
        bool oneway =
            TplConvert<char>::_2bool(myLineParser.get("Einbahn").c_str());
        // check whether the id is already used
        //  (should be the opposite direction)
        if(NBEdgeCont::retrieve(id)!=0) {
            id = '-' + id;
        }
        // add the edge
        if( !NBEdgeCont::insert(new NBEdge(id, id, from, to, type,
             speed, NBTypeCont::getNoLanes(type), length,
             NBTypeCont::getPriority(type)))) {
            addError(
                string(" Duplicate edge occured ('")
                + id + string("')."));
        }
        // nothing more to do, when the edge is a one-way street
        if(oneway) {
            return;
        }
        // add the opposite edge
        id = '-' + id;
        NBEdge *edge = new NBEdge(id, id, from, to, type,
            speed, NBTypeCont::getNoLanes(type), length,
            NBTypeCont::getPriority(type));
        if(!NBEdgeCont::insert(edge)) {
            addError(
                string(" Duplicate edge occured ('")
                + id + string("')."));
            delete edge;
        }
    } catch (OutOfBoundsException) {
        addError2("STRECKE", id, "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("STRECKE", id, "NumberFormat");
    } catch (UnknownElement) {
        addError2("STRECKE", id, "UnknownElement");
    }
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVisumParser_Edges.icc"
//#endif

// Local Variables:
// mode:C++
// End:

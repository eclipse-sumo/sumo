/***************************************************************************
                          NIVisumParser_Nodes.cpp
			  Parser for visum-nodes
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
#include <utils/convert/TplConvert.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/NBNodeCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_Nodes.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVisumParser_Nodes::NIVisumParser_Nodes(NIVisumLoader &parent,
        const std::string &dataName)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName)
{
}


NIVisumParser_Nodes::~NIVisumParser_Nodes()
{
}


void
NIVisumParser_Nodes::myDependentReport()
{
    string id;
    try {
        // get the id
        id = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
        // get the position
        double x = TplConvert<char>::_2float(myLineParser.get("XKoord").c_str());
        double y = TplConvert<char>::_2float(myLineParser.get("YKoord").c_str());
        // add to the list
        if(!NBNodeCont::insert(id, x, -y)) {
            addError(
                string(" Duplicate node occured ('")
                + id + string("')."));
        }
    } catch (OutOfBoundsException) {
        addError2("KNOTEN", id, "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("KNOTEN", id, "NumberFormat");
    } catch (UnknownElement) {
        addError2("KNOTEN", id, "UnknownElement");
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVisumParser_Nodes.icc"
//#endif

// Local Variables:
// mode:C++
// End:

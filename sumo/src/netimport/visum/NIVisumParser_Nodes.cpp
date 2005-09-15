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
// Revision 1.6  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/04/27 12:24:41  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.4  2004/08/02 12:44:28  dkrajzew
// using Position2D instead of two doubles
//
// Revision 1.3  2004/01/12 15:36:08  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.2  2003/03/06 17:14:42  dkrajzew
// more stringent usage of insertion into containers; y-direction flipped
//
// Revision 1.1  2003/02/07 11:14:54  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <utils/convert/TplConvert.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_Nodes.h"

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
NIVisumParser_Nodes::NIVisumParser_Nodes(NIVisumLoader &parent,
        NBNodeCont &nc, const std::string &dataName)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
	myNodeCont(nc)
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
        if(!myNodeCont.insert(id, Position2D(x, y))) {
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

// Local Variables:
// mode:C++
// End:

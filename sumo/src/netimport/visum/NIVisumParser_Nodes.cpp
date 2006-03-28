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
// Revision 1.12  2006/03/28 06:15:48  dkrajzew
// refactoring and extending the Visum-import
//
// Revision 1.11  2006/03/27 07:30:20  dkrajzew
// added projection information to the network
//
// Revision 1.10  2006/03/08 13:02:27  dkrajzew
// some further work on converting geo-coordinates
//
// Revision 1.9  2006/02/23 11:23:53  dkrajzew
// VISION import added
//
// Revision 1.8  2005/10/07 11:41:01  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/23 06:03:50  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/04/27 12:24:41  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.4  2004/08/02 12:44:28  dkrajzew
// using Position2D instead of two SUMOReals
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
        NBNodeCont &nc, projPJ projection, const std::string &dataName)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
	myNodeCont(nc), myProjection(projection)
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
        SUMOReal x = TplConvert<char>::_2SUMOReal(myLineParser.get("XKoord").c_str());
        SUMOReal y = TplConvert<char>::_2SUMOReal(myLineParser.get("YKoord").c_str());
        projUV p;
        if(myProjection!=0) {

            myNodeCont.addGeoreference(Position2D((SUMOReal) x / 100000.0, (SUMOReal) y / 100000.0));

            p.u = x / 100000.0 * DEG_TO_RAD;
            p.v = y / 100000.0 * DEG_TO_RAD;
            p = pj_fwd(p, myProjection);
            x = (SUMOReal) p.u;
            y = (SUMOReal) p.v;
        }
        // add to the list
        if(!myNodeCont.insert(id, Position2D(x, y))) {
            addError(" Duplicate node occured ('" + id + "').");
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

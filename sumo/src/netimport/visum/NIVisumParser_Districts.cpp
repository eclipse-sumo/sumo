/***************************************************************************
                          NIVisumParser_Districts.cpp
			  Parser for visum-districts
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
// Revision 1.7  2005/10/07 11:41:01  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:03:50  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/04/27 12:24:41  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.3  2003/04/01 15:26:15  dkrajzew
// insertion of nodes is now checked, but still unsafe; districts are always weighted
//
// Revision 1.2  2003/03/12 16:41:06  dkrajzew
// correct y-position according to new display
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

#include <netbuild/NBHelpers.h>
#include <utils/common/TplConvert.h>
#include <netbuild/NBDistrict.h>
#include <netbuild/NBDistrictCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_Districts.h"

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
NIVisumParser_Districts::NIVisumParser_Districts(NIVisumLoader &parent,
        NBDistrictCont &dc, const std::string &dataName)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
	myDistrictCont(dc)
{
}


NIVisumParser_Districts::~NIVisumParser_Districts()
{
}


void
NIVisumParser_Districts::myDependentReport()
{
    string id;
    try {
        // get the id
        id = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
        // get the name
        string name = myLineParser.get("NAME");
        // get the information whether the source and the destination
        //  connections are weighted
        bool sourcesWeighted = getWeightedBool("Proz_Q");
        bool destWeighted = getWeightedBool("Proz_Z");
        // get the node information
        SUMOReal x =
            TplConvert<char>::_2SUMOReal(myLineParser.get("XKoord").c_str());
        SUMOReal y =
            TplConvert<char>::_2SUMOReal(myLineParser.get("YKoord").c_str());
        // build the district
        NBDistrict *district = new NBDistrict(id, name, x, y);
        if(!myDistrictCont.insert(district)) {
            addError(
                string(" Duplicate district occured ('")
                + id + string("')."));
            delete district;
        }
/*            // use a special name for the node
            id = string("DistrictCenter_") + id;
            // try to add the node
            if(!NBNodeCont::insert(id, x, y)) {
                addError("visum-file",
                    "Ups, this should not happen: A district lies on a node.");
            }*/
    } catch (OutOfBoundsException) {
        addError2("BEZIRK", id, "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("BEZIRK", id, "NumberFormat");
    } catch (UnknownElement) {
        addError2("BEZIRK", id, "UnknownElement");
    }
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

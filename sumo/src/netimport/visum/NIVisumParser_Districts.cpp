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
// Revision 1.2  2003/03/12 16:41:06  dkrajzew
// correct y-position according to new display
//
// Revision 1.1  2003/02/07 11:14:54  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <netbuild/NBHelpers.h>
#include <utils/convert/TplConvert.h>
#include <netbuild/NBDistrict.h>
#include <netbuild/NBDistrictCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_Districts.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVisumParser_Districts::NIVisumParser_Districts(NIVisumLoader &parent,
        const std::string &dataName)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName)
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
        double x =
            TplConvert<char>::_2float(myLineParser.get("XKoord").c_str());
        double y =
            TplConvert<char>::_2float(myLineParser.get("YKoord").c_str());
        // build the district
        NBDistrict *district = new NBDistrict(id, name,
            sourcesWeighted, destWeighted, x, y);
        if(!NBDistrictCont::insert(district)) {
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
//#ifdef DISABLE_INLINE
//#include "NIVisumParser_Districts.icc"
//#endif

// Local Variables:
// mode:C++
// End:

/****************************************************************************/
/// @file    NIVisumParser_Districts.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 14 Nov 2002
/// @version $Id$
///
// Parser for visum-districts
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

#include <netbuild/NBHelpers.h>
#include <utils/common/TplConvert.h>
#include <netbuild/NBDistrict.h>
#include <netbuild/NBDistrictCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_Districts.h"
#include <utils/geoconv/GeoConvHelper.h>

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
NIVisumParser_Districts::NIVisumParser_Districts(NIVisumLoader &parent,
        NBDistrictCont &dc, const std::string &dataName)
        : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
        myDistrictCont(dc)
{}


NIVisumParser_Districts::~NIVisumParser_Districts()
{}


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
        //bool sourcesWeighted = getWeightedBool("Proz_Q");
        //bool destWeighted = getWeightedBool("Proz_Z");
        // get the node information
        SUMOReal x = getNamedFloat("XKoord");
        SUMOReal y = getNamedFloat("YKoord");
//        myNodeCont.addGeoreference(Position2D((SUMOReal) (x / 100000.0), (SUMOReal) (y / 100000.0)));
        Position2D pos(x, y);
        GeoConvHelper::remap(pos);
        // build the district
        NBDistrict *district = new NBDistrict(id, name, pos.x(), pos.y());
        if (!myDistrictCont.insert(district)) {
            addError(" Duplicate district occured ('" + id + "').");
            delete district;
        }
    } catch (OutOfBoundsException) {
        addError2("BEZIRK", id, "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("BEZIRK", id, "NumberFormat");
    } catch (UnknownElement) {
        addError2("BEZIRK", id, "UnknownElement");
    }
}



/****************************************************************************/


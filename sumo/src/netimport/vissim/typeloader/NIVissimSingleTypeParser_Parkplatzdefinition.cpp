/***************************************************************************
                          NIVissimSingleTypeParser_Parkplatzdefinition.cpp

                             -------------------
    begin                : Wed, 18 Dec 2002
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.1  2003/02/07 11:08:43  dkrajzew
// Vissim import added (preview)
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <utils/convert/TplConvert.h>
#include <utils/convert/ToString.h>
#include <utils/common/IntVector.h>
#include <utils/common/DoubleVector.h>
#include <netbuild/NBDistrictCont.h>
#include <netbuild/NBDistrict.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimDistrictConnection.h"
#include "NIVissimSingleTypeParser_Parkplatzdefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Parkplatzdefinition::NIVissimSingleTypeParser_Parkplatzdefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Parkplatzdefinition::~NIVissimSingleTypeParser_Parkplatzdefinition()
{
}


bool
NIVissimSingleTypeParser_Parkplatzdefinition::parse(std::istream &from)
{
    int id;
    from >> id;

    string tag;
    from >> tag;
    string name = readName(from);

    // parse the districts
    //  and allocate them if not done before
    //  A district may be already saved when another parking place with
    //  the same district was already build.
    IntVector districts;
    DoubleVector percentages;
    tag = myRead(from); // "Bezirke"
    while(tag!="ort") {
        double perc = -1;
        int districtid;
        from >> districtid;
        tag = myRead(from);
        if(tag=="anteil") {
            from >> perc;
        }
        districts.push_back(districtid);
        percentages.push_back(perc);
        string dsid = toString<int>(districtid);
        if(NBDistrictCont::retrieve(dsid)==0) {
            bool weighted = perc>=0;
            NBDistrictCont::insert(
                new NBDistrict(dsid, dsid, weighted, weighted));
        }
        tag = myRead(from);
    }

    from >> tag; // "Strecke"
    int edgeid;
    from >> edgeid;

    double position;
    from >> tag; // "bei"
    from >> position;

    double length;
    from >> tag;
    from >> length;

    from >> tag; // "Kapazität"
    from >> tag; // "Kapazität"-value

    tag = myRead(from);

    IntVector assignedVehicles;
    while(tag!="default") {
        int vclass;
        from >> vclass;
        assignedVehicles.push_back(vclass);
        from >> tag; // "vwunsch"
        from >> tag; // "vwunsch"-value
        tag = myRead(from);
    }

    from >> tag;
    from >> tag;
    return NIVissimDistrictConnection::dictionary(id, name,
        districts, percentages, edgeid, position, assignedVehicles);
}


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
// Revision 1.6  2005/04/27 12:24:38  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.5  2004/01/12 15:33:35  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.4  2003/10/27 10:52:41  dkrajzew
// edges speed setting implemented (only on an edges begin)
//
// Revision 1.3  2003/04/01 15:24:43  dkrajzew
// parsing of parking places patched
//
// Revision 1.2  2003/03/26 12:17:14  dkrajzew
// further debugging/improvements of Vissim-import
//
// Revision 1.1  2003/02/07 11:08:43  dkrajzew
// Vissim import added (preview)
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <vector>
#include <utility>
#include <utils/convert/TplConvert.h>
#include <utils/convert/ToString.h>
#include <utils/common/IntVector.h>
#include <utils/common/DoubleVector.h>
#include <netbuild/NBDistrictCont.h>
#include <netbuild/NBDistrict.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
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
    readUntil(from, "bezirke"); // "Bezirke"
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

    std::vector<std::pair<int, int> > assignedVehicles;
    while(tag!="default") {
        int vclass;
        from >> vclass;
        from >> tag; // "vwunsch"
        int vwunsch;
        from >> vwunsch; // "vwunsch"-value
        assignedVehicles.push_back(std::pair<int, int>(vclass, vwunsch));
        tag = myRead(from);
    }

    from >> tag;
    from >> tag;
//    NIVissimEdge *e = NIVissimEdge::dictionary(edgeid);
//    e->addReferencedDistrict(id);

    // build the district connection
    return NIVissimDistrictConnection::dictionary(id, name,
        districts, percentages, edgeid, position, assignedVehicles);
}


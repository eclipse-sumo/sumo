/***************************************************************************
                          NIVissimSingleTypeParser_Fahrzeugtypdefinition.cpp

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
// Revision 1.3  2005/04/27 12:24:38  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.2  2003/05/20 09:42:37  dkrajzew
// all data types implemented
//
// Revision 1.1  2003/02/07 11:08:42  dkrajzew
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
#include <utils/convert/TplConvert.h>
#include <utils/convert/ToString.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimVehicleType.h"
#include "NIVissimSingleTypeParser_Fahrzeugtypdefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Fahrzeugtypdefinition::NIVissimSingleTypeParser_Fahrzeugtypdefinition(
    NIVissimLoader &parent, NIVissimLoader::ColorMap &colorMap)
	: NIVissimLoader::VissimSingleTypeParser(parent),
    myColorMap(colorMap)
{
}


NIVissimSingleTypeParser_Fahrzeugtypdefinition::~NIVissimSingleTypeParser_Fahrzeugtypdefinition()
{
}


bool
NIVissimSingleTypeParser_Fahrzeugtypdefinition::parse(std::istream &from)
{
    // id
	int id;
    from >> id; // type-checking is missing!
    // name
    string tag;
    from >> tag;
    string name = readName(from);
    // category
    string category;
    from >> tag;
    from >> category;
    // color (optional) and length
    RGBColor color;
    tag = myRead(from);
    while(tag!="laenge") {
        if(tag=="farbe") {
            string colorName = myRead(from);
            NIVissimLoader::ColorMap::iterator i=myColorMap.find(colorName);
            if(i!=myColorMap.end()) {
                color = (*i).second;
            } else {
                int r, g, b;
                r = TplConvert<char>::_2int(colorName.c_str());
                from >> g; // type-checking is missing!
                from >> b; // type-checking is missing!
                color = RGBColor(
                    (double) r / 255.0,
                    (double) g / 255.0,
                    (double) b / 255.0 );
            }
        }
        tag = myRead(from);
    }
    double length;
    from >> length;
    // overread until "Maxbeschleunigung"
    while(tag!="maxbeschleunigung") {
        tag = myRead(from);
    }
    double amax;
    from >> amax; // type-checking is missing!
    // overread until "Maxverzoegerung"
    while(tag!="maxverzoegerung") {
        tag = myRead(from);
    }
    double dmax;
    from >> dmax; // type-checking is missing!
    while(tag!="besetzungsgrad") {
        tag = myRead(from);
    }
    while(tag!="DATAEND") {
        tag = readEndSecure(from, "verlustzeit");
    }
    return NIVissimVehicleType::dictionary(id, name,
        category, length, color, amax, dmax);
}


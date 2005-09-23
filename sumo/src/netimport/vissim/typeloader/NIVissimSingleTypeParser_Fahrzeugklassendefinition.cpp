/***************************************************************************
                          NIVissimSingleTypeParser_Fahrzeugklassendefinition.cpp

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
// Revision 1.4  2005/09/23 06:02:58  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.3  2005/04/27 12:24:37  dkrajzew
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <iostream>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/IntVector.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimVehTypeClass.h"
#include "NIVissimSingleTypeParser_Fahrzeugklassendefinition.h"

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
NIVissimSingleTypeParser_Fahrzeugklassendefinition::NIVissimSingleTypeParser_Fahrzeugklassendefinition(
    NIVissimLoader &parent, NIVissimLoader::ColorMap &colorMap)
	: NIVissimLoader::VissimSingleTypeParser(parent),
    myColorMap(colorMap)
{
}


NIVissimSingleTypeParser_Fahrzeugklassendefinition::~NIVissimSingleTypeParser_Fahrzeugklassendefinition()
{
}


bool
NIVissimSingleTypeParser_Fahrzeugklassendefinition::parse(std::istream &from)
{
    // id
    int id;
    from >> id; // type-checking is missing!
    // name
    string tag;
    from >> tag;
    string name = readName(from);
    // color
    from >> tag;
    string colorName = myRead(from);
    RGBColor color;
    NIVissimLoader::ColorMap::iterator i=myColorMap.find(colorName);
    if(i!=myColorMap.end()) {
        color = (*i).second;
    } else {
        int r, g, b;
        r = TplConvert<char>::_2int(colorName.c_str());
        from >> g; // type-checking is missing!
        from >> b; // type-checking is missing!
        color = RGBColor(
            (SUMOReal) r / (SUMOReal) 255.0,
            (SUMOReal) g / (SUMOReal) 255.0,
            (SUMOReal) b / (SUMOReal) 255.0 );
    }
    // types
    from >> tag;
    IntVector types;
    from >> tag;
    do {
        types.push_back(TplConvert<char>::_2int(tag.c_str()));
        tag = readEndSecure(from);
    } while(tag!="DATAEND");
    return NIVissimVehTypeClass::dictionary(id, name, color, types);
}
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

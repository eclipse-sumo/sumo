/***************************************************************************
                          NIVissimSingleTypeParser_Fahrverhaltendefinition.cpp

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
// Revision 1.2  2003/03/18 13:11:53  dkrajzew
// debugging
//
// Revision 1.1  2003/03/06 17:12:48  dkrajzew
// further data parsing
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include "../NIVissimLoader.h"
#include "NIVissimSingleTypeParser_Fahrverhaltendefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Fahrverhaltendefinition::NIVissimSingleTypeParser_Fahrverhaltendefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Fahrverhaltendefinition::~NIVissimSingleTypeParser_Fahrverhaltendefinition()
{
}


bool
NIVissimSingleTypeParser_Fahrverhaltendefinition::parse(std::istream &from)
{
    string tmp;
    from >> tmp;
    // in the both next cases, we do not have to overread anything
    if(tmp=="BEHALT_ALTE_PARA" || tmp=="ANZ_VM") {
        return true;
    }
    //
    from >> tmp;
    if(tmp=="NAME") {
        readUntil(from, "gelbverhalten");
    }
    return true;
}


/***************************************************************************
                          NIVissimSingleTypeParser_Langsamfahrbereichdefinition.cpp

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
// Revision 1.2  2003/03/06 16:26:56  dkrajzew
// debugging
//
// Revision 1.1  2003/02/07 11:08:42  dkrajzew
// Vissim import added (preview)
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <utils/convert/TplConvert.h>
#include "../NIVissimLoader.h"
#include "NIVissimSingleTypeParser_Langsamfahrbereichdefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Langsamfahrbereichdefinition::NIVissimSingleTypeParser_Langsamfahrbereichdefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Langsamfahrbereichdefinition::~NIVissimSingleTypeParser_Langsamfahrbereichdefinition()
{
}


bool
NIVissimSingleTypeParser_Langsamfahrbereichdefinition::parse(std::istream &from)
{
    string id;
    from >> id;
	readUntil(from, "fahrzeugklasse");
    string tag = "fahrzeugklasse";
    while(tag=="fahrzeugklasse") {
        readUntil(from, "maxverzoegerung");
        tag = myRead(from);
        tag = myRead(from);
    }
    return true;
}


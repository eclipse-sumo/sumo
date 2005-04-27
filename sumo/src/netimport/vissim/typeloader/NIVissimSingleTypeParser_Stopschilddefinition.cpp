/***************************************************************************
                          NIVissimSingleTypeParser_Stopschilddefinition.cpp

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
// Revision 1.4  2005/04/27 12:24:39  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.3  2003/03/20 16:32:24  dkrajzew
// windows eol removed
//
// Revision 1.2  2003/03/06 16:26:58  dkrajzew
// debugging
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
#include <utils/convert/TplConvert.h>
#include "../NIVissimLoader.h"
#include "NIVissimSingleTypeParser_Stopschilddefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Stopschilddefinition::NIVissimSingleTypeParser_Stopschilddefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Stopschilddefinition::~NIVissimSingleTypeParser_Stopschilddefinition()
{
}


bool
NIVissimSingleTypeParser_Stopschilddefinition::parse(std::istream &from)
{
	readUntil(from, "strecke");
    string tag;
    from >> tag; // edge name
    from >> tag; // "spur"
    from >> tag; // lane no
    from >> tag; // "bei"
    from >> tag; // pos
    tag = readEndSecure(from, "fahrzeugklasse");
    while(tag=="fahrzeugklasse") {
        from >> tag; // class no
        from >> tag; // "zeiten"
        from >> tag; // times no
        tag = readEndSecure(from, "fahrzeugklasse");
    }
    return true;
}


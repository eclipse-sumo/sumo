/***************************************************************************
                          NIVissimSingleTypeParser_VWunschentscheidungsdefinition.cpp

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
// Revision 1.2  2003/10/27 10:52:41  dkrajzew
// edges speed setting implemented (only on an edges begin)
//
// Revision 1.1  2003/02/07 11:08:43  dkrajzew
// Vissim import added (preview)
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <utils/convert/TplConvert.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimEdge.h"
#include "NIVissimSingleTypeParser_VWunschentscheidungsdefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_VWunschentscheidungsdefinition::NIVissimSingleTypeParser_VWunschentscheidungsdefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_VWunschentscheidungsdefinition::~NIVissimSingleTypeParser_VWunschentscheidungsdefinition()
{
}


bool
NIVissimSingleTypeParser_VWunschentscheidungsdefinition::parse(std::istream &from)
{
    string tag;
    from >> tag; // id
    from >> tag; // name
    tag = readName(from);
    tag = overrideOptionalLabel(from);
    from >> tag; // strecke
    string edgeid;
    from >> edgeid;
    from >> tag; // spur
    string lane;
    from >> lane;
    from >> tag; // bei
    string pos;
    from >> pos;
    from >> tag; // fahrzeugklasse
    from >> tag; // <fahrzeugklasse>
    from >> tag; // vwunsch
    string vwunsch;
    from >> vwunsch; // vwunsch
    tag = readEndSecure(from, "zeit");
    while(tag!="DATAEND"&&tag!="zeit") {
        from >> tag;
        from >> tag;
        from >> tag;
        tag = myRead(from);
    }
    if(tag=="zeit") {
        from >> tag;
        from >> tag;
        from >> tag;
        from >> tag;
    }
    NIVissimEdge::replaceSpeed(
        TplConvert<char>::_2int(edgeid.c_str()),
        TplConvert<char>::_2int(lane.c_str()) - 1,
        TplConvert<char>::_2float(vwunsch.c_str()));

    return true;
}


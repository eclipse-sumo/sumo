/***************************************************************************
                          NIVissimSingleTypeParser_Zuflussdefinition.cpp

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
// Revision 1.5  2005/10/07 11:40:30  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/23 06:02:58  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.3  2005/04/27 12:24:39  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.2  2003/05/20 09:42:38  dkrajzew
// all data types implemented
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
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <iostream>
#include <utils/common/TplConvert.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimSource.h"
#include "NIVissimSingleTypeParser_Zuflussdefinition.h"

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
NIVissimSingleTypeParser_Zuflussdefinition::NIVissimSingleTypeParser_Zuflussdefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Zuflussdefinition::~NIVissimSingleTypeParser_Zuflussdefinition()
{
}


bool
NIVissimSingleTypeParser_Zuflussdefinition::parse(std::istream &from)
{
	string id, edgeid;
    from >> id; // type-checking is missing!
    string tag, name;
    // override some optional values till q
    while(tag!="q") {
        tag = overrideOptionalLabel(from);
        if(tag=="name") {
            name = readName(from);
        } else if(tag=="strecke") {
            from >> edgeid; // type-checking is missing!
        }
    }
    // read q
    bool exact = false;
    SUMOReal q;
    tag = myRead(from);
    if(tag=="exakt") {
        exact = true;
        tag = myRead(from);
    }
    q = TplConvert<char>::_2SUMOReal(tag.c_str());
    // read the vehicle types
    from >> tag;
    int vehicle_combination;
    from >> vehicle_combination;
    // check whether optional time information is available
    tag = readEndSecure(from);
    SUMOReal beg, end;
    beg = -1;
    end = -1;
    if(tag=="zeit") {
        from >> tag;
        from >> beg;
        from >> tag;
        from >> end;
    }
    return NIVissimSource::dictionary(id, name, edgeid, q, exact, vehicle_combination,
        beg, end);
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

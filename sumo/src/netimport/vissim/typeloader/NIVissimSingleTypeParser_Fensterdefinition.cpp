/***************************************************************************
                          NIVissimSingleTypeParser_Fensterdefinition.cpp

                             -------------------
    begin                : Fri, 21 Mar 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.2  2003/04/01 15:24:43  dkrajzew
// parsing of parking places patched
//
// Revision 1.1  2003/03/26 12:17:14  dkrajzew
// further debugging/improvements of Vissim-import
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
#include "../tempstructs/NIVissimSource.h"
#include "NIVissimSingleTypeParser_Fensterdefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Fensterdefinition::NIVissimSingleTypeParser_Fensterdefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Fensterdefinition::~NIVissimSingleTypeParser_Fensterdefinition()
{
}


bool
NIVissimSingleTypeParser_Fensterdefinition::parse(std::istream &from)
{
	string id;
    from >> id; // "typ"
    string type = myRead(from);
    if(type=="fzinfo") {
        string tmp;
        from >> tmp;
        from >> tmp;
    } else if (type=="ldp"||type=="szp") {
        string tmp;
        readUntil(from, "lsa");
    }
    return true;
}


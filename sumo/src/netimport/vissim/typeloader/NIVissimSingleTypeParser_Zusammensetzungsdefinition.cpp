/***************************************************************************
    NIVissimSingleTypeParser_Zusammensetzungsdefinition.cpp

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
// Revision 1.1  2003/04/09 15:53:26  dkrajzew
// netconvert-changes: further work on Vissim-import, documentation added
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <utils/convert/TplConvert.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimSource.h"
#include "NIVissimSingleTypeParser_Zusammensetzungsdefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Zusammensetzungsdefinition::NIVissimSingleTypeParser_Zusammensetzungsdefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Zusammensetzungsdefinition::~NIVissimSingleTypeParser_Zusammensetzungsdefinition()
{
}


bool
NIVissimSingleTypeParser_Zusammensetzungsdefinition::parse(std::istream &from)
{
    string tag = myRead(from);
    while(tag!="fahrzeugtyp") {
        tag = readEndSecure(from, "fahrzeugtyp");
    }
    do {
        tag = myRead(from); // id
        tag = myRead(from); // "anteil"
        tag = myRead(from); // value
        tag = myRead(from); // "VWunsch"
        tag = myRead(from); // value
        tag = readEndSecure(from, "fahrzeugtyp"); // "fahrzeugtyp"?
    } while(tag=="fahrzeugtyp");
    return true;
}


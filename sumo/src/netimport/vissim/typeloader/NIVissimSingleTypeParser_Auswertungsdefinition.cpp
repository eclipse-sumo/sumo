/***************************************************************************
                          NIVissimSingleTypeParser_Auswertungsdefinition.cpp

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
// Revision 1.5  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.4  2003/08/18 12:39:23  dkrajzew
// missing handling of some vissim3.7-structures added
//
// Revision 1.3  2003/05/20 09:42:37  dkrajzew
// all data types implemented
//
// Revision 1.2  2003/04/01 15:24:43  dkrajzew
// parsing of parking places patched
//
// Revision 1.1  2003/03/26 12:17:14  dkrajzew
// further debugging/improvements of Vissim-import
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
#include "../tempstructs/NIVissimSource.h"
#include "NIVissimSingleTypeParser_Auswertungsdefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Auswertungsdefinition::NIVissimSingleTypeParser_Auswertungsdefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Auswertungsdefinition::~NIVissimSingleTypeParser_Auswertungsdefinition()
{
}


bool
NIVissimSingleTypeParser_Auswertungsdefinition::parse(std::istream &from)
{
	string id;
    from >> id; // "typ"

    if(id=="DATENBANK") { // !!! unverified
        return true;
    }

    string type = myRead(from);
    if(type=="abfluss") {
        while(type!="signalgruppe") {
            type = myRead(from);
        }
		while(type!="DATAEND") {
			type = readEndSecure(from, "messung");
		}
    } else if (type=="vbv") {
    } else if (type=="dichte") {
    } else if (type=="emissionen") {
    } else if (type=="fzprot") {
    } else if (type=="spwprot") {
    } else if (type=="segment") {
        while(type!="konfdatei") {
            type = myRead(from);
        }
    } else if (type=="wegeausw") {
    } else if (type=="knoten") {
    } else if (type=="konvergenz") {
        while(type!="zeit") {
            type = myRead(from);
        }
    }
    return true;
}


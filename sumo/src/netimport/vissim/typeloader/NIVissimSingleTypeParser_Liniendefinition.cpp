/***************************************************************************
                          NIVissimSingleTypeParser_Liniendefinition.cpp

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
#include "../tempstructs/NIVissimClosures.h"
#include "NIVissimSingleTypeParser_Liniendefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Liniendefinition::NIVissimSingleTypeParser_Liniendefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Liniendefinition::~NIVissimSingleTypeParser_Liniendefinition()
{
}


bool
NIVissimSingleTypeParser_Liniendefinition::parse(std::istream &from)
{
    string tag;
    while(tag!="haltestelle") {
        tag = myRead(from);
    }
    while(tag!="DATAEND") {
        from >> tag;
        from >> tag;
        while(tag!="DATAEND"&&tag!="haltestelle") {
            tag = readEndSecure(from, "haltestelle");
        }
    }
    return true;
}


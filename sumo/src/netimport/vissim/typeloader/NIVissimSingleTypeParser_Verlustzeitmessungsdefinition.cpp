/***************************************************************************
                          NIVissimSingleTypeParser_Verlustzeitmessungsdefinition.cpp

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
#include "NIVissimSingleTypeParser_Verlustzeitmessungsdefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Verlustzeitmessungsdefinition::NIVissimSingleTypeParser_Verlustzeitmessungsdefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Verlustzeitmessungsdefinition::~NIVissimSingleTypeParser_Verlustzeitmessungsdefinition()
{
}


bool
NIVissimSingleTypeParser_Verlustzeitmessungsdefinition::parse(std::istream &from)
{
    string tag = myRead(from);
    if(tag=="von") {
        while(tag!="DATAEND") {
            tag = readEndSecure(from);
        }
        return true;
    }

    while(tag!="fahrzeugklassen") {
        tag = myRead(from);
    }
    while(tag!="DATAEND") {
        tag = readEndSecure(from);
    }
    return true;
}


/***************************************************************************
                          NIVissimSingleTypeParser_DynUml.cpp

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
// Revision 1.3  2003/03/20 16:32:23  dkrajzew
// windows eol removed
//
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
#include "NIVissimSingleTypeParser_DynUml.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_DynUml::NIVissimSingleTypeParser_DynUml(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_DynUml::~NIVissimSingleTypeParser_DynUml()
{
}


bool
NIVissimSingleTypeParser_DynUml::parse(std::istream &from)
{
    readUntil(from, "kirchhoffexponent");
    string tag = readEndSecure(from, "reisezeit");
    while(tag!="DATAEND") {
        tag = readEndSecure(from, "reisezeit");
    }
    return true;
}


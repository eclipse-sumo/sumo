/***************************************************************************
    NIVissimSingleTypeParser__XVerteilungsdefinition.cpp

                             -------------------
    begin                : Wed, 30 Apr 2003
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
// Revision 1.2  2003/07/22 15:11:25  dkrajzew
// removed warnings
//
// Revision 1.1  2003/05/20 09:42:38  dkrajzew
// all data types implemented
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <utils/convert/TplConvert.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimSource.h"
#include "NIVissimSingleTypeParser__XVerteilungsdefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser__XVerteilungsdefinition::NIVissimSingleTypeParser__XVerteilungsdefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser__XVerteilungsdefinition::~NIVissimSingleTypeParser__XVerteilungsdefinition()
{
}


bool
NIVissimSingleTypeParser__XVerteilungsdefinition::parse(std::istream &)
{
    return true;
}


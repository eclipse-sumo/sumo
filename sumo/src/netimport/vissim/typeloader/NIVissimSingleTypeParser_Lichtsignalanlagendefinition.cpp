/***************************************************************************
                          NIVissimSingleTypeParser_Lichtsignalanlagendefinition.cpp

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
#include <utils/common/SErrorHandler.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimTL.h"
#include "NIVissimSingleTypeParser_Lichtsignalanlagendefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Lichtsignalanlagendefinition::NIVissimSingleTypeParser_Lichtsignalanlagendefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Lichtsignalanlagendefinition::~NIVissimSingleTypeParser_Lichtsignalanlagendefinition()
{
}


bool
NIVissimSingleTypeParser_Lichtsignalanlagendefinition::parse(std::istream &from)
{
    //
	int id;
    from >> id;
    //
    string tag, name;
    tag = myRead(from);
    if(tag=="name") {
        name = readName(from);
        tag = myRead(from);
    }
    // type
    string type;
    type = myRead(from);
    if(type=="festzeit") {
        return parseFixedTime(id, name, from);
    }
    SErrorHandler::add(
        string("Unsupported LSA-Type '") + type + string("' occured."));
    return false;
}


bool
NIVissimSingleTypeParser_Lichtsignalanlagendefinition::parseFixedTime(
        int id, std::string name, std::istream &from)
{
    string tag;
    from >> tag;
    //
    double absdur;
    from >> absdur; // !!!
    //
    tag = readEndSecure(from);
    double offset = 0;
    if(tag=="versatz") {
        from >> offset; // !!!
    }
    return NIVissimTL::dictionary(id, name, absdur, offset);
}

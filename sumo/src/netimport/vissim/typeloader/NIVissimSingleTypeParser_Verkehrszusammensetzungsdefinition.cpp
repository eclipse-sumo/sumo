/***************************************************************************
                          NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition.cpp

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
// Revision 1.4  2005/04/27 12:24:39  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.3  2003/10/27 10:52:41  dkrajzew
// edges speed setting implemented (only on an edges begin)
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
#include <iostream>
#include <utils/convert/TplConvert.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimVehicleClass.h"
#include "../tempstructs/NIVissimVehicleClassVector.h"
#include "../tempstructs/NIVissimTrafficDescription.h"
#include "NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition::NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition::~NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition()
{
}


bool
NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition::parse(std::istream &from)
{
    // id
	int id;
    from >> id; // type-checking is missing!
    // name
    string tag;
    from >> tag;
    string name = readName(from);
    // assigned vehicle classes
    tag = myRead(from);
    if(tag=="temperatur") {
        tag = myRead(from);
        tag = myRead(from);
    }
    NIVissimVehicleClassVector assignedTypes;
    while(tag!="DATAEND") {
        int type;
        from >> type;
        double percentage;
        from >> tag;
        from >> percentage;
        int vwish;
        from >> tag;
        from >> vwish;
        assignedTypes.push_back(new NIVissimVehicleClass(type, percentage, vwish));
        tag = readEndSecure(from, "Fahrzeugtyp");
    }
    //
    return NIVissimTrafficDescription::dictionary(id, name, assignedTypes);
}


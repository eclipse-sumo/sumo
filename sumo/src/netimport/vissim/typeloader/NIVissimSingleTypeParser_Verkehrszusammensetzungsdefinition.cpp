/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id: $
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utils/common/TplConvert.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimVehicleClass.h"
#include "../tempstructs/NIVissimVehicleClassVector.h"
#include "../tempstructs/NIVissimTrafficDescription.h"
#include "NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition::NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition(NIVissimLoader &parent)
        : NIVissimLoader::VissimSingleTypeParser(parent)
{}


NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition::~NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition()
{}


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
    if (tag=="temperatur") {
        tag = myRead(from);
        tag = myRead(from);
    }
    NIVissimVehicleClassVector assignedTypes;
    while (tag!="DATAEND") {
        int type;
        from >> type;
        SUMOReal percentage;
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



/****************************************************************************/


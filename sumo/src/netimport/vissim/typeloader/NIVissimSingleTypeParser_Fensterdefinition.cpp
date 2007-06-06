/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Fensterdefinition.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 21 Mar 2003
/// @version $Id$
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utils/common/TplConvert.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimSource.h"
#include "NIVissimSingleTypeParser_Fensterdefinition.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Fensterdefinition::NIVissimSingleTypeParser_Fensterdefinition(NIVissimLoader &parent)
        : NIVissimLoader::VissimSingleTypeParser(parent)
{}


NIVissimSingleTypeParser_Fensterdefinition::~NIVissimSingleTypeParser_Fensterdefinition()
{}


bool
NIVissimSingleTypeParser_Fensterdefinition::parse(std::istream &from)
{
    string id;
    from >> id; // "typ"
    string type = myRead(from);
    if (type=="fzinfo") {
        string tmp;
        from >> tmp;
        from >> tmp;
    } else if (type=="ldp"||type=="szp") {
        string tmp;
        readUntil(from, "lsa");
    }
    return true;
}



/****************************************************************************/


/****************************************************************************/
/// @file    NIVisumParser_Types.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 14 Nov 2002
/// @version $Id: $
///
// Parser for visum-road types
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

#include <utils/common/TplConvert.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/NBType.h>
#include <netbuild/NBTypeCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_Types.h"

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
NIVisumParser_Types::NIVisumParser_Types(NIVisumLoader &parent,
        NBTypeCont &tc,
        const std::string &dataName,
        NBCapacity2Lanes &cap2lanes)
        : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
        myCap2Lanes(cap2lanes), myTypeCont(tc)
{}


NIVisumParser_Types::~NIVisumParser_Types()
{}


void
NIVisumParser_Types::myDependentReport()
{
    string id;
    try {
        // get the id
        id = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
        // get the maximum speed
        SUMOReal speed = getNamedFloat("v0-IV", "V0IV");
        // get the priority
        int priority = TplConvert<char>::_2int(myLineParser.get("Rang").c_str());
        // try to retrieve the number of lanes
        SUMOReal cap = getNamedFloat("Kap-IV", "KAPIV");
        int nolanes = myCap2Lanes.get(cap);
        // insert the type
        NBType *type = new NBType(id, nolanes, speed/(SUMOReal) 3.6, 100-priority,
                                  NBEdge::EDGEFUNCTION_NORMAL);
        if (!myTypeCont.insert(type)) {
            addError(" Duplicate type occured ('" + id + "').");
            delete type;
        }
    } catch (OutOfBoundsException) {
        addError2("STRECKENTYP", id, "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("STRECKENTYP", id, "NumberFormat");
    } catch (UnknownElement) {
        addError2("STRECKENTYP", id, "UnknownElement");
    }
}



/****************************************************************************/


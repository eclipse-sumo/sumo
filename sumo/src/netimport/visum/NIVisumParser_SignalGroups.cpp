/****************************************************************************/
/// @file    NIVisumParser_SignalGroups.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 09 May 2003
/// @version $Id$
///
// Parser for visum-SignalGroups
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

#include <utils/common/TplConvert.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_SignalGroups.h"
#include "NIVisumTL.h"

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
NIVisumParser_SignalGroups::NIVisumParser_SignalGroups(NIVisumLoader &parent,
        const std::string &dataName, NIVisumLoader::NIVisumTL_Map &NIVisumTLs)
        : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
        myNIVisumTLs(NIVisumTLs)
{}


NIVisumParser_SignalGroups::~NIVisumParser_SignalGroups()
{}


void
NIVisumParser_SignalGroups::myDependentReport()
{
    std::string id;
    std::string LSAid;
    try {
        // get the id
        id = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
        LSAid = NBHelpers::normalIDRepresentation(myLineParser.get("LsaNr"));
        // StartTime
        SUMOReal StartTime = getNamedFloat("GzStart", "GRUENANF");
        // EndTime
        SUMOReal EndTime = getNamedFloat("GzEnd", "GRUENENDE");
        // add to the list
        (*myNIVisumTLs.find(LSAid)).second->AddSignalGroup(id, (SUMOTime) StartTime, (SUMOTime) EndTime);
    } catch (OutOfBoundsException &) {
        addError2("SignalGroups", "LSA:" + LSAid + " SignalGroup:" + id, "OutOfBounds");
    } catch (NumberFormatException &) {
        addError2("SignalGroups", "LSA:" + LSAid + " SignalGroup:" + id, "NumberFormat");
    } catch (UnknownElement &) {
        addError2("SignalGroups", "LSA:" + LSAid + " SignalGroup:" + id, "UnknownElement");
    }
}



/****************************************************************************/


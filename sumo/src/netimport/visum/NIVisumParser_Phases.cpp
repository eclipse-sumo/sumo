/****************************************************************************/
/// @file    NIVisumParser_Phases.cpp
/// @author  unknown_author
/// @date    Fri, 09 May 2003
/// @version $Id: $
///
// Parser for visum-Phases
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
#include <netbuild/nodes/NBNodeCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_Phases.h"
#include "NIVisumTL.h"

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
NIVisumParser_Phases::NIVisumParser_Phases(NIVisumLoader &parent,
        const std::string &dataName, NIVisumLoader::NIVisumTL_Map &NIVisumTLs)
        : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
        myNIVisumTLs(NIVisumTLs)
{}


NIVisumParser_Phases::~NIVisumParser_Phases()
{}


void
NIVisumParser_Phases::myDependentReport()
{
    std::string Phaseid;
    std::string LSAid;
    try {
        // get the id
        Phaseid = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
        LSAid = NBHelpers::normalIDRepresentation(myLineParser.get("LsaNr"));
        // StartTime
        SUMOReal StartTime = getNamedFloat("GzStart", "GRUENANF");
        // EndTime
        SUMOReal EndTime = getNamedFloat("GzEnd", "GRUENENDE");
        // add to the list
        (*myNIVisumTLs.find(LSAid)).second->AddPhase(Phaseid, (SUMOTime) StartTime, (SUMOTime) EndTime);
    } catch (OutOfBoundsException) {
        addError2("Phases", "LSA:" + LSAid + " Phase:" + Phaseid, "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("Phases", "LSA:" + LSAid + " Phase:" + Phaseid, "NumberFormat");
    } catch (UnknownElement) {
        addError2("Phases", "LSA:" + LSAid + " Phase:" + Phaseid, "UnknownElement");
    }
}



/****************************************************************************/


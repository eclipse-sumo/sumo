/****************************************************************************/
/// @file    NIVisumParser_SignalGroupsToPhases.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 09 May 2003
/// @version $Id$
///
// Parser for visum-SignalGroupsToPhases
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
#include "NIVisumParser_SignalGroupsToPhases.h"
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
NIVisumParser_SignalGroupsToPhases::NIVisumParser_SignalGroupsToPhases(NIVisumLoader &parent,
        const std::string &dataName, NIVisumLoader::NIVisumTL_Map &NIVisumTLs)
        : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
        myNIVisumTLs(NIVisumTLs)
{}


NIVisumParser_SignalGroupsToPhases::~NIVisumParser_SignalGroupsToPhases()
{}


void
NIVisumParser_SignalGroupsToPhases::myDependentReport()
{
    std::string Phaseid;
    std::string LSAid;
    std::string SGid;
    try {
        // get the id
        Phaseid = NBHelpers::normalIDRepresentation(myLineParser.get("PsNr"));
        LSAid = NBHelpers::normalIDRepresentation(myLineParser.get("LsaNr"));
        SGid = NBHelpers::normalIDRepresentation(myLineParser.get("SGNR"));
        // insert
        NIVisumTL::Phase *PH;
        NIVisumTL::SignalGroup *SG;
        NIVisumTL *LSA;
        LSA = (*myNIVisumTLs.find(LSAid)).second;
        SG = LSA->GetSignalGroup(SGid);
        PH = (*LSA->GetPhases()->find(Phaseid)).second;
        (*SG->GetPhases())[Phaseid] = PH;
    } catch (OutOfBoundsException &) {
        addError2("SignalGroupsToPhases", "LSA:" + LSAid + " Phase:" + Phaseid, "OutOfBounds");
    } catch (NumberFormatException &) {
        addError2("SignalGroupsToPhases", "LSA:" + LSAid + " Phase:" + Phaseid, "NumberFormat");
    } catch (UnknownElement &) {
        addError2("SignalGroupsToPhases", "LSA:" + LSAid + " Phase:" + Phaseid, "UnknownElement");
    }
}



/****************************************************************************/


/****************************************************************************/
/// @file    NIVisumParser_TrafficLights.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 09 May 2003
/// @version $Id: $
///
// Parser for visum-TrafficLights
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
#include "NIVisumParser_TrafficLights.h"
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
NIVisumParser_TrafficLights::NIVisumParser_TrafficLights(NIVisumLoader &parent,
        const std::string &dataName, NIVisumLoader::NIVisumTL_Map &NIVisumTLs)
        : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
        myNIVisumTLs(NIVisumTLs)
{}


NIVisumParser_TrafficLights::~NIVisumParser_TrafficLights()
{}


void
NIVisumParser_TrafficLights::myDependentReport()
{
    std::string id;
    try {
        // get the id
        id = NBHelpers::normalIDRepresentation(myLineParser.get("Nr"));
        // cycle time
        SUMOReal CycleTime = getNamedFloat("Umlaufzeit", "UMLZEIT");
        // IntermediateTime
        SUMOReal IntermediateTime = getNamedFloat("StdZwischenzeit", "STDZWZEIT");
        // PhaseBased
        bool PhaseBased =
            myLineParser.know("PhasenBasiert")
            ? TplConvert<char>::_2bool(myLineParser.get("PhasenBasiert").c_str())
            : false;
        // add to the list
        myNIVisumTLs[id] = new NIVisumTL(id, (SUMOTime) CycleTime, (SUMOTime) IntermediateTime, PhaseBased);
    } catch (OutOfBoundsException) {
        addError2("LSA", id, "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("LSA", id, "NumberFormat");
    } catch (UnknownElement) {
        addError2("LSA", id, "UnknownElement");
    }
}



/****************************************************************************/


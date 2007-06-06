/****************************************************************************/
/// @file    NIVisumParser_Turns.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 14 Nov 2002
/// @version $Id$
///
// Parser for visum-turn definitions
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

#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <utils/common/UtilExceptions.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_Turns.h"

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
NIVisumParser_Turns::NIVisumParser_Turns(NIVisumLoader &parent,
        NBNodeCont &nc,
        const std::string &dataName, NIVisumLoader::VSysTypeNames &vsystypes)
        : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
        usedVSysTypes(vsystypes), myNodeCont(nc)
{}


NIVisumParser_Turns::~NIVisumParser_Turns()
{}


void
NIVisumParser_Turns::myDependentReport()
{
    try {
        // retrieve the nodes
        NBNode *from = getNamedNode(myNodeCont, "ABBIEGEBEZIEHUNG", "VonKnot", "VonKnotNr");
        NBNode *via = getNamedNode(myNodeCont, "ABBIEGEBEZIEHUNG", "UeberKnot", "UeberKnotNr");
        NBNode *to = getNamedNode(myNodeCont, "ABBIEGEBEZIEHUNG", "NachKnot", "NachKnotNr");
        if (from==0||via==0||to==0) {
            return;
        }
        // all nodes are known
        if (isVehicleTurning()) {
            // try to set the turning definition
            via->setTurningDefinition(from, to);
        }
    } catch (OutOfBoundsException &) {
        addError2("ABBIEGEBEZIEHUNG", "", "OutOfBounds");
    } catch (NumberFormatException &) {
        addError2("ABBIEGEBEZIEHUNG", "", "NumberFormat");
    } catch (UnknownElement &) {
        addError2("ABBIEGEBEZIEHUNG", "", "UnknownElement");
    }
}


bool
NIVisumParser_Turns::isVehicleTurning()
{
    string type =
        myLineParser.know("VSysCode")
        ? myLineParser.get("VSysCode")
        : myLineParser.get("VSYSSET");
    return usedVSysTypes.find(type)!=usedVSysTypes.end() &&
           usedVSysTypes.find(type)->second=="IV";
}



/****************************************************************************/


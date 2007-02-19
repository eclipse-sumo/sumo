/****************************************************************************/
/// @file    NIVisumParser_VSysTypes.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 14 Nov 2002
/// @version $Id$
///
// Parser for visum-modality types
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

#include <utils/common/UtilExceptions.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_VSysTypes.h"

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
NIVisumParser_VSysTypes::NIVisumParser_VSysTypes(NIVisumLoader &parent,
        const std::string &dataName,
        NIVisumLoader::VSysTypeNames &vsystypes)
        : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
        usedVSysTypes(vsystypes)
{}


NIVisumParser_VSysTypes::~NIVisumParser_VSysTypes()
{}


void
NIVisumParser_VSysTypes::myDependentReport()
{
    try {
        string name =
            myLineParser.know("VSysCode")
            ? myLineParser.get("VSysCode").c_str()
            : myLineParser.get("CODE").c_str();
        string type =
            myLineParser.know("VSysMode")
            ? myLineParser.get("VSysMode").c_str()
            : myLineParser.get("Typ").c_str();
        usedVSysTypes[name] = type;
    } catch (OutOfBoundsException) {
        addError2("VSYS", "", "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("VSYS", "", "NumberFormat");
    } catch (UnknownElement) {
        addError2("VSYS", "", "UnknownElement");
    }
}



/****************************************************************************/


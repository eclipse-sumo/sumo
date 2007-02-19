/****************************************************************************/
/// @file    NIArtemisParser_Signals.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10 Feb 2003
/// @version $Id$
///
// -------------------
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
#include <utils/common/TplConvert.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIArtemisLoader.h"
#include "NIArtemisParser_Signals.h"

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
NIArtemisParser_Signals::NIArtemisParser_Signals(NIArtemisLoader &parent,
        const std::string &dataName)
        : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName)
{}


NIArtemisParser_Signals::~NIArtemisParser_Signals()
{}


void
NIArtemisParser_Signals::myDependentReport()
{
    /*    string nodeid = myLineParser.get("NodeID");
        // scats info omitted
        int noPhases =
            TplConvert<char>::_2int(myLineParser.get("NPhases").c_str());
        int cycle =
            TplConvert<char>::_2int(myLineParser.get("Cycle").c_str());
        int offset =
            TplConvert<char>::_2int(myLineParser.get("Offset").c_str());*/
//    !!! insert into node
}



/****************************************************************************/


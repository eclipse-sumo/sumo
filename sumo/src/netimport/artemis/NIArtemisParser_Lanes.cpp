/****************************************************************************/
/// @file    NIArtemisParser_Lanes.cpp
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/UtilExceptions.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <utils/common/TplConvert.h>
#include "NIArtemisLoader.h"
#include "NIArtemisTempEdgeLanes.h"
#include "NIArtemisParser_Lanes.h"

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
NIArtemisParser_Lanes::NIArtemisParser_Lanes(NBDistrictCont &dc,
        NBNodeCont &nc, NBEdgeCont &ec,
        NIArtemisLoader &parent, const std::string &dataName)
        : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName),
        myNodeCont(nc), myEdgeCont(ec), myDistrictCont(dc)
{}


NIArtemisParser_Lanes::~NIArtemisParser_Lanes()
{}


void
NIArtemisParser_Lanes::myDependentReport()
{
    string link = myLineParser.get("Link");
    int lane =
        TplConvert<char>::_2int(myLineParser.get("Lane").c_str());
    int section =
        TplConvert<char>::_2int(myLineParser.get("Section").c_str());
    SUMOReal start =
        TplConvert<char>::_2SUMOReal(myLineParser.get("Start").c_str());
    SUMOReal end =
        TplConvert<char>::_2SUMOReal(myLineParser.get("End").c_str());
    // purpose omitted
    NIArtemisTempEdgeLanes::add(link, lane, section, start, end);
}


void
NIArtemisParser_Lanes::myClose()
{
    NIArtemisTempEdgeLanes::close(myDistrictCont, myEdgeCont, myNodeCont);
}



/****************************************************************************/


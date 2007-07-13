/****************************************************************************/
/// @file    NIArtemisParser_Segments.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 11 Mar 2003
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
#include "NIArtemisTempSegments.h"
#include "NIArtemisParser_Segments.h"

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
NIArtemisParser_Segments::NIArtemisParser_Segments(NBEdgeCont &ec,
        NIArtemisLoader &parent, const std::string &dataName)
        : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName),
        myEdgeCont(ec)
{}


NIArtemisParser_Segments::~NIArtemisParser_Segments()
{}


void
NIArtemisParser_Segments::myDependentReport()
{
    string linkname = myLineParser.get("Link");
    int segment =
        TplConvert<char>::_2int(myLineParser.get("Segment").c_str());
    SUMOReal x =
        TplConvert<char>::_2SUMOReal(myLineParser.get("StartX").c_str());
    SUMOReal y =
        TplConvert<char>::_2SUMOReal(myLineParser.get("StartY").c_str());
    // length omitted
    // speed omitted!!!
    NIArtemisTempSegments::add(linkname, segment, x, y);
}


void
NIArtemisParser_Segments::myClose()
{
    NIArtemisTempSegments::close(myEdgeCont);
}



/****************************************************************************/


/****************************************************************************/
/// @file    NIVisumParser_NodesToTrafficLights.cpp
/// @author  unknown_author
/// @date    Fri, 09 May 2003
/// @version $Id: $
///
// Parser for visum- NodesToTrafficLights relation
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
#include "NIVisumParser_NodesToTrafficLights.h"
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
NIVisumParser_NodesToTrafficLights::NIVisumParser_NodesToTrafficLights(
    NIVisumLoader &parent, NBNodeCont &nc,
    const std::string &dataName, NIVisumLoader::NIVisumTL_Map &NIVisumTLs)
        : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
        myNIVisumTLs(NIVisumTLs), myNodeCont(nc)
{}


NIVisumParser_NodesToTrafficLights::~NIVisumParser_NodesToTrafficLights()
{}


void
NIVisumParser_NodesToTrafficLights::myDependentReport()
{
    std::string TrafficLight;
    std::string Node;
    try {
        Node = myLineParser.get("KnotNr").c_str();
        TrafficLight = myLineParser.get("LsaNr").c_str();
        // add to the list
        myNIVisumTLs[TrafficLight]->GetNodes()->push_back(myNodeCont.retrieve(Node));
    } catch (OutOfBoundsException) {
        addError2("KNOTENZULSA", "TrafficLight:" + TrafficLight + " Node:" + Node , "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("KNOTENZULSA", "TrafficLight:" + TrafficLight + " Node:" + Node , "NumberFormat");
    } catch (UnknownElement) {
        addError2("KNOTENZULSA", "TrafficLight:" + TrafficLight + " Node:" + Node , "UnknownElement");
    }
}



/****************************************************************************/


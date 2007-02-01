/****************************************************************************/
/// @file    NIArtemisTempSignal.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
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


#include <string>
#include <map>
#include <netbuild/NBConnectionDefs.h>
#include "NIArtemisTempSignal.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

NIArtemisTempSignal::NodeToDescsMap     NIArtemisTempSignal::myDescDict;
NIArtemisTempSignal::ConnectionToPhases NIArtemisTempSignal::myPhasesDict;

NIArtemisTempSignal::PhaseDescription::PhaseDescription(SUMOReal perc,
        size_t start)
        : myPercentage(perc), myBegin(start)
{}


NIArtemisTempSignal::PhaseDescription::~PhaseDescription()
{}




NIArtemisTempSignal::ConnectionPhases::ConnectionPhases(char startPhase,
        char endPhase)
        : myStartPhase(startPhase), myEndPhase(endPhase)
{}


NIArtemisTempSignal::ConnectionPhases::~ConnectionPhases()
{}





void
NIArtemisTempSignal::addPhase(const std::string &nodeid,
                              char phaseName, SUMOReal perc, int start)
{
    myDescDict[nodeid][phaseName] =
        new PhaseDescription(perc, start);
}

void
NIArtemisTempSignal::addConnectionPhases(const NBConnection &c,
        char startPhase, char endPhase,
        const std::string &/*group*/)
{
    myPhasesDict[c] =
        new ConnectionPhases(startPhase, endPhase);
}


void
NIArtemisTempSignal::close()
{
    //throw 1;
}



/****************************************************************************/


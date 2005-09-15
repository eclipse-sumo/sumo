//---------------------------------------------------------------------------//
//                        NIArtemisTempSignal.cpp -  ccc
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.7  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/04/27 12:24:25  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.5  2003/06/05 11:44:51  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H


#include <string>
#include <map>
#include <netbuild/NBConnectionDefs.h>
#include "NIArtemisTempSignal.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

NIArtemisTempSignal::NodeToDescsMap     NIArtemisTempSignal::myDescDict;
NIArtemisTempSignal::ConnectionToPhases NIArtemisTempSignal::myPhasesDict;

NIArtemisTempSignal::PhaseDescription::PhaseDescription(double perc,
                                                        size_t start)
    : myPercentage(perc), myBegin(start)
{
}


NIArtemisTempSignal::PhaseDescription::~PhaseDescription()
{
}




NIArtemisTempSignal::ConnectionPhases::ConnectionPhases(char startPhase,
                                                        char endPhase)
    : myStartPhase(startPhase), myEndPhase(endPhase)
{
}


NIArtemisTempSignal::ConnectionPhases::~ConnectionPhases()
{
}





void
NIArtemisTempSignal::addPhase(const std::string &nodeid,
                              char phaseName, double perc, int start)
{
    myDescDict[nodeid][phaseName] =
        new PhaseDescription(perc, start);
}

void
NIArtemisTempSignal::addConnectionPhases(const NBConnection &c,
                                         char startPhase, char endPhase,
                                         const std::string &group)
{
    myPhasesDict[c] =
        new ConnectionPhases(startPhase, endPhase);
}


void
NIArtemisTempSignal::close()
{
    //throw 1;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



/***************************************************************************
                          NIArtemisParser_SignalPhases.cpp
                             -------------------
    project              : SUMO
    begin                : Mon, 10 Feb 2003
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.1  2003/03/03 15:00:33  dkrajzew
// initial commit for artemis-import files
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/common/UtilExceptions.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <utils/convert/TplConvert.h>
#include "NIArtemisLoader.h"
#include "NIArtemisTempSignal.h"
#include "NIArtemisParser_SignalPhases.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIArtemisParser_SignalPhases::NIArtemisParser_SignalPhases(NIArtemisLoader &parent,
        const std::string &dataName)
    : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName)
{
}


NIArtemisParser_SignalPhases::~NIArtemisParser_SignalPhases()
{
}


void
NIArtemisParser_SignalPhases::myDependentReport()
{
    string nodeid = myLineParser.get("NodeID");
    string phase = myLineParser.get("Phase");
    double perc = 
        TplConvert<char>::_2float(myLineParser.get("PercentCT").c_str());
    int start = 
        TplConvert<char>::_2int(myLineParser.get("Start").c_str());
//    !!! insert
    NIArtemisTempSignal::addPhase(
        nodeid, phase.at(0), perc, start);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIArtemisParser_SignalPhases.icc"
//#endif

// Local Variables:
// mode:C++
// End:




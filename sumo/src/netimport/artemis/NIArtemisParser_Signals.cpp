/***************************************************************************
                          NIArtemisParser_Signals.cpp
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
// Revision 1.2  2003/03/17 14:18:58  dkrajzew
// Windows eol removed
//
// Revision 1.1  2003/03/03 15:00:33  dkrajzew
// initial commit for artemis-import files
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/common/UtilExceptions.h>
#include <utils/convert/TplConvert.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include "NIArtemisLoader.h"
#include "NIArtemisParser_Signals.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIArtemisParser_Signals::NIArtemisParser_Signals(NIArtemisLoader &parent,
        const std::string &dataName)
    : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName)
{
}


NIArtemisParser_Signals::~NIArtemisParser_Signals()
{
}


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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIArtemisParser_Signals.icc"
//#endif

// Local Variables:
// mode:C++
// End:



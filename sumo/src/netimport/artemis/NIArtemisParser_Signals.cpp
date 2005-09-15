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
// Revision 1.5  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/04/27 12:24:25  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.3  2004/01/12 15:30:47  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.2  2003/03/17 14:18:58  dkrajzew
// Windows eol removed
//
// Revision 1.1  2003/03/03 15:00:33  dkrajzew
// initial commit for artemis-import files
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

#include <utils/common/UtilExceptions.h>
#include <utils/convert/TplConvert.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIArtemisLoader.h"
#include "NIArtemisParser_Signals.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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

// Local Variables:
// mode:C++
// End:



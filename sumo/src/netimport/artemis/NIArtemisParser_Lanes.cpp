/***************************************************************************
                          NIArtemisParser_Lanes.cpp
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
// Revision 1.3  2004/01/12 15:30:31  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.2  2003/03/17 14:18:58  dkrajzew
// Windows eol removed
//
// Revision 1.1  2003/03/12 16:44:45  dkrajzew
// further work on artemis-import
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/common/UtilExceptions.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <utils/convert/TplConvert.h>
#include "NIArtemisLoader.h"
#include "NIArtemisTempEdgeLanes.h"
#include "NIArtemisParser_Lanes.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIArtemisParser_Lanes::NIArtemisParser_Lanes(NIArtemisLoader &parent,
        const std::string &dataName)
    : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName)
{
}


NIArtemisParser_Lanes::~NIArtemisParser_Lanes()
{
}


void
NIArtemisParser_Lanes::myDependentReport()
{
    string link = myLineParser.get("Link");
    int lane =
        TplConvert<char>::_2int(myLineParser.get("Lane").c_str());
    int section =
        TplConvert<char>::_2int(myLineParser.get("Section").c_str());
    double start =
        TplConvert<char>::_2float(myLineParser.get("Start").c_str());
    double end =
        TplConvert<char>::_2float(myLineParser.get("End").c_str());
    string mvmt = myLineParser.get("Mvmt");
    // purpose omitted
    NIArtemisTempEdgeLanes::add(link, lane,
        section, start, end, mvmt);
}


void
NIArtemisParser_Lanes::myClose()
{
    NIArtemisTempEdgeLanes::close();
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIArtemisParser_Lanes.icc"
//#endif

// Local Variables:
// mode:C++
// End:



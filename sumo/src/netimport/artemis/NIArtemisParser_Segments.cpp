/***************************************************************************
                          NIArtemisParser_Segments.cpp
                             -------------------
    project              : SUMO
    begin                : Tue, 11 Mar 2003
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
// Revision 1.1  2003/03/12 16:44:45  dkrajzew
// further work on artemis-import
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
#include "NIArtemisTempSegments.h"
#include "NIArtemisParser_Segments.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIArtemisParser_Segments::NIArtemisParser_Segments(NIArtemisLoader &parent,
        const std::string &dataName)
    : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName)
{
}


NIArtemisParser_Segments::~NIArtemisParser_Segments()
{
}


void
NIArtemisParser_Segments::myDependentReport()
{
    string linkname = myLineParser.get("Link");
    int segment =
        TplConvert<char>::_2int(myLineParser.get("Segment").c_str());
    double x =
        TplConvert<char>::_2float(myLineParser.get("StartX").c_str());
    double y =
        TplConvert<char>::_2float(myLineParser.get("StartY").c_str());
    // length omitted
    // speed omitted!!!
    NIArtemisTempSegments::add(linkname, segment, x, y);
}


void
NIArtemisParser_Segments::myClose()
{
    NIArtemisTempSegments::close();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIArtemisParser_Segments.icc"
//#endif

// Local Variables:
// mode:C++
// End:



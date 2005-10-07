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
// Revision 1.7  2005/10/07 11:39:05  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:01:53  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/04/27 12:24:25  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.3  2004/01/12 15:30:31  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.2  2003/03/17 14:18:58  dkrajzew
// Windows eol removed
//
// Revision 1.1  2003/03/12 16:44:45  dkrajzew
// further work on artemis-import
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <utils/common/UtilExceptions.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <utils/common/TplConvert.h>
#include "NIArtemisLoader.h"
#include "NIArtemisTempSegments.h"
#include "NIArtemisParser_Segments.h"

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
NIArtemisParser_Segments::NIArtemisParser_Segments(NBEdgeCont &ec,
        NIArtemisLoader &parent, const std::string &dataName)
    : NIArtemisLoader::NIArtemisSingleDataTypeParser(parent, dataName),
    myEdgeCont(ec)
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



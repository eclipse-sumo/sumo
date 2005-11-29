//---------------------------------------------------------------------------//
//                        ROAbstractRouteDefLoader.cpp -
//  The basic class for loading routes
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
// Revision 1.8  2005/11/29 13:33:08  dkrajzew
// debugging
//
// Revision 1.7  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/05/04 08:46:09  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.2  2004/02/16 13:47:06  dkrajzew
// Type-dependent loader/generator-"API" changed
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// ---------------------------------------------
// Revision 1.7  2003/06/18 11:20:54  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.6  2003/05/20 09:48:35  dkrajzew
// debugging
//
// Revision 1.5  2003/04/09 15:39:11  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.4  2003/03/17 14:25:28  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/03 15:22:36  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:06  dkrajzew
// updated
//
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

#include <string>
#include "ROAbstractRouteDefLoader.h"
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/options/OptionsCont.h>
#include "RONet.h"

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
ROAbstractRouteDefLoader::ROAbstractRouteDefLoader(ROVehicleBuilder &vb,
                                                   RONet &net,
                                                   SUMOTime begin,
                                                   SUMOTime end,
                                                   const std::string &file)
    : _net(net), myBegin(begin), myEnd(end), myVehicleBuilder(vb)
{
}


ROAbstractRouteDefLoader::~ROAbstractRouteDefLoader()
{
}


void
ROAbstractRouteDefLoader::skipUntilBegin()
{
    myReadRoutesAtLeastUntil(myBegin);
}


void
ROAbstractRouteDefLoader::readRoutesAtLeastUntil(SUMOTime time)
{
    if(!myReadRoutesAtLeastUntil(time)) {
        MsgHandler::getErrorInstance()->inform(
            string("Problems on parsing ") + getDataName() +
            string(" file."));
        throw ProcessError();
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



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
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include "ROAbstractRouteDefLoader.h"
#include <utils/convert/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/options/OptionsCont.h>
#include "RONet.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
ROAbstractRouteDefLoader::ROAbstractRouteDefLoader(RONet &net,
												   const std::string &file)
    : _net(net)
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
ROAbstractRouteDefLoader::readRoutesAtLeastUntil(unsigned int time)
{
    if(!myReadRoutesAtLeastUntil(time)) {
        MsgHandler::getErrorInstance()->inform(
            string("Problems on parsing the ") + getDataName() +
            string(" file."));
        throw ProcessError();
    }
}


bool
ROAbstractRouteDefLoader::checkFile(const std::string &file) const
{
    return FileHelpers::exists(file);
}


bool
ROAbstractRouteDefLoader::init(OptionsCont &options)
{
	myBegin = options.getInt("begin");
    return myInit(options);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



//---------------------------------------------------------------------------//
//                        RODFLoader.cpp -
//  Loader used while online-routing
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu, 17 Jun 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.3  2006/08/01 11:30:21  dkrajzew
// patching building
//
// Revision 1.3  2006/05/08 11:14:53  dksumo
// further work on the dfrouter
//
// Revision 1.2  2006/04/11 11:07:58  dksumo
// debugging
//
// Revision 1.1  2006/03/28 06:17:21  dksumo
// extending the dfrouter by distance/length factors
//
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

#include "RODFLoader.h"
#include "RODFNetHandler.h"

#include <router/ROAbstractRouteDefLoader.h>
#include <router/RONet.h>
#include <utils/common/ToString.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include <iomanip>
#include <iostream>
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/XMLHelpers.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
RODFLoader::RODFLoader(OptionsCont &oc, ROVehicleBuilder &vb,
                     bool emptyDestinationsAllowed)
    : ROLoader(oc, vb, emptyDestinationsAllowed)
{
}


RODFLoader::~RODFLoader()
{
}


RONet *
RODFLoader::loadNet(ROAbstractEdgeBuilder &eb)
{
    RONet *net = new RONet(true);
    std::string file = _options.getString("n");
    if(file==""||!FileHelpers::exists(file)) {
        MsgHandler::getErrorInstance()->inform("The network file '" + file + "' could not be found.");
        throw ProcessError();
    }
    MsgHandler::getMessageInstance()->beginProcessMsg("Loading net...");
    RODFNetHandler handler(_options, *net, eb);
	handler.setFileName(file);
	XMLHelpers::runParser(handler, file);
    if(MsgHandler::getErrorInstance()->wasInformed()) {
		MsgHandler::getErrorInstance()->inform("failed.");
        delete net;
        return 0;
    } else {
		MsgHandler::getMessageInstance()->endProcessMsg("done.");
	}
    // build and prepare the parser
    return net;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


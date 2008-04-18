/****************************************************************************/
/// @file    RODFLoader.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 17 Jun 2004
/// @version $Id$
///
// Loader used while online-routing
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
#include <utils/xml/XMLSubSys.h>
#include "RODFNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
RODFLoader::RODFLoader(OptionsCont &oc, ROVehicleBuilder &vb,
                       bool emptyDestinationsAllowed)
        : ROLoader(oc, vb, emptyDestinationsAllowed)
{}


RODFLoader::~RODFLoader()
{}


RODFNet *
RODFLoader::loadNet(ROAbstractEdgeBuilder &eb, bool amInHighwayMode)
{
    RODFNet *net = new RODFNet(amInHighwayMode);
    std::string file = myOptions.getString("n");
    if (file==""||!FileHelpers::exists(file)) {
        delete net;
        throw ProcessError("The network file '" + file + "' could not be found.");
    }
    MsgHandler::getMessageInstance()->beginProcessMsg("Loading net...");
    RODFNetHandler handler(*net, eb);
    handler.setFileName(file);
    if (!XMLSubSys::runParser(handler, file)) {
        MsgHandler::getMessageInstance()->endProcessMsg("failed.");
        delete net;
        return 0;
    } else {
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
    net->buildApproachList();
    return net;
}



/****************************************************************************/


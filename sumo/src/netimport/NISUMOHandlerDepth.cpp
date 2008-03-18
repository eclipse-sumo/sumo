/****************************************************************************/
/// @file    NISUMOHandlerDepth.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id:NISUMOHandlerDepth.cpp 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer of deeper SUMO-structures (connections and logics)
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
#include <string>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "NISUMOHandlerDepth.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


NISUMOHandlerDepth::NISUMOHandlerDepth()
        : SUMOSAXHandler("sumo-network")
{}


NISUMOHandlerDepth::~NISUMOHandlerDepth() throw()
{}



/****************************************************************************/


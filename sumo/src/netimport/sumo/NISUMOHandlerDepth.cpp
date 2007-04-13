/****************************************************************************/
/// @file    NISUMOHandlerDepth.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A handler for deeper SUMO-import (connections and logics)
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
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


NISUMOHandlerDepth::NISUMOHandlerDepth(LoadFilter what)
        : SUMOSAXHandler("sumo-network"),
        _loading(what)
{}


NISUMOHandlerDepth::~NISUMOHandlerDepth()
{}


void
NISUMOHandlerDepth::myStartElement(int /*element*/, const std::string &/*name*/,
                                   const Attributes &/*attrs*/)
{}


void
NISUMOHandlerDepth::myCharacters(int /*element*/, const std::string &/*name*/,
                                 const std::string &/*chars*/)
{}

void
NISUMOHandlerDepth::myEndElement(int /*element*/, const std::string &/*name*/)
{}



/****************************************************************************/


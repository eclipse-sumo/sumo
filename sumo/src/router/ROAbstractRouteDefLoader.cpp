/****************************************************************************/
/// @file    ROAbstractRouteDefLoader.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The abstract base class for loading routes or route definitions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include "ROAbstractRouteDefLoader.h"
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/options/OptionsCont.h>
#include "RONet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ROAbstractRouteDefLoader::ROAbstractRouteDefLoader(RONet &net, SUMOTime begin,
        SUMOTime end) throw()
        : myNet(net), myBegin(begin), myEnd(end)
{}


ROAbstractRouteDefLoader::~ROAbstractRouteDefLoader() throw()
{}


/****************************************************************************/


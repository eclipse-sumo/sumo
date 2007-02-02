/****************************************************************************/
/// @file    MSSaveState.h
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id$
///
// missing_desc
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
#ifndef MSSaveState_h
#define MSSaveState_h
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


enum MSSaveState {
    SAVESTATE_EDGES = 1,
    SAVESTATE_EMITTER = 2,
    SAVESTATE_LOGICS = 4,
    SAVESTATE_ROUTES = 8,
    SAVESTATE_VEHICLES = 16
};


#endif

/****************************************************************************/


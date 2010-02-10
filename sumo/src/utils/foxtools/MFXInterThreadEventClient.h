/****************************************************************************/
/// @file    MFXInterThreadEventClient.h
/// @author  Daniel Krajzewicz
/// @date    2004-03-19
/// @version $Id$
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MFXInterThreadEventClient_h
#define MFXInterThreadEventClient_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif



class MFXInterThreadEventClient {
public:
    MFXInterThreadEventClient() {}
    virtual ~MFXInterThreadEventClient() { }
    virtual void eventOccured() = 0;
};


#endif

/****************************************************************************/


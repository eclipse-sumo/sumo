/****************************************************************************/
/// @file    MFXInterThreadEventClient.h
/// @author  Daniel Krajzewicz
/// @date    2004-03-19
/// @version $Id$
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2004-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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


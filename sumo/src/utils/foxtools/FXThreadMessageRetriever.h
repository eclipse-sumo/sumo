/****************************************************************************/
/// @file    FXThreadMessageRetriever.h
/// @author  Daniel Krajzewicz
/// @date    2004-03-19
/// @version $Id$
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef FXThreadMessageRetriever_h
#define FXThreadMessageRetriever_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "FXBaseObject.h"

class FXThreadMessageRetriever : public FXEX::FXBaseObject {
public:
    FXThreadMessageRetriever() { }
    ~FXThreadMessageRetriever() { }

private:
};


#endif

/****************************************************************************/


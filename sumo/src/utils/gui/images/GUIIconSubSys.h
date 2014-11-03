/****************************************************************************/
/// @file    GUIIconSubSys.h
/// @author  Daniel Krajzewicz
/// @date    Jul 2003
/// @version $Id$
///
// A class dividing the network in rectangular cells
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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
#ifndef GUIIconSubSys_h
#define GUIIconSubSys_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include "GUIIcons.h"


// ===========================================================================
// class definitions
// ===========================================================================
class GUIIconSubSys {
public:
    static void init(FXApp* a);
    static FXIcon* getIcon(GUIIcon which);
    static void close();
private:
    GUIIconSubSys(FXApp* a);
    ~GUIIconSubSys();

private:
    static GUIIconSubSys* myInstance;

private:
    FXIcon* myIcons[ICON_MAX];

};


#endif

/****************************************************************************/


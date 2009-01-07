/****************************************************************************/
/// @file    SUMORTree.h
/// @author  Daniel Krajzewicz
/// @date    27.10.2008
/// @version $Id$
///
// An rtree for networks
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
#ifndef SUMORTree_h
#define SUMORTree_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/windows/GUIVisualizationSettings.h>
#include <utils/geom/Boundary.h>

#include "RTree.h"


// ===========================================================================
// class definitions
// ===========================================================================
class SUMORTree : public RTree<GUIGlObject*, GUIGlObject, float, 2, GUIVisualizationSettings, float>, public Boundary
{
public:
    SUMORTree(Operation operation) 
        : RTree<GUIGlObject*, GUIGlObject, float, 2, GUIVisualizationSettings, float>(operation){
    }

    ~SUMORTree() {
    }

};


#endif

/****************************************************************************/


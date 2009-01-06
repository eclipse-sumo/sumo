/****************************************************************************/
/// @file    GUIGradients.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
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
#ifndef GUIGradients_h
#define GUIGradients_h



// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <utils/common/RGBColor.h>
#include <utils/gui/drawer/GUIGradientStorage.h>
// ===========================================================================
// variable declarations
// ===========================================================================

/// The gradient used
extern std::vector<RGBColor> myDensityGradient;

/// A storage for available gradients
extern GUIGradientStorage *gGradients;


#endif

/****************************************************************************/


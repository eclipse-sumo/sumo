/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIIOGlobals.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004-11-23
/// @version $Id$
///
// The folder used as last
/****************************************************************************/
#ifndef GUIIOGlobals_h
#define GUIIOGlobals_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>


// ===========================================================================
// global variable declarations
// ===========================================================================
/** @brief The folder used as last
 *
 * This value is loaded and stored within the registry on startup/shutdown
 *  of the application. It is changed after a file was loaded/saved.
 */
extern FXString gCurrentFolder;


#endif

/****************************************************************************/


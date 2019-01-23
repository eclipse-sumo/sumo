/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GLIncludes.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    2012
/// @version $Id$
///
// collection of includes for GL usage
/****************************************************************************/
#ifndef GLIncludes_h
#define GLIncludes_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
#endif

#include <GL/gl.h>
#include <GL/glu.h>


#endif

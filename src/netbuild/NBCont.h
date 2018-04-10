/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBCont.h
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// Some typedef definitions fir containers
/****************************************************************************/
#ifndef NBCont_h
#define NBCont_h


// ===========================================================================
// included modules
// ===========================================================================
#include <vector>
#include <set>

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdge;


// ===========================================================================
// container definitions
// ===========================================================================
/// @brief container for (sorted) edges
typedef std::vector<NBEdge*> EdgeVector;

/// @brief container for unique edges
typedef std::set<NBEdge*> EdgeSet;

/// @brief container for (sorted) lanes. The lanes are sorted from rightmost (id=0) to leftmost (id=nolanes-1)
typedef std::vector<int> LaneVector;


#endif

/****************************************************************************/


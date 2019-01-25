/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimEdgePosMap.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
#ifndef NIVissimEdgePosMap_h
#define NIVissimEdgePosMap_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


#include <map>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimEdgePosMap {
public:
    NIVissimEdgePosMap();
    ~NIVissimEdgePosMap();
    void add(int edgeid, double pos);
    void add(int edgeid, double from, double to);
    void join(NIVissimEdgePosMap& with);
private:
    typedef std::pair<double, double> Range;
    typedef std::map<int, Range> ContType;
    ContType myCont;
};


#endif

/****************************************************************************/


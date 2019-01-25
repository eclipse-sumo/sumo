/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimNodeParticipatingEdge.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
#ifndef NIVissimNodeParticipatingEdge_h
#define NIVissimNodeParticipatingEdge_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


#include <string>
#include <utils/geom/Position.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimNodeParticipatingEdge {
public:
    NIVissimNodeParticipatingEdge(int edgeid,
                                  double frompos, double topos);
    ~NIVissimNodeParticipatingEdge();
    int getID() const;
    bool positionLiesWithin(double pos) const;
    double getFromPos() const;
    double getToPos() const;
private:
    int myEdgeID;
    double myFromPos, myToPos;
};


#endif

/****************************************************************************/


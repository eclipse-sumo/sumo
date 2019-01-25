/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimNodeDef_Poly.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
#ifndef NIVissimNodeDef_Poly_h
#define NIVissimNodeDef_Poly_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


#include <string>
#include <map>
#include <utils/geom/PositionVector.h>
#include "NIVissimNodeDef_Edges.h"

class NIVissimNodeDef_Poly :
    public NIVissimNodeDef_Edges {
public:
    NIVissimNodeDef_Poly(int id, const std::string& name,
                         const PositionVector& poly);
    virtual ~NIVissimNodeDef_Poly();
    static bool dictionary(int id, const std::string& name,
                           const PositionVector& poly);
//    virtual void computeBounding();
//    double getEdgePosition(int edgeid) const;
//    void searchAndSetConnections(double offset);
private:
    PositionVector myPoly;

};


#endif

/****************************************************************************/


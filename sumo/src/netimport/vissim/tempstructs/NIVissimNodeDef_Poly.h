/****************************************************************************/
/// @file    NIVissimNodeDef_Poly.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimNodeDef_Poly_h
#define NIVissimNodeDef_Poly_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


#include <string>
#include <map>
#include <utils/geom/Position2DVector.h>
#include "NIVissimNodeDef_Edges.h"

class NIVissimNodeDef_Poly :
            public NIVissimNodeDef_Edges {
public:
    NIVissimNodeDef_Poly(int id, const std::string &name,
                         const Position2DVector &poly);
    virtual ~NIVissimNodeDef_Poly();
    static bool dictionary(int id, const std::string &name,
                           const Position2DVector &poly);
//    virtual void computeBounding();
//    SUMOReal getEdgePosition(int edgeid) const;
    void searchAndSetConnections(SUMOReal offset);
private:
    Position2DVector myPoly;

};


#endif

/****************************************************************************/


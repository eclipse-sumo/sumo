#ifndef NIVissimNodeDef_Poly_h
#define NIVissimNodeDef_Poly_h
//---------------------------------------------------------------------------//
//                        NIVissimNodeDef_Poly.h -  ccc
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.5  2003/06/05 11:46:57  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


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
    virtual void computeBounding();
    double getEdgePosition(int edgeid) const;
    void searchAndSetConnections(double offset);
private:
    Position2DVector myPoly;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NIVissimNodeDef_Poly.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


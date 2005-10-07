#ifndef Polygon2D_h
#define Polygon2D_h
//---------------------------------------------------------------------------//
//                        Polygon2D.h -
//  A 2d-polygon
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jun 2004
//  copyright            : (C) 2004 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.2  2005/10/07 11:47:18  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.1  2005/09/15 12:22:04  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2005/09/09 12:56:51  dksumo
// shape handling added
//
// Revision 1.3  2005/08/01 05:57:15  dksumo
// style adapted
//
// Revision 1.2  2005/06/14 11:29:50  dksumo
// documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2DVector.h>
#include <utils/common/DoubleVector.h>
#include <utils/geom/Position2D.h>
#include <map>


/* =========================================================================
 * class definitions
 * ======================================================================= */
class Polygon2D
{


public:
    /// Constructor
    Polygon2D(const std::string name, const std::string type,
        const RGBColor &color, const Position2DVector &Pos);

    /// Destructor
    virtual ~Polygon2D();

    /// add the incoming Position to the Polygon2D
    void addPolyPosition(Position2DVector &myNewPos);

    // return the name of the Polygon
    const std::string &getName(void) const;

    // return the type of the Polygon
    const std::string &getType(void) const;

    // return the Color of the polygon
    const RGBColor &getColor(void) const;

    // return the Positions Vector of the Polygon
    const Position2DVector &getPosition2DVector(void) const;


protected:
    /// the Name of the Polygon
    std::string myName;

    /// the type of the polygon
    std::string myType;

    /// the Color of the Polygon
    RGBColor myColor;

    /// the Positions of the Polygon
    Position2DVector myPos;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif // !defined(AFX_Polygon2D_H__4491EABC_872C_4BA2_8A8F_FA8045E3D4DD__INCLUDED_)

// Local Variables:
// mode:C++
// End:



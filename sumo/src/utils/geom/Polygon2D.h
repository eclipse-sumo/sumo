#ifndef Polygon2D_h
#define Polygon2D_h
//---------------------------------------------------------------------------//
//                        Polygon2D.h -
//  A 2d-polygon
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jun 2004
//  copyright            : (C) 2004 by Danilo Boyom
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : ---
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
/* =========================================================================
 * included modules
 * ======================================================================= */
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
	Polygon2D();

    /// Constructor
    Polygon2D(const std::string name, const std::string type, const RGBColor color, const Position2DVector &Pos);

    /// Constructor
    Polygon2D(const std::string name, const std::string type, const RGBColor color);

    /// Destructor
    ~Polygon2D();
   
    /// add the incoming Position to the Polygon2D
    void addPolyPosition(Position2DVector &myNewPos);

    // return the name of the Polygon
    std::string getName(void);

    // return the type of the Polygon
    std::string getType(void);

    // return the Color of the polygon
    RGBColor getColor(void);

    // return the Positions Vector of the Polygon
    Position2DVector getPosition2DVector(void);
    

private:
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



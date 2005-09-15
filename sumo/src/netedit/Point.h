#ifndef _POINT_H_
#define _POINT_H_
//---------------------------------------------------------------------------//
//                        Point.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                :
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
// Revision 1.3  2005/09/15 12:03:02  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/05/30 08:18:26  dksumo
// comments added
//

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class Point
{

private:

    //Koordinaten
    int x;
    int y;

public:

    //Konstruktor
    Point(int i, int j);

    //Hole die X-Koordinate des Punktes
    int GetX();

    //Hole die Y-Koordinate des Punktes
    int GetY();

    //Setze die X-Koordinate des Punktes
    void SetX(int i);

    //Setze die Y-Koordinate des Punktes
    void SetY(int j);
};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif
// Local Variables:
// mode:C++
// End:
